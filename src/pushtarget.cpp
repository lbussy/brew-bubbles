/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#include "pushtarget.h"

IPAddress resolveHost(PushTarget *target) {
    LCBUrl lcburl;
    lcburl.setUrl(String(target->url));
    Log.verbose(F("Host lookup: %s." CR), lcburl.getHost().c_str());
    IPAddress returnIP = INADDR_NONE;
    if (WiFi.hostByName(lcburl.getHost().c_str(), returnIP, 10000) == 0) {
        Log.error(F("Host lookup error." CR));
        returnIP = INADDR_NONE;
    }
    return returnIP;
}

bool pushTarget(PushTarget *target) {
    Log.notice(F("Posting to: %s" CR), target->url);
    LCBUrl lcburl;
    lcburl.setUrl(String(target->url));

    if (target->ip == INADDR_NONE) {
        Log.error(F("Name resolution failed for %s." CR), lcburl.getHost().c_str());
        return false;
    }

    Bubbles *bubble = Bubbles::getInstance();
    JsonConfig *config = JsonConfig::getInstance();
    const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
    StaticJsonDocument<capacity> doc;

    if (target->apiName.enabled) doc[target->apiName.name] = F(API_KEY);
    if (target->bubName.enabled) doc[target->bubName.name] = config->bubname;
    if (target->bpm.enabled) doc[target->bpm.name] = bubble->getAvgBpm();
    if (target->ambientTemp.enabled) doc[target->ambientTemp.name] = bubble->getAvgAmbient();
    if (target->vesselTemp.enabled) doc[target->vesselTemp.name] = bubble->getAvgVessel();
    if (target->tempFormat.enabled) {
        if (config->tempinf == true) doc[target->tempFormat.name] = F("F");
        else doc[target->tempFormat.name] = F("C");
    }
    String json;
    serializeJson(doc, json);

    // Use the IP address we resolved if we are connecting with mDNS
    Log.verbose(F("Connecting to: %s @ %s on port %l" CR),
        lcburl.getHost().c_str(),
        target->ip.toString().c_str(),
        lcburl.getPort()
    );

    WiFiClient client;
    //  1 = SUCCESS
    //  0 = FAILED
    // -1 = TIMED_OUT
    // -2 = INVALID_SERVER
    // -3 = TRUNCATED
    // -4 = INVALID_RESPONSE
    IPAddress targetIP = target->ip;
    int port = lcburl.getPort();
    if (client.connect(targetIP, port)) {
        Log.notice(F("Connected to: %s at %s, %l" CR),
            target->target.name, lcburl.getHost().c_str(), port
        );

        // Open POST connection
        Log.verbose(F("POST /%s HTTP/1.1" CR), lcburl.getPath().c_str());
        client.print(F("POST /"));
        client.print(lcburl.getPath().c_str());
        client.println(F(" HTTP/1.1"));

        // Begin headers
        //
        // Host
        Log.verbose(F("Host: %s" CR), lcburl.getHost().c_str());
        client.print(F("Host: "));
        client.println(lcburl.getHost().c_str());
        //
        Log.verbose(F("Connection: close" CR));
        client.println(F("Connection: close"));
        // Content
        Log.verbose(F("Content-Length: %l" CR),json.length());
        client.print(F("Content-Length: "));
        client.println(json.length());
        // Content Type
        Log.verbose(F("Content-Type: application/json" CR)); 
        client.println(F("Content-Type: application/json"));
        // Terminate headers with a blank line
        Log.verbose(F("End headers." CR));
        client.println();
        //
        // End Headers

        // Post JSON
        Log.verbose(F("Posting JSON to target:" CR)); // DEBUG
        Serial.println(json); // DEBUG
        client.println(json);
        // Check the HTTP status (should be "HTTP/1.1 200 OK")
        String response;
        while (client.connected() || client.available()) {
            if (client.available()) {
                response = client.readStringUntil('\n');
            }
        }
        client.stop();
        Log.verbose(F("Status: %s" CR), response.c_str());
        if (response.indexOf("200") >= 0) {
            Log.verbose(F("JSON posted." CR));
            return true;
        } else {
            Log.error(F("Unexpected status: %s" CR), response.c_str());
            return false;
        }
    } else {
        Log.warning(F("Connection failed, Host: %s, Port: %l (Err: %d)" CR),
            lcburl.getHost().c_str(), port, client.connected()
        );
        return false;
    }
}
