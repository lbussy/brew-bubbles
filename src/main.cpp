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
    bool rst = drd.detect(); // Check for double-reset
    serial();

    if (loadConfig())
        Log.notice(F("Configuration loaded." CR));
    else
        Log.error(F("Unable to load cofiguration." CR));

    pinMode(LED, OUTPUT);

    _delay(200); // Let pins settle, else detect is inconsistent
    pinMode(RESETWIFI, INPUT_PULLUP);
    if (digitalRead(RESETWIFI) == LOW) {
        Log.notice(F("%s low, presenting portal." CR), RESETWIFI);
        doWiFi(true);
    } else if (rst == true) {
        Log.notice(F("DRD: Double reset boot, presenting portal." CR));
        doWiFi(true);
    } else {
        Log.verbose(F("DRD: Normal boot." CR));
        doWiFi();
    }

    setClock();         // Set NTP Time
    initWebServer();    // Turn on web server
    mdnssetup();        // Set up mDNS responder
    execspiffs();       // Check for pending SPIFFS update
    loadBpm() ;         // Get last BPM reading if it was a controlled reboot

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop() {
    Bubbles *bubble = Bubbles::getInstance();

    // Bubble loop to create 60 second readings
    Ticker bubUpdate;
    bubUpdate.attach(BUBLOOP, [bubble](){ bubble->update(); });

    // Target timer
    Ticker urlTarget;
    urlTarget.attach(config.urltarget.freq * 60, setDoURLTarget);
    
    // Brewer's friend timer
    Ticker bfTimer;
    bfTimer.attach(config.brewersfriend.freq * 60, setDoBFTarget);

    // mDNS Reset Timer - Helps avoid the host not found issues
    Ticker mDNSTimer;
    mDNSTimer.attach(MDNSTIMER, mdnsreset);

    // Reboot timer - I wish controllers could be counted on to be more
    // stable but at least it only takes a few seconds.
    Ticker rebootTimer;
    rebootTimer.attach(REBOOTTIMER, reboot);

    while (true) {
        // Handle semaphores - No radio work in a Ticker!
        tickerLoop();

        // Regular loop handlers
        MDNS.update();          // Handle mDNS requests

        // If target frequencies needs to be updated, update here
        if (config.urltarget.update) {
            Log.notice(F("Resetting URL Target frequency timer to %l minutes." CR), config.urltarget.freq);
            urlTarget.detach();
            urlTarget.attach(config.urltarget.freq * 60, setDoURLTarget);
            config.urltarget.update = false;
        }
        if (config.brewersfriend.update) {
            Log.notice(F("Resetting Brewer's Friend frequency timer to %l minutes." CR), config.brewersfriend.freq);
            bfTimer.detach();
            bfTimer.attach(config.brewersfriend.freq * 60, setDoBFTarget);
            config.brewersfriend.update = false;
        }

        yield();
    }
}
