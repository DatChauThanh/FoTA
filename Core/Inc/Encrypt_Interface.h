/*******************************************************************************
**  FILENAME     : Decyrpt_Interface.h         			                          **
**                                                                            **
**  VERSION      : 1.0.0                                                      **
**                                                                            **
**  DATE         : 2021-05-03                                                 **
**                                                                            **                                                                            **
**  AUTHOR       : osamahijazi	                                              **
**
**                                                                            **
*******************************************************************************/

/* Define to prevent recursive inclusion */

#ifndef ENCRYPT_INTERFACE_H
#define ENCRYPT_INTERFACE_H

/*****************************************************************************************/
/*                                   Include headres                                     */
/*****************************************************************************************/
#define STORE_AREA_START_ADDRESS 0x0800A800

/************************************************************************************

*Name       :   Encrypt_MainFunction

*Description: * read data from rom 16 byte in each round
              * encryp this buffer by AES module to cypher Data
							
*Pre-Cond   :  buffer flag is not set & sys flag is assign to encrypt
							
*pos-Cond   :  buffer flag is set & sys flag is assign to send 

*Input      :   void

*Output     :   void

*Return     :   void

****************************************************************************************/                                   	

void Encrypt_MainFunction (void) ;

/************************************************************************************

*Name       :   Encrypt_Address_Read_Init

*Description: Init Read address


*Pre-Cond   :  buffer flag is not set & sys flag is assign to encrypt

*pos-Cond   :  buffer flag is set & sys flag is assign to send

*Input      :   void

*Output     :   void

*Return     :   void

****************************************************************************************/
void Encrypt_Address_Read_Init(void) ;

#endif
/******************************************END OF FILE************************************/
