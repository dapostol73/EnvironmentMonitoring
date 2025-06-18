
#include <Arduino.h>
#include <lvgl.h>

#include "DisplayControl.h"
#include "DisplayFonts.h"

DisplayControl displayControl;

void setup() {
  // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    displayControl.init(0, &TekoMedium8pt7b);

    lv_obj_t *label = lv_label_create( lv_screen_active() );
    lv_label_set_text( label, "Hello Arduino, I'm LVGL!" );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
    digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
    delay(500);	// wait for half a second or 500 milliseconds
    digitalWrite (BUILTIN_LED, LOW);	// turn off the LED
    delay(500);	// wait for half a second or 500 milliseconds
}