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
#include "BL_Platform.h"
#include "BL_Transport.h"
#include "ASE_Interface.h"

uint8_t AES_CBC_128_Key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
uint8_t AES_CBC_128_IV[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
//**************************Include***************************//

//**************************Private Define***************************//
#define BL_UPDATE_TRANSPORT_TIMEOUT_MS BL_TRANSPORT_WAIT_FOREVER
//**************************Private Define***************************//

//**************************Function Define***************************//
static uint32_t BL_u32ReadAddressData(uint32_t address){
	uint32_t Local_u32AddressData = BL_Platform_ReadWord(address);
	return Local_u32AddressData;
}

static uint32_t BL_u32CalculateImageCrc(uint32_t image_address, uint32_t image_size_bytes)
{
	return BL_Platform_Crc32Words(image_address, image_size_bytes / WORD_SIZE_IN_BYTE);
}

static uint64_t BL_u64PackLittleEndian64(const uint8_t *data)
{
	return ((uint64_t)data[7] << SHIFT_56_BIT) |
		   ((uint64_t)data[6] << SHIFT_48_BIT) |
		   ((uint64_t)data[5] << SHIFT_40_BIT) |
		   ((uint64_t)data[4] << SHIFT_32_BIT) |
		   ((uint64_t)data[3] << SHIFT_24_BIT) |
		   ((uint64_t)data[2] << SHIFT_16_BIT) |
		   ((uint64_t)data[1] << SHIFT_8_BIT)  |
		   ((uint64_t)data[0] << SHIFT_0_BIT);
}

static BL_Status_t BL_Write16ByteBlock(uint32_t address, const uint8_t *data)
{
	BL_Status_t status;

	status = BL_Platform_WriteDoubleWord(address, BL_u64PackLittleEndian64(&data[0]));
	if (status != BL_STATUS_OK)
	{
		return status;
	}

	return BL_Platform_WriteDoubleWord(address + 8u, BL_u64PackLittleEndian64(&data[8]));
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
	uint32_t Local_u32SizeCode = BL_u32ReadAddressData(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS);
	uint32_t Local_u32CRC =  BL_u32ReadAddressData(FLAG_STATUS_CRC_ACTIVE_REGION_ADDRESS);
	uint32_t Local_u32CRC_check = BL_u32CalculateImageCrc(ACTIVE_IMAGE, Local_u32SizeCode);
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
	uint32_t Local_u32SizeCode = BL_u32ReadAddressData(FLAG_STATUS_SIZE_BACKUP_REGION_ADDRESS);
	uint32_t Local_u32CRC_Backup =  BL_u32ReadAddressData(FLAG_STATUS_CRC_BACKUP_REGION_ADDRESS);
	uint32_t Local_u32CRC_Check_Backup = BL_u32CalculateImageCrc(BACKUP_IMAGE, Local_u32SizeCode);
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

	return BR_IMAGE_IS_NOT_EXISTING;
}

void BL_voidJumpToActiveRegion(void)
{
	BL_Platform_JumpToImage(ACTIVE_IMAGE);
}

void BL_voidJumpToBootloader(void)
{
	//@TODO: In develop
	BL_voidUpdateHeaders();
	BL_voidReceiveUpdate();
}

void BL_voidCopyImageToActiveRegion(void)
{
	uint32_t Local_u32BackupDataAddress = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveDataAddress = BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32BackUpDataWord 	= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32BackupSizeInWord 	= BL_u32ReadAddressData(FLAG_STATUS_SIZE_BACKUP_REGION_ADDRESS);
	Local_u32BackupSizeInWord = Local_u32BackupSizeInWord / 4;
	uint32_t Local_u32CRC = BL_Platform_Crc32Words(BACKUP_IMAGE, Local_u32BackupSizeInWord);

	// Erase the Active region.
	BL_Platform_ErasePages(ACTIVE_IMAGE, FLASH_BANK_NUMOFPAGE);

	//Copy data from backup to active region.
	for(uint32_t Local_uint32Count = 0 ; Local_uint32Count  < Local_u32BackupSizeInWord ; Local_uint32Count++)
	{
		Local_u32ActiveDataAddress = (ACTIVE_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32BackupDataAddress = (BACKUP_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32BackUpDataWord    = BL_Platform_ReadWord(Local_u32BackupDataAddress);
		BL_Platform_WriteWord(Local_u32ActiveDataAddress, Local_u32BackUpDataWord);
	}

	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS , Local_u32BackupSizeInWord*4);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_CRC_ACTIVE_REGION_ADDRESS , Local_u32CRC);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS , BR_SET_IMAGE_ACTIVE );
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BACKUP_REGION_ADDRESS , BR_SET_IMAGE_BACKUP);
}

void BL_voidCopyImageToBackupRegion(void)
{
	uint32_t Local_u32BackupDataAddress 		= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveDataAddress 		= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveDataWord 			= BL_INITIALIZE_WITH_ZERO;
	uint32_t Local_u32ActiveSizeInWord 			= BL_u32ReadAddressData(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS);
	Local_u32ActiveSizeInWord = Local_u32ActiveSizeInWord / 4;
	uint32_t Local_u32CRC = BL_Platform_Crc32Words(ACTIVE_IMAGE, Local_u32ActiveSizeInWord);

	// Erase the Backup region.
	BL_Platform_ErasePages(BACKUP_IMAGE, FLASH_BANK_NUMOFPAGE);

	//Copy data from active region to backup region
	for(uint32_t Local_uint32Count = 0 ; Local_uint32Count  < Local_u32ActiveSizeInWord ; Local_uint32Count++)
	{
		Local_u32ActiveDataAddress = (ACTIVE_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32BackupDataAddress = (BACKUP_IMAGE + (WORD_SIZE_IN_BYTE * Local_uint32Count));
		Local_u32ActiveDataWord    = BL_Platform_ReadWord(Local_u32ActiveDataAddress);
		BL_Platform_WriteWord(Local_u32BackupDataAddress, Local_u32ActiveDataWord);
	}

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
		if( (Local_u32AddressCounter != Copy_u32Address) && (BL_Platform_ReadWord(Local_u32AddressCounter) != ERASED_VALUE))
		{
			Local_u32AddressArray[Local_u16DataIndex] = Local_u32AddressCounter;
			Local_u32DataArray[Local_u16DataIndex] = BL_Platform_ReadWord(Local_u32AddressCounter);
			Local_u16DataIndex++;
		}
		Local_u32AddressCounter = Local_u32AddressCounter + WORD_SIZE_IN_BYTE;
	}

	// Erase the Flag region.
	BL_Platform_ErasePages(FLAG_IMAGE, 1u);
	for (Local_u16DataCounter = 0 ; Local_u16DataCounter < Local_u16DataIndex ; Local_u16DataCounter++ )
	{
		BL_Platform_WriteWord(Local_u32AddressArray[Local_u16DataCounter], Local_u32DataArray[Local_u16DataCounter]);
	}
	BL_Platform_WriteWord(Copy_u32Address, Copy_u32NewData); //Replace new data to flash
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
	BL_Transport_SendUds(NODE_ID_TWO, UDS_MCU_ACKNOWLEDGE_UPGRADE_REQUEST);

	// Wait until FIFO 0 become pending state
	BL_Transport_ReceiveUds(&Local_u8HeaderFlag, BL_UPDATE_TRANSPORT_TIMEOUT_MS);

	if(Local_u8HeaderFlag == UDS_GWY_PROVIDE_HEADER)
	{
		BL_Transport_SendUds(NODE_ID_TWO, UDS_MCU_ACCEPT_RECEIVING_HEADER);
		// Wait until FIFO 0 become pending state
		BL_Transport_ReceiveFrame(Local_u8DataArray, 8u, BL_UPDATE_TRANSPORT_TIMEOUT_MS);

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
	uint32_t Local_u32InactiveImageAddressCounter                 = ACTIVE_IMAGE_START_ADDRESS;
	uint32_t Local_u32SizeOfCode 								  = BL_u32ReadAddressData(FLAG_STATUS_SIZE_ACTIVE_REGION_ADDRESS);
	uint8_t	 Local_u8HeaderFlag                					  = BL_INITIALIZE_WITH_ZERO;
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, AES_CBC_128_Key, AES_CBC_128_IV);

	// Erase the Active region.
	BL_Platform_ErasePages(ACTIVE_IMAGE, FLASH_BANK_NUMOFPAGE);

	//Structure CAN Transmit
	BL_Transport_SendUds(NODE_ID_TWO,UDS_MCU_ACKNOWLEDGE_HEADER_RECEIVED);
	//Loop to receive code update
	while(Local_u32SizeOfCode)
	{
		// Wait until FIFO 0 become pending state
		BL_Transport_ReceiveUds(&Local_u8HeaderFlag, BL_UPDATE_TRANSPORT_TIMEOUT_MS);
		//Check UDS ID
		if(Local_u8HeaderFlag == UDS_GWY_REQUEST_SENDING_LINE_OF_CODE)
		{
			BL_Transport_SendUds(NODE_ID_TWO, UDS_MCU_ACCEPT_RECEIVING_PACKET_OF_CODE);

			if (Local_u32SizeOfCode > SIZEOF_PACKAGE_FIRMWARE){
				BL_Transport_ReceiveBuffer(Local_u8DataArray, SIZEOF_PACKAGE_FIRMWARE, BL_UPDATE_TRANSPORT_TIMEOUT_MS);
				/*access piont to Encrypt SWC*/
				AES_CBC_decrypt_buffer(&ctx, Local_u8DataArray, 16);

				BL_Write16ByteBlock(Local_u32InactiveImageAddressCounter, Local_u8DataArray);

				Local_u32InactiveImageAddressCounter += 16;
				Local_u32SizeOfCode -= SIZEOF_PACKAGE_FIRMWARE;
			}
			else{
				BL_Transport_ReceiveBuffer(Local_u8DataArray, SIZEOF_PACKAGE_FIRMWARE, BL_UPDATE_TRANSPORT_TIMEOUT_MS);
				/*access piont to Encrypt SWC*/
				AES_CBC_decrypt_buffer(&ctx, Local_u8DataArray, 16);

				BL_Write16ByteBlock(Local_u32InactiveImageAddressCounter, Local_u8DataArray);

				Local_u32SizeOfCode -= Local_u32SizeOfCode ;
			}
			BL_Transport_SendUds(NODE_ID_TWO, UDS_MCU_ACKNOWLEDGE_LINE_OF_CODE_RECEIVED);
		}
	}
	// Wait for finish code of GW
	BL_Transport_ReceiveUds(&Local_u8HeaderFlag, BL_UPDATE_TRANSPORT_TIMEOUT_MS);

	if (Local_u8HeaderFlag == UDS_GWY_ACKNOWLEDGE_FINISHING_SENDING_CODE)
	{
		BL_voidFinishBootLoader();
	}
}

void BL_voidFinishBootLoader(void)
{
	//Structure CAN Transmit
	BL_Transport_SendUds(NODE_ID_TWO, UDS_MCU_ACKNOWLEDGE_FINISHING);

	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_ACTIVE_REGION_ADDRESS , BR_SET_IMAGE_ACTIVE);
	BL_voidEraseRestoreHeaderPage(FLAG_STATUS_BOOTLOADER , BL_RESET_BRANCHING_FLAG);
	BL_voidMakeSoftWareReset();
}

void BL_voidMakeSoftWareReset(void)
{
	BL_Platform_RequestReset();
}

//**************************Function Define***************************//
