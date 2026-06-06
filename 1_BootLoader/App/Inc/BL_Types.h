#ifndef BL_TYPES_H_
#define BL_TYPES_H_

#include <stdint.h>
#include <stddef.h>

typedef enum
{
	BL_STATUS_OK = 0,
	BL_STATUS_ERROR,
	BL_STATUS_TIMEOUT,
	BL_STATUS_INVALID_PARAM
} BL_Status_t;

#endif /* BL_TYPES_H_ */
