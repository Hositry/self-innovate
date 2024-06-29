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
#include "lcd.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_fsmc.h"
#include <stdio.h>
#include "timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* PA10为1是读写数据
 * 0X60000000->Bank1 
 * 0XC000000->FSMC_NE4
 * 0X800->PA10,（由于16位，AHDDR[0]不用, 因此1<<11u)
 */
#define LCD_RS_DATA   ((uint32_t)(0X60000000 + 0XC000000 + 0X800))
//#define LCD_RS_DATA   ((uint32_t)(0X60000000 + 0XC000000 + 0X400))
/* PA10为0是读写命令 */
#define LCD_RS_CMD    ((uint32_t)(0X60000000 + 0XC000000 + 0X000))
//#define LCD_RS_CMD    ((uint32_t)(0X60000000 + 0XC000000 + 0X000))
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LCD_Write_Cmd(uint8_t cmd)
{
  *(volatile uint16_t *)LCD_RS_CMD = cmd;
}

void LCD_Write_Data(uint16_t dat)
{
  *(volatile uint16_t *)LCD_RS_DATA = dat;
}

uint16_t LCD_Read_Data(void)
{
  return *(volatile uint16_t *)LCD_RS_DATA;
}

void LCD_Init(void)
{
	volatile uint16_t LcdDevId = 0;
	    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);   //Ê¹ÄÜFSMCÊ±ÖÓ
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE); //Ê¹ÄÜPORTB,D,E,GÒÔ¼°AFIO¸´ÓÃ¹¦ÄÜÊ±ÖÓ

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;           //PB0 ÍÆÍìÊä³ö ±³¹â
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PORTD¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;    //PORTD¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
     
    //PORTE¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; //PORTE¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //PORTG12¸´ÓÃÍÆÍìÊä³ö P0RTG0-->RS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;   //PORTD¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //¸´ÓÃÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure); 

    readWriteTiming.FSMC_AddressSetupTime = 0x01;   //µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª2¸öHCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;    //µØÖ·±£³ÖÊ±¼ä£¨ADDHLD£©Ä£Ê½AÎ´ÓÃµ½
    readWriteTiming.FSMC_DataSetupTime = 0x0f;      //Êý¾Ý±£´æÊ±¼äÎª16¸öHCLK,ÒòÎªÒº¾§Çý¶¯ICµÄ¶ÁÊý¾ÝµÄÊ±ºò£¬ËÙ¶È²»ÄÜÌ«¿ì£¬ÓÈÆä¶Ô1289Õâ¸öIC¡£
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;    //Ä£Ê½A 


    writeTiming.FSMC_AddressSetupTime = 0x00;   //µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª1¸öHCLK
    writeTiming.FSMC_AddressHoldTime = 0x00;    //µØÖ·±£³ÖÊ±¼ä£¨ADDHLD£©Ä£Ê½AÎ´ÓÃµ½
    writeTiming.FSMC_DataSetupTime = 0x03;      //Êý¾Ý±£´æÊ±¼äÎª4¸öHCLK
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;    //Ä£Ê½A 


    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;                      //ÕâÀïÎÒÃÇÊ¹ÓÃNE4£¬Ò²¾Í¶ÔÓ¦BTCR[6],[7]¡£
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;    //²»¸´ÓÃÊý¾ÝµØÖ·
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;                //FSMC_MemoryType_SRAM;  SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;      //´æ´¢Æ÷Êý¾Ý¿í¶ÈÎª16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;   //FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;     //´æ´¢Æ÷Ð´Ê¹ÄÜ
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;         //¶ÁÐ´Ê¹ÓÃ²»Í¬µÄÊ±Ðò
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;        //¶ÁÐ´Ê±Ðò
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                //Ð´Ê±Ðò

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //³õÊ¼»¯FSMCÅäÖÃ

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  //Ê¹ÄÜBANK1 
        
    TIM6_DelayMs(50);       // delay 50 ms
    
    //³¢ÊÔ9341 IDµÄ¶ÁÈ¡
    LCD_Write_Cmd(0XD3);
    LcdDevId = LCD_Read_Data();          //dummy read
    LcdDevId = LCD_Read_Data();          //¶Áµ½0X00
    LcdDevId= LCD_Read_Data();          //¶ÁÈ¡0X93
    LcdDevId <<= 8;
    LcdDevId |= LCD_Read_Data();         //¶ÁÈ¡0X41
    
    if (LcdDevId != 0X9341)            //²»ÊÇ 9341 , ³¢ÊÔ¿´¿´ÊÇ²»ÊÇ ST7789
    {
        LCD_Write_Cmd(0X04);
        LcdDevId = LCD_Read_Data();      //dummy read
        LcdDevId= LCD_Read_Data();      //¶Áµ½0X85
        LcdDevId = LCD_Read_Data();      //¶ÁÈ¡0X85
        LcdDevId <<= 8;
        LcdDevId |= LCD_Read_Data();     //¶ÁÈ¡0X52
	}
#if 0	
  GPIO_InitTypeDef GPIO_InitStructure;
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readTiming; 
  FSMC_NORSRAMTimingInitTypeDef  writeTiming;
  volatile uint16_t LcdDevId = 0;

  /* 使能FSMC对应相应管脚时钟 
   * PG12->FSMC_NE4(LCD_CS)
   * PD5->FSMC_NEW(LCD_WR)
   * PD4->FSMC_NOE(LCD_RD)
   * PB0->LCD_BL_EN
   * PG0->FSMC_A10(LCD_RS)命令数据选择
   * PD14->FSMC_D0(LCD_D0)
   * PD15->FSMC_D1(LCD_D1)
   * PD0->FSMC_D2(LCD_D2)
   * PD1->FSMC_D3(LCD_D3)
   * PE7->FSMC_D4(LCD_D4)
   * PE8->FSMC_D5(LCD_D5)
   * PE9->FSMC_D6(LCD_D6)
   * PE10->FSMC_D7(LCD_D7)
   * PE11->FSMC_D8(LCD_D8)
   * PE12->FSMC_D9(LCD_D9)
   * PE13->FSMC_D10(LCD_D10)
   * PE14->FSMC_D11(LCD_D11)
   * PE15->FSMC_D12(LCD_D12)
   * PD8->FSMC_D13(LCD_D13)
   * PD9->FSMC_D14(LCD_D14)
   * PD10->FSMC_D15(LCD_D15)
   */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE );
  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE);

  /* 配置FSMC相对应的数据线,FSMC-D0~D15 */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_5 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* 配置FSMC相对应的控制线背光 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOG, GPIO_Pin_12);

  /* 配置TFT背光 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* 点亮背光 */
  GPIO_SetBits(GPIOB, GPIO_Pin_0);

  /* 使能FSMC时钟*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );
  /* 地址建立时间（ADDSET）为1个HCLK 2/72M=28ns,由于FSMC性能问题，实际 RD 高电平大于 200ns */
	readTiming.FSMC_AddressSetupTime      = 1;	 
	/* 数据保持时间（DATAST）+ 1个HCLK = 16/72M=16*13.8ns=220ns	*/
	readTiming.FSMC_DataSetupTime         = 15;
  /*选择控制的模式，模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配 */
	readTiming.FSMC_AccessMode            = FSMC_AccessMode_A;	
  /*以下配置与模式B无关， 地址保持时间（ADDHLD），模式A未用到 */
	readTiming.FSMC_AddressHoldTime       = 0x00;
	/* 设置总线转换周期，仅用于复用模式的NOR操作，使用SRAM模式，这里不用管 */
	readTiming.FSMC_BusTurnAroundDuration = 0x00;
	/* 设置时钟分频，仅用于同步类型的存储器，模式A是异步不用管 */
	readTiming.FSMC_CLKDivision           = 0x00;
	/* 数据保持时间，仅用于同步型的NOR，模式A是异步不用管 */
	readTiming.FSMC_DataLatency           = 0x00;

  /* 然后 ADDSET（也存在性能问题）设置为 0， 即 1个HCLK 周期，实际WR高电平时间大于 100ns */
  writeTiming.FSMC_AddressSetupTime = 0;
  /* 里设置 DATAST为3，即4个HCLK周期，时间约为55ns（因为9320 等控制器，这个时间要求比较长，要50ns） */
  writeTiming.FSMC_DataSetupTime = 3;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;
  writeTiming.FSMC_AddressHoldTime = 0x00;
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;

  /* 使用Bank1的4区 */
  FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAM4;
  /* 不复用数据地址 */
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
  /* MTYP[1:0]：存储器类型 */
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
  /* 启用或禁用闪存的突发存取模式，仅对同步突发闪存有效 */
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
  /* 指定等待信号极性，仅在访问时有效闪存处于突发模式有效 */
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
  /* 寄存器写使能 */
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
  /* 扩展模式使能位，也就是是否允许读写不同的时序， 读比写慢，读写一样失序会慢 */
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable;
  /* 写使能位。我们需要向TFTLCD 写数据，故该位必须设置为1 */
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &writeTiming;  
	FSMC_NORSRAMInit ( &FSMC_NORSRAMInitStructure );
  /* MBKEN：存储块使能位 */
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAM4, ENABLE );

  TIM6_DelayMs(50);
  LCD_Write_Cmd(0x0000);
  LCD_Write_Data(0x0001);
  TIM6_DelayMs(50);
  LCD_Write_Cmd(0x0000);
  TIM6_DelayMs(2);
  LcdDevId = LCD_Read_Data();
  printf("0: 0x%x\r\n", LcdDevId);

  /* 0XD3，读ID4指令 */
  LCD_Write_Cmd(0xD3);
  LcdDevId = LCD_Read_Data();                        /* 返回的第一个参数空 */
  printf("1: 0x%x\r\n", LcdDevId);
  LcdDevId = LCD_Read_Data();                        /* 返回的第二个参数0X00 */
  printf("2: 0x%x\r\n", LcdDevId);
  LcdDevId = LCD_Read_Data() & 0xFF;       /* 返回的第三个参数0X93 */
  printf("3: 0x%x\r\n", LcdDevId);
  LcdDevId = LCD_Read_Data() & 0xFFu;    /* 返回的第四个参数0X41 */
  printf("4: 0x%x\r\n", LcdDevId);

  /* IL9341寄存器初始化，需要配置哪些寄存器，需要设置什么值与液晶厂家生产环境密切相关，
   * 所以这些参数由厂家提供，不同厂家可能不同。也可以根据ILI9341芯片手册内容参考修改 */
#if 1
  /*  功耗控制B(CFh)  */
  LCD_Write_Cmd(0xCF);
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x81);
  LCD_Write_Data(0x30);

  /*  电源时序控制  */
	LCD_Write_Cmd(0xED);
	LCD_Write_Data(0x64);
	LCD_Write_Data(0x03);
	LCD_Write_Data(0x12);
	LCD_Write_Data(0x81);

  /*  驱动时序控制A  */
	LCD_Write_Cmd(0xE8);
	LCD_Write_Data(0x85);
	LCD_Write_Data(0x10);
	LCD_Write_Data(0x7A);

  /*  功耗控制A  */
	LCD_Write_Cmd(0xCB);
	LCD_Write_Data(0x39);
	LCD_Write_Data(0x2C);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x34);
	LCD_Write_Data(0x02);

  /*  泵比控制  */
	LCD_Write_Cmd(0xF7);
	LCD_Write_Data(0x20);

  /*  驱动时序控制B  */
	LCD_Write_Cmd(0xEA);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);

  LCD_Write_Cmd(0xC0); //Power control
  LCD_Write_Data(0x1B); //VRH[5:0]

  LCD_Write_Cmd(0xC1); //Power control
  LCD_Write_Data(0x01); //BT[2:0]

  LCD_Write_Cmd(0xC5); //VCOM control
  LCD_Write_Data(0x30);
  LCD_Write_Data(0x30);

  LCD_Write_Cmd(0xC7); //VCOM control2
  LCD_Write_Data(0XB7);

  /* 存储访问控制，这里我们主要关注：MY、MX、MV这三个位，通过这三个位的设置，我们可以控制整个ILI9341的全部扫描方向。这里设置从左到右，从上到下 */
  LCD_Write_Cmd(0x36); 
  LCD_Write_Data(0x08);

  LCD_Write_Cmd(0x3A);
  LCD_Write_Data(0x55);

  LCD_Write_Cmd(0xB1);
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x1A);

  LCD_Write_Cmd(0xB6); // Display Function Control
  LCD_Write_Data(0x0A);
  LCD_Write_Data(0xA2);
  
  LCD_Write_Cmd(0xF2); // 3Gamma Function Disable
  LCD_Write_Data(0x00);

  LCD_Write_Cmd(0x26); //Gamma curve selected
  LCD_Write_Data(0x01);

  LCD_Write_Cmd(0xE0); //Set Gamma
  LCD_Write_Data(0x0F);
  LCD_Write_Data(0x2A);
  LCD_Write_Data(0x28);
  LCD_Write_Data(0x08);
  LCD_Write_Data(0x0E);
  LCD_Write_Data(0x08);
  LCD_Write_Data(0x54);
  LCD_Write_Data(0XA9);
  LCD_Write_Data(0x43);
  LCD_Write_Data(0x0A);
  LCD_Write_Data(0x0F);
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x00);

  LCD_Write_Cmd(0XE1); //Set Gamma
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x15);
  LCD_Write_Data(0x17);
  LCD_Write_Data(0x07);
  LCD_Write_Data(0x11);
  LCD_Write_Data(0x06);
  LCD_Write_Data(0x2B);
  LCD_Write_Data(0x56);
  LCD_Write_Data(0x3C);
  LCD_Write_Data(0x05);
  LCD_Write_Data(0x10);
  LCD_Write_Data(0x0F);
  LCD_Write_Data(0x3F);
  LCD_Write_Data(0x3F);
  LCD_Write_Data(0x0F);

  /* 0X2A，列地址设置指令，在从左到右，从上到下的扫描方式（默认）时，该指令用于设置横坐标（x 坐标） */
  LCD_Write_Cmd(0x2A); 
  /* 起始地址，0 */
  LCD_Write_Data(0x00); //注意，起始地址必须 比 末地址 小！
  LCD_Write_Data(0x00); //并且，地址要小于239或319，否则大出的部分将被忽略
  /* 末地址， 239 */
  LCD_Write_Data(0x00);
  LCD_Write_Data(0xef);

  /* 0X2B，页地址设置指令，在从左到右，从上到下的扫描方式（默认）时，该指令用于设置纵坐标（y 坐标 */
  LCD_Write_Cmd(0x2B);
  /* 起始地址，0 */
  LCD_Write_Data(0x00);
  LCD_Write_Data(0x00);
  /* 末地址， 319 */
  LCD_Write_Data(0x01);
  LCD_Write_Data(0x3f);

  LCD_Write_Cmd(0x11); //Exit Sleep
  TIM6_DelayMs(50);
  LCD_Write_Cmd(0x29); //display on
#endif
#endif
}


