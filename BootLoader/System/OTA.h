/**
  ******************************************************************************
  * @file    OTA.h
  * @author  Yoking.Yan
  * @version V1.0.0
  * @date    16-Sep-2026
  * @brief   OTA升级
  ******************************************************************************
  */
  
#ifndef __OTA_H__
#define __OTA_H__

#include <stdint.h>
#include "ProjConfig.h"


/* OTA升级完成标志位 */
#define OTA_UPDATE_FLAG         0x13131313
/* OTA_InfoTypeDef 结构体大小 */
#define OTA_INFO_SIZE           (sizeof(OTA_InfoTypeDef))
/* OTA Version 字符串长度 */
#define OTA_VERSION_LEN         26

/* OTA_STATUS */
typedef enum {
    OTA_UPDATE       = 0x00000001,
    IAP_XMODEM_START = 0X00000002,
    IAP_XMODEM_DATA  = 0x00000004,
    OTA_SET_VERSION  = 0x00000008,
    UPDATE_FLASH     = 0X00000010,
    DOWNLOAD_FLASH   = 0x00000020,
    EXECUTE_FLASH    = 0x00000040 
} OTA_STATUS;

typedef struct
{
    uint32_t OTA_Flag;                  /* OTA 升级标志 */
    uint32_t FileSize[11];              /* OTA程序固件大小，对应于W25Q64中不同块中程序固件的长度 N号成员固定对应W25Q64中编码的N块
                                            0号成员对应于OTA大小 */   
    uint8_t OTA_Version[32];            /* OTA 版本  */
} OTA_InfoTypeDef;

typedef struct 
{
    uint8_t UpdateBuff[PAGE_SIZE];      /* OTA 升级缓冲区            */
    uint32_t W25Q64_BlockID;            /* OTA 升级外部Flash 块ID    */
    
    
    uint32_t XmodemTimer;               /* OTA Xmodem协议定时器      */
    uint32_t XmodemRecvID;              /* OTA Xmodem协议接收编号    */
    uint32_t XmodemCRC;                 /* OTA Xmodem CRC 校验码     */
} OTA_UpdateTypedef;




/* OTA 外部接口函数 */
void OTA_Init(void);                    /* OTA 初始化 */
/* OTA 设置标志位 */
void OTA_SetBootFlag(uint32_t OTA_FLAG);
void OTA_ClearBootFlag(uint32_t OTA_FLAG);
uint32_t OTA_GetBootFlagStatus(void);
/* OTA_InfoTypeDef 结构体外部接口 */
void OTA_SetOTAFlag(uint32_t OTA_FLAG);
uint32_t OTA_GetOTAFlag(void);
void OTA_SetOTAVersion(char *OTA_VERSION);
uint8_t *OTA_GetOTAVersion(void);
void OTA_SetOTAFilexSize(uint32_t FilexBlockID, uint32_t Value);
uint32_t OTA_GetOTAFilexSize(uint32_t FilexBlockID);
/* OTA_UpdateTypeDef 结构体外部接口 */
void OTA_SetUpdateBuff(uint32_t BuffID, void *Data, uint16_t Len);
uint8_t *OTA_GetUpdateBuff(uint8_t BuffID);
void OTA_SetW25Q64BlockID(uint32_t ID_Value);
uint32_t OTA_GetW25Q64BlockID(void);
/* Xmodem 协议相关 */
void OTA_XmodemTimerAdd(void);
void OTA_XmodemTimerSub(void);
void OTA_XmodemSetTimer(uint32_t Value);
uint32_t OTA_GetXmodemTimer(void);
void OTA_XmodemRecvIDAdd(void);
void OTA_XmodemRecvIDSub(void);
void OTA_XmodemSetRecvID(uint32_t Value);
uint32_t OTA_GetXmodemRecvID(void);
void OTA_XmodemSetCRC(uint16_t CRC_Res);
uint32_t OTA_XmodemGetCRC(void);



void AT24C02_ReadOTA(void);
void AT24C02_WriteOTA(void);









#endif




