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

#ifndef _BUBBLES_H
#define _BUBBLES_H

#include "config.h"
#include "sensors.h"
#include "ntp.h"
#include "pincount.h"
#include <CircularBuffer.h>
#include <Arduino.h>

struct Bubbles
{
private:
    // Private Methods
    float getRawBpm();

    // Private Properties
    CircularBuffer<float, TEMPAVG> tempAmbAvg;
    CircularBuffer<float, TEMPAVG> tempVesAvg;
    CircularBuffer<float, BUBAVG> bubAvg;
    volatile unsigned long ulStart;      // Start time
    volatile unsigned long ulLastReport; // Store time of last report (millis())
    volatile unsigned long ulMicroLast;  // Last pulse time for resolution (micros())
    float lastAmb;
    float lastVes;

public:
    // Public Methods
    bool start();
    bool update(); // Call every 60 seconds
    float getAvgAmbient();
    float getAvgVessel();
    float getAvgBpm();
    void wipeArray(); // Wipe array on format change
    void setLast(double); // Push last reading on reboot

    // Public Properties
    float lastBpm; // Holds most recent count
    String lastTime;
    int sampleSize;
};

extern volatile int pulse;
void setDoBub();

#endif // _BUBBLES_H
