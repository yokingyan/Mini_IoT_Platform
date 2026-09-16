#include "SPI.h"


void Spi_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(__SPI_PORT, __SPI_NSS, (BitAction)BitValue);
}

void Spi_Init(void)
{
    /* 开启时钟 */
	RCC_APB2PeriphClockCmd(__RCC_GPIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    /* 配置 NSS */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = __SPI_NSS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(__SPI_PORT, &GPIO_InitStructure);
    /* 配置输出引脚 SCK MOSI */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = __SPI_SCK | __SPI_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(__SPI_PORT, &GPIO_InitStructure);	
    /* 配置输入引脚 MISO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = __SPI_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(__SPI_PORT, &GPIO_InitStructure);
    /* 配置SPI1 */
    SPI_InitTypeDef SPI_InitStructure;						//定义结构体变量
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;			//模式，选择为SPI主模式
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//方向，选择2线全双工
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//数据宽度，选择为8位
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;		//先行位，选择高位先行
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//波特率分频，选择2分频
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;				//SPI极性，选择低极性
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;			//SPI相位，选择第一个时钟边沿采样，极性和相位决定选择SPI模式0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS，选择由软件控制
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRC多项式，暂时用不到，给默认值7
	SPI_Init(SPI1, &SPI_InitStructure);	
	/*SPI使能*/
	SPI_Cmd(SPI1, ENABLE);	
	/*设置默认电平*/
	Spi_W_SS(1);
}
    
void Spi_Start(void)
{
    Spi_W_SS(0);
}

void Spi_Stop(void)
{
    Spi_W_SS(1);
}
    
uint8_t Spi_SwapByte(uint8_t ByteSend)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(SPI1, ByteSend);	
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);
    return SPI_I2S_ReceiveData(SPI1);
}

void Spi_WriteBytes(uint8_t *Data, uint16_t Len)
{
    uint16_t i;
    for (i = 0; i < Len; ++i)
    {
        Spi_SwapByte(Data[i]);
    }
}

void Spi_ReadBytes(uint8_t *Data, uint16_t Len)
{
    uint16_t i;
    for (i = 0; i < Len; ++i)
    {
        Data[i] = Spi_SwapByte(0xff);
    }
}



