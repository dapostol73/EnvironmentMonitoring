#ifndef _SENSOR_CONTROL_H_
#define _SENSOR_CONTROL_H_

#include <Arduino.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include "SensorData.h"

class SensorControl
{
	private:
        Adafruit_SGP30 m_sgpSensor;
        Adafruit_SHT31 m_shtSensor = Adafruit_SHT31(&Wire1);
        float m_offsetTemp = -4.9;
        float m_offsetHum = 3.0;
        uint32_t m_counter;

    protected:

    public:
        SensorControl();
        void init();
        uint32_t getAbsoluteHumidity(float temperature, float humidity);
        void readSensorData(SensorData* sensorData);
        void printSensorStats(bool all = false);
};

#endif