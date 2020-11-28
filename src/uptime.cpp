/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

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

#include "uptime.h"

unsigned long uptimeNow;

void getNow()
{
    uptimeNow = millis();
}

const int uptimeDays()
{
    if ((millis() - uptimeNow) > 3000)
        getNow(); // Make sure we are current
    // 86400000 millis in a day
    const int days = (int)floor(uptimeNow / 86400000);
    return days;
}

const int uptimeHours()
{
    if ((millis() - uptimeNow) > 3000)
        getNow(); // Make sure we are current
    // Get uptime in millisseconds
    unsigned long m = uptimeNow;
    // Subtract any full days, 86400000 millis in a day
    if (uptimeDays())
        m = m % (uptimeDays() * 86400000);
    // 3600000 millis in an hour
    const int hours = (int)floor(m / 3600000);
    return hours;
}

const int uptimeMinutes()
{
    if ((millis() - uptimeNow) > 3000)
        getNow(); // Make sure we are current
    // Get uptime in millisseconds
    unsigned long m = uptimeNow;
    // Subtract any full hours, 3600000 millis in an hour
    if (uptimeHours())
        m = m % (uptimeHours() * 3600000);
    // 60000 millis in a minute
    const int minutes = (int)floor(m / 60000);
    return minutes;
}

const int uptimeSeconds()
{
    if ((millis() - uptimeNow) > 3000)
        getNow(); // Make sure we are current
    // Get uptime in millisseconds
    unsigned long m = uptimeNow;
    // Subtract any full minutes, 60000 millis in a minute
    if (uptimeMinutes())
        m = m % (uptimeMinutes() * 60000);
    // 1000 millis in a second
    const int seconds = (int)floor(m / 1000);
    return seconds;
}

const int uptimeMillis()
{
    if ((millis() - uptimeNow) > 3000)
        getNow(); // Make sure we are current
    // Get uptime in millisseconds
    unsigned long m = uptimeNow;
    // Subtract any full seconds, 1000 millis in a minute
    if (uptimeSeconds())
        m = m % (uptimeSeconds() * 1000);
    // 1000 millis in a second
    const int millis = (int)floor(m / 1000);
    return millis;
}
