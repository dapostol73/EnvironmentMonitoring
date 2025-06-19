#include "SensorControl.h"


SensorControl::SensorControl()
{
}


void SensorControl::init()
{

    if (!m_shtSensor.begin())
    {
        Serial.println("SHT30 Sensor not found :(");
        while (1);
    }

    if (!m_sgpSensor.begin(&Wire1))
    {
        Serial.println("SGP30 Sensor not found :(");
        digitalWrite (BUILTIN_LED, HIGH);	// turn on the LED
        while (1);
    }
    Serial.print("Found SGP30 serial #");
    Serial.print(m_sgpSensor.serialnumber[0], HEX);
    Serial.print(m_sgpSensor.serialnumber[1], HEX);
    Serial.println(m_sgpSensor.serialnumber[2], HEX);

    // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
    m_sgpSensor.setIAQBaseline(0x8DDF, 0x8C9D);  // Will vary for each sensor!
    //****Baseline values: eCO2: 0x8BBB & TVOC: 0x8C14
    //****Baseline values: eCO2: 0x8D52 & TVOC: 0x8C5C
    //****Baseline values: eCO2: 0x8DC6 & TVOC: 0x8C61
    //****Baseline values: eCO2: 0x8DD4 & TVOC: 0x8C81
    //****Baseline values: eCO2: 0x8DDF & TVOC: 0x8C8D
    //****Baseline values: eCO2: 0x8DE9 & TVOC: 0x8C92
    //****Baseline values: eCO2: 0x8DDA & TVOC: 0x8C9D
    //****Baseline values: eCO2: 0x8DD3 & TVOC: 0x8CB9
    //****Baseline values: eCO2: 0x8DCB & TVOC: 0x8D09
    //****Baseline values: eCO2: 0x8DBD & TVOC: 0x8D06
    //****Baseline values: eCO2: 0x8DAE & TVOC: 0x8D03
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
    sensorData->Temp = m_shtSensor.readTemperature(); // [°C]
    sensorData->Hmd = m_shtSensor.readHumidity(); // [%RH]
    m_sgpSensor.setHumidity(getAbsoluteHumidity(sensorData->Temp, sensorData->Hmd));

    if (!m_sgpSensor.IAQmeasure())
    {
        Serial.println("Measurement failed");
        return;
    }

    sensorData->TVOC = m_sgpSensor.TVOC;
    sensorData->eCO2 = m_sgpSensor.eCO2;
    
    if (!m_sgpSensor.IAQmeasureRaw())
    {
        Serial.println("Raw Measurement failed");
        return;
    }

    sensorData->rawH2 = m_sgpSensor.rawH2;
    sensorData->rawEthanol = m_sgpSensor.rawEthanol;
    sensorData->IsUpdated = true;
}

void SensorControl::printSensorDataStats()
{
    if (!m_sgpSensor.IAQmeasure())
    {
        Serial.println("Measurement failed");
        return;
    }

    Serial.print("TVOC "); Serial.print(m_sgpSensor.TVOC); Serial.print(" ppb\t");
    Serial.print("eCO2 "); Serial.print(m_sgpSensor.eCO2); Serial.println(" ppm");

    if (!m_sgpSensor.IAQmeasureRaw())
    {
        Serial.println("Raw Measurement failed");
        return;
    }
  
    Serial.print("Raw H2 "); Serial.print(m_sgpSensor.rawH2); Serial.print(" \t");
    Serial.print("Raw Ethanol "); Serial.print(m_sgpSensor.rawEthanol); Serial.println("");

    delay(1000);

    m_counter++;
    if (m_counter == 30)
    {
        m_counter = 0;

        uint16_t TVOC_base, eCO2_base;
        if (!m_sgpSensor.getIAQBaseline(&eCO2_base, &TVOC_base)) 
        {
            Serial.println("Failed to get baseline readings");
            return;
        }
        Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
        Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
    }    
}