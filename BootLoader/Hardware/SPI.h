#ifndef __SPI_H__
#define __SPI_H__


#include "stm32f10x.h"                  // Device header

#define __RCC_GPIO        RCC_APB2Periph_GPIOA
#define __SPI_PORT        GPIOA
#define __SPI_NSS         GPIO_Pin_4
#define __SPI_SCK         GPIO_Pin_5
#define __SPI_MISO        GPIO_Pin_6
#define __SPI_MOSI        GPIO_Pin_7


static void Spi_W_SS(uint8_t BitValue);
void Spi_Init(void);
void Spi_Start(void);
void Spi_Stop(void);
uint8_t Spi_SwapByte(uint8_t ByteSend);
void Spi_WriteBytes(uint8_t *Data, uint16_t Len);
void Spi_ReadBytes(uint8_t *Data, uint16_t Len);



#endif
