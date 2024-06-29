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
#include "rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "misc.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  * @param  None
  * @retval None
  */
void RTC_Init(void)
{
    uint32_t tmp = 0xffffffff; 
    NVIC_InitTypeDef NVIC_InitStructure;
    RTC_DateTimeTypeDef_t time;

    time.year = 2023;
    time.month = 10;
    time.date = 8;
    time.week = 7;
    time.hour = 8;
    time.minute = 1;
    time.second = 0;
    /* 电源接口时钟和备份时钟开启 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* 使能后备寄存器访问 */
    PWR_BackupAccessCmd(ENABLE);
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        /* 开启LSE时钟 */
        RCC_LSEConfig(RCC_LSE_ON);
        /* 检查指定的RCC标志位设置与否,等待低速晶振就绪 */
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	
        {
          tmp--;
        }
		    if(tmp == 0)
        {
            /* LSE开启失败 */
            return;	
        }	 
        /* LSE作为RTC时钟源 */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        /* rtc时钟开启 */
        RCC_RTCCLKCmd(ENABLE);
        /* 设置分频，1Hz的频率 */
        RTC_SetPrescaler(32767);
        /* 等待写RTC寄存器完成, 该位指示是否操作完成 */
        RTC_WaitForLastTask();
        // 设置中断来源, RTC秒中断目前不使用
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);需要打开重新验证
        NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn ;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        /* 启用RTC秒中断 */
        //RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
        RTC_SetTime(time);
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
}

/**
 * @brief 普通闰年：公历年份是4的倍数，且不是100的倍数的，为闰年（如2004年、2020年等就是闰年）。
 *        世纪闰年：公历年份是整百数的，必须是400的倍数才是闰年（如1900年不是闰年，2000年是闰年）。
 * 
 * @param year 
 * @return uint8_t 
 */
uint8_t Rtc_IsLeapYear(uint16_t year)
{
  if ( (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) )
  {
    return 1; //是闰年
  }
  return 0;   //是平年
}

/*
 *  函数功能:根据具体日期得到星期
 *  吉姆拉尔森公式  week=(date+2*month+3*(month+1)/5+year+year/4-y/100+y/400)%7
 *  注 : 把一月 二月看成是上一年的13 14月    ,    得到结果 0 -- 6
 * @ CSDN Tyrion.Mon
 */
void Rtc_GetWeek(RTC_DateTimeTypeDef_t *time)
{
    u16 YY = 0;
    u8 MM = 0;

    if (time->month == 1 || time->month == 2)
    {
      MM = time->month + 12;
      YY = time->year - 1;
    }
    else
    {
      MM = time->month;
      YY = time->year;
    }
    time->week = ( (time->date + 2 * MM + 3 * (MM + 1) / 5 + YY + YY / 4 - YY / 100 + YY / 400) % 7 ) + 1;
}

/**
 * @brief  时间戳转换为普通时间
 * @note
 * @param  None
 * @retval None
 * @author PWH
 * @ CSDN Tyrion.Mon
 */
void RTC_TimestampToNormalTime(RTC_DateTimeTypeDef_t *time, uint32_t Timestamp)
{

    uint16_t year = 1970;
    //随着年份迭加，Counter记录从1970 年 1 月 1 日（00:00:00 GMT）到累加到的年份的最后一天的秒数
    uint32_t Counter = 0, CounterTemp = 0; 
    uint8_t Month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t i;

    //假设今天为2018年某一天，则时间戳应小于等于1970-1-1 0:0:0 到 2018-12-31 23:59:59的总秒数
    while (Counter <= Timestamp)    
    {
        //CounterTemp记录完全1970-1-1 0:0:0 到 2017-12-31 23:59:59的总秒数后退出循环
        CounterTemp = Counter;			
        //加上今年（平年）的秒数, 365Day*24H*60Min*60Sec=31536000
        Counter += 31536000; 
        if (Rtc_IsLeapYear(year))
        {
          //闰年多加一天, 24H*60Min*60Sec=86400
          Counter += 86400; 
        }
        year++;
    }
    //跳出循环即表示到达计数值当前年
    time->year = year - 1; 
    Month[1] = (Rtc_IsLeapYear(time->year) ? 29 : 28);
    //Counter = Timestamp - CounterTemp  记录2018年已走的总秒数
    Counter = Timestamp - CounterTemp; 
    //CounterTemp = Counter/(24*3600)  记录2018年已【过去】天数
    CounterTemp = Counter / 86400;     
    //记录今天已走的总秒数   
    Counter -= CounterTemp * 86400;      
    //时, 得到今天的小时
    time->hour = Counter / 3600; 
    //分
    time->minute = Counter % 3600 / 60; 
    //秒
    time->second = Counter % 60; 

    for (i = 0; i < 12; i++)
    {
      //不能包含相等的情况，相等会导致最后一天切换到下一个月第一天时
      if (CounterTemp < Month[i])    									
      {
          //（即CounterTemp已走天数刚好为n个月完整天数相加时（31+28+31...）），
          time->month = i + 1;			  									// 月份不加1，日期溢出（如：出现32号）
          time->date = CounterTemp + 1; 								//应不作处理，CounterTemp = Month[i] = 31时，会继续循环，月份加一，
          break;																				//日期变为1，刚好符合实际日期
      }
      CounterTemp -= Month[i];
    }
    Rtc_GetWeek(time);
}

RTC_DateTimeTypeDef_t RTC_GetTime(void)
{
  RTC_DateTimeTypeDef_t time;
  volatile uint32_t Timestamp = 0;

  /* 读RTC_CNT/RTC_ALR/RTC_PRL前需要等待该位置1 */
  RTC_WaitForSynchro();
  Timestamp = RTC_GetCounter();
  RTC_TimestampToNormalTime(&time, Timestamp);
  return time;
}

/**
 * @brief  普通时间转换为时间戳
 * @note
 * @param  None
 * @retval None
 * @author PWH
 * @ CSDN Tyrion.Mon
 */
uint32_t RTC_NorTimeToTimestamp(RTC_DateTimeTypeDef_t time)     
{
    if (0 >= (int) (time.month -= 2)){    /* 1..12 -> 11,12,1..10 */
         time.month += 12;      /**//* Puts Feb last since it has leap day */
         time.year -= 1;
    }
 
    return (((
             (unsigned long) (time.year/4 - time.year/100 + time.year/400 + 367*time.month/12 + time.date) +
             time.year*365 - 719499
          )*24 + time.hour /**//* now have hours */
       )*60 + time.minute /**//* now have minutes */
    )*60 + time.second; /**//* finally seconds */
}

/**
 * @brief  星期可传任意值
 * @note
 * @param  None
 * @retval None
 * @author PWH
 * @ CSDN Tyrion.Mon
 */
void RTC_SetTime(RTC_DateTimeTypeDef_t time)
{
  RTC_SetCounter(RTC_NorTimeToTimestamp(time));
  /* 等待写RTC寄存器完成, 该位指示是否操作完成 */
  RTC_WaitForLastTask();
}
/**
  * @}
  */


