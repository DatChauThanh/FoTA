#include "GW_App.h"
#include "GW_Platform.h"

int main(void)
{
	if (GW_Platform_Init() == GW_STATUS_OK)
	{
		GW_App_Init();

		while (1)
		{
			GW_App_MainFunction();
		}
	}

	while (1)
	{
	}
}
