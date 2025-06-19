#ifndef _SENSOR_CONTROL_H_
#define _SENSOR_CONTROL_H_

#include <Arduino.h>
#include <Adafruit_SGP30.h>
#include "SensorData.h"

class SensorControl
{
	private:
        Adafruit_SGP30 m_sensor;
        uint32_t m_counter;

    protected:

    public:
        SensorControl();
        void init();
        uint32_t getAbsoluteHumidity(float temperature, float humidity);
        void readSensorData(SensorData* sensorData);
        void printSensorDataStats();
};

#endif