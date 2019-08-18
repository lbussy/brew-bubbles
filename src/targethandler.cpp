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

#include "targethandler.h"

unsigned long ulMStart = 0UL; // Start time // DEBUG

void doTargets() {
    ZuluTime *time = ZuluTime::getInstance();
    time->update();
    unsigned long ulMNow = millis();
    if (ulMNow - ulMStart > BUBLOOP) { // If (now - start) > delay time, do work
        // Do something once per BUBLOOP
        ulMStart = ulMNow;
        doDoc();
    }
}

void doDoc() {
    JsonConfig *config = JsonConfig::getInstance();
    Bubbles *bubble = Bubbles::getInstance();
    ZuluTime *time = ZuluTime::getInstance();

    const size_t capacity = 277;
    DynamicJsonDocument doc(capacity);

    doc["api_key"] = API_KEY;
    doc["vessel"] = config->bubname;
    doc["datetime"] = time->getZuluTime();
    
    if (config->tempinf == true) {
        doc["format"] = "F";
    } else {
        doc["format"] = "C";
    }

    // Get bubbles per minute
    JsonObject data = doc.createNestedObject("data");
    data["bpm"] = bubble->GetPpm();
    data["ambtemp"] = bubble->GetAmbientTemp();
    data["vestemp"] = bubble->GetVesselTemp();

    if (!SPIFFS.begin()) { // Mount SPIFFS
        Log.error(F("Failed to mount SPIFFS." CR));
    } else {
        // Open file for writing
        char filename[14] = "/bubbles.json";
        File file = SPIFFS.open(filename, "w");
        if (!file) {
            Log.error(F("Failed to open json file for writing." CR));
        } else {
            // Serialize the JSON object to the file
            bool success = serializeJson(doc, file);
            // This may fail if the JSON is invalid
            if (!success) {
                Log.error(F("Failed to serialize json." CR));
            } else {
                Log.notice(F("Saved json as %s." CR), filename);
            }
        }
    }

    // Post JSON
    char strBubbleJson[capacity];
    serializeJson(doc, strBubbleJson, sizeof(strBubbleJson));
    httppost(strBubbleJson); // Post JSON date to endpoint
}

bool httppost(String json) {
    HTTPClient http; // Declare object of class HTTPClient
    // Ports other than 80 need to be in the format of: http://192.168.168.199:8080/
    JsonConfig *config = JsonConfig::getInstance();
    Log.verbose(F("Posting to: %s" CR), config->targeturl);
    http.begin(config->targeturl); // Specify request destination
    http.addHeader("Content-Type", "application/json"); // Specify content-type header
    int httpCode = http.POST(json); // Post json

#ifndef DISABLE_LOGGING
    String payload = http.getString(); // Get the response payload
    // Convert String to char array
    int n = payload.length();
    char p[n + 1]; 
    strcpy(p, payload.c_str()); 
    Log.notice(F("HTTP Post return code: %i" CR), httpCode);
    Log.notice(F("HTTP Post response payload: %s" CR), p);
    // Convert String to char array
    int i = json.length();
    char j[i + 1]; 
    strcpy(j, json.c_str()); 
    Log.notice(F("JSON was: %s" CR), j);
#endif //DISABLE_LOGGING

    http.end(); // Close connection
    if(httpCode == 200) { // 200 = ok
        return true;
    } else {
        return false;
    }
}
