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

Bubbles *bubble = Bubbles::getInstance();
//LocalTime *time; // DEBUG

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
    bool rst = drd.detect(); // Check for double-reset
    serial();
    delay(200); // Let pins settle, else detect is inconsistent
    pinMode(RESETWIFI, INPUT_PULLUP);
    if (digitalRead(RESETWIFI) == LOW) {
        Log.notice(F("%s low, resetting wifi and restarting." CR), RESETWIFI);
        disco_restart();
    } else if (rst && !ipl()) {
        Log.notice(F("DRD: Double reset boot, resetting wifi and restarting." CR));
        disco_restart();
    } else {
        Log.verbose(F("DRD: Normal boot, re-using WiFi values." CR));
        wifisetup(false);
    }
    mdnssetup();
    webserversetup();
    otasetup();
    JsonConfig *config = JsonConfig::getInstance();
    if (config->dospiffs == true) { // Update SPIFFS on restart
        execspiffs();
    }
}

void loop() {
    MDNS.update();
    webserverloop();
    bubble->Update();
    ArduinoOTA.handle();
    //time->Update(); // DEBUG
    //doTargets();
    yield();
}

// Addresses: https://github.com/jenscski/DoubleResetDetect/issues/2
bool ipl() { // Determine if this is the first start after loading image
    char thisver[20] = __DATE__ __TIME__; // Sets at compile-time
    char savever[20] = "";
    bool _ipl = false;

    EEPROM.begin(20);
    EEPROM.get(EEPROM_ADDRESS, savever);
    if (strcmp (thisver, savever) != 0) {
        EEPROM.put(EEPROM_ADDRESS, thisver);
        EEPROM.commit();
        _ipl = true;
    }
    EEPROM.end();
    return _ipl;
}
