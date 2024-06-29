#ifndef LED_H
#define LED_H

#include "stm32f10x_gpio.h"

typedef enum 
{
    LED_OFF = 0,
    LED_ON
}LED_State_t;

#define Led2Set(X)  ((X == LED_ON) ? GPIO_ResetBits(GPIOE, GPIO_Pin_5) : GPIO_SetBits(GPIOE, GPIO_Pin_5))
#define Led3Set(X)  ((X == LED_ON) ? GPIO_ResetBits(GPIOB, GPIO_Pin_5) : GPIO_SetBits(GPIOB, GPIO_Pin_5))

extern void LED_Init(void);

#endif /* LED_H */

