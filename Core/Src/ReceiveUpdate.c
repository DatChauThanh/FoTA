/*****************************************************/
 /* Author      : Chau Thanh Dat                     */
 /* Version     : v01                                */
 /* date        : 13/3/2024                          */
/*****************************************************/
/* Library includes */
#include "Rte_ReceiveUpatePort.h"
#include "ReceiveUpdate_Interface.h"
#include "ReceiveUpdate_Private.h"
#include "FlashReprogram_Interface.h"
/**************************************************************************/
/*                         Global Variables                              */
/**************************************************************************/
static uint32_t Global_CrcValue ;
static uint32_t Global_SizeValue ;
static uint8_t Global_NodeId ;
static SystemStateType Global_SystemState ;
static ReceiveUpdateStateType Global_RxInternalSate ;
static uint8_t Global_HeaderBuffer[5];
static uint8_t Global_HeaderReqByte;
static uint8_t Global_NumberOfPackets ;
static uint16_t Global_RemainingBytes ;
static uint8_t Global_RxUserResponse;
static uint8_t Global_RxBuffer[PACKET_SIZE];
static float Global_DownloadPercentage;
static uint8_t Global_DownloadUpdateProgeress;
static uint32_t Global_ReceivedBytes;

/**************************************************************************/
/*                         Module Functions                               */
/**************************************************************************/

void ReceiveUpdate_voidInit(void)
{
	/* Init Variables */
	Global_CrcValue 				= INITIAL_VALUE ;
	Global_SizeValue 				= INITIAL_VALUE ;
	Global_NodeId 					= INITIAL_VALUE ;
	Global_SystemState 				= SYS_IDLE ;
	Global_RxInternalSate 			= RX_IDLE ;
	Global_NumberOfPackets			= INITIAL_VALUE ;
	Global_RemainingBytes 			= INITIAL_VALUE;
	Global_RxUserResponse 			= INITIAL_VALUE;
	Global_DownloadPercentage 		= INITIAL_VALUE ;
	Global_DownloadUpdateProgeress 	= INITIAL_VALUE ;
	Global_ReceivedBytes 			= INITIAL_VALUE;
	/* Init ESP (uart) interrupt to receive requests */
	HAL_UART_Receive_IT(&huart1 , Global_HeaderBuffer , 1);
	FR_voidInitVariables();
}

void ReceiveUpdate_MainFunction (void)
{
	switch(Global_RxInternalSate)
	{
		case RX_IDLE:
		{
			uint8_t Local_Error = RTE_READ_USER_RESPONSE(&Global_RxUserResponse);
			if(Local_Error == E_OK)
			{
				if (Global_RxUserResponse == ACCEPT_UPDATE)
				{
					Global_RxInternalSate = RX_ACCEPT_UPDATE ;
				}
				else if (Global_RxUserResponse == REFUSE_UPDATE)
				{
					Global_RxInternalSate = RX_REFUSE_UPDATE ;
				}
				else
				{
					//Do nothing
				}
			}
			else
			{
				//Report ...
			}
		}
		/*****************************RX_ACCEPT_UPDATE ***********************************/
		case RX_ACCEPT_UPDATE :
		{
			/* Inform ESP to accept request */
			Global_HeaderReqByte = REQUEST_ACCEPTED;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);
			/* Change Internal State */
			Global_RxInternalSate = RX_RECEIVE_HEADER ;
			break;
		}
		/*****************************RX_REFUSE_UPDATE***********************************/
		case RX_REFUSE_UPDATE :
		{
			/* Inform ESP to Reject request */
			Global_HeaderReqByte = REQUEST_REFUSED;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);
			/* Initialize */
			Global_RxUserResponse = INITIAL_VALUE;
			/* Change System state */
			RTE_WRITE_SYSTEM_STATE(SYS_IDLE);
			/* Enable Uart interrupt */
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
			/* Change Internal State */
			Global_RxInternalSate = RX_IDLE ;
			break;
		}
		/*****************************RX_RECEIVE_HEADER***********************************/
		case RX_RECEIVE_HEADER :
		{
			/* Inform ESP to send Header */
			Global_HeaderReqByte = ESP_SEND_HEADER;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);

			/* Receive Header */
			HAL_UART_Receive(&huart1, Global_HeaderBuffer, HEADER_SIZE, HAL_MAX_DELAY);

			/* Process header */
			Global_SizeValue = Global_HeaderBuffer[0] | (Global_HeaderBuffer[1] << SHIFT_BY_8) |(Global_HeaderBuffer[2] << SHIFT_BY_16) | (Global_HeaderBuffer[3] << SHIFT_BY_24);
			Global_NodeId = Global_HeaderBuffer[4];

			/* Write Data To RTE */
			RTE_WRITE_CODE_SIZE(Global_SizeValue);
			RTE_WRITE_NODE_ID(Global_NodeId);
			RTE_WRITE_HEADER_ACK_FLAG(HEADER_SET);

			/* Get info from header */
			Global_NumberOfPackets  = (uint8_t)(Global_SizeValue / PACKET_SIZE) ;
			Global_RemainingBytes = (Global_SizeValue % PACKET_SIZE);

			/* Ack Header */
			Global_HeaderReqByte = HEADER_RECEIVED;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);
			/*Erase Image to flash new firmware from Telematic unit*/
			FR_Erase_Image(IMAGE_NEW_FIRMWARE);
			/* Change State */
			Global_RxInternalSate = RX_RECEIVE_PACKET ;

			break;
		}
		case RX_RECEIVE_PACKET:
		{
			/* Inform ESP to send Package */
			Global_HeaderReqByte = SEND_NEXT_PACKET;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);

			if (Global_NumberOfPackets > 0)
			{
				/* Receive packet */
				HAL_UART_Receive(&huart1, Global_RxBuffer, PACKET_SIZE, HAL_MAX_DELAY);
				Global_NumberOfPackets--;
				/* Store Packet */
				FR_FlashBlockToAddress(Global_RxBuffer , PACKET_SIZE);
				/* Ack Packet */
				Global_HeaderReqByte = PACKET_RECEIVED;
				HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);

				/* Update Received byte to calculate progress */
				Global_ReceivedBytes += PACKET_SIZE ;
			}

			else if ((Global_NumberOfPackets == 0) && (Global_RemainingBytes > 0) )
			{
				/* Receive packet */
				HAL_UART_Receive(&huart1, Global_RxBuffer, Global_RemainingBytes, HAL_MAX_DELAY);
				/* Store Packet */
				FR_FlashBlockToAddress(Global_RxBuffer , Global_RemainingBytes);
				/* Ack last packet */
				Global_HeaderReqByte = LAST_PACKET_RECEIVED;
				HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);
				/* Change Internal state */
				Global_RxInternalSate = RX_END_STATE ;
				/* Update Received byte to calculate progress */
				Global_ReceivedBytes += Global_RemainingBytes ;
			}

			else
			{
				/* Error */
			}
			/*Calculate progress */
			Global_DownloadPercentage = ((float)Global_ReceivedBytes /(float) Global_SizeValue) ;
			Global_DownloadUpdateProgeress = Global_DownloadPercentage * 100 ;
			/* Write to RTE to Signal UserIntrface Module */
			RTE_WRITE_DOWNLOAD_PROGRESS (Global_DownloadUpdateProgeress);
			break;
		}
		/*****************************RX_END_STATE***********************************/
		case RX_END_STATE :
		{
			/* Inform the Esp Code has been received */
			Global_HeaderReqByte = ESP_DOWNLOAD_DONE;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);

			/* Change System state */
			RTE_WRITE_SYSTEM_STATE(SYS_DECRYPT);

			/* Reset Variables */
			Global_CrcValue = INITIAL_VALUE;
			Global_SizeValue = INITIAL_VALUE;
			Global_NodeId = INITIAL_VALUE;
			Global_NumberOfPackets = INITIAL_VALUE;
			Global_RemainingBytes = INITIAL_VALUE;
			Global_RxUserResponse = INITIAL_VALUE;
			Global_DownloadPercentage = INITIAL_VALUE;
			Global_DownloadUpdateProgeress = INITIAL_VALUE;
			Global_ReceivedBytes = INITIAL_VALUE;

			RTE_WRITE_DOWNLOAD_PROGRESS(INITIAL_VALUE);

			/* Reset ReProgram Address Variables */
			FR_voidInitVariables();

			/* Change system state */
			Global_RxInternalSate = RX_IDLE;

			/* Enable Uart interrupt */
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
			break;
		}

		/*****************************DEFUALT (ERROR)***********************************/
		default :
			/* Error */
			break ;

	}
}

/**************************************************************************/
/*                         Esp Uart Handler                               */
/**************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	Std_ReturnType Local_Error ;
	//Testing
	RTE_WRITE_USER_RESPONSE(ACCEPT_UPDATE);

	/* Check of the ESP request */
	if (NEW_UPDATE_REQUEST == Global_HeaderBuffer[0])
	{
		Local_Error = RTE_READ_SYSTEM_STATE(&Global_SystemState);
		/* Check whether a read operation of the system state done successfully or not */
		if (E_OK == Local_Error)
		{
			/* Before accepting update request will check the system state */
			if (SYS_IDLE == Global_SystemState)
			{
				/* Accept the request and change system state */
				//RTE_WRITE_SYSTEM_STATE(SYS_NEW_UPDATE_REQ);
				//testing without user interface
				RTE_WRITE_SYSTEM_STATE(SYS_REC_UPDATE);
				/* Disble the interrupt till receive the code by synch function */
				__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
			}
			else
			{
				/* Refuse the update request as the system is not ready to receive updates */
				Global_HeaderReqByte = GATEWAY_BUSY;
				HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);

			}
		}
		else if (E_NOT_OK == Local_Error)
		{
			/* to Hold the ESP from sending new requests for a while as system state can't be read */
			Global_HeaderReqByte = SYSTEM_STATE_UNDIFINED;
			HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);
		}
		else
		{

		}
	}
	else
	{
		/* Response for unvalid requests */
		Global_HeaderReqByte = INVALID_REQUEST;
		HAL_UART_Transmit(&huart1, &Global_HeaderReqByte, 1, HAL_MAX_DELAY);
	}

}



