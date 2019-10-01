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
        _delay(500);
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
