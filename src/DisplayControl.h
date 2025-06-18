#ifndef _DISPLAY_CONTROL_H_
#define _DISPLAY_CONTROL_H_

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <gfxfont.h>

#define TFT_CS 15 // GFX_NOT_DEFINED for display without CS pin
#define TFT_DC 4
#define TFT_RST 2
#define TFT_SCK 14
#define TFT_MOSI 18
#define TFT_HOR_RES   240
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

extern Arduino_DataBus *DisplayBus;
extern Arduino_GFX *DisplayGFX;

class DisplayControl
{
	private:
        uint32_t m_drawBuffer[DRAW_BUF_SIZE / 4];
		uint8_t m_currentLine = 0;
		uint8_t m_lineHeight  = 10;
		uint8_t m_maxLines    = 0;

        void setMaxLines();
    
    protected:
		const GFXfont *m_gfxFont;
		const GFXfont *m_gfxFontDefault;
		const GFXfont *m_gfxFontTemp;

    public:
        DisplayControl();
        void init(uint16_t rotation, const GFXfont *gfxFont);
		void setFont(const GFXfont *font);

};

#endif