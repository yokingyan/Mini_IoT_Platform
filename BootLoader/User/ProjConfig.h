/**
  ******************************************************************************
  * @file    ProjConfig.h
  * @author  Yoking.Yan
  * @version V1.0.0
  * @date    16-Sep-2026
  * @brief   参数文件，包含系统宏定义以及结构体的定义
  ******************************************************************************
  */

#ifndef __PROJ_CONFIG_H__
#define __PROJ_CONFIG_H__


#include <stdint.h>

/* STM32F1 系列 Flash 映射在 CPU 地址空间 */
#ifndef FLASH_SADDR
#define FLASH_SADDR             0x08000000
#endif

/* 页大小(Bit) */
#ifndef PAGE_SIZE
#define PAGE_SIZE               1024
#endif

/* Flash扇区总数量 */
#define PAGE_NUM                64                          
/* B区扇区数量 */
#define PAGE_B_NUM              20     
/* A区扇区数量 */
#define PAGE_A_NUM              (PAGE_NUM - PAGE_B_NUM)   
/* A区起始扇区编号 */
#define SPAGE_A                 PAGE_B_NUM 
/* A区扇区起始地址 */ 
#define FLASH_SADDR_A           ( FLASH_SADDR + SPAGE_A * PAGE_SIZE )



#endif



