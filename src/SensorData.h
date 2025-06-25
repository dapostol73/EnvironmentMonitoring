
#ifndef _SENSOR_DATA_H_
#define _SENSOR_DATA_H_

#include <Arduino.h>

struct SensorData
{
	bool IsUpdated = false;
    float Temp = 21.0; //temperature
    float Hmd = 45.0; //humidity
    uint16_t AQI = 0;
    uint16_t TVOC = 0; //light
    uint16_t eCO2 = 0; //atmospheric
};

#endif