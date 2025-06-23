#include "SensorControl.h"
//#define BASELINE_TESTING

SensorControl::SensorControl()
{
}


void SensorControl::init()
{
    if (!m_shtSensor.begin())
    {
        Serial.println("SHT3X-DIS Sensor not found :(");
        while (1);
    }
    Serial.print("Found SHT3X-DIS serial #");

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

    if (!m_sgpSensor.IAQinit()) 
    {
        Serial.println("SGP30 IAQinit failed");
    }
    delay(15000);

    // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
    #ifndef BASELINE_TESTING
        m_sgpSensor.setIAQBaseline(0x9021, 0x94E7);  // Will vary for each sensor!
    #endif
    //****Baseline values: eCO2: 0x8DCE & TVOC: 0x9026
    //****Baseline values: eCO2: 0x8C2A & TVOC: 0x8FAD
    //****Baseline values: eCO2: 0x9200 & TVOC: 0x9583
    //****Baseline values: eCO2: 0x9021 & TVOC: 0x94E7

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
    Serial.println(m_shtSensor.readStatus());
    sensorData->Temp = m_shtSensor.readTemperature()+m_offsetTemp; // [°C]
    sensorData->Hmd = m_shtSensor.readHumidity()+m_offsetHum; // [%RH]
    m_sgpSensor.setHumidity(getAbsoluteHumidity(sensorData->Temp, sensorData->Hmd));

    if (m_sgpSensor.IAQmeasure())
    {
        sensorData->TVOC = m_sgpSensor.TVOC;
        sensorData->eCO2 = m_sgpSensor.eCO2;
        Serial.print("TVOC: ");Serial.println(m_sgpSensor.TVOC);
        Serial.print("eCO2: ");Serial.println(m_sgpSensor.eCO2);
    }
    else
    {
        Serial.println("Measurement failed");
    }
    
    if (m_sgpSensor.IAQmeasureRaw())
    {
        sensorData->rawH2 = m_sgpSensor.rawH2;
        sensorData->rawEthanol = m_sgpSensor.rawEthanol;
        Serial.print("Raw H2: ");Serial.println(m_sgpSensor.rawH2);
        Serial.print("Raw Ethanol: ");Serial.println(m_sgpSensor.rawEthanol);
    }
    else
    {
        Serial.println("Raw Measurement failed");
    }

    sensorData->IsUpdated = true;
}

void SensorControl::printSensorStats(bool all)
{
    if (!m_sgpSensor.IAQmeasure())
    {
        Serial.println("Measurement failed");
        return;
    }

    if (all)
    {
        Serial.print("TVOC "); Serial.print(m_sgpSensor.TVOC); Serial.print(" ppb\t");
        Serial.print("eCO2 "); Serial.print(m_sgpSensor.eCO2); Serial.println(" ppm");
    }

    if (!m_sgpSensor.IAQmeasureRaw())
    {
        Serial.println("Raw Measurement failed");
        return;
    }
  
    if (all)
    {
        Serial.print("Raw H2 "); Serial.print(m_sgpSensor.rawH2); Serial.print(" \t");
        Serial.print("Raw Ethanol "); Serial.print(m_sgpSensor.rawEthanol); Serial.println("");
    }    

    #ifdef BASELINE_TESTING
        delay(1000);
        m_counter++;
        Serial.print(".");
        if (m_counter == 30)
        {
            Serial.println();
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
    #endif 
}