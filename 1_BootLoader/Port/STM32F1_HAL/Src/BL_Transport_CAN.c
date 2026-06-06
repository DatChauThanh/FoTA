#include "BL_Transport.h"
#include "main.h"

#define BL_CAN_FRAME_SIZE 8u

static uint8_t BL_Transport_MapNode(uint8_t node)
{
	if (node == 0x01u)
	{
		return 0x50u;
	}

	if (node == 0x02u)
	{
		return 0x60u;
	}

	return node;
}

static BL_Status_t BL_Transport_WaitForRx(uint32_t timeout_ms)
{
	uint32_t start_tick = HAL_GetTick();

	while (!(hcan.Instance->RF0R & CAN_RF0R_FMP0))
	{
		if (timeout_ms != BL_TRANSPORT_WAIT_FOREVER)
		{
			if ((HAL_GetTick() - start_tick) >= timeout_ms)
			{
				return BL_STATUS_TIMEOUT;
			}
		}
	}

	return BL_STATUS_OK;
}

BL_Status_t BL_Transport_SendUds(uint8_t node, uint8_t uds_request)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.StdId = BL_Transport_MapNode(node);
	TxHeader.DLC = 1u;
	TxData[0] = uds_request;

	return (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK) ? BL_STATUS_OK : BL_STATUS_ERROR;
}

BL_Status_t BL_Transport_ReceiveUds(uint8_t *uds_request, uint32_t timeout_ms)
{
	BL_Status_t status;

	if (uds_request == NULL)
	{
		return BL_STATUS_INVALID_PARAM;
	}

	status = BL_Transport_WaitForRx(timeout_ms);
	if (status != BL_STATUS_OK)
	{
		return status;
	}

	if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		return BL_STATUS_ERROR;
	}

	*uds_request = RxData[0];
	return BL_STATUS_OK;
}

BL_Status_t BL_Transport_ReceiveFrame(uint8_t *data_buffer, uint8_t data_length, uint32_t timeout_ms)
{
	BL_Status_t status;

	if ((data_buffer == NULL) || (data_length > BL_CAN_FRAME_SIZE))
	{
		return BL_STATUS_INVALID_PARAM;
	}

	status = BL_Transport_WaitForRx(timeout_ms);
	if (status != BL_STATUS_OK)
	{
		return status;
	}

	if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		return BL_STATUS_ERROR;
	}

	for (uint8_t index = 0u; index < data_length; index++)
	{
		data_buffer[index] = RxData[index];
	}

	return BL_STATUS_OK;
}

BL_Status_t BL_Transport_ReceiveBuffer(uint8_t *data_buffer, uint32_t data_length, uint32_t timeout_ms)
{
	uint32_t received_bytes = 0u;
	BL_Status_t status;

	if (data_buffer == NULL)
	{
		return BL_STATUS_INVALID_PARAM;
	}

	while (received_bytes < data_length)
	{
		uint8_t frame_size = (uint8_t)((data_length - received_bytes) >= BL_CAN_FRAME_SIZE ? BL_CAN_FRAME_SIZE : (data_length - received_bytes));

		status = BL_Transport_ReceiveFrame(&data_buffer[received_bytes], frame_size, timeout_ms);
		if (status != BL_STATUS_OK)
		{
			return status;
		}

		received_bytes += frame_size;
	}

	return BL_STATUS_OK;
}
