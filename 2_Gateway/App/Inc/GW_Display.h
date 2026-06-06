#ifndef GW_DISPLAY_H_
#define GW_DISPLAY_H_

#include <stdint.h>

typedef enum
{
	GW_DISPLAY_FONT_7X10 = 0,
	GW_DISPLAY_FONT_11X18
} GW_DisplayFont_t;

void GW_Display_Init(void);
void GW_Display_Clear(void);
void GW_Display_GotoXY(uint16_t x, uint16_t y);
void GW_Display_Puts(const char *text, GW_DisplayFont_t font, uint8_t color);
void GW_Display_Update(void);

#endif /* GW_DISPLAY_H_ */
