/*
 * mss_gpio.c
 *
 *  Created on: Feb 9, 2025
 *      Author: Admin
 */
#include "../MSS/GPIO/mss_gpio.h"

/*
 *
 */
void MSS_GPIO_ConfigPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t pull)
{
	GPIO_InitTypeDef GPIO_Init={0};
	GPIO_Init.Pin = (uint32_t)GPIO_PIN;
	GPIO_Init.Mode = mode;
	GPIO_Init.Pull = pull;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOx,&GPIO_Init);
}

/*
 *
 */
void MSS_GPIO_ConfigAltrPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t AF_PORT)
{
	GPIO_InitTypeDef GPIO_Init={0};
	GPIO_Init.Pin = GPIO_PIN;
	GPIO_Init.Mode = mode;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Alternate = AF_PORT;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOx,&GPIO_Init);
}

/*
 *
 */
void MSS_GPIO_ConfigAltrPin_PP(GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN,uint32_t mode,uint32_t AF_PORT)
{
	GPIO_InitTypeDef GPIO_Init={0};
	GPIO_Init.Pin = GPIO_PIN;
	GPIO_Init.Mode = mode;
	GPIO_Init.Pull = GPIO_PULLUP;
	GPIO_Init.Alternate = AF_PORT;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOx,&GPIO_Init);
}

/*
 *
 */
void MSS_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin,uint8_t PinState)
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}
/*
 *
 */
uint8_t MSS_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}
/*
 *
 */
uint8_t MSS_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}


