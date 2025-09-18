/*
 * ltc1867.h
 *
 *  Created on: Mar 11, 2025
 *      Author: Admin
 */

#ifndef PLATFORM_DRIVERS_BSP_OFF_CHIP_LTC1867_LTC1867_H_
#define PLATFORM_DRIVERS_BSP_OFF_CHIP_LTC1867_LTC1867_H_

#include<stdint.h>
#include"stm32f4xx_hal.h"


/* ######################### LTC 1863/1867 CHANNEL CONFIG CMD  #######################

    |SD | OS |S1 |S0 |COM| UNI| SLP| ---- > channel Read CMD

	SD = SINGLE/DIFFERENTIAL BIT
	OS = ODD/SIGN BIT
	S1 = ADDRESS SELECT BIT 1
	S0 = ADDRESS SELECT BIT 0
	COM = CH7/COM CONFIGURATION BIT
	UNI = UNIPOLAR/BIPOLAR BIT
	SLP = SLEEP MODE BIT
*/

#define  MAX_CARDS_IN_PORT1            3
#define  IS_VALID_CARD_NUMBER(CARD)   ((CARD >= 1)&&(CARD <= MAX_CARDS_IN_PORT1))

#define  IS_VALID_ADC_CHANNEL_MODE(MODE)  ( MODE >=0 && MODE <= 2 )

#define IS_VALID_SINGLE_ENDED_CHANNEL(CH) ((CH >= 0) && (CH <= 7) )
#define IS_VALID_DIFFERENTIAL_CHANNEL(CH) ((CH == 0)||(CH == 2)||(CH == 4)|| ( CH == 6))


typedef enum {
	ADC_CHANNEL_DIFFERENTIAL,
	ADC_CHANNEL_SINGLE_ENDED,
	ADC_CHANNEL_COMMN_ENDED
}ChnlMode_e;

uint8_t LTC1867_Init(SPI_HandleTypeDef *hspi ,SPI_TypeDef *spi);
uint8_t LTC1867_Deint(SPI_HandleTypeDef *hspi);
uint8_t LTC1867_ChannelRead(SPI_HandleTypeDef *hspi,ChnlMode_e chmode,uint8_t ChnlNum,uint16_t *pdata);
uint8_t LTC1867_ChannelContinuousRead(SPI_HandleTypeDef *hspi,ChnlMode_e ChMode,uint8_t ChnlNum,uint16_t *pBuff);


#endif /* PLATFORM_DRIVERS_BSP_OFF_CHIP_LTC1867_LTC1867_H_ */
