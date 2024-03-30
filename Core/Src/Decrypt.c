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
#include "Decrypt_Interface.h"
#include "Rte_DecryptPort.h"
/* Other Modules */


/********** Global Variables ************/
typedef  uint8_t    BufferType ;
typedef  uint8_t    GlobalFlag ;

  /*Encrypted Data Decrypted Data buffer*/
static BufferType   Global_PlainDataPtr [8];

     /*SWC Global parameters*/
static GlobalFlag Global_SystemState ;
static GlobalFlag Global_BufferFlag  ;

volatile uint32_t Global_ReadAddress ;

void  Decrypt_Address_Read_Init (void)
{
	Global_ReadAddress  = STORE_AREA_START_ADDRESS ;
}

void Decrypt_MainFunction (void)
{

	Std_ReturnType status ;
	
	 /*check system state value "Data receive point "*/
		status = RTE_READ_SYSTEM_STATE(&Global_SystemState);
	
		/* Check if there is an error during read operation */
		if (E_OK == status)
		{			
			/* make sure that system state is Decrypt  */
			if (SYS_DECRYPT == Global_SystemState)
			{
				  /*Read buffer Flag  "Data receive point "*/
			    status = RTE_READ_DECRYPTED_DATA_BUFFER_FLAG(&Global_BufferFlag);
			   	/* Check if there is an error during read operation */
		        if (E_OK == status)
		        {
					   /* make sure that buffer state is not set  */
		            if (BUFFER_NOT_SET == Global_BufferFlag)
			        {
	                              /*access piont to decrypt SWC*/								
			      	          /*Read Decrypted Data from flash rom*/
		                for (uint8_t Local_Counter = 0 ; Local_Counter < 8 ; Local_Counter++)
						{
		                	Global_PlainDataPtr[Local_Counter] = (*(volatile uint8_t*)(Global_ReadAddress) ) ;
							Global_ReadAddress++;
						}
              
						if( E_OK == status )
                        {						                	
//			    		   	if(Global_KeyFlag)
//			     		    {
//
//					        	Global_KeyFlag = ZERO ;
//				                 /*Init AES module decrypt input buffer */
//   	                            AES_uint8KeyExpansion ( key , roundkeys);
//
//				   		    }
//                                status = AES_uint8Decrypt_Buffer (roundkeys , Global_CipherDataPtr , Global_PlainDataPtr , BUFFER_LENGTH);
//
//							    if( E_OK == status )
//                                {
//			                        /*Write decrypted buffer to decrypt port "Data send point "*/
//                                    status = RTE_WRITE_DECRYPTED_DATA_BUFFER (Global_PlainDataPtr) ;
//								}
							status = RTE_WRITE_DECRYPTED_DATA_BUFFER (Global_PlainDataPtr) ;
							 /*Change buffer flag value to set "Data send point "*/
							RTE_WRITE_DECRYPTED_DATA_BUFFER_FLAG(BUFFER_SET);
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
