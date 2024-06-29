#ifndef W25QXX_H
#define W25QXX_H

#include "stm32f10x.h"

extern void W25QXX_Powerdown(void);
extern void W25QXX_Wakeup(void);
extern uint32_t W25QXX_Read_Deviceid(void);
extern uint32_t W25QXX_Read_Jedecid(void);
extern void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
extern void W25QXX_EasrseSector(uint16_t num);
extern void W25QXX_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
extern void W25QXX_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
extern void W25QXX_WriteWithEarse(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

#endif /* W25QXX_H */

