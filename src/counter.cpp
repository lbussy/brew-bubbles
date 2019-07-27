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

#include "counter.h"

Counter *pCounter; // Pointer to Counter class

static ICACHE_RAM_ATTR void HandleInterruptsStatic(void) { // External interrupt handler
    pCounter->HandleInterrupts(); // Calls class member handler
}


Counter::Counter(int pin) { // Counter Constructor
    pCounter = this; // Assign current instance to pointer 
    attachInterrupt(digitalPinToInterrupt(pin), HandleInterruptsStatic, RISING); // FALLING, RISING or CHANGE
    ctPin = pin; // Set input pin
    pinMode(pin, INPUT); // Change pinmode to input
    lastTime = millis(); // Store the last report timer
    pulse = 0; // Reset pulse counter
}

void Counter::HandleInterrupts(void) { // Counter Interrupt handler
    unsigned long now = micros();
    if ((now - ulMicroLast) > RESOLUTION) { // Filter noise/bounce
        noInterrupts(); // Turn off interrupts
        ++pulse; // Increment pulse count
        interrupts(); // Turn on interrupts
    }
}

float Counter::GetPps() { // Return pulses per second
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - lastTime; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 1000.0; // Seconds since last request
    float pps = (pulse / secs); // Calculate PPS
    pulse = 0; // Zero the pulse counter
    lastTime = millis(); // Store the last report timer
    return pps; // Return pulses per second
}

float Counter::GetPpm() { // Return pulses per minute
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - lastTime; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 60000.0; // Minutes since last request
    float ppm = (pulse / secs); // Calculate PPM
    pulse = 0; // Zero the pulse counter
    lastTime = millis(); // Store the last report timer
    return ppm; // Return pulses per minute
}
