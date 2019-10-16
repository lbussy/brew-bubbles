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
    JsonConfig *config = JsonConfig::getInstance();

    Log.verbose(F("Triggered httpPost()." CR));

    if (strlen(config->targeturl) > 3) {
        Log.notice(F("Posting to: %s" CR), config->targeturl);

        LCBUrl url;
        if (url.setUrl(config->targeturl)) {
            printDNSServers();
            printIPAddressOfHost(url.getHost().c_str());

            if (postJson(config->targeturl, API_KEY)) {
                Log.notice(F("Target post ok." CR));
                return;
            } else {
                Log.error(F("Target post failed." CR));
                return;
            }
        } else {
            Log.error(F("Unable to parse target URL." CR));
        }
    } else {
        Log.verbose(F("No target URL in configuration, skipping." CR));
        return;
    }
}

void bfPost() {
    // Log.verbose(F("Triggered bfPost()." CR));
    // JsonConfig *config = JsonConfig::getInstance();
    // if (strlen(config->bfkey) > 3) {
    //     Log.notice(F("Posting to: %s" CR), BFURL);

    //     // Concatenat BF URL
    //     char bfUrl[94];
    //     strcpy (bfUrl, BFURL);
    //     strcat (bfUrl, config->bfkey);

    //     if (postJson(bfUrl, API_KEY)) {
    //         Log.notice(F("BF target post ok." CR));
    //         return;
    //     } else {
    //         Log.warning(F("BF target post failed." CR));
    //         return;
    //     }
    // } else {
    //     Log.verbose(F("No BF key in configuration, skipping." CR));
    //     return;
    // }
}

bool postJson(String targetUrl, const char* key) {
    LCBUrl url;
    if (url.setUrl(targetUrl)) {
        JsonConfig *config = JsonConfig::getInstance();
        Bubbles *bubble = Bubbles::getInstance();
        //const size_t capacity = JSON_OBJECT_SIZE(7);
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

        // Connect to the HTTP server
        WiFiClient client;
        client.setTimeout(10000);
        Log.verbose(F("Connecting to: %s, %l" CR), url.getHost().c_str(), url.getPort());
        int retval = client.connect(url.getHost(), url.getPort());
        //  1 = SUCCESS
        //  0 = FAILED
        // -1 = TIMED_OUT
        // -2 = INVALID_SERVER
        // -3 = TRUNCATED
        // -4 = INVALID_RESPONSE 
        if (!retval == 1) {
            Log.warning(F("Connection failed, Host: %s, Port: %l (Err: %d)" CR), url.getHost().c_str(), url.getPort(), retval);
            return false;
        } else {
            Log.notice(F("Connected to: %s, %l" CR), url.getHost().c_str(), url.getPort());
            Log.verbose("Connected to endpoint." CR);

            // Open POST connection
            Log.verbose(F("POST %s HTTP/1.1" CR), url.getPath().c_str());
            client.print(F("POST "));
            client.print(url.getPath());
            client.println(F(" HTTP/1.1"));

            // Begin headers
            //
            // Host
            Log.verbose(F("Host: %s" CR), url.getHost().c_str());
            client.print(F("Host: "));
            client.println(url.getHost());

            Log.verbose(F("Connection: close" CR));
            client.println(F("Connection: close"));

            // Content
            Log.verbose(F("Content-Length: %l" CR), measureJson(doc));
            client.print(F("Content-Length: "));
            client.println(measureJson(doc));

            // Content Type
            Log.verbose(F("Content-Type: application/json" CR)); 
            client.println(F("Content-Type: application/json"));

            // Key
            if (key) {
                Log.verbose(F("X-AIO-Key: %s" CR), key);
                client.print(F("X-AIO-Key: "));
                client.println(key);
            }

            // Terminate headers with a blank line
            Log.verbose(F("End headers." CR));
            client.println();
            // End Headers

            // Send the JSON document in body
            Serial.println();
            Serial.println(F("JSON:"));
            serializeJsonPretty(doc, Serial);
            Serial.println();
            serializeJson(doc, client);

            // Check the  HTTP status (should be "HTTP/1.1 200 OK")
            char status[32] = {0};
            client.readBytesUntil('\r', status, sizeof(status));
            client.stop();


            Log.verbose(F("Status: %s" CR), status);
            if (strcmp(status + 9, "200 OK") != 0) {
                Log.error(F("Unexpected status: %s" CR), status);
                return false;
            } else {
                Log.notice("JSON posted." CR);
                return true;
            }
        }
    } else {
        Log.error(F("Unable to parse target URL: %s" CR), targetUrl.c_str());
        return false;
    }
}

void printDNSServers() { // DEBUG
    Log.verbose(F("DNS #1: %s, DNS #2: %s" CR), WiFi.dnsIP().toString().c_str(), WiFi.dnsIP(1).toString().c_str());
}

void printIPAddressOfHost(const char* host) { // DEBUG
    IPAddress resolvedIP;
    if (!WiFi.hostByName(host, resolvedIP)) {
        Log.error(F("Host lookup failed for %s." CR), host);
    }
    Log.verbose(F("Host: %s, IP: %s" CR), host, resolvedIP.toString().c_str());
}
