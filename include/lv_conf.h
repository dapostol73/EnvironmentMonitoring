#ifndef LV_CONF_H
#define LV_CONF_H

/* Let LVGL know we're using a simplified config include */
#ifndef LV_CONF_INCLUDE_SIMPLE
    #define LV_CONF_INCLUDE_SIMPLE
#endif

/* Core options */
#define LV_USE_LOG         0    // Disable logging
#define LV_USE_ASSERT_NULL 1    // Enable null pointer checks (optional)
#define LV_USE_FLOAT       1

/* Drivers / features you want to use */
#define LV_USE_DRAW_SW    1     // Software rendering (required in most cases)
#define LV_USE_DISP_BUF   1     // Display buffer management

/* Widgets */
#define LV_USE_LABEL      1     // Enable labels
#define LV_USE_BTN        0     // Enable buttons

/* Display driver support */
#define LV_COLOR_DEPTH    16    // 16-bit color is common for TFTs
#define LV_HOR_RES_MAX    240   // Set to your screen's resolution
#define LV_VER_RES_MAX    240

/* Fonts */
#define LV_FONT_MONTSERRAT_24 1

#endif // LV_CONF_H
