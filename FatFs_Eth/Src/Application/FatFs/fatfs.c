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
/* Includes ------------------------------------------------------------------*/
#include "fatfs.h"
#include<string.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define DEBUG_MSG(X)

/* Private variables ---------------------------------------------------------*/
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */
static uint8_t buffer[_MAX_SS]; /* a work buffer for the f_mkfs() */

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  Initialize and link the FAT file system driver.
  * @retval none
  */
void FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);
}

/**
 * @brief  Mount the FAT file system to a specific drive.
 * @details This function is responsible for mounting the FAT file system to the specified drive, allowing subsequent file
 *          operations (e.g., read, write) to be performed on that drive. It initializes the necessary data structures and
 *          prepares the file system for access. The function must be called before performing any file I/O operations on
 *          the mounted file system.
 * @return FRESULT error code indicating the status of the operation.
 *          - `FR_OK` on success.
 *          - Other values (e.g., `FR_DISK_ERR`, `FR_NOT_READY`, `FR_INVALID_DRIVE`) indicate errors.
 */
FRESULT FATFS_Mount(void)
{
	FRESULT status = 0;
	/* Mount Disk Drive */
	status = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1); /* Mount SD card */
	if(status == FR_NO_FILESYSTEM)
	{
		/*##-3- Create a FAT file system (format) on the logical drive #########*/
		/* WARNING: Formatting the uSD card will delete all content on the device */
		if(f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, buffer, sizeof(buffer)) == FR_OK){
			/* Mount Disk Drive Again */
			status = f_mount(&SDFatFS, (TCHAR const*)SDPath,1);
		}
	}

	return (FRESULT)status;
}

/**
 * @brief  Create a new file in the file system with the given name.
 * @details This function attempts to create a new file in the file system. If the file already exists,
 *   it will be opened for reading or writing, depending on the specified access mode.
 * @param  pFileName  Pointer to a null-terminated string that specifies the name of the file to be created.
 * @return FATFS result code indicating the status of the operation:
 *          - `FR_OK` (0) if the file was successfully created.
 *          - `FR_DISK_ERR` if a disk error occurred.
 *          - Other values indicate various file system or hardware errors.
 */
FRESULT FATFS_FileCreate(uint8_t *pFileName)
{
	FRESULT res = FR_TOO_MANY_OPEN_FILES;
	/* Any open file shall be closed before attempting to open another file. */
	if( SDFile.obj.fs == NULL ) {
		// if note the case leads to  memory leakage for that reason only 'SDFile.fs' Checking
		res = f_open(&SDFile ,(TCHAR const*)pFileName,FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
	}
	return res;
}

/* @Brief : Attempts to open the specified file. If the file does not exist,
 *  the operation fails and returns a file error.
 * @param : filename
 * @param : return file status
 */
FRESULT FATFS_FileOpen(uint8_t *pFileName)
{
	FRESULT res = FR_INVALID_OBJECT;
	/* Any open file shall be closed before attempting to open another file. */
	if( SDFile.obj.fs == NULL )
		res = f_open(&SDFile,(TCHAR const*)pFileName,FA_OPEN_EXISTING | FA_WRITE | FA_READ);  //  check it  FA_
	return res;
}


/* @ Brief  : Close the currently opened file or directory to release system resources.
 * @ retval : file status
 */
FRESULT FATFS_FileClose(void)
{
	FRESULT res;
	/* Close the file to release the resource before reusing or exiting. */
	res = f_close(&SDFile);
	return res;
}


/**
 * @brief  Get the size of a file in the file system.
 * @param  filename  Pointer to a null-terminated string containing the file path.
 * @return File size in bytes, or 0 if the file does not exist or cannot be accessed.
 */
FRESULT FATFS_FileSize (uint8_t *pFile,uint32_t *FileSize)
{
	FILINFO File_info={0};
	FRESULT res = FR_OK;
	res = f_stat((const TCHAR*)pFile,&File_info);
	if(res != FR_OK)
	{
		*FileSize = 0 ;
		return res;
	}
	*FileSize = File_info.fsize;
	return res;
}

/**
 * @brief  Read data from the opened file in the file system.
 * @param  buffer    Pointer to the buffer where the file contents will be stored.
 * @param  length    The maximum number of bytes to read.
 * @param  The number of bytes successfully read, or a zero value if an error occurs.
 * @retval : File Status
 */
FRESULT FATFS_FileRead( uint8_t *pBuffer ,uint32_t pSize ,uint32_t *rbyte)
{
	FRESULT res = FR_INT_ERR;
	uint32_t fsize = SDFile.obj.objsize;
	if(fsize - SDFile.fptr < pSize )
	{
		pSize = fsize - SDFile.fptr;
		if( pSize == 0 )
		{
			return res;
		}
	}
	res = f_read(&SDFile,pBuffer,pSize,(UINT*)rbyte);
	return res;
}

/**
 * @brief  Write data to the opened file in the file system.
 * @param  buffer    Pointer to the data to be written to the file.
 * @param  length    The number of bytes to write.
 * @param  The number of bytes successfully written, or a negative value if an error occurs.
 * @retval File Status
 *
 */
FRESULT FATFS_FileWrite( uint8_t *pBuffer ,uint32_t pSize ,uint32_t *wBytes)
{
	FRESULT res = FR_EXIST;
	res = f_write(&SDFile,pBuffer,pSize,(UINT*)wBytes);
	return res;
}

/**
 * @brief  Retrieve a list of files from the specified directory in the file system.
 * @param  dirPath    Pointer to a null-terminated string specifying the directory path.
 * @param  FileNames  Pointer to the buffer where the file names will be stored.
 * @param  Nbr_files  Pointer to a variable that will store the total number of files found.
 * @param  MaxFiles   Maximum number of files to retrieve. If the number of files exceeds this value, only the first 'MaxFiles' are returned.
 * @return FATFS error code indicating the status of the operation.
 *          - `FR_OK` on success.
 *          - Other values indicate an error (e.g., `FR_DISK_ERR`, `FR_NOT_READY`, etc.).
 */
FRESULT FATFS_GetFileList( uint8_t *dirPath ,uint8_t *FileNames, uint16_t *Nbr_files ,uint8_t MaxFiles)
{
    FRESULT res;    DIR dir;
    UINT FileIndex = 0;  uint16_t FileBufferIndex = 0;
    static FILINFO fno;
    res = f_opendir(&dir, (const TCHAR*)dirPath); /* Open the rooot directory */
    if ( res == FR_OK )
    {
        for (  ; ;  )     {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if ( res != FR_OK || fno.fname[0] == 0 )
                     	break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)                     /* It is a directory */
               continue;
            else  {    /* It is a file copy to buffer  */
            	FileIndex++;
           		memcpy(FileNames+FileBufferIndex ,fno.fname, sizeof(fno.fname));
           		FileBufferIndex += strlen(fno.fname);
           		FileNames[FileBufferIndex++] = '\n';
           		FileNames[FileBufferIndex++] = '\r';
           		memset(fno.fname,0x00,sizeof(fno.fname));
           		if( FileIndex > MaxFiles )/* From Application Buffer Handling 13 Files only */
           			break;
            }
         }
		*Nbr_files = FileIndex;
         f_closedir(&dir);
    }
    return res;
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
