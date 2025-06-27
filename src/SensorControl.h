#ifndef _SENSOR_CONTROL_H_
#define _SENSOR_CONTROL_H_

#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <ScioSense_ENS160.h>
#include "SensorData.h"

class SensorControl
{
	private:
        Adafruit_AHTX0 m_ahtSensor;
        ScioSense_ENS160 m_ensSensor = ScioSense_ENS160(&Wire1, ENS160_I2CADDR_1);
        Adafruit_Sensor *m_tempSensor;
        Adafruit_Sensor *m_humSensor;
        float m_offsetTemp = -11.0;
        float m_offsetHum = 10.0;
        uint32_t m_counter;

    protected:
        float getTemperature();
        float getHumidity();

    public:
        SensorControl();
        void init();
        uint32_t getAbsoluteHumidity(float temperature, float humidity);
        void readSensorData(SensorData* sensorData);
        void printSensorStats(SensorData* sensorData);
};

#endif