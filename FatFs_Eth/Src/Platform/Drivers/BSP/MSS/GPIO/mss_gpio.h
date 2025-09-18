/*
 * mss_gpio.h
 *
 *  Created on: Feb 9, 2025
 *      Author: Admin
 */

#ifndef PLATFORM_DRIVERS_BSP_MSS_GPIO_MSS_GPIO_H_
#define PLATFORM_DRIVERS_BSP_MSS_GPIO_MSS_GPIO_H_

#include<stdint.h>
#include "stm32f4xx_hal.h"


void MSS_GPIO_ConfigPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t AF_PORT);
void MSS_GPIO_ConfigAltrPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t AF_PORT);
void HAL_GPIO_ConfigAltrPin_PP(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t AF_PORT);


#endif /* PLATFORM_DRIVERS_BSP_MSS_GPIO_MSS_GPIO_H_ */
