#include "AT24C02.h"
#include "IIC.h"
#include "ProjConfig.h"
#include "Delay.h"
#include <string.h>


uint8_t AT24C02_WriteByte(uint8_t Addr, uint8_t Data)
{
    IIC_Start();
    IIC_SendByte(AT24C02_W_ADDR);
    if (IIC_ReceiveAck()) return 1;
    IIC_SendByte(Addr);
    if (IIC_ReceiveAck()) return 1;
    IIC_SendByte(Data);
    if (IIC_ReceiveAck()) return 1;
    IIC_Stop();
    return 0;
}

/* Page - 8 Bytes */
uint8_t AT24C02_WritePage(uint8_t Addr, uint8_t *Data)
{
    uint8_t i;
    IIC_Start();
    IIC_SendByte(AT24C02_W_ADDR);
    if (IIC_ReceiveAck()) return 1;
    IIC_SendByte(Addr);
    if (IIC_ReceiveAck()) return 1;
    for (i = 0; i < 8; ++i)
    {
        IIC_SendByte(Data[i]);
        if (IIC_ReceiveAck()) return 1;
    }
    IIC_Stop();
    return 0;
}

uint8_t AT24C02_ReadData(uint8_t Addr, uint8_t *Data, uint16_t Len)
{
    uint8_t i;
    IIC_Start();
    IIC_SendByte(AT24C02_W_ADDR);
    if (IIC_ReceiveAck()) return 1;
    IIC_SendByte(Addr);
    if (IIC_ReceiveAck()) return 1;
    
    IIC_Start();
    IIC_SendByte(AT24C02_R_ADDR);
    if (IIC_ReceiveAck()) return 1;
    for (i = 0; i < Len - 1; ++i)
    {
        Data[i] = IIC_ReceiveByte();
        IIC_SendAck(0);
    }
    Data[Len - 1] = IIC_ReceiveByte();
    IIC_SendAck(1);
    
    IIC_Stop();
    return 0;
}


