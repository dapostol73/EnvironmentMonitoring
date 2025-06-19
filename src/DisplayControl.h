#ifndef _DISPLAY_CONTROL_H_
#define _DISPLAY_CONTROL_H_

#include <Arduino_GFX_Library.h>
#include <lvgl.h>

#include "DisplayFonts.h"

#define TFT_SCK      20
#define TFT_MOSI     19
#define TFT_DC       18
#define TFT_CS       15
#define TFT_RST      14
#define TFT_HOR_RES  240
#define TFT_VER_RES  240
#define TFT_ROTATION LV_DISPLAY_ROTATION_0

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

extern Arduino_DataBus *DisplayBus;
extern Arduino_GFX *DisplayGFX;

class DisplayControl
{
	private:
        uint32_t m_drawBuffer[DRAW_BUF_SIZE / 4];
		uint32_t m_updateFreq = 250;
		uint32_t m_screenWidth;
		uint32_t m_screenHeight;
		lv_obj_t *m_screen;
		lv_obj_t *m_gaugeVOC;
		lv_obj_t *m_labelVOC;
		lv_obj_t *m_gaugeHumidity;
		lv_obj_t *m_labelHumidity;
		lv_color_t m_colorBlack = LV_COLOR_MAKE(0, 0, 0);
		lv_color_t m_colorGray = LV_COLOR_MAKE(128, 128, 128);
		lv_color_t m_colorWhite = LV_COLOR_MAKE(235, 235, 235);
		lv_color_t m_colorAmber = LV_COLOR_MAKE(250, 140, 0);
		lv_color_t m_colorAqua = LV_COLOR_MAKE(0, 64, 255);

		void setupLvglDisplay();
		void setupLvglTouch();
		void setupLvglScreen();
		lv_obj_t* setupLvglGauge(uint16_t size, uint16_t width, uint16_t minValue, uint16_t maxValue, lv_color_t color);
		void createLvglArcSimple(uint16_t size, uint16_t width, float start, float radius, lv_color_t color);
		void createLvglArcLines(uint16_t size, uint16_t width, float start, float radius, uint16_t lines, lv_color_t color);
		
		lv_obj_t* setupLvglLabel(int16_t x, int16_t y, const char * text, lv_color_t color);
    protected:

    public:
        DisplayControl();
        void init(uint16_t rotation);
		void updateVOC(float value);
		void updateHumidity(float value);
};

#endif