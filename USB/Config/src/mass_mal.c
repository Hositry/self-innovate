/**
  ******************************************************************************
  * @file    mass_mal.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Medium Access Layer interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "mass_mal.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Mass_Memory_Size[2];
uint32_t Mass_Block_Size[2];
uint32_t Mass_Block_Count[2];
__IO uint32_t Status = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : MAL_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Init(uint8_t lun)
{
  uint16_t status = MAL_OK;

  /* 统一都当外部Flash处理 
  W25QXX_Wakeup();
  if(W25QXX_Read_Deviceid() == 0x00000016)
  {
    status = MAL_OK;
  }
  else
  {
    status = MAL_FAIL;
  }
  */
  /* 内部FLASH无需初始化，为编程预先解锁 */
  FLASH_Unlock(); 
  status = MAL_OK;

  return status;
}
/*******************************************************************************
* Function Name  : MAL_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
  uint16_t i;
  FLASH_Status sts;
  /* 统一都当作外部FALSH处理，写一扇区内容 
  W25QXX_WriteWithEarse((uint8_t*)Writebuff, Memory_Offset, Transfer_Length);
  if(i < 10)
  {
    addr[i] = Memory_Offset;
    len[i] = Transfer_Length;
    i++;
  }
  */

  for (i = 0; i < Transfer_Length; i += FLS_SECTOR_SIZE)
  {
    if (FLASH_WaitForLastOperation(FLS_WAIT_TIME) != FLASH_TIMEOUT)
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    }
    do
    {
      sts = FLASH_ErasePage(FLS_APP_BASE_ADDR + Memory_Offset + i);
      if (sts == FLASH_TIMEOUT)
      {
        return MAL_FAIL;
      }
    } while (FLASH_COMPLETE != sts);
  }
  
  for (i = 0; i < Transfer_Length; i += 4)
  {
    if (FLASH_WaitForLastOperation(FLS_WAIT_TIME) != FLASH_TIMEOUT)
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    }
    do
    {
      sts = FLASH_ProgramWord(FLS_APP_BASE_ADDR + Memory_Offset + i, Writebuff[i >> 2]);
      if (sts == FLASH_TIMEOUT)
      {
        return MAL_FAIL;
      }
      
    } while (FLASH_COMPLETE != sts);
  }

  return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
  uint16_t i;

  //W25QXX_Read((uint8_t*)Readbuff, Memory_Offset, Transfer_Length);
  for (i = 0; i < Transfer_Length; i += 4)
  {
    Readbuff[i >> 2] = ((vu32 *)(FLS_APP_BASE_ADDR + Memory_Offset))[i >> 2];
  }

  return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : Get status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_GetStatus (uint8_t lun)
{
  #if 0
		if(W25QXX_Read_Deviceid() == 0x00000016)
    {
			//一个扇区为4096，一个块有16个扇区，W25Q128共有128个块
			Mass_Block_Size[0]  = 4096;
			Mass_Block_Count[0] = 2048;
      //U盘的容量大小
			Mass_Memory_Size[0] = Mass_Block_Size[0]*Mass_Block_Count[0];
      return MAL_OK;
    }
  #else
    //一个扇区为2K
    Mass_Block_Size[0]  = FLS_SECTOR_SIZE;
    Mass_Block_Count[0] = 246;
    Mass_Memory_Size[0] = Mass_Block_Size[0] * Mass_Block_Count[0];
  #endif
	return MAL_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

