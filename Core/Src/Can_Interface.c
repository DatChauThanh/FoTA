/*
 * CAN_IF.c
 *
 *  Created on: Mar 6, 2024
 *      Author: CHD9HC
 */
#include <Can_Interface.h>

HAL_StatusTypeDef CAN_IF_Transmit_UDS_Request(uint8_t Node, uint8_t UDS_Req){
   //Modify CAN frame
   uint8_t Local_u8SendToNode;
   if(Node == 0x01){
	   Local_u8SendToNode = 0x50;
   }
   else{
	   Local_u8SendToNode = 0x60;
   }
   TxHeader.IDE = CAN_ID_STD;
   TxHeader.RTR = CAN_RTR_DATA;
   TxHeader.StdId = Local_u8SendToNode;  // ID
   TxHeader.DLC = 1;  // data length
   TxData[0] = UDS_Req;

   // Request Program control.
   return HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}

HAL_StatusTypeDef CAN_IF_Trasmit_Data_Frame(uint8_t Node ,const uint8_t *DataBuffer ,uint8_t DataLength){
	//Check Is buffer NULL
	if(DataBuffer == NULL){
		return HAL_ERROR ;
	}
   //Modify CAN frame
   uint8_t Local_u8SendToNode;
   if(Node == 0x01){
	   Local_u8SendToNode = 0x50; //Send to 0x101
   }
   else{
	   Local_u8SendToNode = 0x60; //Send to 0x102
   }
   TxHeader.IDE = CAN_ID_STD;
   TxHeader.RTR = CAN_RTR_DATA;
   TxHeader.StdId = Local_u8SendToNode;  // ID
   TxHeader.DLC = DataLength;  // data length
   for(uint8_t index = 0 ; index < DataLength ; index++ ){
	  TxData[index] = *( DataBuffer + index );
   }
   // Request Program control.
   return HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}

HAL_StatusTypeDef CAN_IF_Receive_UDS_Respond(uint8_t *UDS_Req){
	// Block until get respond from Bootloader
	while(!(hcan.Instance->RF0R & CAN_RF0R_FMP0));
	HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData);
	// Get respond from Bootloader
	*UDS_Req = RxData[0];
	return HAL_OK;
}

HAL_StatusTypeDef CAN_IF_Receive_Data_Frame(uint8_t *DataBuffer){
	// Block until get respond from Bootloader
	while(!(hcan.Instance->RF0R & CAN_RF0R_FMP0));
	HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData);
	// Get respond from Bootloader
	uint8_t Local_u8DataLength = 8;
	for(uint8_t index = 0 ; index < Local_u8DataLength ; index++ ){
	   DataBuffer[index] = RxData[index];
    }
	return HAL_OK;
}

