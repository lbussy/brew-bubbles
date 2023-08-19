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

#include "bubbles.h"

#include "sensors.h"
#include "ntp.h"
#include "pincount.h"
#include "tools.h"
#include <ArduinoLog.h>

Bubbles bubbles;
volatile bool doBubble;
volatile bool __attribute__((unused)) vesselDisconnected = false;  // Semaphore for vessel disconnected
volatile bool __attribute__((unused)) ambientDisconnected = false; // Semaphore for ambient disconnected

bool Bubbles::start()
{
    interruptSetup();

    // Set starting values
    ulLastReport = millis();
    unsigned long ulNow = millis();
    ulStart = ulNow;
    doBubble = false;

    // Set starting time
    lastTime = getDTS();
    return true;
}

void Bubbles::update()
{ // Regular update loop, once per minute
    // Get NTP Time
    lastTime = getDTS();

    // Store last values
    lastBpm = getRawBpm();
    bubAvg.push(lastBpm);

    // Check Ambient device
    lastAmb = getTemp(AMBSENSOR);
    if ((config.bubble.tempinf && lastAmb == (float)DEVICE_DISCONNECTED_F) || (!config.bubble.tempinf && lastAmb == (float)DEVICE_DISCONNECTED_C))
    {
        // Ambient device disconnected
        ambientDisconnected = true;
        tempAmbAvg.clear();
    }
    else
    {
        // Ambient device connected
        if (ambientDisconnected)
        {
            // Ambient newly connected
            tempAmbAvg.clear();
        }
        ambientDisconnected = false;
    }
    // Push Ambient value to circular buffer
    tempAmbAvg.push(lastAmb);

    // Check Vessel device
    lastVes = getTemp(VESSENSOR);
    if ((config.bubble.tempinf && lastVes == (float)DEVICE_DISCONNECTED_F) || (!config.bubble.tempinf && lastVes == (float)DEVICE_DISCONNECTED_C))
    {
        // Vessel device disconnected
        vesselDisconnected = true;
        tempVesAvg.clear();
    }
    else
    {
        // Vessel device connected
        if (vesselDisconnected)
        {
            // Vessel newly connected
            tempVesAvg.clear();
        }
        vesselDisconnected = false;
    }
    // Push Vessel value to circular buffer
    tempVesAvg.push(lastVes);

    saveBpm();

    Log.verbose(F("Current BPM is %D. Averages: BPM (%d) = %D, Ambient (%d) = %D, Vessel (%d) = %D." CR),
                bubbles.lastBpm,
                bubAvg.size(),
                bubbles.getAvgBpm(),
                tempAmbAvg.size(),
                bubbles.getAvgAmbient(),
                tempVesAvg.size(),
                bubbles.getAvgVessel());

    return;
}

float Bubbles::getRawBpm()
{                                                     // Return raw pulses per minute (resets counter)
    unsigned long thisTime = millis();                // Get timer value now
    unsigned long ulLapsed = thisTime - ulLastReport; // Millis since last run
    float fLapsed = (float)ulLapsed;                  // Cast to float
    float secs = fLapsed / 60000.0;                   // Minutes since last request
    float ppm = (pulse / secs);                       // Calculate PPM
    pulse = 0;                                        // Zero the pulse counter
    ulLastReport = millis();                          // Store the last report timer
    return ppm;                                       // Return pulses per minute
}

float Bubbles::getAvgAmbient()
{
    // Retrieve TEMPAVG readings to calculate average
    float avg = 0.0;
    uint8_t size = tempAmbAvg.size();
    for (int i = 0; i < tempAmbAvg.size(); i++)
    {
        avg += tempAmbAvg[i] / size;
    }
    if (avg)
        return (avg);
    else
        return getTemp(AMBSENSOR);
}

float Bubbles::getAvgVessel()
{
    // Retrieve TEMPAVG readings to calculate average
    float avg = 0.0;
    uint8_t size = tempVesAvg.size();
    for (int i = 0; i < tempVesAvg.size(); i++)
    {
        avg += tempVesAvg[i] / size;
    }
    if (avg)
        return (avg);
    else
        return getTemp(VESSENSOR);
}

float Bubbles::getAvgBpm()
{
    // Return BUBAVG readings to calculate average
    float avg = 0.0;
    uint8_t size = bubAvg.size();
    for (int i = 0; i < bubAvg.size(); i++)
    {
        avg += bubAvg[i] / size;
    }
    return (avg);
}

void Bubbles::setLast(time_t dts, double lastBpm, double lastAmb, double lastVes)
{
    long expAge = dts + (LASTBPMEXP / 60);
    time_t now = time(nullptr);
    if (expAge > now)
    {
        // JSON expired
        Log.verbose(F("Last BPM JSON expired." CR));
        saveBpm();
    }
    else
    {
        Log.notice(F("Loading last BPM." CR));
        // Loading JSON information
        if (!lastBpm)
            bubAvg.push(0);
        else
            bubAvg.push(lastBpm);

        // Don't push 0 to temps, get a fresh value
        if (lastAmb == 0)
        {
            tempAmbAvg.push(getTemp(AMBSENSOR));
        }
        else
        {
            tempAmbAvg.push(lastAmb);
        }
        if (lastVes == 0)
        {
            tempVesAvg.push(getTemp(VESSENSOR));
        }
        else
        {
            tempVesAvg.push(lastVes);
        }
    }
}

unsigned long Bubbles::getCurrentAge()
{
    // Current age of lastBpm report in millis
    return millis() - ulLastReport;
}

void Bubbles::clearLast()
{
    bubAvg.clear();
}

void Bubbles::wipeArray()
{
    tempAmbAvg.clear();
    tempVesAvg.clear();
}

void setDoBub()
{
    // Allow a Bubble count to fire
    doBubble = true;
}
