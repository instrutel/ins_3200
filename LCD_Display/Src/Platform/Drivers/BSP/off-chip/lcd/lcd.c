/**
  ******************************************************************************
  * @file    lcd.c
  * @author  Application Team
  * @brief   This file provides a set of functions needed to manage the LCD with MCP23017
  *          IO Expender devices.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/


#include "lcd.h"
#include "hw_config.h"
#include "GPIO/mss_gpio.h"

#define COMMAND  0
#define DATA     1

#define  LCD_ENABLE	   		HAL_GPIO_WritePin(LCD_EN_PORT,LCD_EN_LINE, 1);\
							HAL_GPIO_WritePin(LCD_EN_PORT,LCD_EN_LINE, 0)

#ifdef LCD_BL_LINE
#define  LCD_BL_ON            HAL_GPIO_WritePin(LCD_BL_LINE, 1)
#define  LCD_BL_OFF			  HAL_GPIO_WritePin(LCD_BL_LINE, 0)
#endif
#ifdef LCD_OP_DATA_LATCH_LINE
#define  OP_DATA_LATCH			HAL_GPIO_WritePin(LCD_OP_DATA_LATCH_LINE, 1);\
								HAL_GPIO_WritePin(LCD_OP_DATA_LATCH_LINE, 0)
#endif

#define  BIT_MASK(byte, bit) (byte&1<<bit)?1:0

/*
 *
 */

static void LCD_LL_Init(void);
static void LCD_IOConfiguration(void);
static void LCD_WriteByte(unsigned char f_CMD_BYTE, unsigned char f_DATA );
static void digoutbyte(unsigned char f_Byte);

/* @ Export Fnctions
 *
 */

// ##########################################    ##################################################################

// ##########################################    ################################################################## /

/* @ Brief : LCD Init
 * @ param : I2c Port
 * @ retval : HAL_Status  HAL_OK/HAL_ERROR
 */
void LCD_PortInit (void)
{
	LCD_IOConfiguration();
	LCD_LL_Init();
}

void LCD_Clear(void)
{
	LCD_WriteByte(COMMAND, 0x01 );
}


void LCD_Display_Char(unsigned char f_Byte)
{
	LCD_WriteByte(DATA,f_Byte);
}

void LCD_Goto(unsigned char f_Line_no, unsigned f_Position)
{
	switch(f_Line_no)
	{
		case 0:
			LCD_WriteByte(COMMAND, 0x80 +  f_Position);
			break;
		case 1:
			LCD_WriteByte(COMMAND, 0xC0 +  f_Position);
			break;
		case 2:
			LCD_WriteByte(COMMAND, 0x94 +  f_Position);
			break;
		case 3:
			LCD_WriteByte(COMMAND, 0xD4 +  f_Position);
			break;
	}
	HAL_Delay(4);

}

void LCD_Display_String(unsigned char *f_Str)
{
	while(*f_Str != '\0')
	{
		LCD_WriteByte(DATA,*f_Str );
		f_Str++;
		HAL_Delay(1);
	}
	HAL_Delay(2);
}

void LCD_Backlight(unsigned char f_cmd)
{
#ifdef LCD_BL_LINE
	if(f_cmd == TRUE)
	{
		LCD_BL_ON;
	}
	else
	{
		LCD_BL_OFF;
	}
#endif
}



/***
 * @brief   Initializes LCD with 8 data lines
 * @param   None
 * @retval  None
 */
static void LCD_IOConfiguration(void)
{
	MSS_GPIO_ConfigPin(LCD_EN_PORT,LCD_EN_LINE,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(LCD_RS_PORT,LCD_RS_LINE,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
#ifdef LCD_BL_LINE
	MSS_GPIO_ConfigPin(LCD_BL_LINE,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
#endif


	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_1,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_2,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_3,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_4,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);

	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_5,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_6,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_7,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	MSS_GPIO_ConfigPin(DISPLAY_DATA_PORT,DATA_LINE_8,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);

}

/*
 *
 */
static void digoutbyte(unsigned char f_Byte)
{
#ifdef LCD_DATA_PORT
	DISPLAY_DATA_PORT->ODR = DISPLAY_DATA_PORT->ODR&(~(0x00FF<<LCD_DATA_LSB_LINE))|(f_Byte<<LCD_DATA_LSB_LINE);
#else
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_1, BIT_MASK(f_Byte,0));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_2, BIT_MASK(f_Byte,1));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_3, BIT_MASK(f_Byte,2));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_4, BIT_MASK(f_Byte,3));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_5, BIT_MASK(f_Byte,4));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_6, BIT_MASK(f_Byte,5));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_7, BIT_MASK(f_Byte,6));
	HAL_GPIO_WritePin(DISPLAY_DATA_PORT,DATA_LINE_8, BIT_MASK(f_Byte,7));
#endif
	HAL_Delay(2);
}

static void LCD_WriteByte(unsigned char f_CMD_BYTE, unsigned char f_DATA )
{
	if(f_CMD_BYTE)
	{
		HAL_GPIO_WritePin(GPIOE,LCD_RS_LINE, SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,LCD_RS_LINE, RESET);
	}
	digoutbyte(f_DATA);
	LCD_ENABLE;
}


static void LCD_LL_Init(void)
{
	HAL_Delay(1000);
	LCD_WriteByte(COMMAND, 0x38 );
	HAL_Delay(100);
	LCD_WriteByte(COMMAND, 0x0C );
	HAL_Delay(100);
	LCD_WriteByte(COMMAND, 0x06 );
	HAL_Delay(100);
	LCD_WriteByte(COMMAND, 0x01 );
	HAL_Delay(800);
	LCD_WriteByte(COMMAND, 0X40 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x08 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X41 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x0C );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X42 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x0E );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X43 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x0F );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X44 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x0E );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X45 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x0C );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X46 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x08 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X47 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x00 );
	HAL_Delay(100);
	LCD_WriteByte(COMMAND, 0X48 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x01 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X49 );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x01 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X4A );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x05 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X4B );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x09 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X4C );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x1F );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X4D );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x08 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X4E );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x04 );
	HAL_Delay(10);
	LCD_WriteByte(COMMAND, 0X4F );
	HAL_Delay(10);
	LCD_WriteByte(DATA, 0x00 );
	HAL_Delay(10);
}
