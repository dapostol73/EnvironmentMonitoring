
#ifndef _SENSOR_DATA_H_
#define _SENSOR_DATA_H_

#include <Arduino.h>

struct SensorData
{
	bool IsUpdated = false;
    float Temp = 0.0; //temperature
    float Hmd = 0.0; //humidity
    uint16_t TVOC = 0.0; //light
    uint16_t eCO2 = 0.0; //atmospheric
    uint16_t rawH2 = 0.0; //altitude
    uint16_t rawEthanol = 0.0; //altitude
};

#endif