/*******************************************************************************
* Title                 :   Transmit 
* Filename              :   Transmit_program.c
* Author                :   Mohamed Hafez
* Modifier				: 	Chau Thanh Dat
* Origin Date           :   30/04/2021
* Version               :   1.0.1
* Compiler              :   ARM V6
* Target                :   STM32F103
* Notes                 :   None 
*
*****************************************************************************/
/*************** SOURCE REVISION LOG *****************************************
*
*    Date    Version   Author          Description 
*  30/04/21   1.0.0   Mohamed Hafez   Initial Release.
*  04/03/24	  1.0.1	  Chau Thanh Dat    HAL
*******************************************************************************/
/** @file Transmit_program.c
 *  @brief This is the source file for transmit to nodes connected on can bus.
 */
/******************************************************************************
* Includes
*******************************************************************************/	
#include "Rte_DataTypes.h"
#include "Rte_SendUpdatePort.h"
#include "Transmit.h"
#include "Transmit_private.h"
#include "CAN_IF.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static uint32_t               Static_uint32CodeSize;
static uint8_t                Static_uint8NodeId;
static uint16_t               Static_uint16PacketsCounter;
static uint16_t               Static_uint16NumberOfPackets;
static uint8_t                Static_uint8NumOfBytesInLastPacket;

static TransmitStates_t     Static_StateVariable;
static State_PtrToFunction  Static_ArrayOfStates[NUBER_OF_STATES];
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/*****************************Module Function Definitions.*********************/

Std_ReturnType Transmit_InitializeModule(void)
{
   // Initalize static variable in this module
   Static_uint32CodeSize                       = INITIALIZE_WITH_ZERO;
   Static_uint8NodeId                          = INITIALIZE_WITH_ZERO;
   Static_uint16PacketsCounter                 = INITIALIZE_WITH_ZERO;
   Static_uint16NumberOfPackets                = INITIALIZE_WITH_ZERO;
   Static_uint8NumOfBytesInLastPacket          = INITIALIZE_WITH_ZERO;
   // Initialize current state variable
   Static_StateVariable                        = IDLE_STATE;
   // Fill state array with function represent each state
   Static_ArrayOfStates[IDLE_STATE]            = Transmit_IdleState;
   Static_ArrayOfStates[GET_TRANSMIT_HEADER]   = Transmit_GetTransmitHeader;
   Static_ArrayOfStates[CONSUME_TRANSMIT_DATA] = Transmit_ConsumeTransmitData;
   Static_ArrayOfStates[FINISHING_STATE]       = Transmit_FinishingState;

   return E_OK;
}

Std_ReturnType Transmit_MainFunction(void)
{
   Std_ReturnType       Local_ReturnStatus = E_OK;
   State_PtrToFunction  RunState;
   RunState           = Static_ArrayOfStates[Static_StateVariable];
   Local_ReturnStatus = RunState(NULL_PTR);
   return Local_ReturnStatus;
}

/*****************************Private Functions' Definitions*******************************/

/*****************************Functions Represent states***********************************/
static Std_ReturnType Transmit_IdleState(void *Cpy_voidPtr)
{
   Std_ReturnType Local_ReturnStatus       = E_OK;
   FlagType       Local_uint8HeaderFlagValue  = HEADER_NOT_SET;
   // Get Header Flag Value.
   Local_ReturnStatus = RTE_READ_HEADER_ACK_FLAG(&Local_uint8HeaderFlagValue);
   if(E_OK == Local_ReturnStatus)
   {
      // Check The Header Flag Value
      if(HEADER_SET == Local_uint8HeaderFlagValue)
      {
         // Go to Get and Transmit Header State.
         Static_StateVariable = GET_TRANSMIT_HEADER;
      }
      else
      {
         Local_ReturnStatus = HEADER_NOT_SET;
      }
   }
   else
   {
      Local_ReturnStatus = E_NOT_OK;
   }
   // Just remove unused parameter warning
   UNUSED(Cpy_voidPtr);
   return Local_ReturnStatus;
}

static Std_ReturnType Transmit_GetTransmitHeader(void *Cpy_voidPtr)
{
   Std_ReturnType Local_ReturnStatus                            = E_OK;
   uint8_t        Local_HeaderBuffer[HEADER_INFORMATION_SIZE]   = {INITIALIZE_WITH_ZERO};
   uint8_t        Local_uint8NodeId                           	= INITIALIZE_WITH_ZERO;
   uint32_t       Local_uint32CodeSize                       	= INITIALIZE_WITH_ZERO;
   uint32_t       Local_uint32CrcValue                      	= INITIALIZE_WITH_ZERO;
   uint8_t        Local_uint8ReceivedAck                        = INITIALIZE_WITH_ZERO;
   // Consume Header Information.     
   Transmit_ConsumeHeader(&Local_uint8NodeId, &Local_uint32CodeSize, &Local_uint32CrcValue);
   // Save Size and Node Id.
   Transmit_SaveHeader(&Local_uint8NodeId, &Local_uint32CodeSize);
   // Handle header information
   Transmit_HandleHeader(Local_uint32CodeSize, Local_uint32CrcValue, Local_HeaderBuffer);
   /************************Sequence Between Gateway and Boot_loader********************/

//   // Request Program control.
//   CAN_IF_Transmit_UDS_Request(Local_uint8NodeId, UDS_CONTROL_SESSION);
//   // Wait Ack from BL
//   CAN_IF_Receive_UDS_Respond(&Local_uint8ReceivedAck);
//
//   // Check received Ack
//   if(UDS_MCU_ACCEPT_UPGRADE_REQUEST == Local_uint8ReceivedAck)
//   {
      // Request sending Header.
      CAN_IF_Transmit_UDS_Request(Local_uint8NodeId, UDS_GWY_REQUEST_SENDING_HEADER);

      // Wait Ack from BL
      CAN_IF_Receive_UDS_Respond(&Local_uint8ReceivedAck);

      if (UDS_MCU_ACCEPT_RECEIVING_HEADER == Local_uint8ReceivedAck)
      {
         // Sending header.
    	 CAN_IF_Trasmit_Data_Frame(Local_uint8NodeId, Local_HeaderBuffer, HEADER_INFORMATION_SIZE);
      }
      else
      {
         //TODO: Handle the situation that the Boot_loader Doesn't reply correctly.
      }
//   }
//   else
//   {
//      //TODO: Handle the situation that the Boot_loader Doesn't reply correctly.
//   }
   // Wait Ack from BL
   CAN_IF_Receive_UDS_Respond(&Local_uint8ReceivedAck);

   // Check received Ack
   if(UDS_MCU_ACKNOWLEDGE_HEADER_RECEIVED == Local_uint8ReceivedAck)
   {
	   Static_StateVariable = CONSUME_TRANSMIT_DATA;
   }
   else
   {
      //TODO: Handle the situation that the Boot_loader Doesn't reply correctly.
   }
   // Just remove unused parameter warning
   UNUSED(Cpy_voidPtr);
   return Local_ReturnStatus;
}

static Std_ReturnType Transmit_ConsumeTransmitData(void *Cpy_voidPtr)
{    
   FlagType         Local_uint8BufferFlagValue              = BUFFER_NOT_SET;
   uint8_t          *Local_uint8DataBuffer                  = NULL_PTR ;
   uint8_t          Local_uint8ReceivedAck                  = INITIALIZE_WITH_ZERO;

   // Calculate number of packets and number of bytes in the last packet.
   Static_uint16NumberOfPackets            = (Static_uint32CodeSize) / (DATA_BUFFER_SIZE);
   Static_uint8NumOfBytesInLastPacket      = (Static_uint32CodeSize) % (DATA_BUFFER_SIZE);
   // Get Buffer Flag Value.
   RTE_READ_DECRYPTED_DATA_BUFFER_FLAG(&Local_uint8BufferFlagValue);
   // Check The Buffer Flag Value
   if(BUFFER_SET == Local_uint8BufferFlagValue)
   {
      // Consume Data and Increase packet counter by 1
      RTE_READ_DECRYPTED_DATA_BUFFER(&Local_uint8DataBuffer);
      // Request Sending line of code code.
      CAN_IF_Transmit_UDS_Request(Static_uint8NodeId, UDS_GWY_REQUEST_SENDING_PACKET_OF_CODE);
      // Wait Ack from BL
      CAN_IF_Receive_UDS_Respond(&Local_uint8ReceivedAck);
      // Check The node ack
      if(UDS_MCU_ACCEPT_RECEIVING_PACKET_OF_CODE == Local_uint8ReceivedAck)
      {
         // condition to know are we will send the last packet or ordinary packet.
         if(Static_uint16PacketsCounter < Static_uint16NumberOfPackets)
         {
            // Sending 64 byte of DATA_BUFFER_SIZE
        	CAN_IF_Trasmit_Data_Frame(Static_uint8NodeId, Local_uint8DataBuffer, DATA_BUFFER_SIZE);
            // Wait Ack from BL
        	CAN_IF_Receive_UDS_Respond(&Local_uint8ReceivedAck);
            if(UDS_MCU_ACKNOWLEDGE_PACKET_OF_CODE_RECEIVED == Local_uint8ReceivedAck)
            {
               // Increase packet counter by 1
				Static_uint16PacketsCounter++;
				if ((Static_uint16PacketsCounter == Static_uint16NumberOfPackets)
					&& (Static_uint8NumOfBytesInLastPacket == ZERO_BYTE))
				{
					Static_StateVariable = FINISHING_STATE;
				}
				else
				{
					// Reset Buffer flag
					RTE_WRITE_DECRYPTED_DATA_BUFFER_FLAG(RESET_FLAG);
					// Change System State To De_crypt state.
					RTE_WRITE_SYSTEM_STATE(SYS_DECRYPT);
				}
            }
            else
            {
               //TODO: Handle the situation that the Boot_loader Doesn't reply correctly.
            }
         }
         else if (Static_uint8NumOfBytesInLastPacket != 0)
         {
            // Sending the rest of data
        	CAN_IF_Trasmit_Data_Frame(Static_uint8NodeId, Local_uint8DataBuffer, Static_uint8NumOfBytesInLastPacket);
            // Wait Ack from BL
            CAN_IF_Receive_UDS_Respond(&Local_uint8ReceivedAck);
            if(UDS_MCU_ACKNOWLEDGE_PACKET_OF_CODE_RECEIVED == Local_uint8ReceivedAck)
            {
               // Go to Finish State.
               Static_StateVariable = FINISHING_STATE;
            }
            else
            {
               //TODO: Handle the situation that the Boot_loader Doesn't reply correctly.
            }
         }
		 else
		 {
				/**/
		 }
      } 
      else
      {
         //TODO: Handle the situation that the Boot_loader Doesn't reply correctly.
      } 
   }  
   // Just remove unused parameter warning
   UNUSED(Cpy_voidPtr);
   return E_OK;
}

static Std_ReturnType Transmit_FinishingState(void *Cpy_voidPtr)
{
   uint8_t Local_uint8_tReceivedAck = INITIALIZE_WITH_ZERO;
   // Reset Buffer flag
   RTE_WRITE_DECRYPTED_DATA_BUFFER_FLAG(RESET_FLAG);
   // Ack node of the end of code transmission
   CAN_IF_Transmit_UDS_Request(Static_uint8NodeId, UDS_GWY_ACKNOWLEDGE_FINISHING_SENDING_CODE);
   // Wait Ack from BL
   CAN_IF_Receive_UDS_Respond(&Local_uint8_tReceivedAck);
   if(UDS_MCU_ACKNOWLEDGE_RECEIVED_CODE == Local_uint8_tReceivedAck)
   {
      // Go To Idle State Here
      Static_StateVariable = IDLE_STATE;
      // Change System State To System Done Update
      RTE_WRITE_SYSTEM_STATE(SYS_DONE_UPDATE);
   }
	 	// Reinit variables 
   Static_uint32CodeSize              = INITIALIZE_WITH_ZERO;
   Static_uint8NodeId                 = INITIALIZE_WITH_ZERO;
   Static_uint16PacketsCounter        = INITIALIZE_WITH_ZERO;
   Static_uint16NumberOfPackets       = INITIALIZE_WITH_ZERO;
   Static_uint8NumOfBytesInLastPacket = INITIALIZE_WITH_ZERO;
   // Just remove unused parameter warning
   UNUSED(Cpy_voidPtr);
   return E_OK;
}

/*******************************Auxiliary Private Function***************************/

static Std_ReturnType Transmit_ConsumeHeader(uint8_t *Cpy_NodeId,uint32_t *Cpy_Size,uint32_t *Cpy_Crc)
{
   Std_ReturnType Local_ReturnStatus = E_OK;
   // Consume Header Information.     
   Local_ReturnStatus                = RTE_READ_NODE_ID  (Cpy_NodeId);
   Local_ReturnStatus                = RTE_READ_CODE_SIZE(Cpy_Size);
   Local_ReturnStatus                = RTE_READ_CRC_VALUE(Cpy_Crc);
   return Local_ReturnStatus;
}

static Std_ReturnType Transmit_HandleHeader(uint32_t Cpy_Size,uint32_t Cpy_Crc,uint8_t *Cpy_PtrToBytes)
{
   // Convert code size into bytes and store it in buffer.
   Cpy_PtrToBytes[FIRST_BYTE]   = (Cpy_Size & GET_BYTE);
   Cpy_PtrToBytes[SECOND_BYTE]  = ((Cpy_Size >> SHIFT_TO_GET_SECOND_BYTE)  & GET_BYTE);
   Cpy_PtrToBytes[THIRD_BYTE]   = ((Cpy_Size >> SHIFT_TO_GET_THIRD_BYTE)   & GET_BYTE);
   Cpy_PtrToBytes[FOURTH_BYTE]  = ((Cpy_Size >> SHIFT_TO_GET_FOURTH_BYTE)  & GET_BYTE);
   // Convert CRC Value into bytes and store it in buffer.
   Cpy_PtrToBytes[FIFTH_BYTE]   = (Cpy_Crc  & GET_BYTE);
   Cpy_PtrToBytes[SIXTH_BYTE]   = ((Cpy_Crc  >> SHIFT_TO_GET_SECOND_BYTE)  & GET_BYTE);
   Cpy_PtrToBytes[SEVENTH_BYTE] = ((Cpy_Crc  >> SHIFT_TO_GET_THIRD_BYTE)   & GET_BYTE);
   Cpy_PtrToBytes[EIGHTH_BYTE]  = ((Cpy_Crc  >> SHIFT_TO_GET_FOURTH_BYTE)  & GET_BYTE);
   return E_OK;
}

static Std_ReturnType Transmit_SaveHeader(uint8_t *Cpy_NodeId,uint32_t *Cpy_Size)
{
   // Store Node Id To Use It to determinr the node.
   Static_uint8NodeId    = *Cpy_NodeId;
   // Store Size To Use It to calculate number of packts.
   Static_uint32CodeSize = *Cpy_Size;
   return E_OK;
}

/*************** END OF FUNCTIONS ***************************************************************************/
