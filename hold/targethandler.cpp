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
        doPostJson(config->targeturl);
    } else {
        Log.verbose(F("No target URL in configuration, skipping." CR));
        return;
    }
}

void bfPost() {
    Log.verbose(F("Triggered bfPost()." CR));
    JsonConfig *config = JsonConfig::getInstance();
    if (strlen(config->bfkey) > 3) {
        char bfUrl[129];
        strcpy (bfUrl, BFURL);
        strcat (bfUrl, config->bfkey);
        doPostJson(bfUrl);
    } else {
        Log.verbose(F("No BF key in configuration, skipping." CR));
        return;
    }
}

bool doPostJson(char* targetUrl) { // Form Target JSON
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

    HTTPClient http;
    WiFiClient client;

    Log.notice(F("Posting to: %s" CR), targetUrl);
    client.setTimeout(10000);
    if (!client.connect("brewpi.local", 80) == 0) {
        Log.error(F("Connection failed." CR));
        return false;
    } else {
        Log.verbose(F("DEBUG: (0)" CR));

        Log.verbose(F("Connected to target." CR));

        Log.verbose(F("DEBUG: (1)" CR));

        // Open connection
        client.println(F("POST /brewpi-api.php HTTP/1.1"));

        Log.verbose(F("DEBUG: (2)" CR));

        // Send the HTTP headers
        client.println(F("Host: brewpi.local"));
        client.println(F("Connection: close"));
        client.print(F("Content-Length: "));
        client.println(measureJson(doc));
        client.println(F("Content-Type: application/json"));
        client.println(F("X-API-KEY " API_KEY));

        Log.verbose(F("DEBUG: (3)" CR));

        // Terminate headers with a blank line
        client.println();

        // Send the JSON document in body
        serializeJson(doc, client);

        // Check the  HTTP status (should be "HTTP/1.1 200 OK")
        char status[32] = {0};
        client.readBytesUntil('\r', status, sizeof(status));
        client.stop();
        if (strcmp(status + 9, "200 OK") != 0) {
            Log.error(F("Unexpected status: %s" CR), status);
            return false;
        } else {
            return true;
        }
    }
}
