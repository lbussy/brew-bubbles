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

#include "main.h"

LocalTime loc;

void setup() {
    serial();
    delay(200); // Let pins settle
    pinMode(RESETWIFI, INPUT_PULLUP);
    if(digitalRead(RESETWIFI) == LOW) wifisetup(true); // Reset wifi
    else wifisetup(false);
    mdnssetup();
    webserversetup();
    loc.StartTime();
    otasetup();
}

void loop() {
    MDNS.update();
    webserverloop();
    bubbles(loc.GetLocalTime());
    ArduinoOTA.handle();
    yield();
}
