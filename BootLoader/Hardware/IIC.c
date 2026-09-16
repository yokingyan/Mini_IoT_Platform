#include "IIC.h"


void IIC_W_SCL(uint8_t BitValue)
{
    GPIO_WriteBit(IIC_GPIO_PORT, IIC_SCL_PIN, (BitAction)BitValue);	
    Delay_us(10);		
}

void IIC_W_SDA(uint8_t BitValue)
{
    GPIO_WriteBit(IIC_GPIO_PORT, IIC_SDA_PIN, (BitAction)BitValue);	
    Delay_us(10);
}

uint8_t IIC_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(IIC_GPIO_PORT, IIC_SDA_PIN);		    //读取SDA电平
	Delay_us(10);												//延时10us，防止时序频率超过要求
	return BitValue;
}


/**
  * 函    数：I2C初始化
  * 参    数：RCC_APB2Periph_GPIOx GPIOx RCC时钟
  * 参    数：GPIOx GPIO引脚
  * 参    数：GPIO_Pinx GPIO Pin
  * 返 回 值：无
  * 注意事项：此函数需要用户实现内容，实现SCL和SDA引脚的初始化
  */
void IIC_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN | IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_GPIO_PORT, &GPIO_InitStructure);	
    
	/*设置默认电平*/
	GPIO_SetBits(IIC_GPIO_PORT, IIC_SCL_PIN | IIC_SDA_PIN);	
}

/**
  * 函    数：I2C起始
  * 参    数：无
  * 返 回 值：无
  */
void IIC_Start(void)
{
    IIC_W_SDA(1);
    IIC_W_SCL(1);
    IIC_W_SDA(0);
    IIC_W_SCL(0);
}

/**
  * 函    数：I2C终止
  * 参    数：无
  * 返 回 值：无
  */
void IIC_Stop(void)
{
    IIC_W_SDA(0);
    IIC_W_SCL(1);
    IIC_W_SDA(1);
}

/**
  * 函    数：I2C发送一个字节
  * 参    数：Byte 要发送的一个字节数据，范围：0x00~0xFF
  * 返 回 值：无
  */
void IIC_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i ++)
    {
        IIC_W_SDA(!!(Byte & (0x80 >> i)));
        IIC_W_SCL(1);
        IIC_W_SCL(0);
    }
}

/**
  * 函    数：I2C接收一个字节
  * 参    数：无
  * 返 回 值：接收到的一个字节数据，范围：0x00~0xFF
  */
uint8_t IIC_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    IIC_W_SDA(1);
    for (i = 0; i < 8; i ++)
    {
        IIC_W_SCL(1);
        if (IIC_R_SDA()){Byte |= (0x80 >> i);}
        IIC_W_SCL(0);
    }
    return Byte;
}

/**
  * 函    数：I2C发送应答位
  * 参    数：Byte 要发送的应答位，范围：0~1，0表示应答，1表示非应答
  * 返 回 值：无
  */
void IIC_SendAck(uint8_t AckBit)
{
    IIC_W_SDA(AckBit);
    IIC_W_SCL(1);
    IIC_W_SCL(0);
}

/**
  * 函    数：I2C接收应答位
  * 参    数：无
  * 返 回 值：接收到的应答位，范围：0~1，0表示应答，1表示非应答
  */
uint8_t IIC_ReceiveAck(void)
{
    uint8_t AckBit;
    IIC_W_SDA(1);
    IIC_W_SCL(1);
    AckBit = IIC_R_SDA();
    IIC_W_SCL(0);
    return AckBit;
}


