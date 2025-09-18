
/*********************************************************************************************************************
 * Copyrights : INSTRUTEL 2014 - 2025
 * Filename   : ltc1867.c
 * Created on : Mar 11, 2025
 * Author	  : Venkatesh
 * ProjectName: ins_3200
 * Processor  : STM32
 ********************************************************************************************************************/

/********************************************************************************************************************
  Description:
  Documentation:
 ********************************************************************************************************************/

/* includes ----------------------------------------------------------*/
#include "ltc1867.h"
#include "GPIO/mss_gpio.h"
#include "hw_config.h"


/*
 *
 */
const uint8_t ADC_SINGLE_ENDED_CHAN_COMMAND[] = { 0x84, 0xC4, 0x94, 0xD4, 0xA4, 0xE4, 0xB4, 0xF4 };
const uint8_t ADC_DIFFERENTIAL_CHAN_COMMAND[] = { 0x04, 0x00,0x14,0x00,   0x24, 0x00, 0x34 };

/* @@ SPI CHIP Selection @@
 */
#define CS_INIT()		MSS_GPIO_ConfigPin(SPI_CS_PORT, SPI_CS_GPIO_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL); \
						MSS_GPIO_WritePin(SPI_CS_PORT, SPI_CS_GPIO_PIN, GPIO_PIN_SET)

#define CS_ENABLE()  	MSS_GPIO_WritePin(SPI_CS_PORT, SPI_CS_GPIO_PIN, GPIO_PIN_RESET)

#define CS_DISABLE()    MSS_GPIO_WritePin(SPI_CS_PORT, SPI_CS_GPIO_PIN, GPIO_PIN_SET)


/* @ Brief : Analog Inputcard init
 * @ param : SPI Port
 */
uint8_t LTC1867_Init(SPI_HandleTypeDef *hspi ,SPI_TypeDef *spi)
{
	HAL_StatusTypeDef  pStatus = HAL_OK;
	/* SPI1 parameter configuration*/
	hspi->Instance = spi;
	hspi->Init.Mode = SPI_MODE_MASTER;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi->Init.NSS = SPI_NSS_SOFT;
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(hspi) != HAL_OK)
	{
		pStatus = HAL_ERROR;
	}
	/* Chip selection */
	CS_INIT();

	return pStatus;
}

/* @ Brief : Read ADC count
 * @ param : channel number
 */
uint8_t LTC1867_ChannelRead(SPI_HandleTypeDef* hspi,ChnlMode_e ChnnlMode,uint8_t ChannelNum,uint16_t *pBuff)
{
	uint8_t Tx_Buffer[3]={0},RxBuffer[3]={0};
	uint8_t ret = HAL_ERROR;
	//---- Channel Modes
	if( ChnnlMode == ADC_CHANNEL_SINGLE_ENDED )	{
		if(IS_VALID_SINGLE_ENDED_CHANNEL(ChannelNum) == 0){
			return HAL_ERROR;
		}
		Tx_Buffer[0] = ADC_SINGLE_ENDED_CHAN_COMMAND[ChannelNum];
		Tx_Buffer[1] = 0x00;
	}
	else{   // ----- diff  Channnel
		if(IS_VALID_DIFFERENTIAL_CHANNEL(ChannelNum) == 0)
				return HAL_ERROR;
		Tx_Buffer[0] = ADC_DIFFERENTIAL_CHAN_COMMAND[ChannelNum];
	}
	//--- Read Adc Count
	CS_ENABLE();  //--- CS_ PIN LOW
	for(uint8_t i =0 ; i < 5 ; i++);
	ret = HAL_SPI_TransmitReceive(hspi,Tx_Buffer,RxBuffer,2,0xffff); // --0xffff SPI Polling timeout time
	for(uint8_t i =0 ; i < 5 ; i++);
	CS_DISABLE();  //--- CS_ PIN HIGH
	//--- ADC Reaponse Ok
	if( ret == HAL_OK )
	{
		*pBuff = ((RxBuffer[0] << 8) + RxBuffer[1]);
	}
	return ret;
}

/* @ Brief : Read ADC count
 * @ param : channel number
 */
uint8_t LTC1867_ChannelContinuousRead(SPI_HandleTypeDef* hspi,ChnlMode_e ChnnlMode,uint8_t ChannelNum,uint16_t *pBuff)
{
	uint8_t Tx_Buffer[3]={0},RxBuffer[3]={0};
	uint8_t pStatus = HAL_ERROR;

	//---- Channel Modes
	if( ChnnlMode == ADC_CHANNEL_SINGLE_ENDED )	{
		if(IS_VALID_SINGLE_ENDED_CHANNEL(ChannelNum) == 0){
			return HAL_ERROR;
		}
		Tx_Buffer[0] = ADC_SINGLE_ENDED_CHAN_COMMAND[ChannelNum];
		Tx_Buffer[1] = 0x00;
	}
	else{   // ----- diff  Channnel
		if(IS_VALID_DIFFERENTIAL_CHANNEL(ChannelNum) == 0)
				return HAL_ERROR;
		Tx_Buffer[0] = ADC_DIFFERENTIAL_CHAN_COMMAND[ChannelNum];
	}
	//--- Read Adc Count
	CS_ENABLE();  //--- CS_ PIN LOW
	for(uint8_t i =0 ; i < 5 ; i++);
	pStatus = HAL_SPI_TransmitReceive(hspi,Tx_Buffer,RxBuffer,2,0xffff); // --0xffff SPI Polling timeout time
	for(uint8_t i =0 ; i < 5 ; i++);
	CS_DISABLE();  //--- CS_ PIN LOW

	/* Delay For Channel ReadOut */
	for(uint8_t i =0 ; i < 10 ; i++);
	/* -- Read ADC Count -- */
	CS_ENABLE();  //--- CS_ PIN LOW
	for(uint8_t i =0 ; i < 5 ; i++);
	pStatus = HAL_SPI_TransmitReceive(hspi,Tx_Buffer,RxBuffer,2,0xffff); // --0xffff SPI Polling timeout time
	for(uint8_t i =0 ; i < 5 ; i++);
	CS_DISABLE();  //--- CS_ PIN HIGH
	//--- ADC Reaponse Ok
	if( pStatus == HAL_OK )
	{
		*pBuff = ((RxBuffer[0] << 8) + RxBuffer[1]);
	}
	return pStatus;
}



uint8_t LTC1867_Deint(SPI_HandleTypeDef *hspi)
{
	//--- Card Deinit
	return HAL_OK;
}
