
#include <Arduino.h>
#include <Adafruit_SGP30.h>

#include "DisplayControl.h"
#include "SensorControl.h"
#include "SensorData.h"

DisplayControl displayControl;
SensorControl sensorControl;
SensorData sensorData;

void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(10); } // Wait for serial console to open!
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    displayControl.init(0);
    sensorControl.init();
}

void loop()
{
    sensorControl.readSensorData(&sensorData);
    if (sensorData.IsUpdated)
    {
        displayControl.updateVOC(sensorData.TVOC);
        displayControl.updateHumidity(sensorData.Hmd);
        sensorData.IsUpdated = false;
    }
    lv_timer_handler(); /* let the GUI do its work */
    digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
    delay(500);	// wait for half a second or 500 milliseconds
    digitalWrite (BUILTIN_LED, LOW);	// turn off the LED
    delay(500);	// wait for half a second or 500 milliseconds
}