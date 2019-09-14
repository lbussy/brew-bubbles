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

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
    bool rst = drd.detect(); // Check for double-reset
    serial(); // TODO:  Includes a 4000 ms delay
    pinMode(LED, OUTPUT);

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

    WebServer *server = WebServer::getInstance();
    server->initialize(PORT); // Turn on web server

    NtpHandler *ntpTime = NtpHandler::getInstance();
    ntpTime->start();

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop() {
    Bubbles *bubble = Bubbles::getInstance();
    JsonConfig *config = JsonConfig::getInstance();
    WebServer *server = WebServer::getInstance();
    Ticker postTimer;
    Ticker bfTimer;
    postTimer.attach(config->targetfreq, httpPost);
    bfTimer.attach(config->bffreq * 60, bfPost);
    while (true) {
        // If timers needs to be updated, update timers
        if (config->updateTargetFreq) {
            Log.notice(F("Resetting Target frequency timer to %l seconds." CR), config->targetfreq);
            postTimer.detach();
            postTimer.attach(config->targetfreq, httpPost);
            config->updateTargetFreq = false;
        }
        if (config->updateBFFreq) {
            Log.notice(F("Resetting Brewer's Friend frequency timer to %l minutes." CR), config->bffreq);
            bfTimer.detach();
            bfTimer.attach(config->bffreq * 60, bfPost);
            config->updateBFFreq = false;
        }
        bubble->Update();
        server->handleLoop();
        MDNS.update();
    }
}
