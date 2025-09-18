/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hw_config.h"
#include "../MSS/GPIO/mss_gpio.h"

/* USER CODE BEGIN Includes */

///* USER CODE END Includes */
//extern DMA_HandleTypeDef hdma_spi1_tx;
//
//extern DMA_HandleTypeDef hdma_spi1_rx;
//
//extern DMA_HandleTypeDef hdma_usart2_rx;
//
//extern DMA_HandleTypeDef hdma_usart2_tx;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief CAN MSP Initialization
* This function configures the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */

  }

}

/**
* @brief CAN MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* CAN1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }

}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hi2c->Instance==I2C1)
	{
		/* Reseting I2C low level hardware */
		__HAL_RCC_I2C1_FORCE_RESET();
		__HAL_RCC_I2C1_RELEASE_RESET();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**I2C1 GPIO Configuration
		PB8     ------> I2C1_SCL
		PB9     ------> I2C1_SDA
		*/
		MSS_GPIO_ConfigAltrPin(I2C1_PORT, I2C1_SCL, GPIO_MODE_AF_PP,GPIO_AF4_I2C1);
		MSS_GPIO_ConfigAltrPin(I2C1_PORT, I2C1_SDA, GPIO_MODE_AF_OD,GPIO_AF4_I2C1);

		/* Peripheral clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE();
		/* I2C1 interrupt Init */
		//    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
		//    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

	}else if (hi2c->Instance == I2C2)
	{
		/* Reseting I2C low level hardware */
		__HAL_RCC_I2C2_FORCE_RESET();
		__HAL_RCC_I2C2_RELEASE_RESET();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**I2C2 GPIO Configuration
		PB10     ------> I2C2_SCL
		PB11     ------> I2C2_SDA
		*/
		GPIO_InitStruct.Pin = I2C2_SCL|I2C2_SDA;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
		HAL_GPIO_Init(I2C1_PORT, &GPIO_InitStruct);

		/* Peripheral clock enable */
		__HAL_RCC_I2C2_CLK_ENABLE();
	}else if (hi2c->Instance == I2C3)
	{
	    /* Reseting I2C low level hardware */
		__HAL_RCC_I2C3_FORCE_RESET();
		__HAL_RCC_I2C3_RELEASE_RESET();
		/* DEINIT I2C */
		HAL_I2C_DeInit(hi2c);

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/* Peripheral clock enable */
		__HAL_RCC_I2C3_CLK_ENABLE();

		/**I2C3 GPIO Configuration
		PA8     ------> I2C3_SCL
		PcC9     ------> I2C2_SDA
		*/
		GPIO_InitStruct.Pin = I2C3_SCL;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
		HAL_GPIO_Init(I2C3_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = I2C3_SDA;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
		HAL_GPIO_Init(I2C3_PORT_Ex, &GPIO_InitStruct);
	}else{
		/* */
	}
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		/* USER CODE BEGIN I2C1_MspDeInit 0 */

		/* USER CODE END I2C1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_I2C1_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		PB6     ------> I2C1_SCL
		PB7     ------> I2C1_SDA
		*/
		HAL_GPIO_DeInit(I2C1_PORT, I2C1_SCL);

		HAL_GPIO_DeInit(I2C1_PORT, I2C1_SDA);

		/* I2C1 interrupt DeInit */
		//    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
		/* USER CODE BEGIN I2C1_MspDeInit 1 */

		/* USER CODE END I2C1_MspDeInit 1 */
	}else if(hi2c->Instance==I2C2)  {
	  /* USER CODE BEGIN I2C1_MspDeInit 0 */

	  /* USER CODE END I2C1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_I2C2_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		PB6     ------> I2C1_SCL
		PB7     ------> I2C1_SDA
		*/
		HAL_GPIO_DeInit(I2C2_PORT, I2C2_SCL);

		HAL_GPIO_DeInit(I2C2_PORT, I2C2_SDA);

		/* I2C1 interrupt DeInit */
	//    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
	  /* USER CODE BEGIN I2C1_MspDeInit 1 */

	  /* USER CODE END I2C1_MspDeInit 1 */
	  }else   if(hi2c->Instance==I2C3)
	  {
	  /* USER CODE BEGIN I2C1_MspDeInit 0 */

	  /* USER CODE END I2C1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_I2C3_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		PB6     ------> I2C1_SCL
		PB7     ------> I2C1_SDA
		*/
		HAL_GPIO_DeInit(I2C3_PORT, I2C3_SCL);

		HAL_GPIO_DeInit(I2C3_PORT_Ex, I2C3_SDA);

		/* I2C1 interrupt DeInit */
	//    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
	  /* USER CODE BEGIN I2C1_MspDeInit 1 */

	  /* USER CODE END I2C1_MspDeInit 1 */
	  }
}

/**
* @brief SD MSP Initialization
* This function configures the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hsd->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */

  }

}

/**
* @brief SD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
  if(hsd->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO interrupt DeInit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }

}

/**
* @brief SPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
	MSS_GPIO_ConfigAltrPin(SPI1_PORT, SPI1_MISO_DIGIO|SPI1_SCK_DIGIO, GPIO_MODE_AF_PP, GPIO_AF5_SPI1);
	MSS_GPIO_ConfigAltrPin(SPI1_ExPORT, SPI1_MOSI_DIGIO, GPIO_MODE_AF_PP, GPIO_AF5_SPI1);

    /* SPI1 interrupt Init */
//    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(SPI1_IRQn);

  }else if (hspi->Instance==SPI2) {
	__HAL_RCC_SPI2_CLK_ENABLE();

	MSS_GPIO_ConfigAltrPin(SPI2_PORT, SPI2_MISO_DIGIO|SPI2_MOSI_DIGIO|SPI2_SCK_DIGIO, GPIO_MODE_AF_PP, GPIO_AF5_SPI2);

//	HAL_NVIC_SetPriority(SPI2_IRQn, 5, 0);
//	HAL_NVIC_EnableIRQ(SPI2_IRQn);
  }else if (hspi->Instance==SPI3) {
	__HAL_RCC_SPI3_CLK_ENABLE();

	MSS_GPIO_ConfigAltrPin(SPI3_PORT, SPI3_MISO_DIGIO|SPI3_MOSI_DIGIO|SPI3_SCK_DIGIO, GPIO_MODE_AF_PP, GPIO_AF6_SPI3);

	//	HAL_NVIC_SetPriority(SPI3_IRQn, 5, 0);
	//	HAL_NVIC_EnableIRQ(SPI3_IRQn);
  }
}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if(hspi->Instance==SPI1)
	{
		/* USER CODE BEGIN SPI1_MspDeInit 0 */

		/* USER CODE END SPI1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		PA5     ------> SPI1_SCK
		PA6     ------> SPI1_MISO
		PB5     ------> SPI1_MOSI
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);

		/* SPI1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(SPI1_IRQn);
	}else if(hspi->Instance==SPI1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();
		HAL_GPIO_DeInit(SPI2_PORT, SPI2_MISO_DIGIO|SPI2_MOSI_DIGIO|SPI2_SCK_DIGIO);
		/* SPI1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
	}else if(hspi->Instance==SPI1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_SPI3_CLK_DISABLE();
		HAL_GPIO_DeInit(SPI3_PORT, SPI3_MISO_DIGIO|SPI3_MOSI_DIGIO|SPI3_SCK_DIGIO);
		/* SPI1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
	}
}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */

  }

}

/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	if( huart->Instance == USART1 )
	{
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration */
		MSS_GPIO_ConfigAltrPin(UART1_PORT,UART1_TX_LINE,GPIO_MODE_AF_PP,GPIO_AF7_USART1);
		MSS_GPIO_ConfigAltrPin(UART1_PORT,UART1_RX_LINE,GPIO_MODE_AF_PP,GPIO_AF7_USART1);

//		HAL_NVIC_SetPriority( USART1_IRQn, 3 , 0 );
//		HAL_NVIC_EnableIRQ( USART1_IRQn );
	}
	else if(huart->Instance == USART2)
	{
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		/**USART2 GPIO Configuration */
		MSS_GPIO_ConfigAltrPin(UART2_PORT,UART2_TX_LINE,GPIO_MODE_AF_PP,GPIO_AF7_USART2);
		MSS_GPIO_ConfigAltrPin(UART2_PORT_EX,UART2_RX_LINE,GPIO_MODE_AF_PP,GPIO_AF7_USART2);
//		HAL_NVIC_SetPriority(USART2_IRQn, 4, 0);
//		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
	else if(huart->Instance == USART3)
	{
		__HAL_RCC_USART3_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		/**USART3 GPIO Configuration */
		MSS_GPIO_ConfigAltrPin(UART3_PORT,UART3_TX_LINE,GPIO_MODE_AF_PP,GPIO_AF7_USART3);
		MSS_GPIO_ConfigAltrPin(UART3_PORT,UART3_RX_LINE,GPIO_MODE_AF_PP,GPIO_AF7_USART3);
//		HAL_NVIC_SetPriority(USART3_IRQn,1, 0);
//		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
	else if(huart->Instance == UART4)
	{
		__HAL_RCC_UART4_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART4 GPIO Configuration */
		MSS_GPIO_ConfigAltrPin(UART4_PORT,UART4_TX_LINE,GPIO_MODE_AF_PP,GPIO_AF8_UART4);
		MSS_GPIO_ConfigAltrPin(UART4_PORT,UART4_RX_LINE,GPIO_MODE_AF_PP,GPIO_AF8_UART4);
//		HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
//		HAL_NVIC_EnableIRQ(UART4_IRQn);
	}
	else if(huart->Instance == UART5)
	{
		__HAL_RCC_UART5_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		/**USART6 GPIO Configuration */
		MSS_GPIO_ConfigAltrPin(UART5_TX_PORT,UART5_TX_LINE,GPIO_MODE_AF_PP,GPIO_AF8_UART5);
		MSS_GPIO_ConfigAltrPin(UART5_RX_PORT,UART5_RX_LINE,GPIO_MODE_AF_PP,GPIO_AF8_UART5);
//		HAL_NVIC_SetPriority(UART5_IRQn, 5, 0);
//		HAL_NVIC_EnableIRQ(UART5_IRQn);

	}
	else if(huart->Instance == USART6)
	{
		__HAL_RCC_USART6_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		/**USART6 GPIO Configuration */
		MSS_GPIO_ConfigAltrPin(UART6_PORT,UART6_TX_LINE,GPIO_MODE_AF_PP,GPIO_AF8_USART6);
		MSS_GPIO_ConfigAltrPin(UART6_PORT,UART6_RX_LINE,GPIO_MODE_AF_PP,GPIO_AF8_USART6);
		HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART6_IRQn);
	}
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA3     ------> USART2_RX
    PD5     ------> USART2_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

    /* USART2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
