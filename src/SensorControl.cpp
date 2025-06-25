#include "SensorControl.h"
//#define BASELINE_TESTING

SensorControl::SensorControl()
{
}


void SensorControl::init()
{
    Wire1.begin();

    if (!m_ahtSensor.begin(&Wire1))
    {
        Serial.println("AHT21 Sensor not found :(");
        while (1);
    }
    Serial.println("Found AHT21");

    if (!m_ensSensor.begin())
    {
        Serial.println("ENS160 Sensor not found :(");
        digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
        while (1);
    }
    Serial.print("Found ENS160 and ");
    Serial.println(m_ensSensor.available() ? "ready." : "failed!");
    m_ensSensor.setMode(ENS160_OPMODE_STD);

    m_tempSensor = m_ahtSensor.getTemperatureSensor();
    m_tempSensor->printSensorDetails();

    m_humSensor = m_ahtSensor.getHumiditySensor();
    m_humSensor->printSensorDetails();
}


uint32_t SensorControl::getAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void SensorControl::readSensorData(SensorData* sensorData)
{
    // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
    //Serial.println(m_ahtSensor.getStatus());
    sensorData->Temp = getTemperature(); // [°C]
    sensorData->Hmd = getHumidity(); // [%RH]
    //m_sgpSensor.setHumidity(getAbsoluteHumidity(sensorData->Temp, sensorData->Hmd));

    if (m_ensSensor.available())
    {
        m_ensSensor.set_envdata(sensorData->Temp, sensorData->Hmd);

        if (m_ensSensor.measure())
        {
            sensorData->AQI = m_ensSensor.getAQI500();
            sensorData->TVOC = m_ensSensor.getTVOC();
            sensorData->eCO2 = m_ensSensor.geteCO2();
        }
        else
        {
            Serial.println("Measurement failed");
        }
    }

    sensorData->IsUpdated = true;
}

void SensorControl::printSensorStats(SensorData* sensorData)
{
    Serial.print("Temperature "); Serial.print(sensorData->Temp); Serial.println(" °C");
    Serial.print("Humidity "); Serial.print(sensorData->Hmd); Serial.println(" %RH");
    Serial.print("AQI500 "); Serial.print(sensorData->AQI); Serial.println();
    Serial.print("TVOC "); Serial.print(sensorData->TVOC); Serial.println(" ppb");
    Serial.print("eCO2 "); Serial.print(sensorData->eCO2); Serial.println(" ppm");
}


float SensorControl::getTemperature()
{
    sensors_event_t temp;
    m_tempSensor->getEvent(&temp);
    return temp.temperature+m_offsetTemp;
}

float SensorControl::getHumidity()
{
    sensors_event_t humidity;
    m_humSensor->getEvent(&humidity);
    return humidity.relative_humidity+m_offsetHum;
}