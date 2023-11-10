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
#include "stm32f10x.h"
#include <stdio.h>
#include "core_cm3.h"
#include "timer.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "key.h"
#include "flash.h"
/*
#include "spi.h"
#include "w25qxx.h"
#include "led.h"
#include "rtc.h"
#include "uart.h"
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BOOT_WAIT_TIME  3000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//RTC_DateTimeTypeDef_t time;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void JumpToApp(void)
{
  uint32_t i = 0;
  void (*SysMemBootJump)(void);        /* 声明一个函数指针 */
  __IO uint32_t AppStartAddr = FLS_CODE_BASE_ADDR; /* APP的起始地址 */

  /* 关闭全局中断,__disable_irq也是关所有可屏蔽的中断 */
  //__disable_irq();
  __set_PRIMASK(1); 

  /* 关闭滴答定时器，复位到默认值 */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* 设置所有时钟到默认状态，使用HSI时钟 */
  RCC_DeInit();

  /* 关闭所有中断，清除所有中断挂起标志 */
  for (i = 0; i < 8; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  /* 使能全局中断，不太可能在跳转前打开全局中断, 应该在跳转后打开 */
  //ENABLE_INT();

  /* 跳转到app，首地址是MSP，地址+4是复位中断服务程序地址 */
  SysMemBootJump = (void (*)(void))(*((uint32_t *)(AppStartAddr + 4)));

  /* 设置主堆栈指针 */
  __set_MSP(*(uint32_t *)AppStartAddr);

  /* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
  __set_CONTROL(0);

  /* 跳转到系统BootLoader */
  SysMemBootJump();

  /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
	while (1)
  {
  }
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  uint8_t key;
  static uint8_t lastKey = KEY_IDLE;
  static uint32_t sysTick;
  static uint8_t bootWaitFlag = 0;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
  Timer6_Init();
  KEY_Init();
  /* USB Init */
  Set_System();//PA11（D+）和PA12 (D-) 的GPIO初始化，看到有的教程是要打开复用时钟RCC_APB2Periph_AFIO
  Set_USBClock();//开启USB时钟和设置分频
  USB_Interrupts_Config();//使能USB接收中断和唤醒中断
  USB_Init();
  sysTick = TIM_GetTimer6Tick();

  while (1)
  {
    key = KEY_Scan();
    /* 按下S1，等待BOOT升级 */
    if( ((key & KEY_S1) == KEY_S1) && ((lastKey & KEY_S1) == 0) )
    {
      bootWaitFlag = 1;
    }
    /* 3s后仍没有按下按键按下, 跳转至APP */
    if (bootWaitFlag == 0)
    {
      if (TIM_GetTimer6Tick() - sysTick > BOOT_WAIT_TIME)
      {
        JumpToApp();
      }
    }
		lastKey = key;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


