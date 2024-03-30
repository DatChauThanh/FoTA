/*****************************************************/
 /* Author      : Mosad                              */
 /* Version     : v01                                */
 /* date        : 29/4/2020                          */
/*****************************************************/
#ifndef RTE_H
#define RTE_H

/* Includes */
#include "Rte_DataTypes.h"

#define E_NOT_OK 	0x01u
#define E_OK 		0x00u

#define   NULL_PTR      ((void *) 0)

typedef    uint8_t   PortStateType  ;
typedef    uint8_t   Std_ReturnType  ;

typedef     uint8_t    FlagType ;
typedef     uint8_t    SystemStateType ;
/* Proto types */

/**************************************************************************/
/*                         Code Size Port                                 */
/**************************************************************************/
Std_ReturnType Rte_WriteCodeSize(uint32_t Cpy_CodeSize);
Std_ReturnType Rte_ReadCodeSize (uint32_t *Cpy_CodeSize);

/**************************************************************************/
/*                         Node Id Port                                   */
/**************************************************************************/
Std_ReturnType Rte_WriteNodeId(uint8_t Cpy_NodeId);
Std_ReturnType Rte_ReadNodeId (uint8_t *Cpy_NodeId);

/**************************************************************************/
/*                         Crc Port                                       */
/**************************************************************************/
Std_ReturnType Rte_WriteCrc(uint32_t Cpy_Crc);
Std_ReturnType Rte_ReadCrc (uint32_t *Cpy_Crc);

/**************************************************************************/
/*                         HeaderAckFlag Port                   */
/**************************************************************************/
Std_ReturnType Rte_WriteHeaderAckFlag(FlagType Cpy_HeaderAckFlag);
Std_ReturnType Rte_ReadHeaderAckFlag (FlagType *Cpy_HeaderAckFlag);

/**************************************************************************/
/*                         Decrypted Data Buffer Port                     */
/**************************************************************************/
Std_ReturnType Rte_WriteDecryptedDataBuffer(uint8_t *Cpy_DecryptedDataBufferPtr);
Std_ReturnType Rte_ReadDecryptedDataBuffer (uint8_t **Cpy_DecryptedDataBufferPtr);

/**************************************************************************/
/*                         DecryptedDataBufferFlag Port                   */
/**************************************************************************/
Std_ReturnType Rte_WriteDecryptedDataBufferFlag(FlagType Cpy_DecryptedDataBufferFlag);
Std_ReturnType Rte_ReadDecryptedDataBufferFlag (FlagType *Cpy_DecryptedDataBufferFlag);

/**************************************************************************/
/*                         SystemState Port                               */
/**************************************************************************/
Std_ReturnType Rte_WriteSystemState(SystemStateType Cpy_SystemState);
Std_ReturnType Rte_ReadSystemState (SystemStateType *Cpy_SystemState);

/**************************************************************************/
/*                         UserResponse Port                              */
/**************************************************************************/
Std_ReturnType Rte_WriteUserResponse(uint8_t Cpy_UserResponse);
Std_ReturnType Rte_ReadUserResponse (uint8_t *Cpy_UserResponse);

/**************************************************************************/
/*                         UpdateProgress Port                            */
/**************************************************************************/
Std_ReturnType Rte_WriteUpdateProgress (uint8_t Cpy_UpdateProgress);
Std_ReturnType Rte_ReadUpdateProgress (uint8_t *Cpy_UpdateProgress);




#endif
