/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "sensors.h"

#include "config.h"
#include "jsonconfig.h"
#include <OneWire.h>
#include <DS18B20.h>
#include <ArduinoJson.h>

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

double getTemp(uint8_t pin)
{
    float retVal;
    if (config.bubble.tempinf)
        retVal = (float)DEVICE_DISCONNECTED_F;
    else
        retVal =  (float)DEVICE_DISCONNECTED_C;
    OneWire oneWire(pin);
    DS18B20 sensor(&oneWire);
    if (!sensor.begin())
    {
        // Not sure how we got here
        return retVal;
    }
    else
    {
        sensor.setResolution(TEMP_12_BIT);
        sensor.requestTemperatures();

        while (!sensor.isConversionComplete())
            ; // Wait for conversion

            if (config.bubble.tempinf)
            {
                retVal = (sensor.getTempC() * (9/5)) + 32;
                if (pin == AMBSENSOR)
                {
                    retVal = retVal + config.calibrate.room;
                }
                else if (pin == VESSENSOR)
                {
                    retVal = retVal + config.calibrate.vessel;
                }
            }
            else
            {
                retVal = sensor.getTempC();
                if (pin == AMBSENSOR)
                {
                    retVal = retVal + config.calibrate.room;
                }
                else if (pin == VESSENSOR)
                {
                    retVal = retVal + config.calibrate.vessel;
                }
            }
    }

    return retVal;
}
