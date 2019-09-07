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

WebServer *server = WebServer::getInstance();
DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
    bool rst = drd.detect(); // Check for double-reset
    serial();

    _delay(200); // Let pins settle, else detect is inconsistent
    pinMode(RESETWIFI, INPUT_PULLUP);
    if (digitalRead(RESETWIFI) == LOW) {
        Log.notice(F("%s low, presenting portal." CR), RESETWIFI);
        presentPortal(true);
    } else if (rst == true) {
        Log.notice(F("DRD: Double reset boot, presenting portal." CR));
        presentPortal(true);
    } else {
        Log.verbose(F("DRD: Normal boot." CR));
        presentPortal(false);
    }

    JsonConfig *config = JsonConfig::getInstance();
    if (!MDNS.begin(config->hostname)) {
        Log.error(F("Error setting up MDNS responder."));
    } else {
        Log.notice(F("mDNS responder started, hostname %s.local." CR), WiFi.hostname().c_str());
        MDNS.addService("http", "tcp", 80);
    }

    server->initialize(PORT); // Turn on web server

    NtpHandler *ntpTime = NtpHandler::getInstance();
    ntpTime->start();

    Log.notice(F("Started Brew Bubbles version %s (%s) [%s]." CR), version(), branch(), build());
}

void loop() {
    Bubbles *bubble = Bubbles::getInstance();
    while (true) {
        bubble->Update();
        server->handleLoop();
        MDNS.update();
    }
}
