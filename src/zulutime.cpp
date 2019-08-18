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

// Time.h & TimeLib.h:  https://github.com/PaulStoffregen/Time
// Timezone.h: https://github.com/JChristensen/Timezone

#include "zulutime.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET, NTP_INTERVAL);

bool ZuluTime::instanceFlag = false;
ZuluTime* ZuluTime::single = NULL;
ZuluTime* ZuluTime::getInstance() {
    if(!instanceFlag) {
        single = new ZuluTime();
        instanceFlag = true;
        single->setup();
        return single;
    } else {
        return single;
    }
}

void ZuluTime::setup() {
    timeClient.begin();
}

void ZuluTime::update() {
    timeClient.update();
}

char* ZuluTime::getZuluTime() {
    unsigned long epochTime = timeClient.getEpochTime();

    // Convert received time stamp to time_t object
    // time_t local, utc, zulu;
    time_t utc, zulu;
    utc = epochTime;

    // Zulu Time definition
    TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};
    Timezone zuluGMT(GMT);
    zulu = zuluGMT.toLocal(utc);

    // Get JS-formatted date/time string
    static char datetime[25];
    sprintf(datetime, "%u-%u-%uT%u:%u:%uZ", year(zulu), month(zulu), day(zulu), hour(zulu), minute(zulu), second(zulu));
    return datetime;
}
