/*******************************************************************************
 * Title                 :   BL
 * Filename              :   BL_Program.c
 * Author                :   Chau_Thanh_Dat
 * Origin Date           :   19/1/2024
 * Version               :   1.0.0
 * Compiler              :   GCC (STM32CubeIDE)
 * Target                :   STM32F103C8T6
 * Notes                 :   None
 *
 *****************************************************************************/

//**************************Include***************************//
#include "BL_Interface.h"
#include "CAN_Interface.h"
#include "aes.h"

uint8_t AES_CBC_128_Key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
uint8_t AES_CBC_128_IV[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
//**************************Include***************************//

//**************************Function Define***************************//
static uint32_t BL_u32ReadAddressData(uint32_t address){
	uint32_t Local_u32AddressData = *((volatile uint32_t*)(address));
	return Local_u32AddressData;
}

void BL_voidBootLoader_Init(void)
{
	// Read Branching Request Update Flag.
	uint32_t Local_u32Flag = BL_u32ReadAddressData(FLAG_STATUS_BOOTLOADER);

	if(Local_u32Flag == BL_BRANCHING_FLAG_RESET)
	{
		// Check images existence, status (and CRC).
		BL_voidCheckActiveRegion();
	}
	else if(Local_u32Flag == BL_BRANCHING_FLAG_SET)
	{
		// Goto Boot_loader to Receive code in the inactive image.
	    BL_voidJumpToBootloader();
	}
	else
	{
		// Do nothing
	}
}

void BL_voidCheckActiveRegion(void)
{
    // Read Images Status To Determine Which Image Will Be Excuted.
	uint32_t Local_u32ActiveImageStatus = BL_u32ReadAddressData(FLAG_STATUS_ACTIVE_REGION_ADDRESS);
	uint32_t Local_u32BackupStatus      = BL_INITIALIZE_EITH_CORRUPTED;
	uint32_t* Local_u32ActiveImagePointer = (uint32_t*)ACTIVE_IMAGE;
	uint32_t Local_u32SizeCode = BL_u32ReadAddressData(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS);
	uint32_t Local_u32CRC =  BL_u32ReadAddressData(FLAG_STATUS_CRC_ACTIVE_REGION_ADDRESS);
	uint32_t Local_u32CRC_check = HAL_CRC_Calculate(&hcrc, Local_u32ActiveImagePointer, Local_u32SizeCode/4);
    // Check Active region Image Status.
    if(Local_u32ActiveImageStatus == BR_IMAGE_IS_ACTIVE)
	{
    	// Active Case. Check CRC
    	if(Local_u32CRC_check == Local_u32CRC){
    		// CRC is correct.
    		BL_voidJumpToActiveRegion();
    	}
    	else{
    		// Make Image in the active region Corrupted As it's CRC is Wrong.
    		BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS, BR_SET_IMAGE_CORRUPTED);
    		// Make Software Reset to return to branch code.
    		BL_voidMakeSoftWareReset();
    	}
	}
    else if(Local_u32ActiveImageStatus == BR_IMAGE_IS_CORRUPTED || Local_u32ActiveImageStatus == BR_IMAGE_IS_NOT_EXISTING)
	{
		// Check the status of the backup image if the image in he active region is corrupted or not exisit.
    	Local_u32BackupStatus    = BL_32CheckBackupRegion();
		if(Local_u32BackupStatus == BR_IMAGE_IS_CORRECT)
		{
			// Move the backup image to the active region to be executed.
			BL_voidCopyImageToActiveRegion();
			// Excute Image in the active region If it's CRC is Correct.
			BL_voidJumpToActiveRegion();
		}
		else if(Local_u32BackupStatus == BR_IMAGE_IS_CORRUPTED || Local_u32BackupStatus == BR_IMAGE_IS_NOT_EXISTING)
		{
			// Set branching flag to go to boot loader to receive a new code as all codes in the flash corrupted.
			BL_voidSetBranchingFlagAndMakeSWR();
		}  
	}
    else
    {
        // Do nothing
    }
}

uint32_t BL_32CheckBackupRegion(void)
{
	uint32_t Local_u32BackupStatus =BL_u32ReadAddressData(FLAG_STATUS_BACKUP_REGION_ADDRESS);
	uint32_t* Local_u32BackupImagePointer = (uint32_t*)BACKUP_IMAGE;
	uint32_t Local_u32SizeCode = BL_u32ReadAddressData(FLAG_STATUS_SIZE_BACKUP_REGION_ADDRESS);
	uint32_t Local_u32CRC_Backup =  BL_u32ReadAddressData(FLAG_STATUS_CRC_BACKUP_REGION_ADDRESS);
	uint32_t Local_u32CRC_Check_Backup = HAL_CRC_Calculate(&hcrc, Local_u32BackupImagePointer, Local_u32SizeCode/4);
	// if exist Backup image
	if(Local_u32BackupStatus == BR_IMAGE_IS_BACKUP)
	{
		if (Local_u32CRC_Check_Backup == Local_u32CRC_Backup){
			return BR_IMAGE_IS_CORRECT;
		}
		else{
			// Handle error
			// Make Image in the backup region Corrupted As it's CRC is Wrong.
			BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BACKUP_REGION_ADDRESS, BR_SET_IMAGE_CORRUPTED);
			// Make Software Reset to return to branch code.
			return BR_IMAGE_IS_CORRUPTED;
		}
	}
}

void BL_voidJumpToActiveRegion(void)
{
	//MX_GPIO_Deinit();
	SysTick->CTRL = 0x0; //Disables SysTick timer and its related interrupt
	HAL_DeInit();
	RCC->CIR = 0x00000000; //Disable all interrupts related to clock

	Application_t AddressToCall = 0 ;
	AddressToCall = *(Application_t*)(ACTIVE_IMAGE + 4); // Point to Reset Handler

	__DMB(); //ARM says to use a DMB instruction before relocating VTOR *
	SCB->VTOR = ACTIVE_IMAGE; //We relocate vector table to the sector 1
	__DSB(); //ARM says to use a DSB instruction just after relocating VTOR */

	AddressToCall();
}

void BL_voidJumpToBootloader(void)
{
	//@TODO: In develop
	BL_voidUpdateHeaders();
	BL_voidReceiveUpdate();
}

void BL_voidCopyImageToActiveRegion(void)
{
	FLASH_EraseInitTypeDef Local_eraseInfo;
	uint32_t Local_u32PageError;
	uint32_t Local_u32BackupDataAddress = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveDataAddress = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32BackUpDataWord 	= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32BackupSizeInWord 	= BL_u32ReadAddressData(FLAG_STATUS_SIZE_BACKUP_REGION_ADDRESS);
	uint32_t* Local_u32BackupImagePointer = (uint32_t*)BACKUP_IMAGE;
	Local_u32BackupSizeInWord = Local_u32BackupSizeInWord / 4;
	uint32_t Local_u32CRC = HAL_CRC_Calculate(&hcrc, Local_u32BackupImagePointer, Local_u32BackupSizeInWord);

	// Erase the Active region.
	Local_eraseInfo.TypeErase = FLASH_TYPEERASE_PAGES;
	Local_eraseInfo.Banks = FLASH_BANK_1;
	Local_eraseInfo.PageAddress = ACTIVE_IMAGE;
	Local_eraseInfo.NbPages =	FLASH_BANK_NUMOFPAGE;

	HAL_FLASH_Unlock(); //Unlocks the flash memory
	HAL_FLASHEx_Erase(&Local_eraseInfo, &Local_u32PageError); //Deletes given sectors
	HAL_FLASH_Lock();  //Locks again the flash memory

	//Copy data from backup to active region.
	HAL_FLASH_Unlock();
	for(uint32_t Local_uint32Count = 0 ; Local_uint32Count  < Local_u32BackupSizeInWord ; Local_uint32Count++)
	{
		Local_u32ActiveDataAddress = (ACTIVE_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32BackupDataAddress = (BACKUP_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32BackUpDataWord    = *((volatile uint32_t*)(Local_u32BackupDataAddress));
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Local_u32ActiveDataAddress, Local_u32BackUpDataWord);
	}
	HAL_FLASH_Lock();

	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS , Local_u32BackupSizeInWord*4);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_CRC_ACTIVE_REGION_ADDRESS , Local_u32CRC);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS , BR_SET_IMAGE_ACTIVE );
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BACKUP_REGION_ADDRESS , BR_SET_IMAGE_BACKUP);
}

void BL_voidCopyImageToBackupRegion(void)
{
	FLASH_EraseInitTypeDef Local_eraseInfo;
	uint32_t Local_u32PageError;
	uint32_t Local_u32BackupDataAddress 		= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveDataAddress 		= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveDataWord 			= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveSizeInWord 			= BL_u32ReadAddressData(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS);
	uint32_t* Local_u32ActiveImagePointer = (uint32_t*)ACTIVE_IMAGE;
	Local_u32ActiveSizeInWord = Local_u32ActiveSizeInWord / 4;
	uint32_t Local_u32CRC = HAL_CRC_Calculate(&hcrc, Local_u32ActiveImagePointer, Local_u32ActiveSizeInWord);

	// Erase the Backup region.
	Local_eraseInfo.TypeErase 	= FLASH_TYPEERASE_PAGES;
	Local_eraseInfo.Banks 		= FLASH_BANK_1;
	Local_eraseInfo.PageAddress = BACKUP_IMAGE;
	Local_eraseInfo.NbPages 	= FLASH_BANK_NUMOFPAGE;

	HAL_FLASH_Unlock(); //Unlocks the flash memory
	HAL_FLASHEx_Erase(&Local_eraseInfo, &Local_u32PageError); //Deletes given sectors
	HAL_FLASH_Lock();  //Locks again the flash memory

	//Copy data from active region to backup region
	HAL_FLASH_Unlock();
	for(uint32_t Local_uint32Count = 0 ; Local_uint32Count  < Local_u32ActiveSizeInWord ; Local_uint32Count++)
	{
		Local_u32ActiveDataAddress = (ACTIVE_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32BackupDataAddress = (BACKUP_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32ActiveDataWord    = *((volatile uint32_t*)(Local_u32ActiveDataAddress));
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Local_u32BackupDataAddress, Local_u32ActiveDataWord);
	}
	HAL_FLASH_Lock();

	// Set
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_SIZE_BACKUP_REGION_ADDRESS , Local_u32ActiveSizeInWord*4 );
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_CRC_BACKUP_REGION_ADDRESS , Local_u32CRC);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS , BR_SET_IMAGE_ACTIVE);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BACKUP_REGION_ADDRESS , BR_SET_IMAGE_BACKUP);
}

void BL_voidEraseRestoreHeaderPage(uint32_t Copy_u32Address, uint32_t Copy_u32NewData)
{
	uint32_t Local_u32AddressArray	[NUMBER_OF_FLAGS];
	uint32_t Local_u32DataArray		[NUMBER_OF_FLAGS];
	uint16_t Local_u16DataIndex        = 0;
	uint16_t Local_u16DataCounter      = 0;
	uint32_t Local_u32AddressCounter   = 0;

	//Copy all flag to array before erase
	for( Local_u32AddressCounter = START_OF_FLAG_REGION ;Local_u32AddressCounter < END_OF_FLAG_REGION;)
	{
		if( (Local_u32AddressCounter != Copy_u32Address) & (*((volatile uint32_t*)(Local_u32AddressCounter)) != ERASED_VALUE))
		{
			Local_u32AddressArray[Local_u16DataIndex] = Local_u32AddressCounter;
			Local_u32DataArray[Local_u16DataIndex] = *((volatile uint32_t*)(Local_u32AddressCounter));
			Local_u16DataIndex++;
		}
		Local_u32AddressCounter = Local_u32AddressCounter + WORD_SIZE_IN_BYTE;
	}

	// Erase the Flag region.
	FLASH_EraseInitTypeDef Local_eraseInfo;
	uint32_t Local_u32PageError;
	Local_eraseInfo.TypeErase = FLASH_TYPEERASE_PAGES;
	Local_eraseInfo.Banks = FLASH_BANK_1;
	Local_eraseInfo.PageAddress = FLAG_IMAGE;
	Local_eraseInfo.NbPages = 1;

	HAL_FLASH_Unlock(); //Unlocks the flash memory
	HAL_FLASHEx_Erase(&Local_eraseInfo, &Local_u32PageError); //Deletes given sectors
	for (Local_u16DataCounter = 0 ; Local_u16DataCounter < Local_u16DataIndex ; Local_u16DataCounter++ )
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Local_u32AddressArray[Local_u16DataCounter], Local_u32DataArray[Local_u16DataCounter]);
	}
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Copy_u32Address, Copy_u32NewData); //Replace new data to flash
	HAL_FLASH_Lock();  //Locks again the flash memory
}

void BL_voidSetBranchingFlagAndMakeSWR(void)
{
	// Set Branching Flag To Receive New Code.
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BOOTLOADER, BL_SET_BRANCHING_FLAG);
	// Make Software Reset.
	BL_voidMakeSoftWareReset();
}

void BL_voidUpdateHeaders(void)
{
	uint8_t  Local_u8DataArray[8]              = {BL_INITIALIZE_WITH_ZERO};
	uint32_t Local_u32ActiveRegionStatus       = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ImageSizeInBytes         = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32CRC_Value			       = BL_INITIALIZE_WITH_ZERO;
	uint8_t	 Local_u8HeaderFlag                = BL_INITIALIZE_WITH_ZERO;

	Local_u32ActiveRegionStatus = BL_u32ReadAddressData(FLAG_STATUS_ACTIVE_REGION_ADDRESS);

	//Structure CAN Transmit
	CAN_IF_Transmit_UDS_Request(NODE_ID_TWO, UDS_MCU_ACKNOWLEDGE_UPGRADE_REQUEST);

	// Wait until FIFO 0 become pending state
	CAN_IF_Receive_UDS_Respond(&Local_u8HeaderFlag);

	if(Local_u8HeaderFlag == UDS_GWY_PROVIDE_HEADER)
	{
		CAN_IF_Transmit_UDS_Request(NODE_ID_TWO, UDS_MCU_ACCEPT_RECEIVING_HEADER);
		// Wait until FIFO 0 become pending state
		CAN_IF_Receive_Data_Frame(Local_u8DataArray);

		Local_u32ImageSizeInBytes = (Local_u8DataArray[3] << SHIFT_24_BIT) | (Local_u8DataArray[2] << SHIFT_16_BIT) | 		\
									(Local_u8DataArray[1] << SHIFT_8_BIT) | (Local_u8DataArray[0] << SHIFT_0_BIT);
		Local_u32CRC_Value		  = (Local_u8DataArray[7] << SHIFT_24_BIT) | (Local_u8DataArray[6] << SHIFT_16_BIT) | 		\
									(Local_u8DataArray[5] << SHIFT_8_BIT) | (Local_u8DataArray[4] << SHIFT_0_BIT);

		if(Local_u32ActiveRegionStatus == BR_IMAGE_IS_ACTIVE )
		{
			BL_voidCopyImageToBackupRegion();
		}

		BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS,BR_SET_IMAGE_CORRUPTED);
		BL_voidEraseRestoreHeaderPage(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS,Local_u32ImageSizeInBytes);
		BL_voidEraseRestoreHeaderPage(FLAG_STATUS_CRC_ACTIVE_REGION_ADDRESS,Local_u32CRC_Value);
		BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BOOTLOADER,BL_RESET_BRANCHING_FLAG);
	}
}

void BL_voidReceiveUpdate(void)
{
	uint8_t  Local_u8DataArray[16]              				  = {BL_INITIALIZE_WITH_ZERO};
	uint64_t Local_u64HighByteDataReceive  						  = BL_INITIALIZE_WITH_ZERO;
	uint64_t Local_u64LowByteDataReceive  						  = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32InactiveImageAddressCounter                 = ACTIVE_IMAGE_START_ADDRESS;
	uint32_t Local_u32SizeOfCode 								  = BL_u32ReadAddressData(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS);
	uint8_t	 Local_u8HeaderFlag                					  = BL_INITIALIZE_WITH_ZERO;
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, AES_CBC_128_Key, AES_CBC_128_IV);

	FLASH_EraseInitTypeDef Local_eraseInfo;
	uint32_t Local_u32PageError;
	// Erase the Active region.
	Local_eraseInfo.TypeErase = FLASH_TYPEERASE_PAGES;
	Local_eraseInfo.Banks = FLASH_BANK_1;
	Local_eraseInfo.PageAddress = ACTIVE_IMAGE;
	Local_eraseInfo.NbPages =	FLASH_BANK_NUMOFPAGE;

	HAL_FLASH_Unlock(); //Unlocks the flash memory
	HAL_FLASHEx_Erase(&Local_eraseInfo, &Local_u32PageError); //Deletes given sectors
	HAL_FLASH_Lock();  //Locks again the flash memory

	//Structure CAN Transmit
	CAN_IF_Transmit_UDS_Request(NODE_ID_TWO,UDS_MCU_ACKNOWLEDGE_HEADER_RECEIVED);
	//Loop to receive code update
	while(Local_u32SizeOfCode)
	{
		// Wait until FIFO 0 become pending state
		CAN_IF_Receive_UDS_Respond(&Local_u8HeaderFlag);
		//Check UDS ID
		if(Local_u8HeaderFlag == UDS_GWY_REQUEST_SENDING_LINE_OF_CODE)
		{
			CAN_IF_Transmit_UDS_Request(NODE_ID_TWO, UDS_MCU_ACCEPT_RECEIVING_PACKET_OF_CODE);

			if (Local_u32SizeOfCode > SIZEOF_PACKAGE_FIRMWARE){
				CAN_IF_Receive_Data_Buffer(Local_u8DataArray, SIZEOF_PACKAGE_FIRMWARE);
				/*access piont to Encrypt SWC*/
				AES_CBC_decrypt_buffer(&ctx, Local_u8DataArray, 16);

				Local_u64HighByteDataReceive= ((uint64_t)Local_u8DataArray[7] << SHIFT_56_BIT) | ((uint64_t)Local_u8DataArray[6] << SHIFT_48_BIT)
											| ((uint64_t)Local_u8DataArray[5] << SHIFT_40_BIT) | ((uint64_t)Local_u8DataArray[4] << SHIFT_32_BIT)
											| ((uint64_t)Local_u8DataArray[3] << SHIFT_24_BIT) | ((uint64_t)Local_u8DataArray[2] << SHIFT_16_BIT)
											| ((uint64_t)Local_u8DataArray[1] << SHIFT_8_BIT)  | ((uint64_t)Local_u8DataArray[0] << SHIFT_0_BIT);

				Local_u64LowByteDataReceive = ((uint64_t)Local_u8DataArray[15] << SHIFT_56_BIT) | ((uint64_t)Local_u8DataArray[14] << SHIFT_48_BIT)
											| ((uint64_t)Local_u8DataArray[13] << SHIFT_40_BIT)  | ((uint64_t)Local_u8DataArray[12] << SHIFT_32_BIT)
											| ((uint64_t)Local_u8DataArray[11] << SHIFT_24_BIT) | ((uint64_t)Local_u8DataArray[10] << SHIFT_16_BIT)
											| ((uint64_t)Local_u8DataArray[9] << SHIFT_8_BIT)  | ((uint64_t)Local_u8DataArray[8] << SHIFT_0_BIT);

				HAL_FLASH_Unlock(); //Unlocks the flash memory
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Local_u32InactiveImageAddressCounter, Local_u64HighByteDataReceive);
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Local_u32InactiveImageAddressCounter + 8, Local_u64LowByteDataReceive);
				HAL_FLASH_Lock();  //Locks again the flash memory

				Local_u32InactiveImageAddressCounter += 16;
				Local_u32SizeOfCode -= SIZEOF_PACKAGE_FIRMWARE;
			}
			else{
				CAN_IF_Receive_Data_Buffer(Local_u8DataArray, SIZEOF_PACKAGE_FIRMWARE);
				/*access piont to Encrypt SWC*/
				AES_CBC_decrypt_buffer(&ctx, Local_u8DataArray, 16);

				Local_u64HighByteDataReceive= ((uint64_t)Local_u8DataArray[7] << SHIFT_56_BIT) | ((uint64_t)Local_u8DataArray[6] << SHIFT_48_BIT)
											| ((uint64_t)Local_u8DataArray[5] << SHIFT_40_BIT) | ((uint64_t)Local_u8DataArray[4] << SHIFT_32_BIT)
											| ((uint64_t)Local_u8DataArray[3] << SHIFT_24_BIT) | ((uint64_t)Local_u8DataArray[2] << SHIFT_16_BIT)
											| ((uint64_t)Local_u8DataArray[1] << SHIFT_8_BIT)  | ((uint64_t)Local_u8DataArray[0] << SHIFT_0_BIT);

				Local_u64LowByteDataReceive = ((uint64_t)Local_u8DataArray[15] << SHIFT_56_BIT) | ((uint64_t)Local_u8DataArray[14] << SHIFT_48_BIT)
											| ((uint64_t)Local_u8DataArray[13] << SHIFT_40_BIT) | ((uint64_t)Local_u8DataArray[12] << SHIFT_32_BIT)
											| ((uint64_t)Local_u8DataArray[11] << SHIFT_24_BIT) | ((uint64_t)Local_u8DataArray[10] << SHIFT_16_BIT)
											| ((uint64_t)Local_u8DataArray[9] << SHIFT_8_BIT)   | ((uint64_t)Local_u8DataArray[8] << SHIFT_0_BIT);

				HAL_FLASH_Unlock(); //Unlocks the flash memory
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Local_u32InactiveImageAddressCounter, Local_u64HighByteDataReceive);
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Local_u32InactiveImageAddressCounter + 8, Local_u64LowByteDataReceive);
				HAL_FLASH_Lock();  //Locks again the flash memory

				Local_u32SizeOfCode -= Local_u32SizeOfCode ;
			}
			CAN_IF_Transmit_UDS_Request(NODE_ID_TWO, UDS_MCU_ACKNOWLEDGE_LINE_OF_CODE_RECEIVED);
		}
	}
	// Wait for finish code of GW
	CAN_IF_Receive_UDS_Respond(&Local_u8HeaderFlag);

	if (Local_u8HeaderFlag == UDS_GWY_ACKNOWLEDGE_FINISHING_SENDING_CODE)
	{
		BL_voidFinishBootLoader();
	}
}

void BL_voidFinishBootLoader(void)
{
	//Structure CAN Transmit
	CAN_IF_Transmit_UDS_Request(NODE_ID_TWO, UDS_MCU_ACKNOWLEDGE_FINISHING);

	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS , BR_SET_IMAGE_ACTIVE);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BOOTLOADER , BL_RESET_BRANCHING_FLAG);
	BL_voidMakeSoftWareReset();
}

void BL_voidMakeSoftWareReset(void)
{
#ifdef Debug
	__HAL_DBGMCU_FREEZE_IWDG();
#endif

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
	hiwdg.Init.Reload = 9;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		Error_Handler();
	}
}

//**************************Function Define***************************//
