#include "GW_Display.h"
#include "SSD1306.h"

static FontDef_t *GW_Display_MapFont(GW_DisplayFont_t font)
{
	switch (font)
	{
		case GW_DISPLAY_FONT_11X18:
			return &Font_11x18;

		case GW_DISPLAY_FONT_7X10:
		default:
			return &Font_7x10;
	}
}

void GW_Display_Init(void)
{
	SSD1306_Init();
}

void GW_Display_Clear(void)
{
	SSD1306_Clear();
}

void GW_Display_GotoXY(uint16_t x, uint16_t y)
{
	SSD1306_GotoXY(x, y);
}

void GW_Display_Puts(const char *text, GW_DisplayFont_t font, uint8_t color)
{
	SSD1306_Puts((char *)text, GW_Display_MapFont(font), color);
}

void GW_Display_Update(void)
{
	SSD1306_UpdateScreen();
}
