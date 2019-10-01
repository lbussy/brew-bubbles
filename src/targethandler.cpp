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

void httpPost() {
    Log.verbose(F("Triggered httpPost()." CR));
    JsonConfig *config = JsonConfig::getInstance();
    if (strlen(config->targeturl) > 3) {
        HTTPClient http; // Declare object of class HTTPClient
        Log.notice(F("Posting to: %s" CR), config->targeturl);
        http.begin(config->targeturl); // Specify request destination
        http.addHeader(F("Content-Type"), F("application/json")); // Specify content-type header
        http.addHeader(F("X-API-KEY"), F(API_KEY));
        int httpCode = http.POST(getPostJson()); // Post json

    #ifndef DISABLE_LOGGING
        String payload = http.getString().c_str(); // Get the response payload
        // Convert String to char array
        int n = payload.length();
        char p[n + 1]; 
        strcpy(p, payload.c_str()); 
        Log.notice(F("Target post return code: %i" CR), httpCode);
        Log.notice(F("Target post response payload: %s" CR), p);
    #endif //DISABLE_LOGGING

        http.end(); // Close connection
        if(httpCode == 200) { // 200 = ok
            Log.notice(F("Target post ok." CR));
            return;
        } else {
            Log.warning(F("Target post failed." CR));
            return;
        }
    } else {
        Log.verbose(F("No target URL in configuration, skipping." CR));
        return;
    }
}

void bfPost() {
    Log.verbose(F("Triggered bfPost()." CR));
    JsonConfig *config = JsonConfig::getInstance();
    if (strlen(config->bfkey) > 3) {
        HTTPClient http; // Declare object of class HTTPClient

        Log.notice(F("Posting to: %s" CR), BFURL);
        char bfUrl[94];
        strcpy (bfUrl, BFURL);
        strcat (bfUrl, config->bfkey);

        http.begin(bfUrl); // Specify request destination
        http.addHeader(F("Content-Type"), F("application/json")); // Specify content-type header
        http.addHeader(F("X-API-KEY"), F(API_KEY));
        int httpCode = http.POST(getPostJson()); // Post json

    #ifndef DISABLE_LOGGING
        String payload = http.getString().c_str(); // Get the response payload
        // Convert String to char array
        int n = payload.length();
        char p[n + 1]; 
        strcpy(p, payload.c_str()); 
        Log.notice(F("BF post return code: %i" CR), httpCode);
        Log.notice(F("BF post response payload: %s" CR), p);
    #endif //DISABLE_LOGGING

        http.end(); // Close connection
        if(httpCode == 200) { // 200 = ok
            Log.notice(F("BF post ok." CR));
            return;
        } else {
            Log.warning(F("BF post failed." CR));
            return;
        }
    } else {
        Log.verbose(F("No BF key in configuration, skipping." CR));
        return;
    }
}

String getPostJson() { // Form Target JSON
    JsonConfig *config = JsonConfig::getInstance();
    Bubbles *bubble = Bubbles::getInstance();
    // const size_t capacity = JSON_OBJECT_SIZE(7);
    const size_t capacity = TJSON;
    StaticJsonDocument<capacity> doc;

    doc[F("api_key")] = F(API_KEY);
    doc[F("device_source")] = F(SOURCE);
    doc[F("name")] = config->bubname;
    doc[F("bpm")] = bubble->getAvgBpm();
    doc[F("ambient")] = bubble->getAvgAmbient();
    doc[F("temp")] = bubble->getAvgVessel();
    if (config->tempinf == true)
        doc[F("temp_unit")] = F("F");
    else
        doc[F("temp_unit")] = F("C");

#ifndef DISABLE_LOGGING
    char output[capacity];
    serializeJson(doc, output);
    Log.verbose(F("Target JSON: %s" CR), output);
#endif
    return doc.as<String>();
}
