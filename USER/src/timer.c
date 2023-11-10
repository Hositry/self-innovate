/**
  ******************************************************************************
  * @file    led.c 
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
#include "timer.h"
#include "misc.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t Timer6_Tick = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Timer6_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 定时器时钟外设时钟是使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /* 自动重装载寄存器周的值(计数值) */
    TIM_TimeBaseStructure.TIM_Period = 1000;
    /* PSC分频(71 + 1) */
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    /* 时钟分频因子 ，基本定时器没有，不用管 */
    //TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    /* 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置 */
    //TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    /* 重复计数器的值，基本定时器没有，不用管 */
    //TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    /* 初始化定时器 */
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    /* 清除计数器中断标志位 */
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    /* 开启计数器中断 */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能计数器 */
    TIM_Cmd(TIM6, ENABLE);
}

void TIM6_IRQHandler(void)
{
  if ( TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET ) {
    Timer6_Tick++;
    TIM_ClearITPendingBit(TIM6 , TIM_FLAG_Update);
  }
}

uint32_t TIM_GetTimer6Tick(void)
{
  return Timer6_Tick;
}

void TIM6_DelayMs(uint16_t ms)
{
  uint16_t preTick = TIM_GetTimer6Tick();

  while (TIM_GetTimer6Tick() < preTick + ms)
  {
    ;
  }
}


