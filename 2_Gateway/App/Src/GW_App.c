#include "GW_App.h"

#include "Encrypt_Interface.h"
#include "ReceiveUpdate_Interface.h"
#include "Rte.h"
#include "Transmit_Interface.h"
#include "UserInterface_Interface.h"

void GW_App_Init(void)
{
	ReceiveUpdate_InitializeModule();
	Transmit_InitializeModule();
	Encrypt_Address_Read_Init();
	UserInterface_InitializeModule();
}

void GW_App_MainFunction(void)
{
	uint8_t state = SYS_IDLE;

	RTE_READ_SYSTEM_STATE(&state);
	if (state == SYS_REC_UPDATE)
	{
		ReceiveUpdate_MainFunction();
	}
	else if (state == SYS_ENCRYPT)
	{
		Encrypt_MainFunction();
	}
	else if (state == SYS_SEND_UPDATE)
	{
		Transmit_MainFunction();
	}
	else
	{
		/* Idle or transition state. */
	}

	UserInterface_MainFunction();
}
