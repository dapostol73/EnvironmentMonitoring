
#include <Arduino.h>
#include <lvgl.h>

#include "DisplayControl.h"

DisplayControl displayControl;

void setup() {
  // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    displayControl.init(0);
}

void loop()
{
    float voc = random(10, 15);
    float hum = random(40, 50);
    displayControl.updateVOC(voc);
    displayControl.updateHumidity(hum);
    digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
    delay(250);	// wait for half a second or 500 milliseconds
    digitalWrite (BUILTIN_LED, LOW);	// turn off the LED
    delay(250);	// wait for half a second or 500 milliseconds
    lv_timer_handler(); /* let the GUI do its work */
}