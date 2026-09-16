#ifndef __AT24C02_H__
#define __AT24C02_H__


#include "stm32f10x.h"                  // Device header


#define AT24C02_W_ADDR          0xA0
#define AT24C02_R_ADDR          0xA1


uint8_t AT24C02_WriteByte(uint8_t Addr, uint8_t Data);
uint8_t AT24C02_WritePage(uint8_t Addr, uint8_t *Data);
uint8_t AT24C02_ReadData(uint8_t Addr, uint8_t *Data, uint16_t Len);
void AT24C02_ReadOTA(void);
void AT24C02_WriteOTA(void);









#endif
