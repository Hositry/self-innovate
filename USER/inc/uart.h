#ifndef UART_H
#define UART_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

extern QueueHandle_t QueueUartRev;
extern QueueHandle_t UartRevCmpSemp;
extern void Uart1_Init(uint32_t baudRate);
extern void Uart2_Init(uint32_t baudRate);
extern void UART_SendDatas(uint8_t *pBuff, uint16_t len);
extern void UART_CmdHandler(void);

#endif /* UART_H */

