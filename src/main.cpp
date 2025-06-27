//*****************************************************************************
// Copyright (c) 2014 A12 Studios Inc. and Demetrius Apostolopoulos.
// All rights reserved.
//
// If Serial Logging support is required add this build flag to platformio.ini
//   build_flags = -D SERIAL_LOGGING
//*****************************************************************************

#include <Arduino.h>

#include "DisplayControl.h"
#include "SensorControl.h"
#include "SensorData.h"
#include "NetworkManager.h"

using namespace A12Studios;

#ifndef SERIAL_LOGGING
// disable Serial output
#define Serial KillDefaultSerial
static class {
public:
    void begin(...) {}
    void print(...) {}
    void println(...) {}
} Serial;
#endif

long timeSinceLastRead = LONG_MIN;
const uint16_t SENSOR_INTERVAL_SECS = 2; // Sensor query every X seconds

DisplayControl displayControl;
SensorControl sensorControl;
SensorData sensorData;
A12Studios::NetworkManager networkManager;

void logPrint(const char * info, bool endLine = true)
{
    #ifdef SERIAL_LOGGING
    endLine ? Serial.println(info) : Serial.print(info);
    #endif
}

void blinkLED(uint8_t times, uint16_t freq)
{
    for (uint8_t i = 0; i < times; i++)
    {
        digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
        delay(freq);	// wait for half a second or 500 milliseconds
        digitalWrite (BUILTIN_LED, LOW);	// turn off the LED
        delay(freq);	// wait for half a second or 500 milliseconds   
    } 
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    while (!Serial) { delay(10); } // Wait for serial console to open!

    logPrint("Environment Monitoring Init");
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    logPrint("Display Control Init");
    displayControl.init(0);
    logPrint("Sensor Control Init");    
    sensorControl.init();
    logPrint("Init Complete"); 
    blinkLED(2, 250);
}

void loop()
{
    if (millis() - timeSinceLastRead > (1000L*SENSOR_INTERVAL_SECS))
    {
        logPrint("Updating sensor data");
        sensorControl.readSensorData(&sensorData);
        timeSinceLastRead = millis();
    }

    if (sensorData.IsUpdated)
    {
        displayControl.update(&sensorData);
        sensorControl.printSensorStats(&sensorData);
        sensorData.IsUpdated = false;
    }
    
    lv_timer_handler(); /* let the GUI do its work */
}