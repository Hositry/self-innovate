/**
 ******************************************************************************
 * @file    flash.c
 * @author
 * @version V1.0.0
 * @date    20-September-2021
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief 
 * 
 * @param startAddr：从0开始
 * @param pData：数据
 * @param len 
 * @return uint8_t 0：失败 1：成功
 */
uint8_t FLS_Write(uint32_t startAddr, uint32_t *pData, uint16_t len)
{
    FLASH_Status sts;
    /* 扇区编号 */
    uint32_t secNo = startAddr / FLS_SECTOR_SIZE;
    /* 扇区首地址 */
    uint32_t secStartAddress = secNo * FLS_SECTOR_SIZE;
    uint16_t i;

    /* 范围检查 */
    if ( (FLS_APP_BASE_ADDR + startAddr + len * 4) >= (FLS_BASE_ADDR + FLS_TOTAL_SIZE) )
    {
        return FLASH_ERROR_PG;
    }

    FLASH_Unlock(); 
    do
    {
        sts = FLASH_ErasePage(FLS_APP_BASE_ADDR + secStartAddress);
        if(sts == FLASH_TIMEOUT)
        {
            /* 超时退出 */
            break;
        }
    } while (sts != FLASH_COMPLETE);
    
    if (sts == FLASH_COMPLETE)
    {
        for (i = 0; i < len; i++)
        {
            do
            {
                sts = FLASH_ProgramWord(FLS_APP_BASE_ADDR + startAddr + i * 4, pData[i]);
                if(sts == FLASH_TIMEOUT)
                {
                    /* 超时退出 */
                    break;
                }
            } while (sts != FLASH_COMPLETE);
        }   
    }
    FLASH_Lock();

    return sts;
}

/**
 * @brief 
 * 
 * @param startAddr 
 * @param pData 
 * @param len 
 */
uint8_t FLS_Read(uint32_t startAddr, uint32_t *pData, uint16_t len)
{
  uint16_t i;

  /* 范围检查 */
  if ( (FLS_APP_BASE_ADDR + startAddr + len * 4) >= (FLS_BASE_ADDR + FLS_TOTAL_SIZE) )
  {
    return FLASH_ERROR_PG;
  }

  for(i = 0; i < len; i++)
  {
    pData[i] = *(__IO uint32_t *) (FLS_APP_BASE_ADDR + startAddr + i * 4);
  }

  return FLASH_COMPLETE;
}