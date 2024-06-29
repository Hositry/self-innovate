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
#include "uart.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include <stdarg.h>
#include <String.h>
#include <stdio.h>
#include "rtc.h"
#include <stdlib.h>
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART_RX_DATA_SIZE   512
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern  uint8_t USART_Rx_Buffer[];
extern uint32_t USART_Rx_ptr_in ;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Uart1_Init(uint32_t baudRate)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* GPIOA Init */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USARTx configured as follow:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - One Stop Bit
      - No parity
      - Hardware flow control disabled (RTS and CTS signals)
      - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = baudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* NVIC Config */
  NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  USART_Cmd(USART1, ENABLE);
}

void Uart2_Init(uint32_t baudRate)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* GPIOA Init */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USARTx configured as follow:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - One Stop Bit
      - No parity
      - Hardware flow control disabled (RTS and CTS signals)
      - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = baudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  /* 注意：第2个参数不能是组合 */
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

  /* NVIC Config */
  NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  USART_Cmd(USART2, ENABLE);
}

void USART1_IRQHandler()
{
#if(TARGET == 1)		
  uint8_t dat;
  if (USART_GetITStatus(USART1, USART_IT_RXNE))
  {
    dat = USART_ReceiveData(USART1);
    USART_SendData(USART1, dat);
  }
#endif	
  /* If overrun condition occurs, clear the ORE flag and recover communication */
  if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(USART1);
  }
}

void USART2_IRQHandler()
{
  uint8_t dat;

  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    dat = USART_ReceiveData(USART2);
    //USART_SendData(USART2, dat);
    USART_ClearFlag(USART2, USART_FLAG_RXNE);
  }
  /* when receive complete, release Semp */
  if (USART_GetITStatus(USART2, USART_IT_IDLE))
  {
    USART_ReceiveData(USART2);
    //USART_ClearFlag(USART2, USART_FLAG_IDLE);
  }
}

#if 1
#pragma import(__use_no_semihosting)
struct __FILE
{
  int handle;
};
FILE __stdout;

// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
  x = x;
}

/* 重定义fputc函数,定向到UART1 */
int fputc(int ch, FILE *f)
{
  /* 重定向到虚拟串口 */  
#if (TARGET == 2)
  USART_Rx_Buffer[USART_Rx_ptr_in] = (uint8_t)ch;
  USART_Rx_ptr_in++;
  /* To avoid buffer overflow */
  if(USART_Rx_ptr_in == USART_RX_DATA_SIZE)
  {
    USART_Rx_ptr_in = 0;
  }
#else  
/* 重定向到 串口1 */
  USART_SendData(USART1, (uint8_t)ch);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
    ;
  }
#endif

  return ch;
}


/* 重定义fputc函数,定向到UART2 
int fputc(int ch, FILE *f)
{
  USART_SendData(USART2, (uint8_t)ch);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {
    ;
  }
  return ch;
}
*/

void UART_SendDatas(uint8_t *pBuff, uint16_t len)
{
  uint16_t i;

  for(i = 0; i < len; i++)
  {
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {
      ;
    }
    USART_SendData(USART2, pBuff[i]);
  }
}

/**
 * @brief 处理串口命令
 * 
 */
void UART_CmdHandler(void)
{

}
#endif

