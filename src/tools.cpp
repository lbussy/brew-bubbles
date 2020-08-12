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

#include "tools.h"

void _delay(unsigned long ulDelay)
{
    Log.verbose(F("DEBUG: In _delay()." CR));
    // Safe semi-blocking delay
#ifdef ESP32
    vTaskDelay(ulDelay); // Builtin to ESP32
#elif defined ESP8266
    unsigned long ulNow = millis();
    unsigned long ulThen = ulNow + ulDelay;
    while (ulThen > millis())
    {
        yield(); // ESP8266 needs to yield()
    }
#endif
}

void resetController()
{
    Log.notice(F("Reboot request - rebooting system." CR));
    _delay(5000);
    saveBpm();
    ESP.restart();
}

void loadBpm()
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
    DynamicJsonDocument doc(capacity);
    const char *bpmFileName = "lastBpm.json";

    // Mount File System
    if (!LittleFS.begin())
    {
        Log.error(F("CONFIG: Failed to mount File System." CR));
        return;
    }

    // Open file for reading
    File file = LittleFS.open(bpmFileName, "r");
    if (!LittleFS.exists(bpmFileName) || !file)
    {
        Log.notice(F("No lastBpm available." CR));
    }
    else
    {
        // Parse the JSON object in the file
        DeserializationError err = deserializeJson(doc, file);
        if (err)
        {
            Log.error(F("Failed to deserialize lastBpm." CR));
            Log.error(err.c_str());
        }
        else
        {
            bubbles.setLast(doc["lastBpm"]);
            Log.notice(F("Loaded lastBpm." CR));
        }
        // Delete file
        LittleFS.remove(bpmFileName);
    }
}

void saveBpm()
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
    DynamicJsonDocument doc(capacity);
    const char *bpmFileName = "lastBpm.json";

    doc["lastBpm"] = bubbles.getAvgBpm();

    // Open file for writing
    File file = LittleFS.open(bpmFileName, "w");
    if (!file)
    {
        Log.error(F("Failed to open lastBpm file." CR));
    }
    else
    {
        // Serialize the JSON object to the file
        bool success = serializeJson(doc, file);
        // This may fail if the JSON is invalid
        if (!success)
        {
            Log.error(F("Failed to serialize lastBpm." CR));
        }
        else
        {
            Log.verbose(F("Saved lastBpm." CR), bpmFileName);
        }
    }
}
