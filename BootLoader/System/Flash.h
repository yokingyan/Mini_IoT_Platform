#ifndef __FLASH_H__
#define __FLASH_H__


#include "stm32f10x.h"   // Device header

#ifndef FLASH_SADDR
#define FLASH_SADDR             0x8000000
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE               1024
#endif


void Flash_ErasePage(uint16_t PageAddressID, uint16_t Count);
void Flash_ProgramWord(uint32_t Address, uint32_t *Data, uint32_t Count);

uint32_t Flash_ReadWord(uint32_t Address);
uint16_t Flash_ReadHalfWord(uint32_t Address);
uint8_t Flash_ReadByte(uint32_t Address);




#endif
