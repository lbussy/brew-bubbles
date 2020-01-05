/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#include "ntphandler.h"

NtpHandler* NtpHandler::single = NULL;

NtpHandler* NtpHandler::getInstance() {
    if (!single) {
        single = new NtpHandler();
        single->setup();
    }
    return single;
}

void NtpHandler::setup() {
    single->syncEventTriggered = false;
    single->hasBeenSet = false;
    NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
        single->ntpEvent = event;
        single->syncEventTriggered = true;
    });
}

void NtpHandler::start() {
    Ticker blinker;
    blinker.attach_ms(NTPBLINK, ntpBlinker);
    NTP.setInterval(63);
    NTP.setNTPTimeout(NTP_TIMEOUT);
    NTP.begin(TIMESERVER, 0, false, 0);
    Log.notice(F("Entering blocking loop to get NTP time."));
    single->update();
    while (!single->hasBeenSet) {
        #ifdef LOG_LEVEL
#ifdef LOG_LEVEL
        Serial.print(F("."));
#endif
        #endif
        _delay(1000);
        single->update();
        yield();
    }
    #ifdef LOG_LEVEL
#ifdef LOG_LEVEL
    Serial.println();
#endif
    single->setJsonTime();
    Log.notice(F("NTP Time: %s." CR), single->Time);
    #endif
    if (blinker.active()) blinker.detach(); // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
}

void NtpHandler::update() {
    if (NTP.getLastNTPSync() > 0) {
        single->hasBeenSet = true;
    }
    single->setJsonTime();
}

void NtpHandler::setJsonTime() {
    sprintf(single->Time, "%04u-%02u-%02uT%02u:%02u:%02uZ", year(), month(), day(), hour(), minute(), second());
}

void ntpBlinker() {
  digitalWrite(LED, !(digitalRead(LED)));  // Invert Current State of LED  
}
