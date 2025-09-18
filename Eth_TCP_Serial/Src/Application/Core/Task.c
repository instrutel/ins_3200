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
#include<string.h>
#include "../MSS/UART/mss_uart.h"
#include "lwip.h"
#include "../TCP/lwip_tcp.h"
#include "sw_config.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define   ENTER               0x0DU

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart;
#define UART_RECV_BUF_SIZE  32U
static uint8_t g_uart_to_tcp[UART_RECV_BUF_SIZE];

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
********  Instrutel Ethernet Application - TCP to Serial Example  *********\n\r\
******************************************************************************\n\r\
 This example project demonstrates Ethernet-based TCP-to-Serial communication.\n\r\
------------------------------------------------------------------------------\n\r\
 - Reads data from UART1 and transmits it as a TCP packet on port 25.\n\r\
 - Receives TCP packets on port 25 and forwards the data to UART1.\n\r\
******************************************************************************\n\r";


const char g_message2[] =
#if TCP_SERVER
"\n\n\n\rSelect an option:\r\n\
----------------------------------------------\r\n\
 Type 2 - Enter data to transmit as a TCP packet\r\n\
 Type 3 - Close the tcp connection \n\r\
 Type h - Display this menu again\r\n";
#else
"\n\n\n\rSelect an option:\r\n\
----------------------------------------------\r\n\
 Type 1 - Initialize TCP Client port\r\n\
 Type 2 - Enter data to transmit as a TCP packet\r\n\
 Type 3 - Close the tcp connection \n\r\
 Type h - Display this menu again\r\n";
#endif

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
void process_packet(uint8_t *data,uint16_t len);
static uint8_t get_data_from_uart(void);
void CoreApplicationSubTask(void);

/* Private user code ---------------------------------------------------------*/
#define PRINT_STRING(X)     MSS_UART_Transmit(&huart,(uint8_t*)X);
#define GET_STRING(X)    	MSS_UART_Receive(&huart,(uint8_t*)X)

/**
 * @brief  Perform early initialization of the board hardware.
 * @details This function initializes the basic hardware components of the board, such as peripherals clock
 *  configuration, peripherals setup and GPIO setup required before the main application starts.
 */
void BoardInit(void)
{
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MSS_UART_Init(&huart,USART1,115200);
	MX_LWIP_Init();

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);
	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r****  Ethernet TCP to SERIAL Test program  ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

    /* print eth ip settings */
    char info_string[50];
    sprintf(info_string, "\n\rIP: %s\n\r", MY_STATIC_IP_ADDRESS);
    PRINT_STRING(info_string);
    sprintf(info_string, "GW: %s\n\r", MY_STATIC_IP_GATEWAY);
    PRINT_STRING(info_string);
    sprintf(info_string, "NM: %s\n\r", MY_STATIC_IP_MASK);
    PRINT_STRING(info_string);

    sprintf(info_string, "\n\rDest-IP: %s\n\r", DEST_IP_ADDRESS);
    PRINT_STRING(info_string);

#if TCP_SERVER
    sprintf(info_string, "\r\nTCP Server Listening Port: %d ", DEST_SERVER_PORT);
    tcp_server_init(DEST_SERVER_PORT, &process_packet);
#else
    sprintf(info_string, "tcp client connecting Port: %d\n\r", DEST_CLIENT_PORT);
#endif
    PRINT_STRING(info_string);
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

	/* Task entry point */
	while(1)
	{
		MX_LWIP_Process();
		rx_bytes = GET_STRING(rx_buff);
		if (rx_bytes > 0)
		{
			switch(rx_buff[0])
			{
				case '1':
#if TCP_SERVER
//					if( tcp_server_init(DEST_SERVER_PORT, &process_packet) == 0) {
//						PRINT_STRING("\r\ntcp server port listening...\r\n");
//					}	else{
//						PRINT_STRING("\r\ntcp server port init failed\r\n");
//					}
#else
					if( tcp_client_connect(DEST_CLIENT_PORT, &process_packet) == 0 ) {
						PRINT_STRING("\r\ntcp client Port initialization...\r\n");
					}	else{
						PRINT_STRING("\r\ntcp client Port Init Failed\r\n");
					}
#endif
					break;
				case '2':
					rx_bytes = get_data_from_uart();
#if TCP_SERVER
    				if( tcp_server_send(g_uart_to_tcp, rx_bytes) != 0) {
#else
    				if( tcp_client_send(g_uart_to_tcp, rx_bytes) != 0){
#endif
						PRINT_STRING("\r\ndata packet :sending failed\r\n");
					} else {
						PRINT_STRING("\r\ndata packet :send Ok\r\n");
					}
					break;
				case '3':
#if TCP_SERVER
					tcp_server_close();
#else
					tcp_client_close();
#endif
					PRINT_STRING("\r\ntcp connection closed\r\n");
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

#define TX_PACKET_TIMEOUT 5000

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
	static uint32_t PacketTimeout_l = 0 ;
	static uint16_t PacketCnt = 1;
	if( HAL_GetTick() > PacketTimeout_l )
	{
		PacketCnt++;
		uint16_t size = sprintf((char*)g_uart_to_tcp,"Tx Message Count : %d",PacketCnt);
#if TCP_SERVER
		tcp_server_send(g_uart_to_tcp, size);
#else
		tcp_client_send(g_uart_to_tcp, size);
#endif
		PacketTimeout_l = HAL_GetTick() + TX_PACKET_TIMEOUT;
	}
}

/**
 * @brief  A sample handler function that processes the received Ethernet packet.
 * @details This function is called after a packet is received , it simply prints
 * 		the data for demonstration purposes.
 * @param  data  Pointer to the received packet data.
 * @param  len   Length of the received packet.
 */
void process_packet(uint8_t *data,uint16_t PayloadLen)
{
	uint8_t lbuf[100] ={0}; uint8_t size = PayloadLen;
	if(PayloadLen > 99 ) size  = 98;
	memcpy(lbuf,data,size);
	PRINT_STRING("\n\r[Rx]: ");
	PRINT_STRING(lbuf);
	size = sprintf((char*)lbuf," Len:%d\r\n",PayloadLen);
	PRINT_STRING(lbuf);
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

    for (count = 0u; count < UART_RECV_BUF_SIZE ; count++)
    {
        g_uart_to_tcp[count] = 0u;
    }

    PRINT_STRING("\r\n Enter the data of 32 bytes: ");

    count = 0u;
    while (!complete)
    {
        rx_size = GET_STRING(rx_buff);
        if(rx_size > 0u)
        {
            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_to_tcp[count++] = rx_buff[0];
            }
            if (UART_RECV_BUF_SIZE == count) {
            	complete = 1u; count -= 1;
            }
        }
    }

    g_uart_to_tcp[count] = 0x00;
//    PRINT_STRING(g_uart_to_tcp);
//	PRINT_STRING( "\r\n" );
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
