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
    //m_gaugeVOC = setupLvglGauge(m_screenWidth, 24, 0, 100, m_colorAmber);
    //m_gaugeHumidity = setupLvglGauge(m_screenWidth-48, 24, 0, 100, m_colorAqua);
    m_gaugeCO2 = createLvglGaugeSimple(m_screenWidth, m_arcWidth, 0, 10000, m_colorYellow, m_colorGrayExtraDark);
    m_gaugeVOC = createLvglGaugeSimple(m_screenWidth-2*(m_arcWidth+m_arcMargin), m_arcWidth, 0, 5500, m_colorAmber, m_colorGrayDark);
    m_gaugeTemperature = createLvglGaugeSimple(m_screenWidth-4*(m_arcWidth+m_arcMargin), m_arcWidth, 0, 50, m_colorRed, m_colorGrayExtraDark);
    m_gaugeHumidity = createLvglGaugeSimple(m_screenWidth-6*(m_arcWidth+m_arcMargin), m_arcWidth, 0, 100, m_colorAqua, m_colorGrayDark);
    m_labelCO2 = createLvglLabel(0, 96, "NA CO2", m_colorYellow);
    m_labelVOC = createLvglLabel(0, 72, "NA VOC", m_colorAmber);
    m_labelTemperature = createLvglLabel(0, 48, "NA C", m_colorRed);
    m_labelHumidity = createLvglLabel(0, 24, "NA RH", m_colorAqua);
    lv_scr_load(m_screenBoot);
    lv_timer_handler();
    lv_scr_load(m_screenMain);
}

void DisplayControl::update(SensorData * sensorData)
{
    updateVOC(sensorData->TVOC);
    updateCO2(sensorData->eCO2);
    updateHumidity(sensorData->Hmd);
    updateTemperature(sensorData->Temp);
}

void DisplayControl::updateVOC(uint16_t value)
{
    char text[20];
    snprintf(text, sizeof(text), "%d VOC", value);
    lv_label_set_text(m_labelVOC, text);
    animateArc(m_gaugeVOC, max((int)value, 100));
}


void DisplayControl::updateCO2(uint16_t value)
{
    char text[20];
    snprintf(text, sizeof(text), "%d CO2", value);
    lv_label_set_text(m_labelCO2, text);
    animateArc(m_gaugeCO2, max((int)value, 100));
}

void DisplayControl::updateHumidity(float value)
{
    char text[20];
    snprintf(text, sizeof(text), "%.1f RH", value);
    lv_label_set_text(m_labelHumidity, text);
    animateArc(m_gaugeHumidity, max(value, 1.0f));
}

void DisplayControl::updateTemperature(float value)
{
    char text[20];
    snprintf(text, sizeof(text), "%.1f C", value);
    lv_label_set_text(m_labelTemperature, text);
    animateArc(m_gaugeTemperature, max(value, 1.0f));
}

void DisplayControl::animateArc(lv_obj_t * lv_arc, uint32_t value)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, lv_arc);
    lv_anim_set_values(&a, lv_arc_get_value(lv_arc), value);
    lv_anim_set_time(&a, m_updateFreq);
    lv_anim_set_exec_cb(&a, ArcCallback);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);
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
    m_screenBoot = lv_obj_create(NULL);
    m_screenMain = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(m_screenBoot, m_colorBlack, 0);
    lv_obj_t *img1 = lv_image_create(m_screenBoot);
    lv_image_set_src(img1, &a12_logo_horizantal_256x128_blue);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(m_screenMain, m_colorBlack, 0);
}


lv_obj_t* DisplayControl::createLvglLabel(int16_t x, int16_t y, const char * text, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(m_screenMain);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &CalibriBold28pt7b, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, x, y);

    return label;
}

lv_obj_t* DisplayControl::createLvglGauge(uint16_t size, uint16_t width, uint16_t minValue, uint16_t maxValue, lv_color_t color)
{
    lv_obj_t *mainArc = lv_arc_create(m_screenMain);
    uint16_t outsideWidth = width*0.75;
    uint16_t outsideSize = size-(width-outsideWidth);
    lv_obj_set_size(mainArc, outsideSize, outsideSize);
    lv_obj_align(mainArc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(mainArc, m_start, m_end);

    lv_obj_set_style_arc_color(mainArc, m_colorBlack, LV_PART_MAIN);
    lv_obj_set_style_arc_color(mainArc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(mainArc, m_round, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(mainArc, outsideWidth, LV_PART_INDICATOR);
    lv_obj_remove_style(mainArc, NULL, LV_PART_KNOB);
    lv_arc_set_range(mainArc, minValue, maxValue);

    createLvglArcLines(size-width, width*0.5, m_start, m_radius, 20, m_colorGrayMed);
    createLvglArcLines(size, width, m_start, m_radius, 5, m_colorWhite);

    uint16_t insideWidth = 4;
    uint16_t insideSize = size-(2*width)+(2*insideWidth);
    createLvglArcSimple(insideSize, insideWidth, m_start, m_radius, m_colorWhite);

    return mainArc;
}

lv_obj_t* DisplayControl::createLvglGaugeSimple(uint16_t size, uint16_t width, uint16_t minValue, uint16_t maxValue, lv_color_t fgColor, lv_color_t bgColor)
{
    lv_obj_t *simpleArc = lv_arc_create(m_screenMain);
    lv_obj_set_size(simpleArc, size, size);
    lv_obj_align(simpleArc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(simpleArc, m_start, m_end);
    lv_obj_set_style_arc_color(simpleArc, bgColor, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(simpleArc, m_round, LV_PART_MAIN);
    lv_obj_set_style_arc_color(simpleArc, fgColor, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(simpleArc, m_round, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(simpleArc, width, LV_PART_INDICATOR);
    lv_obj_remove_style(simpleArc, NULL, LV_PART_KNOB);
    lv_arc_set_range(simpleArc, minValue, maxValue);

    return simpleArc;
}

void DisplayControl::createLvglArcSimple(uint16_t size, uint16_t width, float start, float radius, lv_color_t color)
{
    lv_obj_t *lineArc = lv_arc_create(m_screenMain);
    lv_obj_set_size(lineArc, size, size);
    lv_obj_align(lineArc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(lineArc, start, start+radius);

    lv_obj_set_style_arc_color(lineArc, color, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(lineArc, m_round, LV_PART_MAIN);
    lv_obj_set_style_arc_width(lineArc, width, LV_PART_MAIN);
    lv_obj_remove_style(lineArc, NULL, LV_PART_INDICATOR);
    lv_obj_remove_style(lineArc, NULL, LV_PART_KNOB);
}

void DisplayControl::createLvglArcLines(uint16_t size, uint16_t width, float start, float radius, uint16_t lines, lv_color_t color)
{
    float segment = radius/lines;
    for (uint8_t i = 0; i <= lines; i++)
    {
        lv_obj_t *lineArc = lv_arc_create(m_screenMain);
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


