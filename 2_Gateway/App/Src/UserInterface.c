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
#include "GW_Display.h"
#include "GW_Platform.h"
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
	GW_Display_Init(); // initialize the display
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
			RTE_WRITE_USER_RESPONSE(ACCEPT_UPDATE);
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
			RTE_WRITE_SYSTEM_STATE(SYS_IDLE);
			
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
					GW_Platform_DelayMs(2000); // hold for 2 seconds
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
					GW_Platform_DelayMs(2000);
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
			/*Reset Gateway*/
			GW_Platform_RequestReset();
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
	GW_Display_GotoXY (0,10); // goto 10, 10
    GW_Display_Puts ("FOTA SYSTEM", GW_DISPLAY_FONT_11X18, 1); // print Hello
    GW_Display_GotoXY (5, 30);
    GW_Display_Puts ("Waiting Update!!", GW_DISPLAY_FONT_7X10, 1);
    GW_Display_Update(); // update screen
}


static void Interface_GetResponseScreen (void)
{
	uint8_t 	Local_NodeID                 = 0;
	uint32_t 	Local_Codesize               = 0;
	RTE_READ_NODE_ID(&Local_NodeID);
	RTE_READ_CODE_SIZE(&Local_Codesize);
	char Local_DateBuffer[4];
	uint8_t Local_Estimate_time = Local_Codesize / Bandwidth_Avarage + 20;

	GW_Display_GotoXY (20, 0);
	GW_Display_Puts ("New firmware", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (20, 10);
	GW_Display_Puts ("available for", GW_DISPLAY_FONT_7X10, 1);
	switch(Local_NodeID)
	{
	  case 1:
	  {
		  GW_Display_GotoXY (20, 20);
		  GW_Display_Puts ("Collision MCU", GW_DISPLAY_FONT_7X10, 1);
		  break;
	  }
	  case 2:
	  {
		  GW_Display_GotoXY (20, 20);
		  GW_Display_Puts ("Lighting MCU", GW_DISPLAY_FONT_7X10, 1);
		  break;
	  }
	  default:
	  {
		GW_Display_GotoXY (20, 20);
		GW_Display_Puts ("Unknown MCU", GW_DISPLAY_FONT_7X10, 1);
		break;
	  }
	}


	sprintf(Local_DateBuffer, "%d", Local_Estimate_time);
	GW_Display_GotoXY (20, 30);
	GW_Display_Puts ("Estimate: <", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (95, 30);
	GW_Display_Puts (Local_DateBuffer, GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (110, 30);
	GW_Display_Puts ("s", GW_DISPLAY_FONT_7X10, 1);

	GW_Display_GotoXY (30, 40);
	GW_Display_Puts ("Accept", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (30, 50);
	GW_Display_Puts ("Reject", GW_DISPLAY_FONT_7X10, 1);

	GW_Display_GotoXY (20, 40);
	GW_Display_Puts (">", GW_DISPLAY_FONT_7X10, 1);	//Cursor init point to Accept

	GW_Display_Update(); //display
}

static void Interface_DownloadingScreen (void)
{
	/* Writeing Text */
	GW_Display_GotoXY (40, 10);
	GW_Display_Puts ("Download", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (40, 20);
	GW_Display_Puts ("  in", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (40, 30);
	GW_Display_Puts ("Progress", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (50, 40);
	GW_Display_Puts ("  0%", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_Update(); //display
}

static void Interface_UpdateDownloadingScreen (uint8_t Cpy_Progress)
{
	/**/
	char local_DateBuffer[4];
	/* Clear Current Progress Text */
	sprintf(local_DateBuffer, "%d", Cpy_Progress);
	GW_Display_GotoXY (50, 40);
	GW_Display_Puts ("   %", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (50, 40);
	GW_Display_Puts (local_DateBuffer, GW_DISPLAY_FONT_7X10, 1);
	GW_Display_Update(); //display
}

static void Interface_DoneScreen (void)
{
	/* Writeing Text */
	GW_Display_GotoXY (40, 10);
	GW_Display_Puts ("Download", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (35, 20);
	GW_Display_Puts ("Completed", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (50, 30);
	GW_Display_Puts (" -_-  ", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_Update(); //display
}

static void Interface_InstallScreen(void)
{
	/* Update Screan */
	GW_Display_GotoXY (40, 10);
	GW_Display_Puts ("Installing", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (40, 20);
	GW_Display_Puts (" Firmware", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_GotoXY (40, 30);
	GW_Display_Puts (" to MCU ", GW_DISPLAY_FONT_7X10, 1);
	GW_Display_Update(); //display
}

static void Interface_CleanScrean(void)
{
	GW_Display_Clear();
	GW_Display_Update();
}

static void Interface_ProcessButton (void)
{
	while(1)
	{
	  if(GW_Platform_GetSwitchButton() == GW_BUTTON_PRESSED)
	  {
		  while(GW_Platform_GetSwitchButton() == GW_BUTTON_PRESSED);// Hold until button release
		  if(Global_CursorState == UI_CURSOR_AT_ACCEPT)
		  {
			  GW_Display_GotoXY (20, 40);
			  GW_Display_Puts (" ", GW_DISPLAY_FONT_7X10, 1);	//Cursor init point to Accept
			  GW_Display_GotoXY (20, 50);
			  GW_Display_Puts (">", GW_DISPLAY_FONT_7X10, 1);	//Cursor init point to Accept
			  Global_CursorState = UI_CURSOR_AT_REJECT;
		  }
		  else if(Global_CursorState == UI_CURSOR_AT_REJECT)
		  {
			  GW_Display_GotoXY (20, 40);
			  GW_Display_Puts (">", GW_DISPLAY_FONT_7X10, 1);	//Cursor init point to Accept
			  GW_Display_GotoXY (20, 50);
			  GW_Display_Puts (" ", GW_DISPLAY_FONT_7X10, 1);	//Cursor init point to Accept
			  Global_CursorState = UI_CURSOR_AT_ACCEPT;
		  }
		  else
		  {
			  //error
		  }
	  }
	  GW_Display_Update(); //display
	  GW_Platform_DelayMs(100);
	  if(GW_Platform_GetOkButton() == GW_BUTTON_PRESSED)
	  {
		 while(GW_Platform_GetOkButton() == GW_BUTTON_PRESSED);// Hold until button release
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




