/* Define to prevent recursive inclusion */

#ifndef ULTRASENSOR_INTERFACE_H
#define ULTRASENSOR_INTERFACE_H

/*****************************************************************************************/
/*                                   Include headres                                     */
/*****************************************************************************************/
#include "main.h"

/************************************************************************************

*Name       :   UltraSensor_MainFunction

*Description: * read data from HCSR04 sensor
              * write to RTE port

*Pre-Cond   :  buffer flag is not set & sys flag is assign to encrypt

*pos-Cond   :  buffer flag is set & sys flag is assign to send

*Input      :   void

*Output     :   void

*Return     :   void

****************************************************************************************/

void UltraSensor_MainFunction (void) ;

static void UltraSensor_HCSR04_Read (void);

static void UltraSensor_Delay_MicroSec (uint16_t time);

#endif
