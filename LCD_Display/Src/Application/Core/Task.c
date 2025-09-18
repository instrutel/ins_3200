/**
  ******************************************************************************
  * @file           : Task.c
  * @author			: Instrutel application team
  * @brief          : Core Task application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Private includes ----------------------------------------------------------*/
#include<stdio.h>
#include "sw_config.h"
#include "lcd.h"

#include "../../Utilities/Interface/bsp_if.h"
#include "../../Utilities/Misc/uart_helper.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU

/* Private variables ---------------------------------------------------------*/

volatile static uint8_t ChannelNum = 0;
static uint32_t pTimeOut  = 0;

#define UART_RECV_BUF_SIZE  32U
static uint8_t g_uart_to_debug[UART_RECV_BUF_SIZE];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
********  Instrutel SPI Application - LTC1867 ADC Example  ********\n\r\
******************************************************************************\n\r\
 This example project demonstrates how to use SPI to read data from the LTC1867 ADC\n\r\
 and display the results on an LCD screen.\n\r\
------------------------------------------------------------------------------\n\r\
******************************************************************************\n\r";

const char g_message2[] =
"\n\n\n\rSelect an option:\r\n\
----------------------------------------\r\n\
 Type 1  - Read a single ADC channel\r\n\
 Type 2  - Read all ADC channels\r\n\
 Type h  - Display the menu again\r\n";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static uint8_t get_data_from_uart(void);
void CoreApplicationSubTask(uint8_t cmd);
void CoreApplicationInit(void);


/* Private user code ---------------------------------------------------------*/
#define PRINT_STRING(X)     Uart_PutString((uint8_t*)X);
#define GET_STRING(X,Y)    	Uart_GetLine((uint8_t*)X,Y)

#define PRINT_CHAR(X)       Uart_PutChar((uint8_t)X);
#define GET_CHAR(X)    	    Uart_GetChar((uint8_t*)X)


/**
 * @brief  Perform early initialization of the board hardware.
 * @details This function initializes the basic hardware components of the board, such as peripherals clock
 *  configuration, peripherals setup and GPIO setup required before the main application starts.
 */
void BoardInit(void)
{
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	Uart_PortInit(USART1);

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);

	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("/*         LTC1867 ADC - Test Program                  */\n\r");
    PRINT_STRING("/*******************************************************/\n\r");

    PRINT_STRING(g_message2);
}


/**
 * @brief  Core task function for the application.
 * @details This function implements the core functionality of the application.
 * 		It typically handles the main control flow,processes inputs, updates system states,
 * 		and calls other tasks or functions as necessary.
 */
void Core_Task(void)
{
	uint16_t rx_bytes = 0; uint8_t rx_buff[2] ={0};
	static uint8_t ChnnlReadType = 2;

	/* Initialize the core application */
	CoreApplicationInit();
	/* Task entry point */
	while(1)
	{
		rx_bytes = GET_CHAR(rx_buff);
		if (rx_bytes > 0)
		{
			switch(rx_buff[0])
			{
				case '1':
					/* single Channel read */
					ChnnlReadType = 1;
					rx_bytes = get_data_from_uart();
					if(rx_bytes)  ChannelNum =  (g_uart_to_debug[0] - '0');
					pTimeOut  = HAL_GetTick() ;
					LCD_Clear();
					LCD_Goto(0,2); // line and pos
					LCD_Display_String((uint8_t*)"LCD TEST PROGRAM");
					break;
				case '2':
					/* read all the callen data */
					ChnnlReadType = 2; ChannelNum = 0;
					pTimeOut  = HAL_GetTick();
					LCD_Clear();
					LCD_Goto(0,2); // line and pos
					LCD_Display_String((uint8_t*)"LCD TEST PROGRAM");
					break;
				case 'h':
					PRINT_STRING(g_message2);
					break;
			}
			rx_bytes = 0;
		}
		CoreApplicationSubTask(ChnnlReadType);
	}
}


/**
  * @brief  Initialize the core application.
  * @details This function is responsible for initializing the main application components,
  *          setting up system-level modules, and preparing the environment for runtime tasks.
  * @retval None
  */
void CoreApplicationInit(void)
{

	BSP_IfInit();
	/* --- LCD Init --- */
	LCD_PortInit();
	LCD_Clear();
	LCD_Goto(0,2); // line and pos
	LCD_Display_String((uint8_t*)"LCD TEST PROGRAM");
}

/**
  * @brief  Execute a core application subtask based on command.
  * @details This function handles a specific subtask or operation within the core application,
  *          depending on the provided command code. Each command corresponds to a predefined
  *          functionality or feature within the application layer.
  * @param  cmd  The command identifier indicating which subtask to perform.
  * @retval None
  */
void CoreApplicationSubTask(uint8_t cmd)
{
	if(HAL_GetTick() > pTimeOut)
	{
		char TxBuff[30] ={0};
		switch(cmd)
		{
			case 2:
				PRINT_STRING("\n\r/********** MULTI CHANNEL READ MODE  **********/\r\n");
				/*  SUB task */
				for (uint8_t i = 1; i < 3 ; ++i )
				{
					g_uart_to_debug[0] = ChannelNum;
					BSP_IfRead(0, g_uart_to_debug, UART_RECV_BUF_SIZE);
					uint16_t adccount = ((uint16_t)g_uart_to_debug[0] << 8) | (uint8_t)g_uart_to_debug[1];
					sprintf(TxBuff, "CH-%d : %05u ", ChannelNum, adccount);
					LCD_Goto(i,2); // line and pos
					LCD_Display_String((uint8_t*)TxBuff);
					ChannelNum = (ChannelNum+1) % 8;
					PRINT_STRING(TxBuff);
				}
				pTimeOut = HAL_GetTick() + 5000;
				break;
			case 1:
				/*  SUB task */
				PRINT_STRING("\n\r/********** SINGLE CHANNEL READ MODE  **********/\r\n");
				g_uart_to_debug[0] = ChannelNum;
				BSP_IfRead(0, g_uart_to_debug, UART_RECV_BUF_SIZE);
				uint16_t adccount = ((uint16_t)g_uart_to_debug[0] << 8) | (uint8_t)g_uart_to_debug[1];
				sprintf(TxBuff, "CH-%d : %05u ", ChannelNum, adccount);
				LCD_Goto(1,2); // line and pos
				LCD_Display_String((uint8_t*)TxBuff); //clear the display port
				PRINT_STRING(TxBuff);
				pTimeOut = HAL_GetTick() + 1000;
				break;
		}
	}
}
#if 0
/**
  * @brief  Process raw ADC count and convert to scaled value.
  * @details This function takes a 16-bit raw ADC count and processes it
  *          into a scaled long value, suitable for application-level use.
  * @param  adc_count  The raw 16-bit ADC value.
  * @retval long       The processed or scaled result.
  */
long ProcessCountValue(uint16_t adc_count)
{
	unsigned char i;
	long slope, param_value = 0;

	uint8_t NumOfCalibPoints =  (sizeof(Def_4_20MA_CalibPoints)/sizeof(Calib_point_t));
	Calib_point_t *CalibPoints = (Calib_point_t *)Def_4_20MA_CalibPoints;

	for( i = 0; i < NumOfCalibPoints ; i++ )
	{
		if( (adc_count > CalibPoints[i].Count) && ((adc_count < CalibPoints[i+1].Count)) )
			break;
		else if( (adc_count < CalibPoints[i].Count) && ((adc_count > CalibPoints[i+1].Count)) )
			break;
	}
	if(i < (NumOfCalibPoints-1))
	{
		slope = ((CalibPoints[i].pValue -CalibPoints[i+1].pValue)*10000)/(CalibPoints[i].Count - CalibPoints[i+1].Count );
		param_value= slope*(adc_count-CalibPoints[i].Count) + CalibPoints[i].pValue *10000;
		param_value= param_value/10000;
	}
	return param_value;
}

#endif
/*------------------------------------------------------------------------------*/

/**
 * @brief  Retrieve one byte of data from the UART receive buffer store it in dest buffer.
 * @details This function reads bytes of data from the UART peripheral.
 * @return uint8_t  The byte of data received from the UART.
 */
static uint8_t get_data_from_uart(void)
{
    uint8_t complete = 0;
    uint8_t rx_buff[3] ={0};
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < 30u; count++)
    {
        g_uart_to_debug[count] = 0u;
    }

    PRINT_STRING("\r\n Enter the channel number(0 to 7) to read : ");

    count = 0u;
    while (!complete)
    {
        rx_size = GET_CHAR(rx_buff);
        if(rx_size > 0u)
        {
        	PRINT_CHAR( rx_buff[0] );
        	count++;
            if ( ENTER == rx_buff[0] ) {
                complete = 1u; count =0;
            } else if(rx_buff[0] >= '0' &&  rx_buff[0] <= '7'){
            	g_uart_to_debug[0] = rx_buff[0] ;
            	complete = 1u; count = 1;
            }
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u; count = 0;
            }
        }
    }
    return(count);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PE8 PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,0);

}
