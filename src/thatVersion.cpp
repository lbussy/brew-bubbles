/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

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

#include "thatVersion.h"

#include "config.h"
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>

ThatVersion __attribute__((unused)) thatVersion;

bool getThatVersion()
{
    // Create a WiFiClient instance to establish the connection
    // Make a request to the server
    WiFiClient client;

    if (client.connect(UPGRADEURL, UPGRADEPORT)) {
        Log.notice(F("Connected to server" LF));
        client.println("GET " + String(UPGRADEENDPOINT) + " HTTP/1.1");
        client.print("Host: ");
        client.println(UPGRADEURL);
        client.println("Connection: close");
        client.println();
    } else {
        Serial.println("Connection failed");
        return false;
    }

    // Wait for server response
    while (client.connected())
    {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
        break; // Empty line, end of headers
        }
    }

    // Read and process multi-line response
    DynamicJsonDocument verDoc(192);
    DeserializationError error = deserializeJson(verDoc, client);
    if (error)
    {
        Log.error(F("Deserialization error: " LF), error.c_str());
        return false;
    }
    else
    {
        thatVersion.load(verDoc.as<JsonObject>());
        return true;
    }
}

void ThatVersion::save(JsonObject obj) const
{
    obj["fw_version"] = fw_version;
    obj["fs_version"] = fs_version;
}

void ThatVersion::load(JsonObjectConst obj)
{
    if (!obj["version"].isNull())
        strlcpy(thatVersion.version, (const char*)obj["version"], sizeof(version));
    if (!obj["fw_version"].isNull())
        strlcpy(thatVersion.fw_version, (const char*)obj["fw_version"], sizeof(fw_version));
    if (!obj["fs_version"].isNull())
        strlcpy(thatVersion.fs_version, (const char*)obj["fs_version"], sizeof(fs_version));

    Log.notice(F("[DEBUG] Version: %s, FW Version: %s, FS Version: %s" LF), version, fw_version, fs_version);
}
