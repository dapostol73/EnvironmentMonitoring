
#include <Arduino.h>

#include "DisplayControl.h"
#include "SensorControl.h"
#include "SensorData.h"

long timeSinceLastRead = LONG_MIN;
const uint16_t SENSOR_INTERVAL_SECS = 1; // Sensor query every 5 seconds

DisplayControl displayControl;
SensorControl sensorControl;
SensorData sensorData;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    while (!Serial) { delay(10); } // Wait for serial console to open!

    Serial.println("Environment Monitoring Init");
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.println("Display Control Init");
    displayControl.init(0);
    Serial.println("Sensor Control Init");    
    sensorControl.init();
    Serial.println("Init Complete"); 
    delay(1000);
}

void loop()
{
    if (millis() - timeSinceLastRead > (1000L*SENSOR_INTERVAL_SECS))
	{
		Serial.println("Updating sensor data");
        sensorControl.readSensorData(&sensorData);
		timeSinceLastRead = millis();
        digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
        delay(500);	// wait for half a second or 500 milliseconds
        digitalWrite (BUILTIN_LED, LOW);	// turn off the LED
        delay(500);	// wait for half a second or 500 milliseconds
	}

    if (sensorData.IsUpdated)
    {
        displayControl.update(&sensorData);
        sensorData.IsUpdated = false;
    }

    //sensorControl.printSensorStats(true);
    lv_timer_handler(); /* let the GUI do its work */
}