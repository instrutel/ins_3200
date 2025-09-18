/**
  ******************************************************************************
  * @file   fatfs.h
  * @brief  Header for fatfs applications
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __fatfs_H
#define __fatfs_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h" /* defines SD_Driver as external */

 /* Private variables ---------------------------------------------------------*/

extern uint8_t retSD; /* Return value for SD */
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFS; /* File system object for SD logical drive */
extern FIL SDFile; /* File object for SD */

/* function prototypes -----------------------------------------------*/

void FATFS_Init(void);
FRESULT FATFS_Mount(void);
FRESULT FATFS_FileCreate(uint8_t *pFileName);
FRESULT FATFS_FileOpen(uint8_t *pFileName);
FRESULT FATFS_FileClose(void);
FRESULT FATFS_FileSize (uint8_t *pFile,uint32_t *FileSize);
FRESULT FATFS_GetFileList( uint8_t *dirPath ,uint8_t *FileNames, uint16_t *Nbr_files ,uint8_t MaxFiles);
FRESULT FATFS_FileRead( uint8_t *pBuffer ,uint32_t pSize ,uint32_t *rbyte);
FRESULT FATFS_FileWrite( uint8_t *pBuffer ,uint32_t psize ,uint32_t *wbyte);

#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */
