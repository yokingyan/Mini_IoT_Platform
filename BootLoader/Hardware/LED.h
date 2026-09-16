#ifndef __LED_H__
#define __LED_H__

#define LED_1 GPIO_Pin_11
#define LED_2 GPIO_Pin_10
#define LED_3 GPIO_Pin_1
#define LED_4 GPIO_Pin_0


#include "stm32f10x.h"   // Device header


void LED_Init(void);
void LED_On(uint16_t pin_num);
void LED_Off(uint16_t pin_num);
void LED_Turn(uint16_t pin_num);


#endif
