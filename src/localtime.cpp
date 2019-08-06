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

// Define NTP properties
#define NTP_OFFSET 60 * 60 // In seconds
#define NTP_INTERVAL 60 * 1000 // In miliseconds
#define NTP_ADDRESS "pool.ntp.org"

// Set up the NTP UDP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

void timesetup() {
    timeClient.begin();
}

void timeloop() {
    // Update the NTP client and get the UNIX UTC timestamp 
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();

    // Convert received time stamp to time_t object
    time_t local, utc;
    utc = epochTime;

    // Convert the UTC UNIX timestamp to local time
    // TODO: Construct all timezones
    // https://github.com/JChristensen/Timezone
    TimeChangeRule usEDT = {
        "EDT",
        Second,
        Sun,
        Mar,
        2,
        -300
    }; // UTC - 5 hours - change this as needed
    TimeChangeRule usEST = {
        "EST",
        First,
        Sun,
        Nov,
        2,
        -360
    }; // UTC - 6 hours - change this as needed
    Timezone usEastern(usEDT, usEST);
    local = usEastern.toLocal(utc);

    // Year (4 digit)
    Serial.print(year(local));
    Serial.print("/");

    // Month (2 digit)
    Serial.print(month(local));
    Serial.print("/");

    // Day (2 digit)
    Serial.print(day(local));
    Serial.print(" ");

    // Hour (2 digit, 24-hour)
    Serial.print(hour(local));
    Serial.print(":");

    // Minute (2 digit)
    Serial.print(minute(local));
    Serial.print(":");

    // Second (2 digit)
    Serial.println(second(local));
}
