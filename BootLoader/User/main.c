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
#include "OTA.h"


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
        
         if (OTA_GetBootFlagStatus() & IAP_XMODEM_START)
         {
             if (OTA_GetXmodemTimer() >= 100)
             {
                 Serial_Printf("C");
                 OTA_XmodemSetTimer(0);
             }
             OTA_XmodemTimerAdd();
         }
        
        if (OTA_GetBootFlagStatus() & OTA_UPDATE)
        {
            // 更新 A 区
            Serial_Printf("长度%d字节\r\n", OTA_GetOTAFilexSize(OTA_GetW25Q64BlockID()));
            if (OTA_GetOTAFilexSize(OTA_GetW25Q64BlockID()) % 4 == 0)
            {
                Flash_ErasePage(SPAGE_A, PAGE_A_NUM);
                for (i = 0; i < OTA_GetOTAFilexSize(OTA_GetW25Q64BlockID()) / PAGE_SIZE; i++)
                {
                    W25Q64_ReadData(i * 1024 + OTA_GetW25Q64BlockID() * 64 * 1024, OTA_GetUpdateBuff(0), PAGE_SIZE);
                    Flash_ProgramWord(FLASH_SADDR_A + i * PAGE_SIZE, (uint32_t *)OTA_GetUpdateBuff(0), PAGE_SIZE);
                    
                    uint8_t temp[16*4];
                    W25Q64_ReadData(0x10000, temp, sizeof(temp));
                    //再转成uint32_t打印
                    for(int k=0;k<16;k++)
                    {
                        uint32_t val = *(uint32_t*)(temp + k*4);
                        Serial_Printf("BlackID:%d W25Q 0x%08X : 0x%08X\r\n", OTA_GetW25Q64BlockID(), 0x10000 + k*4, val);
                    }
                }
                if (OTA_GetOTAFilexSize(OTA_GetW25Q64BlockID()) % 1024 != 0)
                {
                    W25Q64_ReadData(i * 1024 + OTA_GetW25Q64BlockID() * 64 * 1024,
                                        OTA_GetUpdateBuff(0), 
                                        OTA_GetOTAFilexSize(OTA_GetW25Q64BlockID()) % 1024);
                    Flash_ProgramWord(FLASH_SADDR_A + i * PAGE_SIZE, (uint32_t *)OTA_GetUpdateBuff(0), 
                                        OTA_GetOTAFilexSize(OTA_GetW25Q64BlockID()) % 1024);
                }
                if (OTA_GetW25Q64BlockID() == 0)
                {
                    OTA_SetBootFlag(0);
                    AT24C02_WriteOTA();
                }
                Serial_Printf("A区更新完毕\r\n");
                Delay_ms(100);
                NVIC_SystemReset();
            }
            else
            {
                Serial_Printf("长度错误\r\n");
                OTA_ClearBootFlag(OTA_UPDATE);
            }
        }
    }
}
