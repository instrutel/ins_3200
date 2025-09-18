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
void MSS_GPIO_ConfigAltrPin_PP(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t AF_PORT);
void MSS_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint8_t Val);
uint8_t MSS_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN);
uint8_t MSS_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);


#endif /* PLATFORM_DRIVERS_BSP_MSS_GPIO_MSS_GPIO_H_ */
