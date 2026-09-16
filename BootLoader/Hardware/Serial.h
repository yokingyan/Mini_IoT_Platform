#ifndef __SERIAL_H__
#define __SERIAL_H__


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>


#define U1_RX_SIZE 				        2048     // 接收数组缓冲区长度
#define U1_RX_MAX					    256	     //单次接收最大量
#define NUM								10       //se指针对结构体数组长度

typedef struct
{
	uint8_t *start;
	uint8_t *end;
} UCB_URxBuff_ptr;

typedef struct
{
	uint16_t RxCounter;					// 统计写入数据计数器
	UCB_URxBuff_ptr RxDataPtr[NUM];
	UCB_URxBuff_ptr *RxDataIn;				// 标记DMA写入地址
	UCB_URxBuff_ptr *RxDataOut;				// 标记DMA读取地址
	UCB_URxBuff_ptr *RxDataEnd;				// 标记数组边界
} UCB;

extern uint8_t U1_RxBuff[U1_RX_SIZE];
extern UCB U1_CB;


void Serial_Init(uint32_t BandRate);
void SerialDMA_Init(void);
void SerialRxPtr_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

#endif


