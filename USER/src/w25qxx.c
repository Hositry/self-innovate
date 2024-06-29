/**
 ******************************************************************************
 * @file    Project/STM32F10x_StdPeriph_Template/main.c
 * @author  MCD Application Team
 * @version V3.6.0
 * @date    20-September-2021
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2011 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "w25qxx.h"
#include "spi.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable      0x06
#define W25X_WriteDisable     0x04
#define W25X_ReadStatusReg    0x05
#define W25X_WriteStatusReg   0x01
#define W25X_ReadData         0x03
#define W25X_FastReadData     0x0B
#define W25X_FastReadDual     0x3B
#define W25X_PageProgram      0x02
#define W25X_BlockErase       0xD8
#define W25X_SectorErase      0x20
#define W25X_ChipErase        0xC7
#define W25X_PowerDown        0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID         0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID    0x9F
#define W25X_WIP_Flag         0x01 /* 忙标志 */
#define W25X_Dummy_Byte       0xFF
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t W25X_SectorBuff[4096];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  FLASH进入掉电模式
 * @param  None
 * @retval None
 */
void W25QXX_Powerdown(void)
{
  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_PowerDown);
  Spi2SetCS(LEVEL_HIGH);
}

/**
 * @brief  FLASH从掉电模式唤醒
 * @param  None
 * @retval None
 */
void W25QXX_Wakeup(void)
{
  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_ReleasePowerDown);
  Spi2SetCS(LEVEL_HIGH);
}

/**
 * @brief  读取FLASH器件ID
 * @param  None
 * @retval None
 */
uint32_t W25QXX_Read_Deviceid(void)
{
  uint32_t Temp = 0;

  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_DeviceID);
  SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  Temp = SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  Spi2SetCS(LEVEL_HIGH);

  return Temp;
}

/**
 * @brief  读取jedecid
 * @param  None
 * @retval None
 */
uint32_t W25QXX_Read_Jedecid(void)
{
  uint32_t temp = 0;
  uint8_t temp0 = 0, temp1 = 0, temp2 = 0;

  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_JedecDeviceID);
  temp0 = SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  temp1 = SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  temp2 = SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  Spi2SetCS(LEVEL_HIGH);
  temp = (temp0 << 16) | (temp1 << 8) | temp2;

  return temp;
}

/**
 * @brief  写使能函数
 * @param  None
 * @retval None
 */
static void W25QXX_Enable_Write(void)
{
  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_WriteEnable);
  Spi2SetCS(LEVEL_HIGH);
}

/**
 * @brief  flash判忙函数, 没超时退出保护
 * @param  None
 * @retval None
 */
static void W25QXX_WaitforWriteend(void)
{
  uint8_t flash_status = 0;

  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_ReadStatusReg);
  do
  {
    flash_status = SPI_WirteOrRead_byte(W25X_Dummy_Byte);
  } while ((flash_status & W25X_WIP_Flag) == SET);
  Spi2SetCS(LEVEL_HIGH);
}

/**
 * @brief  读取FALSH
 * @param  None
 * @retval None
 */
void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_ReadData);
  SPI_WirteOrRead_byte((ReadAddr & 0xFF0000) >> 16);
  SPI_WirteOrRead_byte((ReadAddr & 0xFF00) >> 8);
  SPI_WirteOrRead_byte(ReadAddr & 0xFF);
  while (NumByteToRead--)
  {
    *pBuffer = SPI_WirteOrRead_byte(W25X_Dummy_Byte);
    pBuffer++;
  }
  Spi2SetCS(LEVEL_HIGH);
}

/**
 * @brief  擦除(扇区)
 * @param  None
 * @retval None
 */
void W25QXX_EasrseSector(uint16_t num)
{
  uint32_t WriteAddr;

  WriteAddr = num * 4096;
  W25QXX_Enable_Write();
  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_SectorErase); // 扇区擦除指令
  SPI_WirteOrRead_byte(WriteAddr >> 16);  // 24~16地址
  SPI_WirteOrRead_byte(WriteAddr >> 8);   // 16~8地址
  SPI_WirteOrRead_byte(WriteAddr);        // 8~0地址
  Spi2SetCS(LEVEL_HIGH);
  W25QXX_WaitforWriteend();
}

/**
 * @brief  写入一页数据
 * @param  None
 * @retval None
 */
void W25QXX_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t i;

  W25QXX_Enable_Write();
  Spi2SetCS(LEVEL_LOW);
  SPI_WirteOrRead_byte(W25X_PageProgram); // 页编程指令
  SPI_WirteOrRead_byte(WriteAddr >> 16);  // 24~16地址
  SPI_WirteOrRead_byte(WriteAddr >> 8);   // 16~8地址
  SPI_WirteOrRead_byte(WriteAddr);        // 8~0地址
  for (i = 0; i < NumByteToWrite; i++)
  {
    SPI_WirteOrRead_byte(pBuffer[i]); // 8~0地址
  }
  Spi2SetCS(LEVEL_HIGH);
  W25QXX_WaitforWriteend();
}

/**
 * @brief  写入FALSH(不带擦除)
 * @param  None
 * @retval None
 */
void W25QXX_WriteWithoutEarse(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t pageremain = 0;

  /* 单页剩余的字节数 */
  pageremain = 256 - WriteAddr % 256;
  /* 写入的字节数小于一页，pageremain就等于要写入的字节数 */
  if (NumByteToWrite <= pageremain)
  {
    pageremain = NumByteToWrite;
  }
  while (1)
  {
    W25QXX_WritePage(pBuffer, WriteAddr, pageremain);
    /* 写入结束了，退出 */
    if (NumByteToWrite == pageremain)
    {
      break;
    }
    /* NumByteToWrite>pageremain */
    else
    {
      pBuffer += pageremain;
      WriteAddr += pageremain;
      /* 减去已经写入了的字节数 */
      NumByteToWrite -= pageremain;
      /* 一次可以写入256个字节 */
      if (NumByteToWrite > 256)
      {
        pageremain = 256;
      }
      /* 不够256个字节了 */
      else
      {
        pageremain = NumByteToWrite;
      }
    }
  }
}

/**
 * @brief  写入一扇区内容(带擦除)
 * @param  None
 * @retval None
 */
void W25QXX_WriteWithEarse(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint32_t secpos;
  uint16_t secoff;
  uint16_t secremain;
  uint16_t i;
  uint8_t *w25qxx_buf = W25X_SectorBuff;

  /* WriteAddr对应第几个扇区,相当于除4096 */
  secpos = WriteAddr >> 12;
  /* 在扇区内的偏移 */
  secoff = WriteAddr % 4096;
  /* 扇区剩余空间大小 */
  secremain = 4096 - secoff;

  /* 要写的字节数不超过本扇区 */
  if (NumByteToWrite <= secremain)
  {
    secremain = NumByteToWrite;
  }

  while (1)
  {
    /* 读出整个扇区的内容 */
    W25QXX_Read(w25qxx_buf, secpos << 12, 4096);
    /* 擦除检测 */
    for (i = 0; i < secremain; i++)
    {
      if (w25qxx_buf[secoff + i] != 0XFF)
      {
        /* 需要擦除 */
        break;
      }
    }
    /* 需要擦除 */
    if (i < secremain)
    {
      /* 擦除这个扇区 */
      W25QXX_EasrseSector(secpos);
      /* 擦除过程中把数据拷贝到缓存中 */
      for (i = 0; i < secremain; i++)
      {
        w25qxx_buf[i + secoff] = pBuffer[i];
      }
      /* 写回到整个扇区 */
      W25QXX_WriteWithoutEarse(w25qxx_buf, secpos << 12, 4096);
    }
    /* 已经擦除了, 直接写入扇区剩余区间 */
    else
    {
      W25QXX_WriteWithoutEarse(pBuffer, WriteAddr, secremain);
    }
    /* 写入结束了 */
    if (NumByteToWrite == secremain)
    {
      break;
    }
    /* 写入未结束 */
    else
    {
      /* 扇区地址增1 */
      secpos++;
      /* 偏移位置为0 */
      secoff = 0;
      /* 指针偏移 */
      pBuffer += secremain;
      /* 写地址偏移 */
      WriteAddr += secremain;
      /* 字节数递减 */
      NumByteToWrite -= secremain;
      /* 下一个扇区还是写不完 */
      if (NumByteToWrite > 4096)
      {
        secremain = 4096;
      }
      /* 下一个扇区可以写完了 */
      else
      {
        secremain = NumByteToWrite;
      }
    }
  }
}

/**
 * @brief  写入一页(带擦除)
 * @param  None
 * @retval None
 */
void W25QXX_WriteWithEarsePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint32_t pageNum = 0;
  uint16_t pageRemain = 0;
  /* 注意在操作系统中不能这样定义缓存 */
  uint8_t pageBuff[256];
  uint8_t pageStartAddr = 0;

  /* WriteAddr地址所在的页还剩多少字节 */
  pageRemain = 256 - WriteAddr % 256;
  /* WriteAddr地址所对应的页数 */
  pageNum = WriteAddr / 256;
  /* WriteAddr地址所对应的页开始位置 */
  pageStartAddr = WriteAddr % 256;
  
  /* 要写的内容超过本页 */
  if(NumByteToWrite > pageRemain)
  {
    /* 先写完本页 */
    W25QXX_Read(pageBuff, pageNum * 256, 256);
    W25QXX_EasrseSector(pageNum);
    memcpy(pageBuff + pageStartAddr, pBuffer, pageRemain);
    W25QXX_WritePage(pageBuff, pageNum * 256, 256);
    NumByteToWrite -= pageRemain;
  }
  /* 要写的内容不超过本页 */
  else
  {
    /* 读本页数据放到缓存中 */
    W25QXX_Read(pageBuff, pageNum * 256, 256);
    /* 擦除本页 */
    W25QXX_EasrseSector(pageNum);
    /* 将要写的数据放回缓存中对应的位置 */
    memcpy(pageBuff + pageStartAddr, pBuffer, NumByteToWrite);
    /* 写回本页 */
    W25QXX_WritePage(pageBuff, pageNum * 256, 256);
    NumByteToWrite = 0;
  }
}
