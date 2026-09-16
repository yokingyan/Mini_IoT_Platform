#include "W25Q64.h"                 
#include "SPI.h"
#include "W25Q64_Ins.h"

/**
  * 函    数：W25Q64初始化
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_Init(void)
{
	Spi_Init();					//先初始化底层的SPI
}

/**
  * 函    数：W25Q64读取ID号
  * 参    数：MID 工厂ID，使用输出参数的形式返回
  * 参    数：DID 设备ID，使用输出参数的形式返回
  * 返 回 值：无
  */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	Spi_Start();								//SPI起始
	Spi_SwapByte(W25Q64_JEDEC_ID);			//交换发送读取ID的指令
	*MID = Spi_SwapByte(W25Q64_DUMMY_BYTE);	//交换接收MID，通过输出参数返回
	*DID = Spi_SwapByte(W25Q64_DUMMY_BYTE);	//交换接收DID高8位
	*DID <<= 8;									//高8位移到高位
	*DID |= Spi_SwapByte(W25Q64_DUMMY_BYTE);	//或上交换接收DID的低8位，通过输出参数返回
	Spi_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64写使能
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WriteEnable(void)
{
	Spi_Start();								//SPI起始
	Spi_SwapByte(W25Q64_WRITE_ENABLE);		//交换发送写使能的指令
	Spi_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64等待忙
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	Spi_Start();								//SPI起始
	Spi_SwapByte(W25Q64_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	Timeout = 1e5;							//给定超时计数时间
	while ((Spi_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;								//等待时，计数值自减
		if (Timeout == 0)						//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;								//跳出等待，不等了
		}
	}
	Spi_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64页编程
  * 参    数：DataArray	用于写入数据的数组
  * 参    数：PageID 要写入页的编号，范围：0~32767
  * 返 回 值：无
  * 注意事项：写入的地址范围不能跨页
  */
void W25Q64_PageProgram(uint8_t *DataArray, uint16_t PageID)
{
	uint16_t i;
	
    W25Q64_WaitBusy();	
	W25Q64_WriteEnable();						//写使能
	
	Spi_Start();								//SPI起始
	Spi_SwapByte(W25Q64_PAGE_PROGRAM);		//交换发送页编程的指令
	Spi_SwapByte(PageID * 256 >> 16);				//交换发送地址23~16位
	Spi_SwapByte(PageID * 256 >> 8);				//交换发送地址15~8位
	Spi_SwapByte(PageID * 256 >> 0);					//交换发送地址7~0位
	for (i = 0; i < 256; i++)				//循环Count次
	{
		Spi_SwapByte(DataArray[i]);			//依次在起始地址后写入数据
	}
	Spi_Stop();								//SPI终止
	
	W25Q64_WaitBusy();							//等待忙
}

/**
  * 函    数：W25Q64扇区擦除（4KB）
  * 参    数：Address 指定扇区的地址，范围：0x000000~0x7FFFFF
  * 返 回 值：无
  */
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();						//写使能
	
	Spi_Start();								//SPI起始
	Spi_SwapByte(W25Q64_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	Spi_SwapByte(Address >> 16);				//交换发送地址23~16位
	Spi_SwapByte(Address >> 8);				//交换发送地址15~8位
	Spi_SwapByte(Address >> 0);					//交换发送地址7~0位
	Spi_Stop();								//SPI终止
	
	W25Q64_WaitBusy();							//等待忙
}

/**
  * 函    数：W25Q64块区擦除（64KB）
  * 参    数：Address 指定扇区的编号，范围：0 ~ 127
  * 返 回 值：无
  */
void W25Q64_Block64KErase(uint8_t BlockID)
{
    W25Q64_WaitBusy();
    W25Q64_WriteEnable();
    
    Spi_Start();
    Spi_SwapByte(W25Q64_BLOCK_ERASE_64KB);
    Spi_SwapByte((BlockID * 64 * 1024) >> 16);
    Spi_SwapByte((BlockID * 64 * 1024) >> 8);	
    Spi_SwapByte((BlockID * 64 * 1024) >> 0);
    Spi_Stop();
    
    W25Q64_WaitBusy();
}

/**
  * 函    数：W25Q64读取数据
  * 参    数：Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray 用于接收读取数据的数组，通过输出参数返回
  * 参    数：Count 要读取数据的数量，范围：0~0x800000
  * 返 回 值：无
  */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
    W25Q64_WaitBusy();
	Spi_Start();								//SPI起始
	Spi_SwapByte(W25Q64_READ_DATA);			//交换发送读取数据的指令
	Spi_SwapByte(Address >> 16);				//交换发送地址23~16位
	Spi_SwapByte(Address >> 8);				//交换发送地址15~8位
	Spi_SwapByte(Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		DataArray[i] = Spi_SwapByte(W25Q64_DUMMY_BYTE);	//依次在起始地址后读取数据
	}
	Spi_Stop();								//SPI终止
}
