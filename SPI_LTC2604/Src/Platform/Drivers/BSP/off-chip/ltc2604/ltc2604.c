/*********************************************************************************************************************
 * Copyrights : INSTRUTEL 2014 - 2025
 * Filename   : ltc2604.c
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

#include "ltc2604.h"


#define LTC2604_CMD_WRITE          0x00
#define LTC2604_CMD_UPDATE         0x10
#define LTC2604_CMD_WR_UPDATE_N    0x20
#define LTC2604_CMD_WR_UPDATE_ALL  0x30
#define LTC2604_CMD_POWER_DOWN     0x40
#define LTC2604_CMD_NOP            0xF0

#define  ltc_delay(X)      HAL_Delay(X)

/* @ Brief : Analog Outputcard init
 * @ param : SPI Port
 */
uint8_t LTC2604_Init()
{
	uint8_t  pStatus = HAL_OK;
//	pStatus = SPI_Init();
	return pStatus;
}

/* @Brief : DAC Card De-initialization
 * @param : IO_InterfaceHwConfig_t*
 * @retval:
 */
uint8_t LTC2604_Deinit()
{
	//--- Card Deinit
	return HAL_OK;
}

/* @ Brief : Read DAC Vout
 * @ param : channel number
 */
uint8_t LTC2604_ChannelWrte()
{
#if 0
	uint8_t Tx_Buffer[4]={0},RxBuffer[4]={0};
	HAL_StatusTypeDef pStatus = HAL_ERROR;

	/* TX Buffer Filling */
	Tx_Buffer[1] = ( LTC2604_CMD_WR_UPDATE_N |ChannelNum );
	Tx_Buffer[2] = (uint8_t )(DacValue >> 8 ); Tx_Buffer[3] = (uint8_t )(DacValue);

	//--- Read Adc Count
	DIGIO_WritePin(ChipSelect, 0);  //--- CS_ PIN HIGH
	ltc_delay(1);
	pStatus = SPI_Write_Read(&SPI_Transfer, Tx_Buffer, RxBuffer, 4, NULL); // --0xffff SPI Polling timeout time
	ltc_delay(1);
	DIGIO_WritePin(ChipSelect, 1);  //--- CS_ PIN LOW
	/* Delay For Channel ReadOut */
	ltc_delay(10);   // todo test
	/* -- Read ADC Count -- */
	DIGIO_WritePin(SPI_Transfer.ChipSelect, 0);  //--- CS_ PIN HIGH
	ltc_delay(1);
	pStatus = SPI_Write_Read(&SPI_Transfer, Tx_Buffer, RxBuffer, 4, NULL) ; // --0xffff SPI Polling timeout time
	ltc_delay(1);
	DIGIO_WritePin(SPI_Transfer.ChipSelect, 1);  //--- CS_ PIN LOW

	//--- ADC Reaponse Ok
	if( pStatus == HAL_OK )
	{
		*pBuff = ((RxBuffer[2] << 8) + RxBuffer[3]);
	}
#endif
	return pStatus;
}


