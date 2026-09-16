#include "LED.h"


void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED_1 | LED_2 | LED_3 | LED_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, LED_1 | LED_2 | LED_3 | LED_4);
}

void LED_On(uint16_t pin_num)
{
	GPIO_ResetBits(GPIOB, pin_num);
}

void LED_Off(uint16_t pin_num)
{
	GPIO_SetBits(GPIOB, pin_num);
}

void LED_Turn(uint16_t pin_num)
{
	if (GPIO_ReadOutputDataBit(GPIOB, pin_num) == 0) 
		GPIO_WriteBit(GPIOB, pin_num, Bit_SET);
	else GPIO_WriteBit(GPIOB, pin_num, Bit_RESET);
}
