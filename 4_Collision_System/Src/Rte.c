/*****************************************************/
 /* Author      : Mosad                              */
 /* Version     : v01                                */
 /* date        : 29/4/2020                          */
/*****************************************************/

/* Module includes */
#include "Rte.h"

/* Local Data Types */

/* Local Macros */
#define INITIAL_VALUE 0x0u

/*Local Port Status Macros*/
#define IDLE          0x0u
#define BUSY          0x1u

/* Rte Golable Variables */
static uint32_t Global_DistanceeData = INITIAL_VALUE ;

/* Rte protection flag */
static PortStateType Global_DistanceeDataPortState = IDLE;


/**************************************************************************/
/*                         NodeId Port                                    */
/**************************************************************************/
Std_ReturnType Rte_WriteDistanceData (uint32_t DistanceData)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if (Global_DistanceeDataPortState == IDLE)
	{
		/* Lock the port to write */
		Global_DistanceeDataPortState = BUSY ;
		/* Write the data to the port */
		Global_DistanceeData = DistanceData ;
		/* Unlock the port after done writing */
		Global_DistanceeDataPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

Std_ReturnType Rte_ReadDistanceData (uint32_t *DistanceData)
{
	Std_ReturnType Local_ReturnError = E_OK ;
	
	if ( (Global_DistanceeDataPortState == IDLE) && (DistanceData != NULL_PTR))
	{
		/* Lock the port to write */
		Global_DistanceeDataPortState = BUSY ;
		/* Write the data to the port */
		 (*DistanceData) = Global_DistanceeData ;
		/* Unlock the port after done writing */
		 Global_DistanceeDataPortState = IDLE ;
	}
	else
	{
		Local_ReturnError = E_NOT_OK ;
	}
	return Local_ReturnError ;
}

