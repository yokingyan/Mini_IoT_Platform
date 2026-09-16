#include "stm32f10x.h"   // Device header
#include "ProjConfig.h"
#include "Delay.h"
#include "LED.h"
#include "Serial.h"
#include "IIC.h"
#include "AT24C02.h"
#include "W25Q64.h"                 
#include "SPI.h"
#include "Flash.h"
#include "BootLoader.h"


/* OTA Info 结构体 */
OTA_InfoTypeDef OTA_InfoStructure;
/* OTA Update 结构体 */
OTA_UpdateTypedef OTA_UpdateStructure;
/* OTA Status 状态值 */
uint32_t OTA_BootSTAFlag;


int main(void)
{
    uint8_t i;
    /* Hardware Init */
    Serial_Init(9600);
    IIC_Init();
    /* // */
    W25Q64_Init();
    AT24C02_ReadOTA();
    
    BootLoader_Branch();
    
    while (1)
    {
        Delay_ms(10);
        if (U1_CB.RxDataOut != U1_CB.RxDataIn)
        {
            BootLoader_Event(U1_CB.RxDataOut->start, U1_CB.RxDataOut->end - U1_CB.RxDataOut->start + 1);
            U1_CB.RxDataOut++;
            if (U1_CB.RxDataOut == U1_CB.RxDataEnd) U1_CB.RxDataOut = &U1_CB.RxDataPtr[0];
        }
        
         if (OTA_BootSTAFlag & IAP_XMODEM_START)
         {
             if (OTA_UpdateStructure.XmodemTimer >= 100)
             {
                 Serial_Printf("C");
                 OTA_UpdateStructure.XmodemTimer = 0;
             }
             OTA_UpdateStructure.XmodemTimer++;
         }
        
        if (OTA_BootSTAFlag & OTA_UPDATE)
        {
            // 更新 A 区
            Serial_Printf("长度%d字节\r\n", OTA_InfoStructure.FileLen[OTA_UpdateStructure.W25Q64_BlockID]);
            if (OTA_InfoStructure.FileLen[OTA_UpdateStructure.W25Q64_BlockID] % 4 == 0)
            {
                Flash_ErasePage(SPAGE_A, PAGE_A_NUM);
                for (i = 0; i < OTA_InfoStructure.FileLen[OTA_UpdateStructure.W25Q64_BlockID] / PAGE_SIZE; i++)
                {
                    W25Q64_ReadData(i * 1024 + OTA_UpdateStructure.W25Q64_BlockID * 64 * 1024, OTA_UpdateStructure.Updatabuff, PAGE_SIZE);
                    Flash_ProgramWord(FLASH_SADDR_A + i * PAGE_SIZE, (uint32_t *)OTA_UpdateStructure.Updatabuff, PAGE_SIZE);
                    
                    uint8_t temp[16*4];
                    W25Q64_ReadData(0x10000, temp, sizeof(temp));
                    //再转成uint32_t打印
                    for(int k=0;k<16;k++)
                    {
                        uint32_t val = *(uint32_t*)(temp + k*4);
                        Serial_Printf("BlackID:%d W25Q 0x%08X : 0x%08X\r\n", OTA_UpdateStructure.W25Q64_BlockID, 0x10000 + k*4, val);
                    }
                }
                if (OTA_InfoStructure.FileLen[OTA_UpdateStructure.W25Q64_BlockID] % 1024 != 0)
                {
                    W25Q64_ReadData(i * 1024 + OTA_UpdateStructure.W25Q64_BlockID * 64 * 1024,
                                        OTA_UpdateStructure.Updatabuff, 
                                        OTA_InfoStructure.FileLen[OTA_UpdateStructure.W25Q64_BlockID] % 1024);
                    Flash_ProgramWord(FLASH_SADDR_A + i * PAGE_SIZE, (uint32_t *)OTA_UpdateStructure.Updatabuff, 
                                        OTA_InfoStructure.FileLen[OTA_UpdateStructure.W25Q64_BlockID] % 1024);
                }
                if (OTA_UpdateStructure.W25Q64_BlockID == 0)
                {
                    OTA_InfoStructure.OTA_Flag = 0;
                    AT24C02_WriteOTA();
                }
                Serial_Printf("A区更新完毕\r\n");
                Delay_ms(100);
                NVIC_SystemReset();
            }
            else
            {
                Serial_Printf("长度错误\r\n");
                OTA_BootSTAFlag &= ~OTA_UPDATE;
            }
        }
    }
}
