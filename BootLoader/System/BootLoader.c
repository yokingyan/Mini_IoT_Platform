#include "BootLoader.h"
#include "ProjConfig.h"
#include "Serial.h"
#include "Delay.h"
#include "Flash.h"
#include "String.h"
#include "AT24C02.h"
#include "W25Q64.h"
#include "OTA.h"


Load_a Load_A;// 声明一个函数指针变量 Load_A


void BootLoader_Branch(void)
{
    if (!BootLoader_Enter(50))
    {
        if (OTA_GetOTAFlag() == OTA_UPDATE_FLAG)      // OTA更新事件
        {
            Serial_Printf("OTA更新\r\n");
            OTA_SetBootFlag(OTA_UPDATE);
            OTA_SetW25Q64BlockID(0);
        }
        else
        {
            Serial_Printf("OTA跳转A分区\r\n");
            Load_A_Program(FLASH_SADDR_A);
        }
    }
    Serial_Printf("进入BootLoader命令行\r\n");
    BootLoader_Menu();

}

__asm void MSR_SP(uint32_t Addr)
{
    MSR MSP, r0                 // 把 r0 的值写入 MSP 寄存器，手动修改主栈栈顶（MSP）地址。
    BX r14                      // 函数返回，切回 C 代码调用处
}

void Load_A_Program(uint32_t Addr)
{
    /* 判断sp栈顶指针的范围是否合法，在对应型号的RAM控件范围内 */
    if( (*(uint32_t *)Addr >= 0x20000000U) && (*(uint32_t *)Addr <= 0x20004fffU) )  
    {
        __disable_irq(); // 关中断
        
        MSR_SP(*(volatile uint32_t *)Addr);
        
        Load_A = (Load_a)*(uint32_t *)(Addr + 4);
        
        // 复位外设
        BootLoader_Clear();       
        
        Load_A();
        while (1);
    }
    else Serial_Printf("跳转A分区失败\r\n");
}

void BootLoader_Clear(void)
{
    USART_DeInit(USART1);//复位串口
    GPIO_DeInit(GPIOA);  //复位GPIO
    GPIO_DeInit(GPIOB);
}
     
uint8_t BootLoader_Enter(uint8_t Timeout)
{
    Serial_Printf("输入b，进入BootLoader命令行\r\n");
    while (Timeout--)
    {
        Delay_ms(100);
        if (U1_RxBuff[0] == 'b') return 1;
    }
    return 0;
}

void BootLoader_Menu(void)
{
    Serial_Printf("\r\n");
    Serial_Printf("[1] 擦除A区\r\n");
    Serial_Printf("[2] 串口IAP下载A区程序\r\n");
    Serial_Printf("[3] 设置OTA版本号\r\n");
    Serial_Printf("[4] 查询OTA版本号\r\n");
    Serial_Printf("[5] 外部Flash下载程序\r\n");
    Serial_Printf("[6] 运行外部Flash程序\r\n");
    Serial_Printf("[7] 重启\r\n");
    Serial_Printf("\r\n");
    
}

void BootLoader_Event(uint8_t *Data, uint16_t Len)
{
    uint8_t i;
    int Temp;
    if (OTA_GetBootFlagStatus() == 0)
    {
        if ((Len == 1) && (Data[0] == '1'))
        {
            Serial_Printf("擦除A区...\r\n");
            Flash_ErasePage(SPAGE_A, PAGE_A_NUM);
        }
        else if ((Len == 1) && (Data[0] == '2'))
        {
            Serial_Printf("串口IAP下载A区程序（bin）\r\n");
            Flash_ErasePage(SPAGE_A, PAGE_A_NUM);
            OTA_SetBootFlag(IAP_XMODEM_START | IAP_XMODEM_DATA);
            OTA_XmodemSetTimer(0);
            OTA_XmodemSetRecvID(0);
        }
        else if ((Len == 1) && (Data[0] == '3'))
        {
            Serial_Printf("设置版本号\r\n");
            OTA_SetBootFlag(OTA_SET_VERSION);
        }
        else if ((Len == 1) && (Data[0] == '4'))
        {
            Serial_Printf("查询版本号\r\n");
            AT24C02_ReadOTA();
            Serial_Printf("当前版本号:%s\r\n", OTA_GetOTAVersion());
            BootLoader_Menu();
        }
        else if ((Len == 1) && (Data[0] == '5'))
        {
            Serial_Printf("外部Flash下载程序 [1-9] \r\n");
            OTA_SetBootFlag(UPDATE_FLASH);
        }
        else if ((Len == 1) && (Data[0] == '6'))
        {
            Serial_Printf("运行Flash程序 [1-9] \r\n");
            OTA_SetBootFlag(EXECUTE_FLASH);            
        }
        else if ((Len == 1) && (Data[0] == '7'))
        {
            Serial_Printf("重启...\r\n");
            Delay_ms(100);
            NVIC_SystemReset();
        }
    }
    else if (OTA_GetBootFlagStatus() & IAP_XMODEM_DATA)
    {
        if ((Len == 133) && (Data[0] == 0x01))   // 属于Xmodem数据包
        {
            OTA_ClearBootFlag(IAP_XMODEM_START);
            OTA_XmodemSetCRC(Xmdoem_CRC16(&Data[3], 128));
            if (OTA_XmodemGetCRC() == (Data[131] << 8) + Data[132])
            {
                OTA_SetUpdateBuff((OTA_GetXmodemRecvID() % (PAGE_SIZE / 128)) * 128, &Data[3], 128);
                OTA_XmodemRecvIDAdd();
                if ((OTA_GetXmodemRecvID() % (PAGE_SIZE / 128)) == 0)
                {
                    // 外部Flash  BUG
                    if (OTA_GetBootFlagStatus() & DOWNLOAD_FLASH)
                    {
                        uint32_t blk_id = OTA_GetW25Q64BlockID();
                        uint32_t chunk_idx = (OTA_GetXmodemRecvID() / (PAGE_SIZE / 128)) - 1;
                        for (i = 0; i < 4; ++i) 
                        {
                            uint32_t curr_page = blk_id * 256 + chunk_idx * 4 + i;
                            W25Q64_PageProgram(OTA_GetUpdateBuff(i * 256), curr_page);
                            
                        }
                    }
                    // 内部Flash
                    else Flash_ProgramWord(FLASH_SADDR_A + ((OTA_GetXmodemRecvID() / (PAGE_SIZE / 128)) - 1) * PAGE_SIZE, (uint32_t *)OTA_GetUpdateBuff(0), PAGE_SIZE);
                }
                Serial_Printf("\x06");
            }
            else Serial_Printf("\x15");
        }
         if ((Len == 1) && (Data[0] == 0x04))
         {
             Serial_Printf("\x06");
             if ((OTA_GetXmodemRecvID() % (PAGE_SIZE / 128)) != 0)
            {
                // 外部Flash  BUG 
                if (OTA_GetBootFlagStatus() & DOWNLOAD_FLASH)
                {
                    uint32_t blk_id = OTA_GetW25Q64BlockID();
                    uint32_t chunk_idx = (OTA_GetXmodemRecvID() / 8);
                    uint32_t start_page = blk_id * 256 + chunk_idx * 4;
                    uint32_t remain_packet_cnt = OTA_GetXmodemRecvID() % 8;
                    for (i = 0; i < 4; ++i) 
                        if(i*256 < remain_packet_cnt *128)
                            W25Q64_PageProgram(OTA_GetUpdateBuff(i * 256), start_page + i);
                }
                else 
                    Flash_ProgramWord(FLASH_SADDR_A + (OTA_GetXmodemRecvID() / (PAGE_SIZE / 128)) * PAGE_SIZE, (uint32_t *)OTA_GetUpdateBuff(0), (OTA_GetXmodemRecvID() % (PAGE_SIZE / 128)) * 128);
            }
            OTA_ClearBootFlag(IAP_XMODEM_DATA);
            if (OTA_GetBootFlagStatus() & DOWNLOAD_FLASH)
            {
                OTA_ClearBootFlag(DOWNLOAD_FLASH);
                OTA_SetOTAFilexSize(OTA_GetW25Q64BlockID(), OTA_GetXmodemRecvID() * 128);
                AT24C02_WriteOTA();
                Delay_ms(100);
                BootLoader_Menu();
            }
            else 
            {
                Delay_ms(100);
                NVIC_SystemReset();
            }
         }
    }
    else if (OTA_GetBootFlagStatus() & OTA_SET_VERSION)
    {
        if (Len == OTA_VERSION_LEN)
        {
            if (sscanf((char *)Data, "Ver-%d.%d.%d-%d/%d/%d-%d:%d", &Temp, &Temp, &Temp, &Temp, &Temp, &Temp, &Temp, &Temp) == 8)
            {
                OTA_SetOTAVersion((char *)Data);
                // 保存置 24c02
                AT24C02_WriteOTA();
                Serial_Printf("版本号更新成功\r\n");
                OTA_ClearBootFlag(OTA_SET_VERSION);
                BootLoader_Menu();
            } else Serial_Printf("版本号格式错误!!!\r\n");
        } 
        else Serial_Printf("版本号长度错误!!!\r\n");
    }
    else if (OTA_GetBootFlagStatus() & UPDATE_FLASH)
    {
        if ( Len == 1 )
        {
            if ((Data[0] >= '1') && (Data[0] <= '9'))
            {
                OTA_SetW25Q64BlockID(Data[0] - '0');
                OTA_SetBootFlag(IAP_XMODEM_START | IAP_XMODEM_DATA | DOWNLOAD_FLASH);
                OTA_XmodemSetTimer(0);
                OTA_XmodemSetRecvID(0);
                OTA_SetOTAFilexSize(OTA_GetW25Q64BlockID(), 0);
                W25Q64_Block64KErase(OTA_GetW25Q64BlockID());
                Serial_Printf("串口IAP下载A区程序到外部Flash %d 块（bin）\r\n", OTA_GetW25Q64BlockID());
                OTA_ClearBootFlag(UPDATE_FLASH);
            }
            else Serial_Printf("数据格式错误!!!\r\n");
        } else Serial_Printf("数据格式错误!!!\r\n");
    }
    else if (OTA_GetBootFlagStatus() & EXECUTE_FLASH)
    {
        if ( Len == 1 )
        {
            if ((Data[0] >= '1') && (Data[0] <= '9'))
            {
                OTA_SetW25Q64BlockID(Data[0] - '0');
                OTA_SetBootFlag(OTA_UPDATE);
                OTA_ClearBootFlag(EXECUTE_FLASH);
            } 
            else Serial_Printf("数据格式错误!!!\r\n");
        } else Serial_Printf("数据格式错误!!!\r\n");
    }
}

/**~
  * 函    数：Xmdoem CRC校验
              Poly 1021   初始值0000
  * 参    数：Data 待校验的数据
  * 参    数：Len 数据长度
  * 返 回 值：CRC 校验结果
  */
uint16_t Xmdoem_CRC16(uint8_t *Data,uint16_t Len)
{
    uint8_t i;
    uint16_t CRC_Init = 0x0000;
    uint16_t CRC_Poly = 0x1021;
    
    while (Len--)
    {
        CRC_Init = (*Data << 8) ^ CRC_Init;
        for (i = 0; i < 8; ++i)
        {
            if (CRC_Init & 0x8000) CRC_Init = (CRC_Init << 1) ^ CRC_Poly;
            else CRC_Init = (CRC_Init << 1);            
        }
        Data++;
    }
    return CRC_Init;
}



