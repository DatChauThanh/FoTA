/*
 * ReceiveUpdate_Private.h
 *
 *  Created on: Mar 13, 2024
 *      Author: Chau Thanh Dat
 */

#ifndef INC_RECEIVEUPDATE_PRIVATE_H_
#define INC_RECEIVEUPDATE_PRIVATE_H_

/**************************************************************************/
/*                         Private DataTypes                              */
/**************************************************************************/
typedef   uint8_t     ReceiveUpdateStateType ;
/* Range of values */
#define      RX_IDLE                  0x00
#define      RX_ACCEPT_UPDATE         0x01
#define      RX_RECEIVE_HEADER        0x02
#define      RX_RECEIVE_PACKET        0x03
#define      RX_END_STATE             0x04
#define      RX_REFUSE_UPDATE         0x05

/**************************************************************************/
/*                         Private Macros                                */
/**************************************************************************/
/* NodeMcu To Gateway messages */
#define   NEW_UPDATE_REQUEST     0x01u

/* Gateway To NodeMCU errors */
#define   SYSTEM_STATE_UNDIFINED     0x02
#define   GATEWAY_BUSY               0x03
#define   INVALID_REQUEST            0X04
#define   REQUEST_ACCEPTED           0x05
#define   REQUEST_REFUSED            0x06
#define   ESP_SEND_HEADER            0x07
#define   HEADER_RECEIVED            0x08
#define   HEADER_ERROR               0x09
#define   PACKET_RECEIVED            0x0A
#define   LAST_PACKET_RECEIVED       0x0B
#define   SEND_NEXT_PACKET           0x0C
#define   ESP_DOWNLOAD_DONE          0x0D


/**/
#define   INITIAL_VALUE    0
#define   SHIFT_BY_8       8
#define   SHIFT_BY_16      16
#define   SHIFT_BY_24      24
#define   PACKET_SIZE      1024
#define   HEADER_SIZE      5

/**************************************************************************/
/*                         Private Functions                              */
/**************************************************************************/

#endif /* INC_RECEIVEUPDATE_PRIVATE_H_ */
