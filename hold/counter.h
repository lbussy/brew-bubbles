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

#ifndef COUNTER_H
#define COUNTER_H

#include "config.h"
#include <Arduino.h>

class Counter {
    private:
        int ctPin; // Store pin
        volatile unsigned int pulse; // Store pulse count
        volatile unsigned long ulLastReport; // Store time of last report (millis())
        volatile unsigned long ulMicroLast; // Last pulse time for resolution (micros())

    public:
        Counter (int pin);
        void HandleInterrupts(void);
        float GetPps();
        float GetPpm();
};

#endif // COUNTER_H
