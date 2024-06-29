#ifndef SPI_H
#define SPI_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x.h"

typedef enum 
{
    LEVEL_LOW = 0,
    LEVEL_HIGH
}IO_Level_t;

#define Spi2SetCS(X)  ((X == LEVEL_LOW) ? GPIO_ResetBits(GPIOB, GPIO_Pin_12) : GPIO_SetBits(GPIOB, GPIO_Pin_12))

extern void SPI2_Init(void);
extern uint8_t SPI_WirteOrRead_byte(uint8_t byte);

#endif /* SPI_H */

