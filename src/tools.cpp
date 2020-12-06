/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

#include "tools.h"

void _delay(unsigned long ulDelay)
{
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
    const size_t capacity = JSON_OBJECT_SIZE(4) + 40;
    DynamicJsonDocument doc(capacity);
    const char *bpmFileName = LASTBPM_JSON;

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
        bubbles.clearLast();
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
            bubbles.setLast(doc["dts"], doc["lastBpm"], doc["lastAmb"], doc["lastVes"]);
        }
    }
}

void saveBpm()
{
    const size_t capacity = JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);
    const char *bpmFileName = LASTBPM_JSON;

    const float lastBpm = bubbles.getAvgBpm();
    const float lastAmb = bubbles.getAvgAmbient();
    const float lastVes = bubbles.getAvgVessel();
    const time_t dts = time(nullptr); // 1546300800

    doc["lastBpm"] = lastBpm;
    doc["lastAmb"] = lastAmb;
    doc["lastVes"] = lastVes;
    doc["dts"] = dts;

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

void maintenanceLoop()
{
    if (ESP.getFreeHeap() < MINFREEHEAP)
    {
        Log.warning(F("Maintenance: Heap memory has degraded below safe minimum, restarting." CR));
        resetController();
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        Log.warning(F("Maintenance: WiFi not connected, reconnecting." CR));
        doNonBlock = true;
        doWiFi(); // With doNonBlock, this should be non-blocking
    }
    if (millis() > ESPREBOOT)
    {
        // The ms clock will rollover after ~49 days.  To be on the safe side,
        // restart the ESP after about 42 days to reset the ms clock.
        Log.warning(F("Maintenance: Six week routine restart."));
        ESP.restart();
    }
    if (lastNTPUpdate > NTPRESET)
    {
        // Reset NTP (blocking) every measured 24 hours
        Log.notice(F("Maintenance: Setting time"));
        setClock();
    }
}

void setDoReset()
{
    doReset = true; // Semaphore required for reset in callback
}

void setDoURLTarget()
{
    doURLTarget = true; // Semaphore required for Ticker + radio event
}

void setDoBFTarget()
{
    doBFTarget = true; // Semaphore required for Ticker + radio event
}

void setDoBrewfTarget()
{
    doBrewfTarget = true; // Semaphore required for Ticker + radio event
}

void setDoTSTarget()
{
    doTSTarget = true; // Semaphore required for Ticker + radio event
}

void setDoOTA()
{
    doOTA = true; // Semaphore required for OTA in callback
}

void tickerLoop()
{
    Target *target = Target::getInstance();
    BFTarget *bfTarget = BFTarget::getInstance();
    BrewfTarget *brewfTarget = BrewfTarget::getInstance();

    // Trigger Bubble check
    //
    if (doBubble)
    {
        doBubble = false;
        bubbles.update();
    }

    // Handle JSON posts
    //
    // Do URL Target post
    if (doURLTarget)
    {
        doURLTarget = false;
        target->push();
    }
    //
    // Do Brewer's Friend Post
    if (doBFTarget)
    { // Do BF post
        doBFTarget = false;
        bfTarget->push();
    }
    //
    // Do Brewfather Post
    if (doBrewfTarget)
    { // Do BF post
        doBrewfTarget = false;
        brewfTarget->push();
    }
    //
    // Do ThingSpeak Post
    if (doTSTarget)
    { // Do TS post
        doTSTarget = false;
        pushThingspeak();
    }

    // Check for Reset Pending
    // Necessary because we cannot delay in a callback
    if (doReset)
    {
        doReset = false;
        resetController();
    }

    if (doOTA)
    {
        doOTA = false;
        execfw();
    }
}
