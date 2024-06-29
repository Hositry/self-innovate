#ifndef RTC_H
#define RTC_H

#include "stm32f10x.h"

typedef struct RTC_DateTimeTypeDef
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;                 
    //公历日月年周
    uint16_t year;
    uint8_t  month;
    uint8_t  date;
    uint8_t  week;            
}RTC_DateTimeTypeDef_t;

extern void RTC_Init(void);
extern RTC_DateTimeTypeDef_t RTC_GetTime(void);
extern void RTC_SetTime(RTC_DateTimeTypeDef_t time);

#endif /* RTC_H */

