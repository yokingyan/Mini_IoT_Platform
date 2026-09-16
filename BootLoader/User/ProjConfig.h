#ifndef __PROJ_CONFIG_H__
#define __PROJ_CONFIG_H__


#include <stdint.h>

#ifndef FLASH_SADDR
#define FLASH_SADDR             0x08000000
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE               1024
#endif

#define PAGE_NUM                64                          //flash扇区总数量
#define PAGE_B_NUM              20                          //B区扇区数量
#define PAGE_A_NUM              (PAGE_NUM - PAGE_B_NUM)     //A区扇区数量
#define SPAGE_A                 PAGE_B_NUM                  //A区起始扇区编号
#define FLASH_SADDR_A           ( FLASH_SADDR + SPAGE_A * PAGE_SIZE )
#define OTA_UPDATE_FLAG         0x13131313
#define OTA_INFO_SIZE           (sizeof(OTA_InfoTypeDef))


typedef enum {
    OTA_UPDATE = 0x00000001,
    IAP_XMODEM_START = 0X00000002,
    IAP_XMODEM_DATA  = 0x00000004,
    OTA_SET_VERSION  = 0x00000008,
    UPDATE_FLASH     = 0X00000010,
    DOWNLOAD_FLASH   = 0x00000020,
    EXECUTE_FLASH    = 0x00000040 
} OTA_STATUS;


typedef struct
{
    uint32_t OTA_Flag;
    uint32_t FileLen[11];   //0号成员固定对应OTA大小 W25Q64中不同块中程序固件的长度，0号成员固定对应W25Q64中编码的0块，用于OTA
    uint8_t OTA_Version[32];
} OTA_InfoTypeDef;

typedef struct 
{
    uint8_t Updatabuff[PAGE_SIZE];
    uint32_t W25Q64_BlockID;
    uint32_t XmodemTimer;
    uint32_t XmodemRecvID;
    uint32_t XmodemCRC;
} OTA_UpdateTypedef;


extern OTA_InfoTypeDef OTA_InfoStructure;
extern OTA_UpdateTypedef OTA_UpdateStructure;
extern uint32_t OTA_BootSTAFlag;



#endif



