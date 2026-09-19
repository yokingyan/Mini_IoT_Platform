#include "OTA.h"
#include <string.h>
#include "AT24C02.h"
#include "Delay.h"


/* OTA基础信息结构体，保存版本、校验、固件大小等信息 */
static OTA_InfoTypeDef OTA_InfoStructure;
/* OTA升级过程控制结构体，管理分包接收、升级状态机 */
static OTA_UpdateTypedef OTA_UpdateStructure;
/* OTA启动状态标志，供Bootloader判断是否加载新固件 */
static uint32_t OTA_BootFlag;






/**
 *	函数：OTA 初始化
 *	参数：无
 *	返回值：无
 */
void OTA_Init(void)
{
    // 1.OTA_Info 结构体初始化
    memset(&OTA_InfoStructure, 0, OTA_INFO_SIZE);
    // 参数初始化
    OTA_InfoStructure.OTA_Flag = 0;
    // 2. 升级过程控制结构体清零（接收包ID、blockID、缓存全部重置）
    memset(&OTA_UpdateStructure, 0, sizeof(OTA_UpdateTypedef));
    // 3. 清除标志位
    OTA_BootFlag = 0;
}

/**
 *	函数：OTA 更新BootFlag
 *	参数：OTA_FLAG 当前OTA BootFlag
 *	返回值：无
 */
void OTA_SetBootFlag(uint32_t OTA_FLAG)
{
    __disable_irq();
    OTA_BootFlag |= OTA_FLAG;
    __enable_irq();
}
    
/**
 *	函数：OTA 清除当前状态
 *	参数：OTA_FLAG 当前OTA BootFlag
 *	返回值：无
 */
void OTA_ClearBootFlag(uint32_t OTA_FLAG)
{
    __disable_irq();
    OTA_BootFlag &= ~OTA_FLAG;
    __enable_irq();
}
  
/**
 *	函数：OTA 获取Boot Flag
 *	参数：无
 *	返回值：当前OTA Boot Flag
 */
uint32_t OTA_GetBootFlagStatus(void)
{
    return OTA_BootFlag;
}

/**
 *	函数：设置OTA Flag
 *	参数：OTA_FLAG OTA更新标志
 *	返回值：无
 */
void OTA_SetOTAFlag(uint32_t OTA_FLAG)
{
    OTA_InfoStructure.OTA_Flag = OTA_FLAG;
}
  
/**
 *	函数：获取OTA更新标志
 *	参数：无
 *	返回值：OTA更新标志
 */    
uint32_t OTA_GetOTAFlag(void)
{
    return OTA_InfoStructure.OTA_Flag;
}

void OTA_SetOTAVersion(char *OTA_VERSION)
{
    memset(OTA_InfoStructure.OTA_Version, 0, 32);
    memcpy(OTA_InfoStructure.OTA_Version, OTA_VERSION, OTA_VERSION_LEN);
}
    
uint8_t *OTA_GetOTAVersion(void)
{
    return OTA_InfoStructure.OTA_Version;
}

void OTA_SetOTAFilexSize(uint32_t FilexBlockID, uint32_t Value)
{
    OTA_InfoStructure.FileSize[FilexBlockID] = Value;
}
    
uint32_t OTA_GetOTAFilexSize(uint32_t FilexBlockID)
{
    return OTA_InfoStructure.FileSize[FilexBlockID];
}

void OTA_SetUpdateBuff(uint32_t BuffID, void *Data, uint16_t Len)
{
    memcpy(&OTA_UpdateStructure.UpdateBuff[BuffID], Data, Len);
}
    
uint8_t *OTA_GetUpdateBuff(uint8_t BuffID)
{
    return &OTA_UpdateStructure.UpdateBuff[BuffID];
}

void OTA_SetW25Q64BlockID(uint32_t ID_Value)
{
    OTA_UpdateStructure.W25Q64_BlockID = ID_Value;
}
    
uint32_t OTA_GetW25Q64BlockID(void)
{
    return OTA_UpdateStructure.W25Q64_BlockID;
}

void OTA_XmodemTimerAdd(void)
{
    OTA_UpdateStructure.XmodemTimer += 1;
}
    
void OTA_XmodemTimerSub(void)
{
    OTA_UpdateStructure.XmodemTimer -= 1;
}

void OTA_XmodemSetTimer(uint32_t Value)
{
    OTA_UpdateStructure.XmodemTimer = Value;
}

uint32_t OTA_GetXmodemTimer(void)
{
    return OTA_UpdateStructure.XmodemTimer;
}

void OTA_XmodemRecvIDAdd(void)
{
    OTA_UpdateStructure.XmodemRecvID += 1;
}
    
void OTA_XmodemRecvIDSub(void)
{
    OTA_UpdateStructure.XmodemRecvID -= 1;
}

void OTA_XmodemSetRecvID(uint32_t Value)
{
    OTA_UpdateStructure.XmodemRecvID = Value;
}

uint32_t OTA_GetXmodemRecvID(void)
{
    return OTA_UpdateStructure.XmodemRecvID;
}

void OTA_XmodemSetCRC(uint16_t CRC_Res)
{
    OTA_UpdateStructure.XmodemCRC = CRC_Res;
}
    
uint32_t OTA_XmodemGetCRC(void)
{
    return OTA_UpdateStructure.XmodemCRC;
}



/* 24c02 */
/* 获取OTA Info */
void AT24C02_ReadOTA(void)
{
    memset(&OTA_InfoStructure, 0, OTA_INFO_SIZE);
    AT24C02_ReadData(0, (uint8_t *)&OTA_InfoStructure, OTA_INFO_SIZE);
}

/* 设置OTA Info */
void AT24C02_WriteOTA(void)
{
    uint8_t i;
    uint8_t *wptr = (uint8_t *)&OTA_InfoStructure;
    for (i = 0; i < (OTA_INFO_SIZE / 8); ++i)
    {
        AT24C02_WritePage(i * 8, wptr + i * 8);
        Delay_ms(5);
    }
}



