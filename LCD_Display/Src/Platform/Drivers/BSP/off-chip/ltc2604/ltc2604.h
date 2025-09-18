/*********************************************************************************************************************
 * Copyrights : INSTRUTEL- 2014 - 2025
 * Filename   : ltc2604.h
 * Created on : 04-Jan-2025
 * Author	  : Venkatesh
 * ProjectName: ins_3200
 * Processor  : STM32
 ********************************************************************************************************************/

/********************************************************************************************************************
  Description:
  Documentation:
 ********************************************************************************************************************/

#ifndef PLATFORM_DRIVERS_BSP_OFF_CHIP_LTC2604_LTC2604_H_
#define PLATFORM_DRIVERS_BSP_OFF_CHIP_LTC2604_LTC2604_H_


/* includes ----------------------------------------------------------*/

#include "stm32f4xx_hal.h"

uint8_t LTC2604_Init(void);
uint8_t LTC2604_Deinit();
uint8_t LTC2604_ChannelWrte();


#endif /* DEV_LIB_INCLUDES_DEV_LTC2604_H_ */


#endif /* PLATFORM_DRIVERS_BSP_OFF_CHIP_LTC2604_LTC2604_H_ */
