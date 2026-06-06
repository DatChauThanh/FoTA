/*
 * Flash_Interface.h
 *
 *  Created on: Mar 14, 2024
 *      Author: CHD9HC
 */

#ifndef INC_FLASH_INTERFACE_H_
#define INC_FLASH_INTERFACE_H_

#define STORE_AREA_START_ADDRESS 0x0800A800
#define IMAGE_NEW_FIRMWARE 	STORE_AREA_START_ADDRESS

#define INITIAL_ZERO 0x0

#define FLASH_ADDRESS_STEP	2
#define FLASH_STORE_NUM_PAGES 22

#include "GW_Types.h"
#include <stdint.h>
void FR_voidInitVariables(void);
GW_Status_t FR_Erase_Image(uint32_t ImageAddress);
GW_Status_t FR_FlashHalfWordToAddress(uint32_t Copy_Address , uint16_t Copy_u16DataAddress);
GW_Status_t FR_FlashWordToAddress(uint32_t Copy_Address , uint32_t Copy_u32DataAddress);
GW_Status_t FR_FlashBlockToAddress(const uint8_t *pData , uint16_t SizeOfDataBuffer);

#endif /* INC_FLASH_INTERFACE_H_ */
