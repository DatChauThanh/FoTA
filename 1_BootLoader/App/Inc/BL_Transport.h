#ifndef BL_TRANSPORT_H_
#define BL_TRANSPORT_H_

#include "BL_Types.h"

#define BL_TRANSPORT_WAIT_FOREVER 0xFFFFFFFFu

BL_Status_t BL_Transport_SendUds(uint8_t node, uint8_t uds_request);
BL_Status_t BL_Transport_ReceiveUds(uint8_t *uds_request, uint32_t timeout_ms);
BL_Status_t BL_Transport_ReceiveFrame(uint8_t *data_buffer, uint8_t data_length, uint32_t timeout_ms);
BL_Status_t BL_Transport_ReceiveBuffer(uint8_t *data_buffer, uint32_t data_length, uint32_t timeout_ms);

#endif /* BL_TRANSPORT_H_ */
