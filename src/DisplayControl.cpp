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

void TouchRead( lv_indev_t * indev, lv_indev_data_t * data )
{
    /*For example  ("my_..." functions needs to be implemented by you)
    int32_t x, y;
    bool touched = my_get_touch( &x, &y );

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;

        data->point.x = x;
        data->point.y = y;
    }
     */
}

void ArcCallback(void *arc, int32_t value)
{
    lv_arc_set_value((lv_obj_t*)arc, value);
}

DisplayControl::DisplayControl()
{
}


void DisplayControl::init(uint16_t rotation)
{
    DisplayGFX->begin();
    m_screenWidth = DisplayGFX->width();
    m_screenHeight = DisplayGFX->height();
    DisplayGFX->setRotation(rotation);
    DisplayGFX->fillScreen(BLACK);
    DisplayGFX->setTextColor(WHITE);
    setupLvglDisplay();
    setupLvglTouch();
    setupLvglScreen();
    m_gaugeVOC = setupLvglGauge(m_screenWidth, 24, 0, 100, m_colorAmber);
    m_gaugeHumidity = setupLvglGauge(m_screenWidth-48, 24, 0, 100, m_colorAqua);
    m_labelVOC = setupLvglLabel(0, -16, "VOC: NA", m_colorAmber);
    m_labelHumidity = setupLvglLabel(0, 16, "Hum: NA", m_colorAqua);
}

void DisplayControl::updateVOC(uint16_t value)
{
    float percentage = map(value, 0, 5500, 0, 100);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, m_gaugeVOC);
    lv_anim_set_values(&a, lv_arc_get_value(m_gaugeVOC), percentage);
    lv_anim_set_time(&a, m_updateFreq);
    lv_anim_set_exec_cb(&a, ArcCallback);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    char text[20];
    snprintf(text, sizeof(text), "VOC: %d", value);
    lv_label_set_text(m_labelVOC, text);
}

void DisplayControl::updateHumidity(float value)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, m_gaugeHumidity);
    lv_anim_set_values(&a, lv_arc_get_value(m_gaugeHumidity), value);
    lv_anim_set_time(&a, m_updateFreq);
    lv_anim_set_exec_cb(&a, ArcCallback);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    char text[20];
    snprintf(text, sizeof(text), "Hum: %.1f", value);
    lv_label_set_text(m_labelHumidity, text);
}

void DisplayControl::setupLvglDisplay()
{
    lv_init();
    lv_tick_set_cb(millis);
    lv_display_t *disp;
    disp = lv_display_create(m_screenWidth, m_screenHeight);
    lv_display_set_flush_cb(disp, FlushDisplay);
    lv_display_set_buffers(disp, m_drawBuffer, NULL, sizeof(m_drawBuffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void DisplayControl::setupLvglTouch()
{
    /*Initialize the (dummy) input device driver*/
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, TouchRead);
}

void DisplayControl::setupLvglScreen()
{
    m_screen = lv_scr_act();
    lv_obj_set_style_bg_color(m_screen, m_colorBlack, 0);
}

lv_obj_t* DisplayControl::setupLvglGauge(uint16_t size, uint16_t width, uint16_t minValue, uint16_t maxValue, lv_color_t color)
{
    lv_obj_t *mainArc = lv_arc_create(m_screen);
    uint16_t outsideWidth = width*0.75;
    uint16_t outsideSize = size-(width-outsideWidth);
    float start = 135.0;
    float radius = 270.0;
    float end = start+radius;
    lv_obj_set_size(mainArc, outsideSize, outsideSize);
    lv_obj_align(mainArc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(mainArc, start, end);

    lv_obj_set_style_arc_color(mainArc, m_colorBlack, LV_PART_MAIN);
    lv_obj_set_style_arc_color(mainArc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(mainArc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(mainArc, outsideWidth, LV_PART_INDICATOR);
    lv_obj_remove_style(mainArc, NULL, LV_PART_KNOB);

    lv_arc_set_range(mainArc, minValue, maxValue);
    lv_arc_set_value(mainArc, maxValue*0.3);

    createLvglArcLines(size-width, width*0.5, start, radius, 20, m_colorGray);
    createLvglArcLines(size, width, start, radius, 5, m_colorWhite);

    uint16_t insideWidth = 4;
    uint16_t insideSize = size-(2*width)+(2*insideWidth);
    createLvglArcSimple(insideSize, insideWidth, start, radius, m_colorWhite);

    return mainArc;
}

void DisplayControl::createLvglArcSimple(uint16_t size, uint16_t width, float start, float radius, lv_color_t color)
{
    lv_obj_t *lineArc = lv_arc_create(m_screen);
    lv_obj_set_size(lineArc, size, size);
    lv_obj_align(lineArc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(lineArc, start, start+radius);

    lv_obj_set_style_arc_color(lineArc, color, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(lineArc, false, LV_PART_MAIN);
    lv_obj_set_style_arc_width(lineArc, width, LV_PART_MAIN);
    lv_obj_remove_style(lineArc, NULL, LV_PART_INDICATOR);
    lv_obj_remove_style(lineArc, NULL, LV_PART_KNOB);
}

void DisplayControl::createLvglArcLines(uint16_t size, uint16_t width, float start, float radius, uint16_t lines, lv_color_t color)
{
    float segment = radius/lines;
    for (uint8_t i = 0; i <= lines; i++)
    {
        lv_obj_t *lineArc = lv_arc_create(m_screen);
        uint16_t lineStart = start+(segment*i);
        lv_obj_set_size(lineArc, size, size);
        lv_obj_align(lineArc, LV_ALIGN_CENTER, 0, 0);
        lv_arc_set_bg_angles(lineArc, lineStart, lineStart+2);

        lv_obj_set_style_arc_color(lineArc, color, LV_PART_MAIN);
        lv_obj_set_style_arc_rounded(lineArc, false, LV_PART_MAIN);
        lv_obj_set_style_arc_width(lineArc, width, LV_PART_MAIN);
        lv_obj_remove_style(lineArc, NULL, LV_PART_INDICATOR);
        lv_obj_remove_style(lineArc, NULL, LV_PART_KNOB);
    }    
}

lv_obj_t* DisplayControl::setupLvglLabel(int16_t x, int16_t y, const char * text, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(m_screen);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &CalibriBold28pt7b, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, x, y);

    return label;
}