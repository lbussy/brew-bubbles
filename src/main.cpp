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

// DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);
// DRD_TIMEOUT =    Maximum number of seconds between resets that counts
//                  as a double reset
// DRD_ADDRESS =    Address to the block in the RTC user memory change it
//                  if it collides with another usage of the address block
DoubleResetDetect drd(3.0, 0x00);

unsigned long ulMNow = millis(); // Time in millis now // DEBUG
unsigned long ulMStart = 0UL; // Start time // DEBUG

void setup() {
    serial();
    delay(200); // Let pins settle, else next detect is flakey
    pinMode(RESETWIFI, INPUT_PULLUP);
    if (digitalRead(RESETWIFI) == LOW) {
        Log.notice(F("%s low, resetting wifi and restarting." CR), RESETWIFI);
        disco_restart();
    } else if (drd.detect()) {
        Log.notice(F("Double reset boot, resetting wifi and restarting." CR));
        //disco_restart();
    } else {
        Log.verbose(F("Normal boot, re-using WiFi values." CR));
        wifisetup(false);
    }
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
    ulMNow = millis();
    if (ulMNow - ulMStart > BUBLOOP) { // If (now - start) > delay time, do work
        ulMStart = ulMNow;
    }
    yield();
}
