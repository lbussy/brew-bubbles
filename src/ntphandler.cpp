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

bool NtpHandler::instanceFlag = false;
NtpHandler* NtpHandler::single = NULL;

NtpHandler* NtpHandler::getInstance() {
    if(!instanceFlag) {
        single = new NtpHandler();
        instanceFlag = true;
        single->setup();
        return single;
    } else {
        return single;
    }
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
    NTP.setInterval(63);
    NTP.setNTPTimeout(NTP_TIMEOUT);
    NTP.begin(TIMESERVER, 0, false, 0);
}

void NtpHandler::update() {
    if (NTP.getLastNTPSync() > 0) {
        single->hasBeenSet = true;
    }
}

char* NtpHandler::getJsonTime() {
    char* datetime = new  char[21];
    sprintf(datetime, "%04u-%02u-%02uT%02u:%02u:%02uZ", year(), month(), day(), hour(), minute(), second());
    Log.verbose(F("NtpHandler: Creating date/time: %s" CR), datetime);
    return(datetime);
}
