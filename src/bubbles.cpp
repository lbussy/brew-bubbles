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
unsigned long ulNow = millis(); // Time in millis now
unsigned long ulStart = 0UL; // Start time

void bubbles(char* localTime) {
    ulNow = millis();

    if (ulNow - ulStart > BUBLOOP) { // If (now - start) > delay time, do work
        ulStart = ulNow;

        /*         
        Sample (277 bytes to allow for string duplication):
        {
            "api_key":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "vessel":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "datetime":"2019-11-16T23:59:01.123Z",
            "format":"F",
            "data":{
                "bpm":99.999,
                "ambtemp":70.3625,
                "vestemp":-196.6
            }
        }
        */

        // Declare doc
        const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
        DynamicJsonDocument doc(capacity);

        JsonConfig *config;
        config = JsonConfig::getInstance();
        
        doc["api_key"] = API_KEY;
        doc["vessel"] = config->bubname;
        doc["datetime"] = localTime;

        // Get bubbles per minute
        JsonObject data = doc.createNestedObject("data");
        float fBpm = counter.GetPpm();
        data["bpm"] = fBpm;

#ifdef READTEMP
        bool present = false;

#ifdef AMBSENSOR
        OneWire ambient(AMBSENSOR);
        byte addrAmb[8];
        while (ambient.search(addrAmb)) { // Make sure we have a sensor
            DallasTemperature sensorAmbient( & ambient);
            sensorAmbient.begin();
            sensorAmbient.requestTemperatures();

            float fAmbTemp;
            if (config->tempinf == true)
                fAmbTemp = sensorAmbient.getTempFByIndex(0);
            else
                fAmbTemp = sensorAmbient.getTempCByIndex(0);
            data["ambtemp"] = fAmbTemp;
            present = true;
        }
#endif // AMBSENSOR

#ifdef VESSENSOR
        OneWire vessel(VESSENSOR);
        byte addrVes[8];
        while (ambient.search(addrVes)) { // Make sure we have a sensor
            DallasTemperature sensorVessel( & vessel);
            sensorVessel.begin();
            sensorVessel.requestTemperatures();

            float fVesTemp;
            if (config->tempinf == true)
                fVesTemp = sensorVessel.getTempFByIndex(0);
            else
                fVesTemp = sensorVessel.getTempCByIndex(0);
            data["vestemp"] = fVesTemp;
            present = true;
        }
#endif // VESSENSOR

        if (present) { // If we have a sensor
            if (config->tempinf == true) {
                doc["format"] = "F";
            } else {
                doc["format"] = "C";
            }
        }
            

#endif // READTEMP

        if (!SPIFFS.begin()) { // Mount SPIFFS
            Log.error("Failed to mount SPIFFS." CR);
        } else {
            // Open file for writing
            char filename[14] = "/bubbles.json";
            File file = SPIFFS.open(filename, "w");
            if (!file) {
                Log.error("Failed to open json file for writing." CR);
            } else {
                // Serialize the JSON object to the file
                bool success = serializeJson(doc, file);
                // This may fail if the JSON is invalid
                if (!success) {
                    Log.error("Failed to serialize json." CR);
                } else {
                    Log.notice("Saved json as %s." CR, filename);
                }
            }
        }

        // Post JSON
        char strBubbleJson[capacity];
        serializeJson(doc, strBubbleJson, sizeof(strBubbleJson));
        httppost(strBubbleJson); // Post JSON date to endpoint
    }
    return;
}
