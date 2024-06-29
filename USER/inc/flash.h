#ifndef FLASH_H
#define FLASH_H

#include "stm32f10x_flash.h"

#define FLS_BOOT_SIZE         0x5000/* boot大小设置为20K，其余全部给APP */
#define FLS_TOTAL_SIZE        0x80000/* 总的FLASH大小为512K */
#define FLS_BASE_ADDR         0x08000000
#define FLS_SECTOR_SIZE       2048 
#define FLS_APP_BASE_ADDR     (FLS_BASE_ADDR + FLS_BOOT_SIZE)/* APP的基地址 */
#define FLS_WAIT_TIME         100000
/* 跳转地址为总FLASH容量-格式化后可用的U盘大小（512K-468K = 44K）实际看FLASH是0xC800 */
#define FLS_CODE_BASE_ADDR    0x0800c800

extern uint8_t FLS_Write(uint32_t startAddr, uint32_t *pData, uint16_t len);
extern uint8_t FLS_Read(uint32_t startAddr, uint32_t *pData, uint16_t len);

#endif /* FLASH_H */

