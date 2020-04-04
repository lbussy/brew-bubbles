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

#include "bubbles.h"

Bubbles bubbles;

volatile bool dobubble;

void Bubbles::start() {
        interruptSetup();

        // Set starting values
        ulLastReport = millis();
        unsigned long ulNow = millis();
        ulStart = ulNow;
        lastBpm = 0.0;
        lastAmb = 0.0;
        lastVes = 0.0;
        dobubble = false;

        // Set starting time
        lastTime = getDTS();
        update();
        Log.notice(F("Bubble counter initialized." CR));
}

void Bubbles::update() { // Regular update loop, once per minute
    // Get NTP Time
    DNOT("triggered");
    lastTime = getDTS();
    DVER("got DTS");

    // Store last values
    lastBpm = getRawBpm();
    DVER("got Raw BPM");
    lastAmb = getTemp(AMBSENSOR);
    DVER("got Ambient");
    lastVes = getTemp(VESSENSOR);
    DVER("got Vessel");

    // Push values to circular buffers
    tempAmbAvg.push(lastAmb);
    tempVesAvg.push(lastVes);
    bubAvg.push(lastBpm);

    Log.verbose(F("Current BPM is %D. Averages (%l in sample): BPM = %D, Ambient = %D, Vessel = %D." CR),
        lastBpm,
        tempVesAvg.size(),
        getAvgBpm(),
        getAvgAmbient(),
        getAvgVessel()
    );
}

float Bubbles::getRawBpm() { // Return raw pulses per minute (resets counter)
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - ulLastReport; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 60000.0; // Minutes since last request
    float ppm = (pulse / secs); // Calculate PPM
    pulse = 0; // Zero the pulse counter
    ulLastReport = millis(); // Store the last report timer
    return ppm; // Return pulses per minute
}

float Bubbles::getAvgAmbient() {
    // Retrieve TEMPAVG readings to calculate average
    float avg = 0.0;
    uint8_t size = tempAmbAvg.size();
    for (int i = 0; i < tempAmbAvg.size(); i++) {
        // float thisTemp = tempAmbAvg[i];
        avg += tempAmbAvg[i] / size;
    }
    return(avg);
}

float Bubbles::getAvgVessel() {
    // Return TEMPAVG readings to calculate average
    float avg = 0.0;
    uint8_t size = tempVesAvg.size();
    for (int i = 0; i < tempVesAvg.size(); i++) {
        // float thisTemp = tempVesAvg[i];
        avg += tempVesAvg[i] / size;
    }
    return(avg);
}

float Bubbles::getAvgBpm() {
    // Return BUBAVG readings to calculate average
    float avg = 0.0;
    uint8_t size = bubAvg.size();
    for (int i = 0; i < bubAvg.size(); i++) {
        avg += bubAvg[i] / size;
    }
    return(avg);
}

void Bubbles::setLast(double last) {
    bubAvg.push(last);
}

void doBub() {
    // Allow a Bubble count to fire
    dobubble = true;
}
