/*****************************************************/
 /* Author      : Mosad    							 */
 /* Modifier	: Chau Thanh Dat			         */
 /* Version     : v02                                */
 /* date        : 8/3/2024                           */
/*****************************************************/
/* Library includes */
#include "Rte_UserInterfacePort.h"

/* Module includes */
#include "UserInterface_Interface.h"
#include "UserInterface_Private.h"
#include "SSD1306.h"
#include <stdio.h>
/**************************************************************************/
/*                         Global Variables                              */
/**************************************************************************/
static UserInterfaceStateType Global_UiInternalState ;
static SystemStateType Global_SystemState ;
static UiCursorStateType Global_CursorState ;
static uint8_t Global_DownloadProgress ;

/**************************************************************************/
/*                         Module Functions                               */
/**************************************************************************/

void UserInterface_InitializeModule(void)
{
	/* Init Variables */
	Global_UiInternalState = UI_IDLE ;
	Global_CursorState = UI_CURSOR_AT_ACCEPT ;
	Global_DownloadProgress = 0 ;
	/* Init Screan */
	SSD1306_Init(); // initialize the display
	Interface_IdleScreen();
}


void UserInterface_MainFunction (void)
{
	switch (Global_UiInternalState)
	{
		/*****************************UI_IDLE***********************************/
		case UI_IDLE :
		{
			Std_ReturnType Local_Error = RTE_READ_SYSTEM_STATE(&Global_SystemState);
			if (E_OK == Local_Error)
			{
				if (SYS_NEW_UPDATE_REQ == Global_SystemState)
				{
					/* Clear Screen first before writing*/
					Interface_CleanScrean();
					/* Init Get Response scereen */
					Interface_GetResponseScreen();
					/* Change internal state */
					Global_UiInternalState = UI_GET_RESPONSE ;
				}
				else
				{
					/* Stay in UI_IDLE state otherwise */
				}
			}
			else 
			{
				/* Error */
			}
			break;
		}
		
		/*****************************UI_GET_RESPONSE ***********************************/
		case UI_GET_RESPONSE :
		{
			Interface_ProcessButton();
			break;
		}
		
		/*****************************UI_ACCEPT_UPDATE***********************************/
		case UI_ACCEPT_UPDATE :
		{
			/* Clear Screan */
			Interface_CleanScrean();
			/* Init Download Screen */
			Interface_DownloadingScreen();
			
			/* Update user response */
			RTE_WRITE_USER_RESPONSE (ACCEPT_UPDATE);
			/* Update system state */
			RTE_WRITE_SYSTEM_STATE(SYS_REC_UPDATE);
			
			/* Update Internal state */
			Global_UiInternalState = UI_DOWNLOADING ;
			break;
		}
		
		/*****************************UI_REJECT_UPDATE***********************************/
		case UI_REJECT_UPDATE :
		{
			/* Clear Screan */
			Interface_CleanScrean();
			
			/* Update user response */
			RTE_WRITE_USER_RESPONSE (REFUSE_UPDATE);
			/* Update system state */
			RTE_WRITE_SYSTEM_STATE(SYS_REC_UPDATE);
			
			/* Reinit vaiables */
			Global_UiInternalState = UI_IDLE ;
			Global_CursorState = UI_CURSOR_AT_ACCEPT ;
			Interface_IdleScreen();
			break;
		}
		
		/*****************************UI_DOWNLOADING***********************************/
		case UI_DOWNLOADING : 
		{
			Std_ReturnType Local_Error ;
			Local_Error = RTE_READ_DOWNLOAD_PROGRESS(&Global_DownloadProgress);
			
			if (E_OK == Local_Error)
			{
				Interface_UpdateDownloadingScreen(Global_DownloadProgress);
				if (100 == Global_DownloadProgress)
				{
					/* Clear Screan */
					HAL_Delay(2000); // hold for 2 seconds
					Interface_CleanScrean();
					Interface_InstallScreen();
					/* Update internal state */
					Global_UiInternalState = UI_INSTALLING ;
				}
				else
				{
					
				}
			}
			else if (E_NOT_OK == Local_Error)
			{
				/* Report Error*/
			}
			else
			{
				
			}
			break;
		}
		
		/*****************************UI_INSTALLING***********************************/
		case UI_INSTALLING : 
		{
			Std_ReturnType Local_Error = RTE_READ_SYSTEM_STATE(&Global_SystemState);
			if (E_OK == Local_Error)
			{
				if (SYS_DONE_UPDATE == Global_SystemState)
				{
					/* CleanScrean*/
					Interface_CleanScrean();
					/* Update screan*/
					Interface_DoneScreen();
					/* Wait for a time */
					HAL_Delay(2000);
					/* Update internal state */
					Global_UiInternalState = UI_END_STATE ;
					
				}
				
				else
				{
					
				}
			}
			else
			{
				/* Report error*/
			}
			break;
		}
		
		/*****************************UI_END_STATE***********************************/
		case UI_END_STATE : 
		{
			/* Reinit vaiables */
			Global_UiInternalState = UI_IDLE ;
			Global_CursorState = UI_CURSOR_AT_ACCEPT ;
			Global_DownloadProgress = 0 ;
			
			Interface_CleanScrean();
			/* Update screen */
			Interface_IdleScreen();
			
			/* Return system state to idle */
			RTE_WRITE_SYSTEM_STATE(SYS_IDLE);
			break;
		}
		/*****************************DEFUALT (ERROR)***********************************/
		default :
			/* Error */
			break ;
	}
}

/**************************************************************************/
/*                        Private Functions                               */
/**************************************************************************/
static void Interface_IdleScreen (void)
{
	/* Set Backgroun color */
	SSD1306_GotoXY (0,10); // goto 10, 10
    SSD1306_Puts ("FOTA SYSTEM", &Font_11x18, 1); // print Hello
    SSD1306_GotoXY (5, 30);
    SSD1306_Puts ("Waiting Update!!", &Font_7x10, 1);
    SSD1306_UpdateScreen(); // update screen
}


static void Interface_GetResponseScreen (void)
{
	SSD1306_GotoXY (20, 0);
    SSD1306_Puts ("New Update", &Font_7x10, 1);
    SSD1306_GotoXY (20, 10);
    SSD1306_Puts ("Available", &Font_7x10, 1);
    SSD1306_GotoXY (30, 30);
    SSD1306_Puts ("Accept", &Font_7x10, 1);
    SSD1306_GotoXY (30, 40);
    SSD1306_Puts ("Reject", &Font_7x10, 1);

    SSD1306_GotoXY (20, 30);
    SSD1306_Puts (">", &Font_7x10, 1);	//Cursor init point to Accept

    SSD1306_UpdateScreen(); //display
}

static void Interface_DownloadingScreen (void)
{
	/* Writeing Text */
	SSD1306_GotoXY (40, 10);
	SSD1306_Puts ("Download", &Font_7x10, 1);
	SSD1306_GotoXY (40, 20);
	SSD1306_Puts ("  in", &Font_7x10, 1);
	SSD1306_GotoXY (40, 30);
	SSD1306_Puts ("Progress", &Font_7x10, 1);
	SSD1306_GotoXY (50, 40);
	SSD1306_Puts ("  0%", &Font_7x10, 1);
	SSD1306_UpdateScreen(); //display
}

static void Interface_UpdateDownloadingScreen (uint8_t Cpy_Progress)
{
	/**/
	char local_DateBuffer[4];
	/* Clear Current Progress Text */
	sprintf(local_DateBuffer, "%d", Cpy_Progress);
	SSD1306_GotoXY (50, 40);
	SSD1306_Puts ("   %", &Font_7x10, 1);
	SSD1306_GotoXY (50, 40);
	SSD1306_Puts (local_DateBuffer, &Font_7x10, 1);
	SSD1306_UpdateScreen(); //display
}

static void Interface_DoneScreen (void)
{
	/* Writeing Text */
	SSD1306_GotoXY (40, 10);
	SSD1306_Puts ("Download", &Font_7x10, 1);
	SSD1306_GotoXY (35, 20);
	SSD1306_Puts ("Completed", &Font_7x10, 1);
	SSD1306_GotoXY (50, 30);
	SSD1306_Puts (" -_-  ", &Font_7x10, 1);
	SSD1306_UpdateScreen(); //display
}

static void Interface_InstallScreen(void)
{
	/* Update Screan */
	SSD1306_GotoXY (40, 10);
	SSD1306_Puts ("Installing", &Font_7x10, 1);
	SSD1306_GotoXY (40, 20);
	SSD1306_Puts (" Firmware", &Font_7x10, 1);
	SSD1306_GotoXY (40, 30);
	SSD1306_Puts (" to MCU ", &Font_7x10, 1);
	SSD1306_UpdateScreen(); //display
}

static void Interface_CleanScrean(void)
{
	SSD1306_Clear();
	SSD1306_UpdateScreen();
}

static void Interface_ProcessButton (void)
{
	while(1)
	{
	  if(HAL_GPIO_ReadPin(SWITCH_BTN_GPIO_Port, SWITCH_BTN_Pin) ==  GPIO_PIN_RESET)
	  {
		  while(HAL_GPIO_ReadPin(SWITCH_BTN_GPIO_Port, SWITCH_BTN_Pin) ==  GPIO_PIN_RESET);// Hold until button release
		  if(Global_CursorState == UI_CURSOR_AT_ACCEPT)
		  {
			  SSD1306_GotoXY (20, 30);
			  SSD1306_Puts (" ", &Font_7x10, 1);	//Cursor init point to Accept
			  SSD1306_GotoXY (20, 40);
			  SSD1306_Puts (">", &Font_7x10, 1);	//Cursor init point to Accept
			  Global_CursorState = UI_CURSOR_AT_REJECT;
		  }
		  else if(Global_CursorState == UI_CURSOR_AT_REJECT)
		  {
			  SSD1306_GotoXY (20, 30);
			  SSD1306_Puts (">", &Font_7x10, 1);	//Cursor init point to Accept
			  SSD1306_GotoXY (20, 40);
			  SSD1306_Puts (" ", &Font_7x10, 1);	//Cursor init point to Accept
			  Global_CursorState = UI_CURSOR_AT_ACCEPT;
		  }
		  else
		  {
			  //error
		  }
	  }
	  SSD1306_UpdateScreen(); //display
	  HAL_Delay(100);
	  if(HAL_GPIO_ReadPin(OK_BNT_GPIO_Port, OK_BNT_Pin) ==  GPIO_PIN_RESET)
	  {
		 while(HAL_GPIO_ReadPin(OK_BNT_GPIO_Port, OK_BNT_Pin) ==  GPIO_PIN_RESET);// Hold until button release
		 if(Global_CursorState == UI_CURSOR_AT_ACCEPT)
		 {
			Global_UiInternalState = UI_ACCEPT_UPDATE ;
			break ;
		 }
		 else if (Global_CursorState == UI_CURSOR_AT_REJECT)
		 {
			Global_UiInternalState = UI_REJECT_UPDATE ;
			break ;
		 }
	  }
	}
}




