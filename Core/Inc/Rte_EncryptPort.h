/*****************************************************/
 /* Author      : Mosad                              */
 /* Version     : v01                                */
 /* date        : 29/4/2020                          */
/*****************************************************/
#ifndef RTE_DECRYPT_PORT_H
#define RTE_DECRYPT_PORT_H

#include "Rte.h"

/**************************************************************************/
/*                         Provided Ports                                 */
/**************************************************************************/
#define     RTE_WRITE_ENCRYPTED_DATA_BUFFER         Rte_WriteEncryptedDataBuffer
#define     RTE_WRITE_ENCRYPTED_DATA_BUFFER_FLAG    Rte_WriteEncryptedDataBufferFlag
#define     RTE_WRITE_SYSTEM_STATE                  Rte_WriteSystemState


/**************************************************************************/
/*                         Received Ports                                 */
/**************************************************************************/
#define    RTE_READ_CODE_SIZE                    Rte_ReadCodeSize
#define    RTE_READ_SYSTEM_STATE                 Rte_ReadSystemState
#define    RTE_READ_ENCRYPTED_DATA_BUFFER_FLAG   Rte_ReadEncryptedDataBufferFlag

#endif
