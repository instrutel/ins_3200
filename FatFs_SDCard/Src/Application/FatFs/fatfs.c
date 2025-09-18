/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */
static uint8_t buffer[_MAX_SS]; /* a work buffer for the f_mkfs() */

const char filename[] = { "test.txt" };

/* USER CODE BEGIN Variables */
static void FATFS_DEBUG(char*ptr);

/* USER CODE END Variables */

void FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Main routine for FatFs Application
  * @param  None
  * @retval None
  */
void FATFS_Application(void)
{
	FRESULT res;                                           /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                      /* File write/read counts */
	uint8_t wtext[] = "This is SDCard working example with FatFs"; /* File write buffer */
	uint8_t rtext[100];                                    /* File read buffer */

	/* Register the file system object to the FatFs module */
	if( f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) != FR_OK )
	{
		/* FatFs Initialization Error */
		FATFS_DEBUG("FatFs mount Error");
	}
	else
	{
		/*##-3- Create a FAT file system (format) on the logical drive #########*/
		/* WARNING: Formatting the uSD card will delete all content on the device */
		if(f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, buffer, sizeof(buffer)) != FR_OK)
		{
			/* FatFs Format Error */
			Error_Handler();
		} else {
			/* Create and Open a new text file object with write access */
			if(f_open(&SDFile, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
			{
				/* 'STM32.TXT' file Open for write Error */
				FATFS_DEBUG("file open for write Error");
			} else {
				/* Write data to the text file */
				res = f_write(&SDFile, wtext, sizeof(wtext), (void *)&byteswritten);

				if((byteswritten == 0) || (res != FR_OK))
				{
					/* 'STM32.TXT' file Write or EOF Error */
					FATFS_DEBUG("file writing Error");
				} else {
					/* Close the open text file */
					f_close(&SDFile);

					/* Open the text file object with read access */
					if(f_open(&SDFile, "STM32.TXT", FA_READ) != FR_OK)
					{
						/* 'STM32.TXT' file Open for read Error */
						FATFS_DEBUG("file open for read Error");
					} else {
						/* Read data from the text file */
						res = f_read(&SDFile, rtext, sizeof(rtext), (void *)&bytesread);

						if((bytesread == 0) || (res != FR_OK))
						{
							/* 'STM32.TXT' file Read or EOF Error */
							FATFS_DEBUG("file reading Error");
						} else {
							/* Close the open text file */
							f_close(&SDFile);

							/* Compare read data with the expected data */
							if((bytesread != byteswritten))
							{
								/* Read data is different from the expected data */
								FATFS_DEBUG("fatfs read-write test failed");
							}
							else { FATFS_DEBUG("fatfs read-write test pass"); }
						}
					}
				}
			}
		}
	}
  /* Unlink the USB disk I/O driver */
  FATFS_UnLinkDriver(SDPath);
  FATFS_DEBUG("fatfs unlink driver");
}

/*
 *
 */
static void FATFS_DEBUG(char *ptr)
{
	SerialPortDebug(ptr);
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */
