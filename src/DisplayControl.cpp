#include "DisplayControl.h"

Arduino_DataBus *DisplayBus = new Arduino_HWSPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI);
Arduino_GFX *DisplayGFX = new Arduino_GC9A01(DisplayBus, TFT_RST, 0 /* rotation */, true /* IPS */);

/* LVGL calls it when a rendered image needs to copied to the display*/
void FlushDisplay(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    #ifndef DIRECT_RENDER_MODE
        uint32_t w = lv_area_get_width(area);
        uint32_t h = lv_area_get_height(area);

        DisplayGFX->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    #endif // #ifndef DIRECT_RENDER_MODE

    /*Call it to tell LVGL you are ready*/
    lv_disp_flush_ready(disp);
}

DisplayControl::DisplayControl()
{
}


void DisplayControl::init(uint16_t rotation, const GFXfont *gfxFont)
{
    DisplayGFX->begin();
    DisplayGFX->setRotation(rotation);
    m_gfxFontDefault = gfxFont;
    setFont(gfxFont);
    DisplayGFX->fillScreen(BLACK);
    DisplayGFX->setTextColor(WHITE);

    lv_init();
    lv_tick_set_cb(millis);
    lv_display_t *disp;
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, FlushDisplay);
    lv_display_set_buffers(disp, m_drawBuffer, NULL, sizeof(m_drawBuffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void DisplayControl::setMaxLines()
{
    switch (DisplayGFX->getRotation())
    {
        case 1:
        case 3:
            m_maxLines = floor(TFT_HOR_RES/m_lineHeight);
            break;
        default:
            m_maxLines = floor(TFT_VER_RES/m_lineHeight);
            break;
    } 
}

void DisplayControl::setFont(const GFXfont *gfxFont)
{
    m_gfxFont = gfxFont;
    DisplayGFX->setFont(m_gfxFont);
    m_lineHeight = pgm_read_byte(&m_gfxFont->yAdvance);
    
    setMaxLines();
}