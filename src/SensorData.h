
#ifndef _SENSOR_DATA_H_
#define _SENSOR_DATA_H_

#include <Arduino.h>

namespace A12Studios
{
    struct SensorData
    {
        bool IsUpdated = false;
        float Temp = 0.0;  //Temperature °C
        float Hmd = 0.0;   //Relative Humidity %
        uint8_t AQI = 0;   //Air Quality Index
        uint16_t TVOC = 0; //Total Volatile Organic Compounds ppb
        uint16_t eCO2 = 0; //Crabon Dioxide ppm
    };

    struct SensorDataAverage
    {
        float Temp = 0.0;    //Temperature °C
        float Hmd = 0.0;     //Relative Humidity %
        uint16_t AQI = 0;    //Air Quality Index
        uint32_t TVOC = 0;   //Total Volatile Organic Compounds ppb
        uint32_t eCO2 = 0;   //Crabon Dioxide ppm
        uint8_t Samples = 0; //Number of samples to average
    };
}

#endif