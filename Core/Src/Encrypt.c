/*******************************************************************************
**  FILENAME     : Decyrpt.C            			                          **
**                                                                            **
**  VERSION      : 1.0.0                                                      **
**                                                                            **
**  DATE         : 2021-05-03                                                 **
**                                                                            **
**  AUTHOR       : osamahijazi	                                              **
*******************************************************************************/
/* Library includes */

/* Module includes */
#include <Encrypt_Interface.h>
#include <Rte_EncryptPort.h>
#include "aes.h"
/* Other Modules */
uint8_t AES_CBC_128_Key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
uint8_t AES_CBC_128_IV[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

/********** Global Variables ************/
typedef  uint8_t    BufferType ;
typedef  uint8_t    GlobalFlag ;

  /*Encrypted Data Encrypted Data buffer*/
static BufferType Global_CipherTextPtr [16];

     /*SWC Global parameters*/
static GlobalFlag Global_SystemState ;
static GlobalFlag Global_BufferFlag  ;

volatile uint32_t Global_ReadAddress ;

struct AES_ctx ctx;

void Encrypt_Address_Read_Init (void)
{
	Global_ReadAddress  = STORE_AREA_START_ADDRESS ;
	AES_init_ctx_iv(&ctx, AES_CBC_128_Key, AES_CBC_128_IV);
}

void Encrypt_MainFunction (void)
{

	Std_ReturnType status ;
	
	 /*check system state value "Data receive point "*/
		status = RTE_READ_SYSTEM_STATE(&Global_SystemState);
	
		/* Check if there is an error during read operation */
		if (E_OK == status)
		{			
			/* make sure that system state is Decrypt  */
			if (SYS_ENCRYPT == Global_SystemState)
			{
				  /*Read buffer Flag  "Data receive point "*/
			    status = RTE_READ_ENCRYPTED_DATA_BUFFER_FLAG(&Global_BufferFlag);
			   	/* Check if there is an error during read operation */
		        if (E_OK == status)
		        {
					   /* make sure that buffer state is not set  */
		            if (BUFFER_NOT_SET == Global_BufferFlag)
			        {
			      	    /*Read Decrypted Data from flash rom*/
		                for (uint8_t Local_Counter = 0 ; Local_Counter < 16 ; Local_Counter++)
						{
		                	Global_CipherTextPtr[Local_Counter] = (*(volatile uint8_t*)(Global_ReadAddress) ) ;
							Global_ReadAddress++;
						}

		                /*access piont to Encrypt SWC*/
		                AES_CBC_encrypt_buffer(&ctx, Global_CipherTextPtr, 16);
		                //@TODO: Add encrypto module

						if( E_OK == status )
                        {						                	

							status = RTE_WRITE_ENCRYPTED_DATA_BUFFER (Global_CipherTextPtr) ;
							 /*Change buffer flag value to set "Data send point "*/
							RTE_WRITE_ENCRYPTED_DATA_BUFFER_FLAG(BUFFER_SET);
							 /*Change system flag to SEND , "Data send point "*/
							RTE_WRITE_SYSTEM_STATE(SYS_SEND_UPDATE);
					   }												
				   }  
			       else
				   {
			    	   	   	   	   	   /* Do nothing */
									/*buffer is not set */								
				   }													
			   }			 
	        }
			else
			{
									/* Do nothing */
							/*system state is not Decrypt*/							
			}						
        }				      
}
/************************************************END OF FUNCTION*******************************/
