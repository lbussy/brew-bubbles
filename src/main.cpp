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
    serial();
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
        Log.notice(F("mDNS responder started, hostname: %s.local." CR), WiFi.hostname().c_str());
        MDNS.addService("http", "tcp", 80);
    }

    WebServer *server = WebServer::getInstance();
    server->initialize(PORT); // Turn on web server

    NtpHandler *ntpTime = NtpHandler::getInstance();
    ntpTime->start();
    
    execspiffs(); // Check for pending SPIFFS update

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop() {
    JsonConfig *config = JsonConfig::getInstance();
    WebServer *server = WebServer::getInstance();
    Bubbles *bubble = Bubbles::getInstance();

    // Circular buffer/averages
    // Ticker aTempAvgUpdate;
    // Ticker vTempAvgUpdate;
    // Ticker bAvgUpdate;
    //aTempAvgUpdate.attach(60, NULL); // DEBUG disabled
    //vTempAvgUpdate.attach(60, NULL); // DEBUG disabled
    //bAvgUpdate.attach(60, NULL); // DEBUG disabled

    // Regular Bubble loop to create 60 second JSON
    Ticker bubUpdate;
    bubUpdate.attach(BUBLOOP, [bubble](){ bubble->update(); });

    // Target loops
    // Ticker postTimer; // DEBUG disabled
    // Ticker bfTimer; // DEBUG disabled
    // postTimer.attach(config->targetfreq, httpPost); // DEBUG disabled
    // bfTimer.attach(config->bffreq * 60, bfPost); // DEBUG disabled

    while (true) {
        // If timers needs to be updated, update timers
        // if (config->updateTargetFreq) { // DEBUG disabled
        //     Log.notice(F("Resetting Target frequency timer to %l seconds." CR), config->targetfreq); // DEBUG disabled
        //     postTimer.detach(); // DEBUG disabled
        //     postTimer.attach(config->targetfreq, httpPost); // DEBUG disabled
        //     config->updateTargetFreq = false; // DEBUG disabled
        // } // DEBUG disabled
        // if (config->updateBFFreq) { // DEBUG disabled
        //     Log.notice(F("Resetting Brewer's Friend frequency timer to %l minutes." CR), config->bffreq); // DEBUG disabled
        //     bfTimer.detach(); // DEBUG disabled
        //     bfTimer.attach(config->bffreq * 60, bfPost); // DEBUG disabled
        //     config->updateBFFreq = false; // DEBUG disabled
        // } // DEBUG disabled
        server->handleLoop();
        MDNS.update(); 
        if (digitalRead(COUNTPIN) == HIGH) // Non-interrupt driven LED logic
            digitalWrite(LED, LOW); // Turn LED on when not obstructed
        else
            digitalWrite(LED, HIGH); // Make sure LED turns off after a bubble
    }
}
