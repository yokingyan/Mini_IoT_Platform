#include "Flash.h"


/**
  * 函    数：FLASH页擦除
  * 参    数：PageAddress 要擦除页的页地址
  * 参    数：Count       要擦除的页数
  * 返 回 值：无
  */
void Flash_ErasePage(uint16_t PageAddressID, uint16_t Count)
{
    uint16_t i;
    FLASH_Unlock();
    for (i = 0; i < Count; ++i) 
        FLASH_ErasePage((FLASH_SADDR + PageAddressID * PAGE_SIZE) + (PAGE_SIZE * i));
    FLASH_Lock();	
}

/**
  * 函    数：FLASH编程字
  * 参    数：Address 要写入数据的字地址
  * 参    数：Data 要写入的32位数据
  * 参    数：Count 要写入的数据次数
  * 返 回 值：无
  */
void Flash_ProgramWord(uint32_t Address, uint32_t *Data, uint32_t Count)
{
	FLASH_Unlock();							//解锁
    while (Count)
    {
        FLASH_ProgramWord(Address, *Data);
        Count -= 4;
        Address += 4;
        ++Data;
    }
	FLASH_Lock();							//加锁
}

/**
  * 函    数：FLASH读取一个32位的字
  * 参    数：Address 要读取数据的字地址
  * 返 回 值：指定地址下的数据
  */
uint32_t Flash_ReadWord(uint32_t Address)
{
	return *((__IO uint32_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个16位的半字
  * 参    数：Address 要读取数据的半字地址
  * 返 回 值：指定地址下的数据
  */
uint16_t Flash_ReadHalfWord(uint32_t Address)
{
	return *((__IO uint16_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个8位的字节
  * 参    数：Address 要读取数据的字节地址
  * 返 回 值：指定地址下的数据
  */
uint8_t Flash_ReadByte(uint32_t Address)
{
	return *((__IO uint8_t *)(Address));	//使用指针访问指定地址下的数据并返回
}


