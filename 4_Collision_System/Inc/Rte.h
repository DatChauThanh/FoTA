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
/*                      Distanc Data Port                                 */
/**************************************************************************/
Std_ReturnType Rte_WriteDistanceData(uint32_t DistanceData);
Std_ReturnType Rte_ReadDistanceData (uint32_t *DistanceData);






#endif
