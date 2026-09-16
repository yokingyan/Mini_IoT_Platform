#ifndef __BOOT_LOADER_H__
#define __BOOT_LOADER_H__


#include <stdint.h>


typedef void (*Load_a)(void);

void BootLoader_Branch(void);
__asm void MSR_SP(uint32_t Addr);
void Load_A_Program(uint32_t Addr);
void BootLoader_Clear(void);
uint8_t BootLoader_Enter(uint8_t Timeout);
void BootLoader_Menu(void);
void BootLoader_Event(uint8_t *Data, uint16_t Len);


uint16_t Xmdoem_CRC16(uint8_t *Data,uint16_t Len);





#endif
