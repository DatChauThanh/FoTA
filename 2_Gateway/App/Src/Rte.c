/*****************************************************/
 /* Author      : Mosad                              */
 /* Version     : v01                                */
 /* date        : 29/4/2020                          */
/*****************************************************/

/* Module includes */
#include "Rte.h"

/* Local Data Types */

/* Local Macros */
#define INITIAL_VALUE 0x0u

/*Local Port Status Macros*/
#define IDLE          0x0u
#define BUSY          0x1u

/* Rte Golable Variables */
static uint32_t Global_CrcValue                         = INITIAL_VALUE ;
static uint32_t Global_CodeSizeValue                    = INITIAL_VALUE ;
static uint8_t  Global_NodeId                           = INITIAL_VALUE ;
static FlagType Global_HeaderAckFlag                  	= INITIAL_VALUE ;
static uint8_t *Global_EncryptedDataBufferPtr           = NULL_PTR	 	;
static FlagType Global_EncryptedDataBufferFlag        	= INITIAL_VALUE ;
static SystemStateType Global_SystemStateMachine      	= INITIAL_VALUE ;
static uint8_t Global_UserResponse                      = INITIAL_VALUE ;
static uint8_t Global_UpdateProgress                    = INITIAL_VALUE ;

/* Rte protection flag */
static PortStateType Global_CrcPortState                   = IDLE;
static PortStateType Global_CodeSizePortState              = IDLE;
static PortStateType Global_NodeIdPortState                = IDLE;
static PortStateType Global_HeaderAckFlagState             = IDLE;
static PortStateType Global_EncryptedDataBufferState       = IDLE;
static PortStateType Global_EncryptedDataBufferFlagState   = IDLE;
static PortStateType Global_SystemStateMachineState        = IDLE;
static PortStateType Global_UserResponsePortState          = IDLE;
static PortStateType Global_UpdateProgressPortState        = IDLE;

/**************************************************************************/
/*                         NodeId Port                                    */
/**************************************************************************/
Std_ReturnType Rte_WriteNodeId (uint8_t Cpy_NodeId)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_NodeIdPortState == IDLE)
	{
		/* Lock the port to write */
		Global_NodeIdPortState = BUSY ;
		/* Write the data to the port */
		Global_NodeId = Cpy_NodeId ; 
		/* Unlock the port after done writing */
		Global_NodeIdPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadNodeId (uint8_t *Cpy_NodeId)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_NodeIdPortState == IDLE) && (Cpy_NodeId != NULL_PTR))
	{
		/* Lock the port to write */
		Global_NodeIdPortState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_NodeId) = Global_NodeId ; 
		/* Unlock the port after done writing */
		Global_NodeIdPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         CodeSize Port                                  */
/**************************************************************************/
Std_ReturnType Rte_WriteCodeSize(uint32_t Cpy_CodeSize)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_CodeSizePortState == IDLE)
	{
		/* Lock the port to write */
		Global_CodeSizePortState = BUSY ;
		/* Write the data to the port */
		Global_CodeSizeValue = Cpy_CodeSize ; 
		/* Unlock the port after done writing */
		Global_CodeSizePortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadCodeSize(uint32_t *Cpy_CodeSize)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_CodeSizePortState == IDLE) && (Cpy_CodeSize != NULL_PTR))
	{
		/* Lock the port to write */
		Global_CodeSizePortState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_CodeSize) = Global_CodeSizeValue ; 
		/* Unlock the port after done writing */
		Global_CodeSizePortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         Crc Port                                       */
/**************************************************************************/
Std_ReturnType Rte_WriteCrc(uint32_t Cpy_Crc)
{
	Std_ReturnType Local_ReturnError = E_OK ;

	if (Global_CrcPortState == IDLE)
	{
		/* Lock the port to write */
		Global_CrcPortState = BUSY ;
		/* Write the data to the port */
		Global_CrcValue = Cpy_Crc ;
		/* Unlock the port after done writing */
		Global_CrcPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadCrc(uint32_t *Cpy_Crc)
{
	Std_ReturnType Local_ReturnError = E_OK ;

	if ( (Global_CrcPortState == IDLE) && (Cpy_Crc != NULL_PTR) )
	{
		/* Lock the port to write */
		Global_CrcPortState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_Crc) = Global_CrcValue ;
		/* Unlock the port after done writing */
		Global_CrcPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         Header Ack Flag Port                           */
/**************************************************************************/
Std_ReturnType Rte_WriteHeaderAckFlag(FlagType Cpy_HeaderAckFlag)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_HeaderAckFlagState == IDLE)
	{
		/* Lock the port to write */
		Global_HeaderAckFlagState = BUSY ;
		/* Write the data to the port */
		Global_HeaderAckFlag = Cpy_HeaderAckFlag ; 
		/* Unlock the port after done writing */
		Global_HeaderAckFlagState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadHeaderAckFlag(FlagType *Cpy_HeaderAckFlag)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_HeaderAckFlagState == IDLE) && (Cpy_HeaderAckFlag != NULL_PTR) )
	{
		/* Lock the port to write */
		Global_HeaderAckFlagState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_HeaderAckFlag) = Global_HeaderAckFlag ; 
		/* Unlock the port after done writing */
		Global_HeaderAckFlagState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         Encrypted Data Buffer Port                     */
/**************************************************************************/
Std_ReturnType Rte_WriteEncryptedDataBuffer(uint8_t *Cpy_EncryptedDataBufferPtr)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ((Global_EncryptedDataBufferState == IDLE)&& (Cpy_EncryptedDataBufferPtr != NULL_PTR))
	{
		/* Lock the port to write */
		Global_EncryptedDataBufferState = BUSY ;
		/* Write the data to the port */
		Global_EncryptedDataBufferPtr = Cpy_EncryptedDataBufferPtr ;
		/* Unlock the port after done writing */
		Global_EncryptedDataBufferState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadEncryptedDataBuffer(uint8_t **Cpy_EncryptedDataBufferPtr)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_EncryptedDataBufferState == IDLE) && (Cpy_EncryptedDataBufferPtr != NULL_PTR))
	{
		/* Lock the port to write */
		Global_EncryptedDataBufferState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_EncryptedDataBufferPtr) = Global_EncryptedDataBufferPtr ;
		/* Unlock the port after done writing */
		Global_EncryptedDataBufferState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         Encrypted Data Buffer Flag Port                */
/**************************************************************************/
Std_ReturnType Rte_WriteEncryptedDataBufferFlag(FlagType Cpy_EncryptedDataBufferFlag)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_EncryptedDataBufferFlagState == IDLE)
	{
		/* Lock the port to write */
		Global_EncryptedDataBufferFlagState = BUSY ;
		/* Write the data to the port */
		Global_EncryptedDataBufferFlag = Cpy_EncryptedDataBufferFlag ;
		/* Unlock the port after done writing */
		Global_EncryptedDataBufferFlagState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadEncryptedDataBufferFlag(FlagType *Cpy_EncryptedDataBufferFlag)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_EncryptedDataBufferFlagState == IDLE) && (Cpy_EncryptedDataBufferFlag != NULL_PTR) )
	{
		/* Lock the port to write */
		Global_EncryptedDataBufferFlagState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_EncryptedDataBufferFlag) = Global_EncryptedDataBufferFlag ;
		/* Unlock the port after done writing */
		Global_EncryptedDataBufferFlagState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         SystemState Port                               */
/**************************************************************************/
Std_ReturnType Rte_WriteSystemState(SystemStateType Cpy_SystemState)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_SystemStateMachineState == IDLE)
	{
		/* Lock the port to write */
		Global_SystemStateMachineState = BUSY ;
		/* Write the data to the port */
		Global_SystemStateMachine = Cpy_SystemState ; 
		/* Unlock the port after done writing */
		Global_SystemStateMachineState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadSystemState(SystemStateType *Cpy_SystemState)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_SystemStateMachineState == IDLE) && (Cpy_SystemState != NULL_PTR))
	{
		/* Lock the port to write */
		Global_SystemStateMachineState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_SystemState) = Global_SystemStateMachine ; 
		/* Unlock the port after done writing */
		Global_SystemStateMachineState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         UserResponse Port                              */
/**************************************************************************/
Std_ReturnType Rte_WriteUserResponse(uint8_t Cpy_UserResponse)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_UserResponsePortState == IDLE)
	{
		/* Lock the port to write */
		Global_UserResponsePortState = BUSY ;
		/* Write the data to the port */
		Global_UserResponse = Cpy_UserResponse ; 
		/* Unlock the port after done writing */
		Global_UserResponsePortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadUserResponse(uint8_t *Cpy_UserResponse)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_UserResponsePortState == IDLE) && (Cpy_UserResponse != NULL_PTR) )
	{
		/* Lock the port to write */
		Global_UserResponsePortState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_UserResponse) = Global_UserResponse ; 
		/* Unlock the port after done writing */
		Global_UserResponsePortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

/**************************************************************************/
/*                         Update Progress Port                           */
/**************************************************************************/
Std_ReturnType Rte_WriteUpdateProgress(uint8_t Cpy_UpdateProgress)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_UpdateProgressPortState == IDLE)
	{
		/* Lock the port to write */
		Global_UpdateProgressPortState = BUSY ;
		/* Write the data to the port */
		Global_UpdateProgress = Cpy_UpdateProgress ; 
		/* Unlock the port after done writing */
		Global_UpdateProgressPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadUpdateProgress(uint8_t *Cpy_UpdateProgress)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_UpdateProgressPortState == IDLE) && (Cpy_UpdateProgress != NULL_PTR) )
	{
		/* Lock the port to write */
		Global_UpdateProgressPortState = BUSY ;
		/* Write the data to the port */
		 (*Cpy_UpdateProgress) = Global_UpdateProgress ; 
		/* Unlock the port after done writing */
		Global_UpdateProgressPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

