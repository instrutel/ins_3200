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
#include "lwip.h"
#include "lwip_udp.h"
#include "sw_config.h"
#include "../MSS/UART/mss_uart.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define   ENTER               0x0DU
#define   TX_PACKET_TIMEOUT   2000U
#define   TX_PACKET			  "HELLO WORLD..!"
#define   TX_PACKET_SIZE      14
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart;

/******************************************************************************
 *  Greeting messages displayed over the UART.
 */
const char g_greeting_msg[] =
"\n\r******************************************************************************\n\r\
******* Instrutel Ethernet Application UDP to serial Example  ***********\n\r\
******************************************************************************\n\r\
 Example project demonstrates the use of Ethernet UDP-Serial Transmission and Reception \n\r\
------------------------------------------------------------------------------\n\r\
 Reads data from UART1 and transmits it as a UDP packet from port 25 \n\r\
------------------------------------------------------------------------------\n\r\
 Receives the UDP packet on port 23 and forwards the data to UART1. \n\r\
******************************************************************************\n\r";

const char g_message2[] =
"\r\nType 1  Enter the data to transmit data-gram packet \r\n\
 Type 2  Close the UDP connection \r\n\
 Type 3  Re-Connect the UDP port \r\n\
 Type h  Display the menu options \r\n\
";

#define UART_RECV_BUF_SIZE  32U
static uint8_t g_uart_to_udp[UART_RECV_BUF_SIZE];

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
	MX_GPIO_Init();
	MSS_UART_Init(&huart,USART1,115200);
	/* Initialize all configured peripherals */
	MX_LWIP_Init();

	/* Display greeting message */
	PRINT_STRING((const char*)g_greeting_msg);

	PRINT_STRING("\n\r/*******************************************************/\n\r");
    PRINT_STRING("\n\r ****    Ethernet UDP to serial Test program    ****\n\r");
    PRINT_STRING("\n\r/*******************************************************/\n\r");

    /* Print eth ip setting */
    char info_string[40]; /* truncate to leave right amount of space padding */
    sprintf(info_string, "IP: %s\n\r", MY_STATIC_IP_ADDRESS);
    PRINT_STRING(info_string);
    sprintf(info_string, "GW: %s\n\r", MY_STATIC_IP_GATEWAY);
    PRINT_STRING(info_string);
    sprintf(info_string, "NM: %s\n\r", MY_STATIC_IP_MASK);
    PRINT_STRING(info_string);

    sprintf(info_string, "\n\rDest-IP: %s\n\r", DEST_IP_ADDRESS);
    PRINT_STRING(info_string);

#if UDP_SERVER
    sprintf(info_string, "UDP SERVER Listening local Port:%d\n\r", LOCAL_PORT);
#else
    sprintf(info_string, "\r\nUDP local-Port:%d Dest-port:%d\n\r", LOCAL_PORT,DEST_CLIENT_PORT);
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
#if UDP_SERVER
	PRINT_STRING("\r\nInitializing UDP server...");
	udp_server_init(LOCAL_PORT, &process_packet);
	PRINT_STRING("\r\nWaiting for UDP client connection request...");
#else
	udp_client_connect(LOCAL_PORT,&process_packet);
#endif
	PRINT_STRING("\r\nUDP packets are transmitted every 2 seconds. Monitor the UDP console\r\n");
	/* Task entry point */
	while(1)
	{
		MX_LWIP_Process();
		CoreApplicationSubTask();
		rx_bytes = GET_STRING(rx_buff);
        if (rx_bytes > 0)
        {
        	switch(rx_buff[0])
        	{
				case '3':
	#if UDP_SERVER
					if(udp_server_init(LOCAL_PORT, &process_packet) == 0 )
					{
						PRINT_STRING("\r\nUDP Server Port in Listening Mode...\r\n");
					}else{
						PRINT_STRING("\r\nUDP Server Port Init Failed\r\n");
					}
	#else
					if( udp_client_connect(LOCAL_PORT,&process_packet) == 0 ) {
						PRINT_STRING("\r\nudp client Port initialization...\r\n");
					}	else{
						PRINT_STRING("\r\nudp client Port Init Failed\r\n");
					}
	#endif
					break;
				case '1':
					rx_bytes = get_data_from_uart();
	#if UDP_SERVER
					if( udp_server_send(g_uart_to_udp, rx_bytes) == 0){
	#else
					if( udp_client_send(g_uart_to_udp, rx_bytes) != 0){
	#endif
						PRINT_STRING("\r\n[TX]: data packet sent failed\n");
					} else {
						PRINT_STRING("\r\n[TX]: data packet sent\n");
					}
					break;
				case '2':
	#if UDP_SERVER
					udp_server_cleanup();
	#else
					udp_client_cleanup();
	#endif
					PRINT_STRING("\r\nudp connection closed\r\n");
					break;
				case 'h':
					PRINT_STRING(g_message2);
					break;
			}
			rx_bytes = 0;
        }
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
	static uint32_t PacketTimeout_l = 0 ;

	if( HAL_GetTick() > PacketTimeout_l )
	{
		/* send udp tx packet for every 2s */
		udp_server_send(TX_PACKET,TX_PACKET_SIZE);
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
void process_packet(uint8_t *data,uint16_t len)
{
	PRINT_STRING("\r\n\n[RX]: ");
	PRINT_STRING(data);
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
    uint8_t rx_buff[1];
    uint8_t count = 0;
    uint8_t rx_size = 0;

    for (count = 0u; count < UART_RECV_BUF_SIZE; count++)
    {
    	g_uart_to_udp[count] = 0u;
    }

    PRINT_STRING("\r\n Enter the data of 32 bytes: ");

    while (!complete)
    {
        rx_size = GET_STRING(rx_buff);
        if(rx_size > 0u)
        {
            if ( ENTER == rx_buff[0] ) {
                complete = 1u;
            } else {
            	g_uart_to_udp[count++] = rx_buff[0];
            }
            if (UART_RECV_BUF_SIZE == count) {
                complete = 1u; count -= 1;
            }
        }
    }
    g_uart_to_udp[count] = 0x00;
//	PRINT_STRING( g_uart_to_udp );
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

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
