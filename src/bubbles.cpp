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

Bubbles *pBubbles; // Pointer to Counter class

static ICACHE_RAM_ATTR void HandleInterruptsStatic(void) { // External interrupt handler
    pBubbles->handleInterrupts(); // Calls class member handler
}

Bubbles* Bubbles::single = NULL; // Holds pointer to class

Bubbles* Bubbles::getInstance() {
    if (!single) {
        single = new Bubbles();
        pinMode(COUNTPIN, INPUT);       // Change pinmode to input
        attachInterrupt(digitalPinToInterrupt(COUNTPIN), HandleInterruptsStatic, RISING); // FALLING, RISING or CHANGE
        pBubbles = single;              // Assign current instance to pointer
        single->ulLastReport = millis();// Store the last report timer
        single->ulMicroLast = millis(); // Starting point for debouce timer
        single->pulse = 0;              // Reset pulse counter
        single->doBub = false;
        // Set starting values
        unsigned long ulNow = millis();
        single->ulStart = ulNow;
        single->lastBpm = 0.0;
        single->lastAmb = 0.0;
        single->lastVes = 0.0;

        // Set starting time
        single->lastTime = getDTS();
    }
    return single;
}

void Bubbles::update() { // Regular update loop, once per minute
    // Get NTP Time
    single->lastTime = getDTS();

    // Store last values
    single->lastBpm = single->getRawBpm();
    single->lastAmb = getTemp(AMBSENSOR);
    single->lastVes = getTemp(VESSENSOR);

    // Push values to circular buffers
    tempAmbAvg.push(single->lastAmb);
    tempVesAvg.push(single->lastVes);
    bubAvg.push(single->lastBpm);

    Log.verbose(F("Current BPM is %D. Averages (%l in sample): BPM = %D, Ambient = %D, Vessel = %D." CR),
        single->lastBpm,
        tempVesAvg.size(),
        single->getAvgBpm(),
        single->getAvgAmbient(),
        single->getAvgVessel()
    );
}

void Bubbles::handleInterrupts(void) { // Bubble Interrupt handler
    digitalWrite(LED, LOW);
    unsigned long now = micros();
    if ((now - single->ulMicroLast) > RESOLUTION) { // Filter noise/bounce
        single->pulse++;    // Increment pulse count
        single->ulMicroLast = now;
    }
    single->doBub = true;
}

float Bubbles::getRawBpm() { // Return raw pulses per minute (resets counter)
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - single->ulLastReport; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 60000.0; // Minutes since last request
    float ppm = (pulse / secs); // Calculate PPM
    single->pulse = 0; // Zero the pulse counter
    single->ulLastReport = millis(); // Store the last report timer
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
    doBubUpdate = true; // Semaphore required for Ticker + radio event
}

void bubLoop() {
    Bubbles *bubble = Bubbles::getInstance();
    // Do Bubble update post
    if (doBubUpdate) {
        doBubUpdate = false;
        bubble->update();
    }
}
