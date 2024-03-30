/*
 * FlashProgram_Interface.c
 *
 *  Created on: Mar 14, 2024
 *      Author: Chau Thanh Dat
 */
#include "FlashReprogram_Interface.h"
/*Init Flash read / write Address volatile Variable */

volatile uint32_t Global_WriteAddress ;


/*	FR interface Function */
void FR_voidInitVariables (void)
{
	Global_WriteAddress = STORE_AREA_START_ADDRESS ;
}


HAL_StatusTypeDef FR_FlashBlockToAddress(const uint8_t *pData , uint16_t SizeOfDataBuffer)
{
	HAL_StatusTypeDef	Local_retVal = HAL_OK;
	uint16_t Local_Word = INITIAL_ZERO ;
	uint16_t Local_Counter = INITIAL_ZERO;

	if (pData == NULL)
	{
		Local_retVal =  HAL_ERROR;
	}
	else
	{
		for(Local_Counter = INITIAL_ZERO; Local_Counter < SizeOfDataBuffer ;Local_Counter += FLASH_ADDRESS_STEP)
		{
			Local_Word = pData[Local_Counter] | (pData[Local_Counter+1] << 8) ;
			FR_FlashHalfWordToAddress(Global_WriteAddress,Local_Word);
			Global_WriteAddress += FLASH_ADDRESS_STEP ;
		}
	}
	return Local_retVal;
}

HAL_StatusTypeDef FR_Erase_Image(uint32_t ImageAddress)
{
	HAL_StatusTypeDef	Local_retVal;

	FLASH_EraseInitTypeDef Local_eraseInfo;
	uint32_t Local_u32PageError;
	Local_eraseInfo.TypeErase = FLASH_TYPEERASE_PAGES;
	Local_eraseInfo.Banks = FLASH_BANK_1;
	Local_eraseInfo.PageAddress = ImageAddress;
	Local_eraseInfo.NbPages =	22;

	HAL_FLASH_Unlock(); //Unlocks the flash memory
	Local_retVal = HAL_FLASHEx_Erase(&Local_eraseInfo, &Local_u32PageError); //Deletes given sectors
	HAL_FLASH_Lock();

	return Local_retVal;
}


HAL_StatusTypeDef FR_FlashHalfWordToAddress(uint32_t Copy_Address , uint16_t Copy_u16DataAddress)
{
	HAL_StatusTypeDef	Local_retVal;

	HAL_FLASH_Unlock();
	Local_retVal = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Copy_Address, Copy_u16DataAddress);
	HAL_FLASH_Lock();

	return Local_retVal;
}

HAL_StatusTypeDef FR_FlashWordToAddress(uint32_t Copy_Address , uint32_t Copy_u32DataAddress)
{
	HAL_StatusTypeDef	Local_retVal;

	HAL_FLASH_Unlock();
	Local_retVal = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Copy_Address, Copy_u32DataAddress);
	HAL_FLASH_Lock();

	return Local_retVal;

}

