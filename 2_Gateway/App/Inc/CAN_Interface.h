/*
 * CAN_IF.h
 *
 *  Created on: Mar 6, 2024
 *      Author: Chau Thanh Dat
 */

#ifndef INC_CAN_INTERFACE_H_
#define INC_CAN_INTERFACE_H_

#include "GW_Types.h"
#include <stdint.h>

#define LIGHT_CONTROL_NODE 0x01
#define OPTION_NODE	0x02

GW_Status_t CAN_IF_Transmit_UDS_Request(uint8_t Node, uint8_t UDS_Req);
GW_Status_t CAN_IF_Trasmit_Data_Frame(uint8_t Node, const uint8_t *DataBuffer, uint8_t DataLength);
GW_Status_t CAN_IF_Receive_UDS_Respond(uint8_t *UDS_Req);
GW_Status_t CAN_IF_Receive_Data_Frame(uint8_t *DataBuffer);
GW_Status_t CAN_IF_Trasmit_Data_Buffer(uint8_t Node ,const uint8_t *DataBuffer ,uint8_t Copy_DataLength);
#endif /* INC_CAN_INTERFACE_H_ */
