#ifndef KEY_H
#define KEY_H

#include "stm32f10x_gpio.h"

typedef enum 
{
    KEY_IDLE = 0,
    KEY_S1 = 1,
    KEY_S2 = 2,
    KEY_S3 = 4,
    KEY_S4 = 8
}KEY_State_t;

extern void KEY_Init(void);
extern uint8_t KEY_Scan(void);

#endif /* KEY_H */

