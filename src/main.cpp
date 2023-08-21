/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

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

#include "main.h"
#include "serialhandler.h"
#include "config.h"
#include "execota.h"
#include "jsonconfig.h"
#include "webserver.h"
#include "wifihandler.h"
#include "version.h"
#include "pushtarget.h"
#include "target.h"
#include "brewfather.h"
#include "brewersfriend.h"
#include "pushhelper.h"
#include "bubbles.h"
#include "mdns.h"
#include "ntp.h"
#include "thatVersion.h"
#include "tools.h"
#include <ESP_DoubleResetDetector.h>
#include <ArduinoLog.h>
#include <Ticker.h>
#include <Arduino.h>
#include <ESP8266mDNS.h>

DoubleResetDetector *drd;

Ticker getThatVer; // Poll for server version
Ticker bubUpdate;  // Bubble loop to get periodic readings
Ticker urlTarget;  // Target timer
Ticker bfTimer;    // Brewer's Friend timer
Ticker brewfTimer; // Brewfather timer
Ticker tsTimer;    // ThingSpeak timer

void setup()
{
    drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
    setSerial();

    if (!LittleFS.begin())
    {
        Log.fatal(F("Unable to mount filesystem." CR));
        while (1)
        {
            ;
        }
    }

    if (loadConfig())
        Log.notice(F("Configuration loaded." CR));
    else
        Log.error(F("Unable to load configuration." CR));

    pinMode(LED, OUTPUT);
    pinMode(RESETWIFI, INPUT_PULLUP);

    _delay(200); // Let pins settle, else detect is inconsistent

    if (digitalRead(RESETWIFI) == LOW)
    {
        Log.notice(F("%s low, presenting portal." CR), stringify(RESETWIFI));
        doWiFi(true);
    }
    else if (!config.nodrd && drd->detectDoubleReset())
    {
        Log.notice(F("DRD: Triggered, presenting portal." CR));
        doWiFi(true);
    }
    else
    {
        Log.verbose(F("DRD: Normal boot." CR));
        config.nodrd = false;
        doWiFi();
    }

    execspiffs();         // Check for pending File System update
    setClock();           // Set NTP Time
    loadBpm();            // Get last BPM reading if it was a controlled reboot
    mdnssetup();          // Set up mDNS responder
    initWebServer();      // Turn on web server
    if (getThatVersion()) // Get server version at startup
        Log.notice(F("Obtained available version." CR));
    if (bubbles.start()) // Initialize bubble counter
        Log.notice(F("Bubble counter initialized." CR));

    Log.notice(F("Started %s version %s/%s (%s) [%s]." CR), API_KEY, fw_version(), fs_version(), branch(), build());

    getThatVer.attach(POLLSERVERVERSION, getThatVersion);             // Poll for server version
    bubUpdate.attach(BUBLOOP, setDoBub);                              // Bubble loop to get periodic readings
    urlTarget.attach(config.urltarget.freq * 60, setDoURLTarget);     // Target timer
    bfTimer.attach(config.brewersfriend.freq * 60, setDoBFTarget);    // Brewer's Friend timer
    brewfTimer.attach(config.brewfather.freq * 60, setDoBrewfTarget); // Brewfather timer
    tsTimer.attach(config.thingspeak.freq * 60, setDoTSTarget);       // ThingSpeak timer
}

void loop()
{
    // Handle DRD timeout
    drd->loop();

    // Handle semaphores - No radio work in a Ticker!
    tickerLoop();

    // Toggle LED according to sensor
    digitalWrite(LED, !digitalRead(COUNTPIN));

    // Handle mDNS requests
    MDNS.update();

    // If target frequencies needs to be updated, update here
    if (config.urltarget.update)
    {
        Log.notice(F("Resetting URL Target frequency timer to %l minutes." CR), config.urltarget.freq);
        urlTarget.detach();
        urlTarget.attach(config.urltarget.freq * 60, setDoURLTarget);
        config.urltarget.update = false;
        saveConfig();
    }
    if (config.brewersfriend.update)
    {
        Log.notice(F("Resetting Brewer's Friend frequency timer to %l minutes." CR), config.brewersfriend.freq);
        bfTimer.detach();
        bfTimer.attach(config.brewersfriend.freq * 60, setDoBFTarget);
        config.brewersfriend.update = false;
        saveConfig();
    }
    if (config.brewfather.update)
    {
        Log.notice(F("Resetting Brewfather frequency timer to %l minutes." CR), config.brewfather.freq);
        brewfTimer.detach();
        brewfTimer.attach(config.brewfather.freq * 60, setDoBrewfTarget);
        config.brewfather.update = false;
        saveConfig();
    }
    if (config.thingspeak.update)
    {
        Log.notice(F("Resetting ThingSpeak frequency timer to %l minutes." CR), config.thingspeak.freq);
        tsTimer.detach();
        tsTimer.attach(config.thingspeak.freq * 60, setDoTSTarget);
        config.thingspeak.update = false;
        saveConfig();
    }
    serialLoop();
    maintenanceLoop();
    yield();
}
