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
#include <Arduino.h>

class Bubbles {
    private:
        // Singleton Declarations
        Bubbles() {};
        static Bubbles *single;
        // Other Declarations
        void start();
        CircularBuffer<float, TEMPAVG> tempAmbAvg;
        CircularBuffer<float, TEMPAVG> tempVesAvg;
        CircularBuffer<float, BUBAVG> bubAvg;
        volatile unsigned long ulStart;     // Start time
        volatile unsigned int pulse;        // Store pulse count
        volatile unsigned long ulLastReport;// Store time of last report (millis())
        volatile unsigned long ulMicroLast; // Last pulse time for resolution (micros())
        float lastBpm;                      // Holds most recent count
        float lastAmb;
        float lastVes;
        float getRawBpm();
        float getAmbientTemp();
        float getVesselTemp();

    public:
        // Singleton Declarations
        static Bubbles* getInstance();
        ~Bubbles() {single = NULL;}
        // Other Declarations
        void handleInterrupts(void);
        void update();                  // Call every 60 seconds
        char* lastTime;
        float getAvgAmbient();
        float getAvgVessel();
        float getAvgBpm();
};

#endif // _BUBBLES_H
