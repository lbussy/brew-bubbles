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

Bubbles __attribute__((unused)) bubbles;

static ICACHE_RAM_ATTR void HandleInterruptsStatic(void) { // External interrupt handler
    bubbles.handleInterrupts(); // Calls class member handler
}

void Bubbles::start() {
        pinMode(COUNTPIN, INPUT);       // Change pinmode to input
        attachInterrupt(digitalPinToInterrupt(COUNTPIN), HandleInterruptsStatic, RISING); // FALLING, RISING or CHANGE
        bubbles.ulLastReport = millis();// Store the last report timer
        bubbles.ulMicroLast = millis(); // Starting point for debouce timer
        bubbles.pulse = 0;              // Reset pulse counter
        bubbles.doBub = false;
        // Set starting values
        unsigned long ulNow = millis();
        bubbles.ulStart = ulNow;
        bubbles.lastBpm = 0.0;
        bubbles.lastAmb = 0.0;
        bubbles.lastVes = 0.0;

        // Set starting time
        bubbles.lastTime = getDTS();
}

void Bubbles::update() { // Regular update loop, once per minute
    // Get NTP Time
    BUB_NOT("started");
    bubbles.lastTime = getDTS();
    return;

    // Store last values
    bubbles.lastBpm = bubbles.getRawBpm();
    bubbles.lastAmb = getTemp(AMBSENSOR);
    bubbles.lastVes = getTemp(VESSENSOR);

    // Push values to circular buffers
    tempAmbAvg.push(bubbles.lastAmb);
    tempVesAvg.push(bubbles.lastVes);
    bubAvg.push(bubbles.lastBpm);

    Log.verbose(F("Current BPM is %D. Averages (%l in sample): BPM = %D, Ambient = %D, Vessel = %D." CR),
        bubbles.lastBpm,
        tempVesAvg.size(),
        bubbles.getAvgBpm(),
        bubbles.getAvgAmbient(),
        bubbles.getAvgVessel()
    );
}

void Bubbles::handleInterrupts(void) { // Bubbles Interrupt handler
    digitalWrite(LED, LOW);
    unsigned long now = micros();
    if ((now - bubbles.ulMicroLast) > RESOLUTION) { // Filter noise/bounce
        bubbles.pulse++;    // Increment pulse count
        bubbles.ulMicroLast = now;
    }
    bubbles.doBub = true;
}

float Bubbles::getRawBpm() { // Return raw pulses per minute (resets counter)
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - bubbles.ulLastReport; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 60000.0; // Minutes since last request
    float ppm = (pulse / secs); // Calculate PPM
    bubbles.pulse = 0; // Zero the pulse counter
    bubbles.ulLastReport = millis(); // Store the last report timer
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
        // float thisTemp = bubAvg[i];
        avg += bubAvg[i] / size;
    }
    return(avg);
}

void Bubbles::setLast(double last) {
    bubAvg.push(last);
}

void setDoBubUpdate() {
    BUB_NOT("running" CR);
    doBubUpdate = true;
}
