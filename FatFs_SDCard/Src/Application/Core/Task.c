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
#include "../MSS/UART/mss_uart.h"
#include "sw_config.h"
#include "fatfs.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU
#define UART_RECV_BUF_SIZE 32

/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;
UART_HandleTypeDef huart;
static char g_uart_recv[UART_RECV_BUF_SIZE+1] ;

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
******* Instrutel Ethernet Application TCP to Serial Example  ***********\n\r\
******************************************************************************\n\r\
 Example project demonstrates the use of Ethernet TCP-to-Serial Transmission and Reception \n\r\
------------------------------------------------------------------------------\n\r\
 Reads data from UART1 and transmits it as a tcp packet from port 25 \n\r\
------------------------------------------------------------------------------\n\r\
 Receives the tcp packet on port 25 and forwards the data to UART1. \n\r\
******************************************************************************\n\r";

const uint8_t g_menu_msg[] = "\n\n\n\rSelect option: \r\n\
        1) Mount \r\n\
        2) Create FATFS File System \r\n\
        3) Open Directory \r\n\
        4) Open File \r\n\
        5) Print first file name of the directory \r\n\
        6) Write into the file \r\n\
        7) Read from the file \r\n\
        8) Close File \r\n\
        9) Unmount \r\n";

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
void recv_packet(uint8_t *data,uint16_t len);
static uint8_t get_data_from_uart(void);

/* Private user code ---------------------------------------------------------*/
#define PRINT_STRING(X)     HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,0); \
							MSS_UART_Transmit(&huart,(uint8_t*)X); \
							HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,1)

#define GET_STRING(X)    	MSS_UART_Receive(&huart,(uint8_t*)X)

/**
  * @brief : Board Initialization
  * @param : None
  * @retval: int
  */
void BoardInit(void)
{
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MSS_UART_Init(&huart,USART1,115200);
	MX_SDIO_SD_Init();
	FATFS_Init();

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r *************  SDCard-FatFs Test program  *************\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");
}


/* @brief : Core application main task
 * @param : None
 * @retval: None
 */
void Core_Task(void)
{
	uint16_t rx_bytes = 0; uint8_t rx_buff[2] ={0};

	/* -- FatFs Application --- */
	FATFS_Application();
	/* Task entry point */
	while(1)
	{
		rx_bytes = GET_STRING(rx_buff);
		if (rx_bytes > 0)
		{
			switch(rx_buff[0])
			{
				case '1':
					break;
				case '2':
					break;
				case '3':
					break;
				case 'h':
					PRINT_STRING(rx_buff[0]);
					break;
			}
			rx_bytes = 0;
		}
	}
}

/*@Brief : Data receive packets handler
 *@param : Received Data packet
 *@param : Data length
 */
void recv_packet(uint8_t *data,uint16_t len)
{
	uint8_t lbuf[100] ={0};
	sprintf((char*)lbuf, "\n\rRecvd: %s size:%d\r\n",data,len);
	PRINT_STRING(lbuf);
}

/*------------------------------------------------------------------------------
  Receive the data from UART
 */
static uint8_t get_data_from_uart(void)
{
    uint8_t complete = 0;
    uint8_t rx_buff[2] ={0};
    uint8_t count = 0;
    uint8_t rx_size = 0;

    /* Clear the uart rec buffer */
    for (count = 0u; count < UART_RECV_BUF_SIZE ; count++) {
    	g_uart_recv[count] = 0u;
    }
    PRINT_STRING("\r\n Enter the data to send over tcp\r\n");
    while (!complete)
    {
        rx_size = GET_STRING(rx_buff);
        if(rx_size > 0u)
        {
//        	PRINT_STRING( rx_buff );

            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_recv[count++] = rx_buff[0];
            }
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u;
            }
        }
    }
    return(count);
}

/*
 *
 */
void SerialPortDebug(char*str)
{
	MSS_UART_Transmit(&huart,"\r\n");
	MSS_UART_Transmit(&huart,(uint8_t*)str);
}


/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_4B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

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
