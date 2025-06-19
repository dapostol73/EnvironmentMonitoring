
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
    delay(500);
    while (!Serial) { delay(10); } // Wait for serial console to open!
    Serial.println("Environment Monitoring Init");
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.println("Display Control Init");
    displayControl.init(0);
    Serial.println("Sensor Control Init");    
    sensorControl.init();
    Serial.println("Init Complete"); 
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