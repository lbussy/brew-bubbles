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

Bubbles* Bubbles::getInstance() {
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
    single->lastTime = (char*)F("1970-01-01T00:00:00Z"); // Set to epoch as a placeholder
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
    //return pps; // Return pulses per second
    return 10.5;
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
    unsigned long ulNow = millis();
    if (ulNow - single->ulStart > BUBLOOP) {
        // If (now - start) > delay time, get new value
        single->ulStart = ulNow;
        single->lastPpm = single->GetRawPpm();
        NtpHandler *ntpTime = NtpHandler::getInstance();
        single->lastTime = ntpTime->getJsonTime();
        Log.verbose(F("Update(): Last time is %s, last Ppm is %l:" CR), single->lastTime, single->lastPpm);
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

char* Bubbles::CreateBubbleJson() {
    //const size_t capacity = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
    const size_t capacity = 277;
    DynamicJsonDocument doc(capacity);
    JsonConfig *config = JsonConfig::getInstance();

    doc["api_key"] = API_KEY;
    doc["vessel"] = config->bubname;
    doc["datetime"] = single->lastTime;
    
    if (config->tempinf == true) {
        doc["format"] = "F";
    } else {
        doc["format"] = "C";
    }

    // Get bubbles per minute
    JsonObject data = doc.createNestedObject("data");
    data["bpm"] = single->lastPpm;
    data["ambtemp"] = single->GetAmbientTemp();
    data["vestemp"] = single->GetVesselTemp();

    char output[capacity] = {};
    serializeJson(doc, output, sizeof(output));
    return output;
}
