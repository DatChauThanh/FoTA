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
static uint8_t Global_SwipeButtonState ;
static uint8_t Global_OkButtonState ;
static uint8_t Global_DownloadProgress ;

/**************************************************************************/
/*                         Module Functions                               */
/**************************************************************************/

void UserInterface_voidInit(void)
{
	/* Init Variables */
	Global_UiInternalState = UI_IDLE ;
	Global_CursorState = UI_CURSOR_AT_ACCEPT ;
	Global_SwipeButtonState = RELEASED ;
	Global_OkButtonState = RELEASED ;
	Global_DownloadProgress = 0 ;
	/* Init Screan */
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
			Std_ReturnType Local_ErrorSwipeButton = E_OK ;
			Std_ReturnType Local_ErrorOkButton = E_OK ;
//			Local_ErrorSwipeButton = Button_GetState(BUTTON_SWIPE_ID , &Global_SwipeButtonState);
//			Local_ErrorOkButton = Button_GetState(BUTTON_OK_ID , &Global_OkButtonState);
			Global_SwipeButtonState = RELEASED;	//
			Global_OkButtonState = PRESSED;		// for testing due to UI has not ready yet
			if ((E_OK == Local_ErrorSwipeButton) && (E_OK == Local_ErrorOkButton))
			{
				Interface_ProcessSwipeButton();
				Interface_ProcessOkButton();
			}
			else
			{
				/* Report Error */
			}
			
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
			Global_SwipeButtonState = RELEASED ;
			Global_OkButtonState = RELEASED ;
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
					Interface_CleanScrean();
					
					/* Update Screan */
					SSD1306_GotoXY (0, 30);
					SSD1306_Puts ("Install", &Font_7x10, 1);

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
			Global_SwipeButtonState = RELEASED ;
			Global_OkButtonState = RELEASED ;
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
	SSD1306_GotoXY (0, 30);
	SSD1306_Puts ("Hello Dat", &Font_7x10, 1);
	SSD1306_GotoXY (10, 30);
	SSD1306_Puts ("Waiting", &Font_7x10, 1);

	SSD1306_GotoXY (20, 30);
	SSD1306_Puts ("Update", &Font_7x10, 1);

	SSD1306_UpdateScreen(); //display
}


static void Interface_GetResponseScreen (void)
{
	/* Writeing Text */
	SSD1306_GotoXY (0, 30);
	SSD1306_Puts ("New", &Font_7x10, 1);
	SSD1306_GotoXY (10, 30);
	SSD1306_Puts ("Update", &Font_7x10, 1);

	SSD1306_GotoXY (20, 30);
	SSD1306_Puts ("Accept", &Font_7x10, 1);
	SSD1306_GotoXY (30, 30);
	SSD1306_Puts ("Reject", &Font_7x10, 1);

	SSD1306_GotoXY (20, 20);
	SSD1306_Puts (">", &Font_7x10, 1);	//Cursor init point to Accept

	SSD1306_UpdateScreen(); //display

}

static void Interface_DownloadingScreen (void)
{
	/* Writeing Text */
	SSD1306_GotoXY (0, 30);
	SSD1306_Puts ("Download", &Font_7x10, 1);
	SSD1306_GotoXY (10, 30);
	SSD1306_Puts ("  in", &Font_7x10, 1);
	SSD1306_GotoXY (20, 30);
	SSD1306_Puts ("Progress", &Font_7x10, 1);
	SSD1306_GotoXY (30, 30);
	SSD1306_Puts ("  0%", &Font_7x10, 1);

	SSD1306_UpdateScreen(); //display
}

static void Interface_UpdateDownloadingScreen (uint8_t Cpy_Progress)
{
	/**/
	char local_DateBuffer[4];
	/* Clear Current Progress Text */
	SSD1306_GotoXY (30, 30);
	SSD1306_Puts ("   ", &Font_7x10, 1);
	sprintf(local_DateBuffer, "%.3i", Cpy_Progress);
	SSD1306_GotoXY (30, 30);
	SSD1306_Puts (local_DateBuffer, &Font_7x10, 1);
	SSD1306_UpdateScreen();

}

static void Interface_DoneScreen (void)
{
	/* Writeing Text */
	SSD1306_GotoXY (0, 30);
	SSD1306_Puts ("Done", &Font_7x10, 1);
	SSD1306_GotoXY (10, 30);
	SSD1306_Puts ("Install", &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

static void Interface_CleanScrean(void)
{
	SSD1306_DrawFilledRectangle(0 , 0 , SSD1306_WIDTH-1 , SSD1306_HEIGHT-1 , SSD1306_COLOR_BLACK);
	SSD1306_UpdateScreen();
}

static void Interface_ProcessSwipeButton (void)
{
	if (PRESSED == Global_SwipeButtonState)
	{
		switch (Global_CursorState)
		{
			case UI_CURSOR_AT_ACCEPT : 
				/* Delete cursor from the current option */
				SSD1306_GotoXY (20, 20);
				SSD1306_Puts (" ", &Font_7x10, 1);	//
				/* Put the cursor at the next option */
				SSD1306_GotoXY (30, 20);
				SSD1306_Puts (">", &Font_7x10, 1);	//Cursor point to Reject
				/* Update cursor state and screen */
				SSD1306_UpdateScreen();
				Global_CursorState = UI_CURSOR_AT_REJECT ;
				break ;
			
			case UI_CURSOR_AT_REJECT :
				/* Delete cursor from the current option */
				SSD1306_GotoXY (20, 20);
				SSD1306_Puts (">", &Font_7x10, 1);	//
				/* Put the cursor at the next option */
				SSD1306_GotoXY (30, 20);
				SSD1306_Puts (" ", &Font_7x10, 1);	//Cursor point to Reject
				/* Update cursor state and screen */
				SSD1306_UpdateScreen();
				Global_CursorState = UI_CURSOR_AT_ACCEPT ;
				break ;
		}
	}
	else 
	{
		/*Do Nothing */
	}
}

static void Interface_ProcessOkButton (void)
{
	if (PRESSED == Global_OkButtonState)
	{
		switch (Global_CursorState)
		{
			case UI_CURSOR_AT_ACCEPT : 
				/* Update System State */
				Global_UiInternalState = UI_ACCEPT_UPDATE ;
				break ;
			
			case UI_CURSOR_AT_REJECT :
				Global_UiInternalState = UI_REJECT_UPDATE ;
				break ;
		}
	}
}



