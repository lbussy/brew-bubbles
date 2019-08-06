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

//  Time.h & TimeLib.h:  https://github.com/PaulStoffregen/Time

#include "localtime.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

LocalTime::LocalTime() {

}

void LocalTime::StartTime() {
    timeClient.begin();
}

char* LocalTime::GetLocalTime() {
    // Update the NTP client and get the UNIX UTC timestamp 
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();

    // Convert received time stamp to time_t object
    time_t local, utc, zulu;
    utc = epochTime;

    // Convert the UTC UNIX timestamp to local time
    // TODO: Construct all timezones
    // https://github.com/JChristensen/Timezone
    //
    // US Eastern Time
    TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -300}; // UTC - 5 hours - change this as needed
    TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -360}; // UTC - 6 hours - change this as needed
    Timezone usEastern(usEDT, usEST);
    local = usEastern.toLocal(utc);

    // Zulu Time
    TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0}; // Zulu Time
    Timezone zuluGMT(GMT);
    zulu = zuluGMT.toLocal(utc);

    // Get JS-formatted date/time string
    static char datetime[19];
    sprintf(datetime, "%u-%u-%uT%u:%u:%uZ", year(zulu), month(zulu), day(zulu), hour(zulu), minute(zulu), second(zulu));
    return datetime;
}
