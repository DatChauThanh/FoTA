#ifndef GW_PLATFORM_H_
#define GW_PLATFORM_H_

#include "GW_Types.h"

#define GW_PLATFORM_WAIT_FOREVER 0xFFFFFFFFu

typedef enum
{
	GW_BUTTON_RELEASED = 0,
	GW_BUTTON_PRESSED
} GW_ButtonState_t;

GW_Status_t GW_Platform_Init(void);

GW_Status_t GW_Platform_TelematicsReceiveIt(uint8_t *data, uint16_t length);
GW_Status_t GW_Platform_TelematicsReceive(uint8_t *data, uint16_t length, uint32_t timeout_ms);
GW_Status_t GW_Platform_TelematicsTransmit(const uint8_t *data, uint16_t length, uint32_t timeout_ms);
void GW_Platform_TelematicsEnableRxInterrupt(void);
void GW_Platform_TelematicsDisableRxInterrupt(void);

uint32_t GW_Platform_Crc32Words(uint32_t start_address, uint32_t word_count);
void GW_Platform_DelayMs(uint32_t delay_ms);
void GW_Platform_RequestReset(void);
GW_ButtonState_t GW_Platform_GetSwitchButton(void);
GW_ButtonState_t GW_Platform_GetOkButton(void);
void GW_Platform_FatalError(void);

#endif /* GW_PLATFORM_H_ */
