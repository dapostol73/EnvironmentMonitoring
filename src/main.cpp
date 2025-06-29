//*****************************************************************************
// Copyright (c) 2014 A12 Studios Inc. and Demetrius Apostolopoulos.
// All rights reserved.
//
// If Serial Logging support is required add this build flag to platformio.ini
//   build_flags = -D SERIAL_LOGGING
//*****************************************************************************

#include <Arduino.h>

#include "UserSettings.h"
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

DisplayControl displayControl;
SensorControl sensorControl;
SensorData sensorData;
ApplicationSettings appSettings;
A12Studios::NetworkManager networkManager;

long timeSinceLastRead = LONG_MIN;
long timeSinceLastUpload = LONG_MIN;
const uint16_t SENSOR_INTERVAL_SECS = 2; // Sensor query every X seconds
const uint16_t UPLOAD_INTERVAL_SECS = 1 * 60; // Upload every X minutes

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

void initNetwork()
{
	networkManager.init();
	uint8_t appSetID = networkManager.scanSettingsID(AppSettings, AppSettingsCount);
	appSettings = AppSettings[appSetID];
	networkManager.connectWiFi(appSettings.WifiSettings);
    networkManager.printStats();
}

void uploadSensorData(ThingSpeakInfo* thingSpeakInfo, SensorData* sensorData)
{
	if(!networkManager.NetClient.connect(thingSpeakInfo->Host, thingSpeakInfo->Port))
	{
		#ifdef SERIAL_LOGGING
		Serial.println("Connection to thinkspeak.com failed");
		#endif
		return;
	}

	// Three values(field1 field2 field3 field4) have been set in thingspeak.com 
	networkManager.NetClient.print(String("GET ") + "/update?api_key=" + thingSpeakInfo->APIKeyWrite
				+ "&field1=" + sensorData->AQI
				+ "&field2=" + sensorData->TVOC
				+ "&field3=" + sensorData->eCO2
				+ "&field4=" + sensorData->Temp
				+ "&field5=" + sensorData->Hmd
				+ " HTTP/1.1\r\n" 
				+ "Host: " + thingSpeakInfo->Host + "\r\n" 
				+ "Connection: close\r\n\r\n");

	while(networkManager.NetClient.available())
	{
		String line = networkManager.NetClient.readStringUntil('\r');
		#ifdef SERIAL_LOGGING
		Serial.print(line);
		#endif
	}

	#ifdef SERIAL_LOGGING
	Serial.println("Updated ThingSpeak");
	#endif
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
    initNetwork();
    blinkLED(4, 125);
}

void loop()
{
    if (millis() - timeSinceLastRead > (1000L*SENSOR_INTERVAL_SECS))
    {
        logPrint("Updating sensor data");
        sensorControl.readData(&sensorData);
        timeSinceLastRead = millis();
    }

    if (sensorData.IsUpdated)
    {
        displayControl.update(&sensorData);
        sensorControl.printStats(&sensorData);
        sensorData.IsUpdated = false;
    }
    
    lv_timer_handler(); /* let the GUI do its work */

    if (millis() - timeSinceLastUpload > (1000L*UPLOAD_INTERVAL_SECS))
    {
        logPrint("Upload sensor data");
        uploadSensorData(&appSettings.ThingSpeakSettings, &sensorData);
        timeSinceLastUpload = millis();
    }
}