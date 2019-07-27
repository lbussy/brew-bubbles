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

Counter counter(COUNTPIN); // Create an instance of the counter

void Bubbles() {
    unsigned long ulNow = millis();         // Time in millis now
    const char* hostname = HOSTNAME;        // Hostname (TODO: Get this from wifi setup)
    unsigned long ulStart = 0UL;            // Start time

#ifdef READTEMP

#ifdef AMBSENSOR
    OneWire ambient(AMBSENSOR);
    DallasTemperature sensorAmbient(&ambient);
    sensorAmbient.begin();
#endif // AMBSENSOR

#ifdef VESSENSOR
    OneWire vessel(VESSENSOR);
    DallasTemperature sensorVessel(&vessel);
    sensorVessel.begin();
#endif // VESSENSOR

#endif // READTEMP

    if( ulNow - ulStart > DELAY) { // If (now - start) > delay time do work
        ulStart = ulNow;
        float fBpm = counter.GetPpm();
        sensorAmbient.requestTemperatures();
        sensorVessel.requestTemperatures();

#ifdef READTEMP

#if TEMPFORMAT == F
        const char* format = "F";
#else
        const char* format = "C";
#endif // TEMPFORMAT

#ifdef AMBSENSOR
#if TEMPFORMAT == F
        float fAmbTemp = sensorAmbient.getTempFByIndex(0);
#else
        float fAmbTemp = sensorAmbient.getTempCByIndex(0);
#endif // TEMPFORMAT
#endif // AMBSENSOR

#ifdef VESSENSOR
#if TEMPFORMAT == F
        float fVesTemp = sensorVessel.getTempFByIndex(0);
#else
        float fVesTemp = sensorVessel.getTempCByIndex(0);
#endif // TEMPFORMAT
#endif // VESSENSOR

#endif // READTEMP

        // Serialize data with ArduinoJson

        /* Example JSON
        {
            "host": "brewbubbles",
            "format": "F",
            "bubbles": {
                "bpm": 11.52,
                "ambtemp": 68.23,
                "vestemp": 45.11
            }
        }
        */

        const size_t capacity = 2*JSON_OBJECT_SIZE(3);
        DynamicJsonDocument bubbleJson(capacity);

        bubbleJson["hostname"] = hostname;

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

        // Serialize to memory
        //serializeJson(bubbleJson, strBubbleJson, sizeof(strBubbleJson));

#if DEBUG > 0
        serializeJson(bubbleJson, Serial); // Print JSON to Serial
        Serial.println();
        Serial.print("Bubbles Per Minute: ");
        Serial.print(fBpm);
        Serial.print("  Ambient Temp: ");
        Serial.print(fAmbTemp);
        Serial.print("º");
        Serial.print(format);
        Serial.print("  Vessel Temp: ");
        Serial.print(fVesTemp);
        Serial.print("º");
        Serial.print(format);
        Serial.println();
#endif // DEBUG
    }
}
