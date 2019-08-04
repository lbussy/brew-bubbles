/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (Brew-Bubbles).

Brew Bubbles is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Brew Bubbles is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along
with Brew Bubbles. If not, see <https://www.gnu.org/licenses/>. */

#include "bubbles.h"

Counter counter(COUNTPIN);      // Create an instance of the counter
unsigned long ulNow = millis(); // Time in millis now
unsigned long ulStart = 0UL;    // Start time

void Bubbles() {
    const char* hostname = HOSTNAME;    // Hostname (TODO: Get this from wifi setup)
    ulNow = millis();

    if( ulNow - ulStart > BUBLOOP) { // If (now - start) > delay time, do work
        ulStart = ulNow;
        float fBpm = counter.GetPpm();

#ifdef READTEMP

#ifdef AMBSENSOR
        OneWire ambient(AMBSENSOR);
        DallasTemperature sensorAmbient(&ambient);
        sensorAmbient.begin();
        sensorAmbient.requestTemperatures();

#if TEMPFORMAT == F
        float fAmbTemp = sensorAmbient.getTempFByIndex(0);
#else
        float fAmbTemp = sensorAmbient.getTempCByIndex(0);
#endif // TEMPFORMAT

#endif // AMBSENSOR

#ifdef VESSENSOR
        OneWire vessel(VESSENSOR);
        DallasTemperature sensorVessel(&vessel);
        sensorVessel.begin();
        sensorVessel.requestTemperatures();

#if TEMPFORMAT == F
        float fVesTemp = sensorVessel.getTempFByIndex(0);
#else
        float fVesTemp = sensorVessel.getTempCByIndex(0);
#endif // TEMPFORMAT

#endif // VESSENSOR

#if TEMPFORMAT == F
        const char* format = "F";
#else
        const char* format = "C";
#endif // TEMPFORMAT

#endif // READTEMP

        // Serialize data with ArduinoJson

        /*         
        Sample (230 bytes to allow for string duplication):
        {
                "api_key":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                "vessel":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                "format":"F",
                "bubbles":{
                        "bpm":0,
                        "ambtemp":70.3625,
                        "vestemp":-196.6
                }
        }
        */

        const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
        DynamicJsonDocument bubbleJson(capacity);

        bubbleJson["api_key"] = API_KEY;
        bubbleJson["vessel"] = hostname;
        
#ifdef READTEMP
        bubbleJson["format"] = format;
#endif // READTEMP

        JsonObject bubbles = bubbleJson.createNestedObject("bubbles");
        bubbles["bpm"] = fBpm;

#ifdef AMBSENSOR
        bubbles["ambtemp"] = fAmbTemp;
#endif // AMBSENSOR

#ifdef VESSENSOR
        bubbles["vestemp"] = fVesTemp;
#endif // VESSENSOR

        // Serialize JSON
        char strBubbleJson[230];
        serializeJson(bubbleJson, strBubbleJson, sizeof(strBubbleJson));
        httppost(strBubbleJson); // Post JSON date to endpoint
    }
}
