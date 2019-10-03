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

#ifndef _BUB_STRINGS_H
#define _BUB_STRINGS_H

#include <Arduino.h>

// const dataType variableName[] PROGMEM = {data0, data1, data3…?};

namespace {
    const char _wfmHtml1[] PROGMEM = "<p>Enter a custom hostname if you would like something other than \"brewbubbles.\" Do not enter the \".local\" portion, this will be added automatically.</p>";
    const char _wfmHtml2[] PROGMEM = "<p>If you would like to provide static IP information, enter it here.  All fields must be correctly filled in or else configuration will not be applied.</p>";
    const char _wfmIpPattern[] PROGMEM = "pattern='((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$'";
    const char _wfmHostPattern[] PROGMEM = "pattern='^[a-zA-Z][a-zA-Z\d-]{1,22}[a-zA-Z\d]$'";
    std::vector<const char *> _wfmPortalMenu PROGMEM = {
        "wifi",
        "wifinoscan",
        "sep",
        "info",
        "param",
        "close",
        "sep",
        "erase",
        "restart",
        "exit"
    };
}

#endif // _BUB_STRINGS_H
