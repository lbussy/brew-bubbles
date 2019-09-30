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

#ifndef _BUBBLES_H
#define _BUBBLES_H

#include "config.h"
#include "jsonconfig.h"
#include "ntphandler.h"
#include "DallasTemperature.h"
#include "OneWire.h"
#include <CircularBuffer.h>

class Bubbles {
    private:
        Bubbles() {};
        // Singleton Declarations
        static bool instanceFlag;
        static Bubbles *single;
        void start();
        // Other Declarations
        volatile unsigned long ulStart;     // Start time
        volatile unsigned int pulse;        // Store pulse count
        volatile unsigned long ulLastReport;// Store time of last report (millis())
        volatile unsigned long ulMicroLast; // Last pulse time for resolution (micros())
        float lastPpm;                      // Holds most recent count
        char* lastTime;
        JsonConfig *config;
        CircularBuffer<float, TEMPAVG> *tempAmbAvg;
        CircularBuffer<float, TEMPAVG> *tempVesAvg;
        CircularBuffer<float, BUBAVG> *bubAvg;
        float getRawBpm();
        void createBubbleJson();
        float getBpm();
        float getAmbientTemp();
        float getVesselTemp();

    public:
        // Singleton Declarations
        static Bubbles* getInstance();
        ~Bubbles();
        // Other Declarations
        void handleInterrupts(void);
        void update();          // Call every 60 seconds
        float getAvgAmbient();
        float getAvgVessel();
        float getAvgBpm();
        char bubStatus[BUBBLEJSON];         // Hold the Bubble JSON
};

#endif // _BUBBLES_H
