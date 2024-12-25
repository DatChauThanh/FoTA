/*
 * CAN_IF.h
 *
 *  Created on: Mar 6, 2024
 *      Author: Chau Thanh Dat
 */

#ifndef INC_CAN_INTERFACE_H_
#define INC_CAN_INTERFACE_H_

#include "main.h"

#define LIGHT_CONTROL_NODE 0x01
#define OPTION_NODE	0x02

/******************************************************************************
* Function : CAN_IF_Transmit_UDS_Request(uint8_t Node, uint8_t UDS_Req)
*//**
* \b Description:
*
* This function is used to send UDS_Req code to Node
*
* PRE-CONDITION:  Can Peripheral enable
*
* POST-CONDITION: None
*
* @param [in]     uint32_t
*
* @return 		   HAL_StatusTypeDef
*
* \b Example Example:
* @code
* 	CAN_IF_Transmit_UDS_Request(uint8_t Node, uint8_t UDS_Req);
*
* @endcode
*
* @see CAN_IF_Transmit_UDS_Request(uint8_t Node, uint8_t UDS_Req)
*
*******************************************************************************/
HAL_StatusTypeDef CAN_IF_Transmit_UDS_Request(uint8_t Node, uint8_t UDS_Req);

/******************************************************************************
* Function : CAN_IF_Trasmit_Data_Frame(uint8_t Node, const uint8_t *DataBuffer, uint8_t DataLength);
*//**
* \b Description:
*
* This function is used to send DataBuffer with DataLength to Node
*
* PRE-CONDITION:  Can Peripheral enable
*
* POST-CONDITION: None
*
* @param [in]     uint8_t 	Node to send data
* @param [in]     uint8_t* 	Pointer to buffer
* @param [in]     uint8_t	DataLength of buffer
*
* @return 		   HAL_StatusTypeDef
*
* \b Example Example:
* @code
* 	CAN_IF_Trasmit_Data_Frame(uint8_t Node, const uint8_t *DataBuffer, uint8_t DataLength);
*
* @endcode
*
* @see CAN_IF_Trasmit_Data_Frame(uint8_t Node, const uint8_t *DataBuffer, uint8_t DataLength);
*
*******************************************************************************/
HAL_StatusTypeDef CAN_IF_Trasmit_Data_Frame(uint8_t Node, const uint8_t *DataBuffer, uint8_t DataLength);

/******************************************************************************
* Function : CAN_IF_Receive_UDS_Respond(uint8_t *UDS_Req)
*//**
* \b Description:
*
* This function is used to receive UDS code
*
* PRE-CONDITION:  Can Peripheral enable
*
* POST-CONDITION: None
*
* @param [in]     uint8_t*
*
* @return 		  HAL_StatusTypeDef
*
* \b Example Example:
* @code
* 	CAN_IF_Receive_UDS_Respond(uint8_t *UDS_Req);
*
* @endcode
*
* @see CAN_IF_Receive_UDS_Respond(uint8_t *UDS_Req)
*
*******************************************************************************/
HAL_StatusTypeDef CAN_IF_Receive_UDS_Respond(uint8_t *UDS_Req);

/******************************************************************************
* Function : BL_ReadAddressData(uint32_t address)
*//**
* \b Description:
*
* This function is used to receive data from fifo0
*
* PRE-CONDITION:  Can Peripheral enable
*
* POST-CONDITION: None
*
* @param [in]     uint8_t* Pointer to buffer Rx
*
* @return 		   HAL_StatusTypeDef
*
* \b Example Example:
* @code
* 	CAN_IF_Receive_Data_Frame(uint8_t *DataBuffer);
*
* @endcode
*
* @see CAN_IF_Receive_Data_Frame(uint8_t *DataBuffer)
*
*******************************************************************************/
HAL_StatusTypeDef CAN_IF_Receive_Data_Frame(uint8_t *DataBuffer);

/******************************************************************************
* Function :CAN_IF_Receive_Data_Buffer(uint8_t *DataBuffer , uint32_t Copy_DataLength);
*//**
* \b Description:
*
* This function is used to receive data from fifo0
*
* PRE-CONDITION:  Can Peripheral enable
*
* POST-CONDITION: None
*
* @param [in]      uint8_t* Pointer to buffer Rx with Copy_DataLength
*
* @return 		   HAL_StatusTypeDef
*
* \b Example Example:
* @code
* 	CAN_IF_Receive_Data_Buffer(uint8_t *DataBuffer , uint32_t Copy_DataLength);
*
* @endcode
*
* @see CAN_IF_Receive_Data_Buffer(uint8_t *DataBuffer , uint32_t Copy_DataLength);
*
*******************************************************************************/
HAL_StatusTypeDef CAN_IF_Receive_Data_Buffer(uint8_t *DataBuffer , uint32_t Copy_DataLength);
#endif /* INC_CAN_INTERFACE_H_ */
