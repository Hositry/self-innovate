#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x_tim.h"

extern void Timer6_Init(void);
extern void TIM6_IRQHandler(void);
extern void TIM6_DelayMs(uint16_t ms);
extern uint32_t TIM_GetTimer6Tick(void);

#endif /* TIMER_H */

