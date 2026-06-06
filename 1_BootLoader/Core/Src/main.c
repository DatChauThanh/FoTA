#include "BL_Interface.h"
#include "BL_Platform.h"

int main(void)
{
	if (BL_Platform_Init() == BL_STATUS_OK)
	{
		BL_voidBootLoader_Init();
	}

	while (1)
	{
	}
}
