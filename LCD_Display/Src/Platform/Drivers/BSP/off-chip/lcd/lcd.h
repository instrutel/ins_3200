/**
  ******************************************************************************
  * @file    lcd.h
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


#ifndef PLATFORM_DRIVERS_BSP_OFF_CHIP_LCD_LCD_H_
#define PLATFORM_DRIVERS_BSP_OFF_CHIP_LCD_LCD_H_

/* Includes ------------------------------------------------------------------*/
#include<stdint.h>

void LCD_PortInit(void);
void LCD_Display_Char(unsigned char f_Byte);
void LCD_Display_String(unsigned char *f_Str);
void LCD_Goto(unsigned char f_Line_no, unsigned f_Position);
void LCD_Backlight(unsigned char f_cmd);
void LCD_Clear(void);

#endif /* PLATFORM_DRIVERS_BSP_OFF_CHIP_LCD_LCD_H_ */
