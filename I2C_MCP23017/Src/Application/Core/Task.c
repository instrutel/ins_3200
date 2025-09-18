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

#include "../../Utilities/Interface/bsp_if.h"
#include "../../Utilities/Misc/uart_helper.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define ENTER           	0x0DU

#define MCP_PORT_A			1
#define MCP_PORT_B			2

/* Private variables ---------------------------------------------------------*/

#define UART_RECV_BUF_SIZE  120U
static uint8_t g_uart_to_debug[UART_RECV_BUF_SIZE];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
******* [Instrutel] I2C Application MCP-23017 IO Expander Example  ***********\n\r\
******************************************************************************\n\r";

const char g_message2[] = "\n\n\n\rSelect option: \r\n \
 Type 1  Set IO-Expander GPIOs  HIGH \n\r \
 Type 2  Set IO-Expander GPIOs  LOW  \n\r \
 Type h  Display the menu options \n\r";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static uint8_t get_data_from_uart(void);
void CoreApplicationSubTask(void);

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
	/* debug port init */
	Uart_PortInit(USART1);

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r****  MCP23X17 16-bit GPIO Expander Test Program  ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

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
	uint8_t rx_bytes = 0; uint8_t rx_buff[2];
	uint8_t Outputs = 0xff;
    /* --  BSP Interface Init -- */
	PRINT_STRING("\r\n Initializing the IO-Exp : INPUT  card at 0x07 addr\r\n");
	PRINT_STRING("\r\n Initializing the IO-Exp : OUTPUT card at 0x00 addr\r\n");

	if( BSP_IfInit() != 0 )  {
	    PRINT_STRING("\n\r Initializing the IO-Exp : ERR\n\r");
	}else  {
		 PRINT_STRING("\n\r Initializing the IO-Exp : OK\n\r");
	}
	/* Task entry point */
	while(1)
	{
		rx_bytes = GET_CHAR(rx_buff);
		if (rx_bytes > 0)
		{
			switch(rx_buff[0])
			{
				case '1':

					PRINT_STRING("\r\n/**** Setting High IO-Expander DATA *****/\r\n");
					Outputs = 0xff;
					BSP_IfWrite(MCP_PORT_A, &Outputs, 0);
					BSP_IfWrite(MCP_PORT_B, &Outputs, 0);
					break;
				case '2':
					PRINT_STRING("\r\n/**** Setting Low IO-Expander DATA *****/\r\n");
					Outputs = 0x00;
					BSP_IfWrite(MCP_PORT_A, &Outputs, 0);
					BSP_IfWrite(MCP_PORT_B, &Outputs, 0);
					break;
				case 'h':
					PRINT_STRING(g_message2);
					break;
			}
			rx_bytes = 0;
		}
		CoreApplicationSubTask();
	}
}

/**
  * @brief  Execute a core application subtask based on command.
  * @details This function handles a specific subtask or operation within the core application,
  *          depending on the provided command code. Each command corresponds to a predefined
  *          functionality or feature within the application layer.
  * @param  cmd  The command identifier indicating which subtask to perform.
  * @retval None
  */
void CoreApplicationSubTask(void)
{
	static uint32_t pTimeOut = 0;
	if(HAL_GetTick() > pTimeOut)
	{
		uint8_t tx_buff[5] ={0};
		BSP_IfRead(0,tx_buff,0);

		sprintf((char*)g_uart_to_debug,
		        "\r\n/**** MCP GPIO PORT STATUS ****/\r\n"
		        " GPIO Input  - PORTA: 0x%02X  PORTB: 0x%02X\r"
		        " GPIO Output - PORTA: 0x%02X  PORTB: 0x%02X\r\n",
		        tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3]);

		PRINT_STRING(g_uart_to_debug);
		pTimeOut = HAL_GetTick() + 1000;
	}
}


/*------------------------------------------------------------------------------*/

/**
 * @brief  Retrieve one byte of data from the UART receive buffer store it in dest buffer.
 * @details This function reads bytes of data from the UART peripheral.
 * @return uint8_t  The byte of data received from the UART.
 */
static uint8_t get_data_from_uart(void)
{
    uint8_t complete = 0;
    uint8_t rx_buff[2] ={0};
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < UART_RECV_BUF_SIZE; count++)
    {
        g_uart_to_debug[count] = 0u;
    }

    PRINT_STRING("\r\n Enter the data to send :\r\n");

    count = 0u;
    while (!complete)
    {
        rx_size = GET_CHAR(rx_buff);
        if(rx_size > 0u)
        {
        	PRINT_CHAR( rx_buff[0] );

            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_to_debug[count++] = rx_buff[0];
            }
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u; count--;
            }
        }
    }
    g_uart_to_debug[count] = 0x00;
    PRINT_STRING(g_uart_to_debug);
	PRINT_STRING( "\r\n" );
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
