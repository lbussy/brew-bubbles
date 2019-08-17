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

#include "bubbles.h"

Bubbles *pBubbles; // Pointer to Counter class

static ICACHE_RAM_ATTR void HandleInterruptsStatic(void) { // External interrupt handler
    pBubbles->HandleInterrupts(); // Calls class member handler
}

bool Bubbles::instanceFlag = false;
Bubbles* Bubbles::single = NULL; // Holds pointer to class

Bubbles* Bubbles::getInstance() { // This is where we really create the instance
    if(!instanceFlag) {
        single = new Bubbles();
        instanceFlag = true;
        single->Setup();
        return single;
    } else {
        return single;
    }
}

void Bubbles::Setup() {
    pinMode(COUNTPIN, INPUT);       // Change pinmode to input
    attachInterrupt(digitalPinToInterrupt(COUNTPIN), HandleInterruptsStatic, FALLING); // FALLING, RISING or CHANGE
    pBubbles = single;              // Assign current instance to pointer 
    single->ulLastReport = millis();// Store the last report timer
    single->pulse = 0;              // Reset pulse counter
    single->ulStart = 0UL;          // Start time
}

Bubbles::~Bubbles() {
    instanceFlag = false;
}

void Bubbles::HandleInterrupts(void) { // Bubble Interrupt handler
    noInterrupts(); // Turn off interrupts
    unsigned long now = micros();
    if ((now - ulMicroLast) > RESOLUTION) { // Filter noise/bounce
        single->pulse++;    // Increment pulse count
    }
    interrupts();   // Turn on interrupts
}

float Bubbles::GetRawPps() { // Return raw pulses per second (resets counter)
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - single->ulLastReport; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 1000.0; // Seconds since last request
    float pps = (pulse / secs); // Calculate PPS
    single->pulse = 0; // Zero the pulse counter
    single->ulLastReport = millis(); // Store the last report timer
    return pps; // Return pulses per second
}

float Bubbles::GetRawPpm() { // Return raw pulses per minute (resets counter)
    unsigned long thisTime = millis(); // Get timer value now
    unsigned long ulLapsed = thisTime - single->ulLastReport; // Millis since last run
    float fLapsed = (float) ulLapsed; // Cast to float
    float secs = fLapsed / 60000.0; // Minutes since last request
    float ppm = (pulse / secs); // Calculate PPM
    single->pulse = 0; // Zero the pulse counter
    single->ulLastReport = millis(); // Store the last report timer
    return ppm; // Return pulses per minute
}

void Bubbles::Update() {
    return;
    Log.verbose(F("Inside Update()." CR));
    Log.verbose(F("millis() = %l." CR), millis());
    delay(200);
    unsigned long ulNow = 0;
    Log.verbose(F("ulNow = %l, ulStart = %l." CR), ulNow, single->ulStart);
    ulNow = millis();
    Log.verbose(F("ulNow = %l, ulStart = %l." CR), ulNow, single->ulStart);
    if (ulNow - single->ulStart > BUBLOOP) { // If (now - start) > delay time, get new value
        Log.verbose(F("ulNow - single->ulStart > BUBLOOP" CR));
        single->ulStart = ulNow;
        Log.verbose(F("Reset ulStart to %l." CR), single->ulStart);
        single->lastPpm = GetRawPpm();
        Log.verbose(F("lastPpm = %l" CR), single->lastPpm);
    }
}

float Bubbles::GetAmbientTemp() {
    OneWire ambient(AMBSENSOR);
    byte addrAmb[8];
    float fAmbTemp = -100.0;
    while (ambient.search(addrAmb)) { // Make sure we have a sensor
        DallasTemperature sensorAmbient(&ambient);
        sensorAmbient.begin();
        sensorAmbient.requestTemperatures();

        JsonConfig *config = JsonConfig::getInstance();
        if (config->tempinf == true)
            fAmbTemp = sensorAmbient.getTempFByIndex(0);
        else
            fAmbTemp = sensorAmbient.getTempCByIndex(0);
    }
    return fAmbTemp;
}

float Bubbles::GetVesselTemp() {
    OneWire vessel(VESSENSOR);
    byte addrVes[8];
    float fVesTemp = -100.0;
    while (vessel.search(addrVes)) { // Make sure we have a sensor
        DallasTemperature sensorVessel(&vessel);
        sensorVessel.begin();
        sensorVessel.requestTemperatures();

        JsonConfig *config = JsonConfig::getInstance();
        if (config->tempinf == true)
            fVesTemp = sensorVessel.getTempFByIndex(0);
        else
            fVesTemp = sensorVessel.getTempCByIndex(0);
    }
    return fVesTemp;
}

float Bubbles::GetPpm() {
    return single->lastPpm;
}
