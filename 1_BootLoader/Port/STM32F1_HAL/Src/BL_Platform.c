#include "BL_Platform.h"
#include "BL_Interface.h"
#include "main.h"

uint32_t BL_Platform_ReadWord(uint32_t address)
{
	return *((volatile uint32_t *)address);
}

uint32_t BL_Platform_Crc32Words(uint32_t start_address, uint32_t word_count)
{
	return HAL_CRC_Calculate(&hcrc, (uint32_t *)start_address, word_count);
}

BL_Status_t BL_Platform_ErasePages(uint32_t start_address, uint32_t page_count)
{
	FLASH_EraseInitTypeDef erase_info;
	uint32_t page_error = 0u;
	HAL_StatusTypeDef hal_status;

	erase_info.TypeErase = FLASH_TYPEERASE_PAGES;
	erase_info.Banks = FLASH_BANK_1;
	erase_info.PageAddress = start_address;
	erase_info.NbPages = page_count;

	HAL_FLASH_Unlock();
	hal_status = HAL_FLASHEx_Erase(&erase_info, &page_error);
	HAL_FLASH_Lock();

	return (hal_status == HAL_OK) ? BL_STATUS_OK : BL_STATUS_ERROR;
}

BL_Status_t BL_Platform_WriteWord(uint32_t address, uint32_t data)
{
	HAL_StatusTypeDef hal_status;

	HAL_FLASH_Unlock();
	hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
	HAL_FLASH_Lock();

	return (hal_status == HAL_OK) ? BL_STATUS_OK : BL_STATUS_ERROR;
}

BL_Status_t BL_Platform_WriteDoubleWord(uint32_t address, uint64_t data)
{
	HAL_StatusTypeDef hal_status;

	HAL_FLASH_Unlock();
	hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
	HAL_FLASH_Lock();

	return (hal_status == HAL_OK) ? BL_STATUS_OK : BL_STATUS_ERROR;
}

void BL_Platform_JumpToImage(uint32_t image_base_address)
{
	Application_t address_to_call = *(Application_t *)(image_base_address + 4u);

	SysTick->CTRL = 0x0u;
	HAL_DeInit();
	RCC->CIR = 0x00000000u;

	__DMB();
	SCB->VTOR = image_base_address;
	__DSB();

	address_to_call();
}

void BL_Platform_RequestReset(void)
{
#ifdef Debug
	__HAL_DBGMCU_FREEZE_IWDG();
#endif

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
	hiwdg.Init.Reload = 9;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		BL_Platform_FatalError();
	}
}

void BL_Platform_FatalError(void)
{
	Error_Handler();
}
