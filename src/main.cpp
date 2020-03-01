/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#include "main.h"

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
    // bool rst = drd.detect(); // Check for double-reset
    serial();
    pinMode(LED, OUTPUT);

    _delay(200); // Let pins settle, else detect is inconsistent
    pinMode(RESETWIFI, INPUT_PULLUP);
    // DEBUG:  Commented startup lines
    // if (digitalRead(RESETWIFI) == LOW) {
    //     Log.notice(F("%s low, presenting portal." CR), RESETWIFI);
    //     doWiFi(true);
    // } else if (rst == true) {
    //     Log.notice(F("DRD: Double reset boot, presenting portal." CR));
    //     doWiFi(true);
    // } else {
        Log.verbose(F("DRD: Normal boot." CR));
        doWiFi();
    // }

    NtpHandler *ntpTime = NtpHandler::getInstance();
    ntpTime->start();

    initWebServer(); // Turn on web server

    mdnssetup(); // Set up mDNS responder
    
    execspiffs();   // Check for pending SPIFFS update
    loadBpm() ;     // Get last Bpm reading if it was a controlled reboot

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop() {
    JsonConfig *config = JsonConfig::getInstance();
    Bubbles *bubble = Bubbles::getInstance();

    // Bubble loop to create 60 second readings
    Ticker bubUpdate;
    bubUpdate.attach(BUBLOOP, [bubble](){ bubble->update(); });

    // Target timer
    Ticker urlTarget;
    // config->targetfreq * 60
    urlTarget.attach(5, [](){doURLTarget = true;});
    
    // Brewer's friend timer
    // Ticker bfTimer;
    // bfTimer.attach(config->bffreq * 60, [](){doBFTarget = true;});

    // mDNS Reset Timer - Helps avoid the host not found issues
    Ticker mDNSTimer;
    mDNSTimer.attach(MDNSTIMER, mdnsreset);

    // Reboot timer - I wish controllers could be counted on to be more
    // stable but at least it only takes a few seconds.
    Ticker rebootTimer;
    rebootTimer.attach(REBOOTTIMER, reboot);

    while (true) {
        // Handle semaphores - No radio work in a Ticker
        tickerLoop();

        // Regular loop handlers
        MDNS.update();          // Handle mDNS requests

        // If target frequencies needs to be updated, update here
        if (config->updateTargetFreq) {
            Log.notice(F("Resetting URL Target frequency timer to %l minutes." CR), config->targetfreq);
            urlTarget.detach();
            urlTarget.attach(config->targetfreq * 60, [](){doURLTarget = true;});
            config->updateTargetFreq = false;
        }
        // if (config->updateBFFreq) {
        //     Log.notice(F("Resetting Brewer's Friend frequency timer to %l minutes." CR), config->bffreq);
        //     bfTimer.detach();
        //     bfTimer.attach(config->bffreq * 60, [](){doBFTarget = true;});
        //     config->updateBFFreq = false;
        // }

        yield();
    }
}
