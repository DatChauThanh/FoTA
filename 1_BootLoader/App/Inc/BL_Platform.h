#ifndef BL_PLATFORM_H_
#define BL_PLATFORM_H_

#include "BL_Types.h"

BL_Status_t BL_Platform_Init(void);

uint32_t BL_Platform_ReadWord(uint32_t address);
uint32_t BL_Platform_Crc32Words(uint32_t start_address, uint32_t word_count);

BL_Status_t BL_Platform_ErasePages(uint32_t start_address, uint32_t page_count);
BL_Status_t BL_Platform_WriteWord(uint32_t address, uint32_t data);
BL_Status_t BL_Platform_WriteDoubleWord(uint32_t address, uint64_t data);

void BL_Platform_JumpToImage(uint32_t image_base_address);
void BL_Platform_RequestReset(void);
void BL_Platform_FatalError(void);

#endif /* BL_PLATFORM_H_ */
