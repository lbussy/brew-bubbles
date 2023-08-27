/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

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

#include "wifihandler.h"

#include "config.h"
#include "jsonconfig.h"
#include "tools.h"
#include "mdns.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif

#include <Ticker.h>
#include <ArduinoLog.h>

extern struct Config config;
extern const size_t capacitySerial;
extern const size_t capacityDeserial;

WiFiManager wm;
bool shouldSaveConfig = false;
Ticker blinker;

void doWiFi()
{
    doWiFi(false);
}

void doWiFi(bool dontUseStoredCreds)
{
    // Eliminate 4-way handshake errors
    WiFi.disconnect();
    WiFi.enableSTA(true);
#ifdef ESP8266
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#elif ESP32
    WiFi.setSleep(false);
#endif
    wm.setBreakAfterConfig(true);

    // WiFiManager Callbacks
    wm.setAPCallback(apCallback);                 // Called after AP has started
    wm.setSaveConfigCallback(saveConfigCallback); // Called only if optional parameters are saved, or setBreakAfterConfig(true)

#ifndef DISABLE_LOGGING
    if (Log.getLevel())
        wm.setDebugOutput(false); // Verbose debug is enabled by default
    else
        wm.setDebugOutput(false);
#else
    wm.setDebugOutput(false);
#endif

    std::vector<const char *> _wfmPortalMenu = {
        "wifi",
        "wifinoscan",
        "sep",
        "info",
        //"param",
        //"close",
        "erase",
        "restart",
        "exit"};

    wm.setMenu(_wfmPortalMenu);     // Set menu items
    wm.setCountry(WIFI_COUNTRY);    // Setting WiFi country seems to improve OSX soft ap connectivity
    wm.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel
    wm.setShowStaticFields(true);   // Force show static ip fields
    wm.setShowDnsFields(true);      // Force show dns field always

    // Allow non-default host name
    WiFiManagerParameter hostname("hostname", "Custom Hostname", HOSTNAME, 32);
    wm.addParameter(&hostname);

    if (doNonBlock)
    {
        // Enable nonblocking portal (if configured)
        wm.setConfigPortalBlocking(false);
    }

    if (dontUseStoredCreds)
    {
        // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        wm.setConfigPortalTimeout(120);

        if (wm.startConfigPortal(config.apconfig.ssid, config.apconfig.passphrase))
        {
            // We finished with portal, do we need this?
        }
        else
        {
            // Hit timeout on voluntary portal
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.notice(F("Hit timeout for on-demand portal, exiting." LF));
            config.nodrd = true;
            saveConfig();
            ESP.reset();
        }
    }
    else
    { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        wm.setConnectTimeout(30);
        wm.setConfigPortalTimeout(120);
        if (!wm.autoConnect(config.apconfig.ssid, config.apconfig.passphrase))
        {
            Log.warning(F("Failed to connect and/or hit timeout." LF));
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Restarting." LF));
            config.nodrd = true;
            saveConfig();
            _delay(100);
            ESP.restart();
        }
        else
        {
            // We finished with portal (We were configured)
#ifdef ESP8266
            WiFi.setSleepMode(WIFI_NONE_SLEEP); // Make sure sleep is disabled
#endif
            blinker.detach();        // Turn off blinker
            digitalWrite(LED, HIGH); // Turn off LED
#ifdef ESP8266
            WiFi.hostname(config.hostname);
#elif defined ESP32
            WiFi.setHostname(config.hostname);
#endif
        }
    }

    if (shouldSaveConfig)
    { // Save configuration
        if (hostname.getValue() != config.hostname)
        {
            Log.notice(F("Saving custom hostname configuration: %s." LF), hostname.getValue());
            strlcpy(config.hostname, hostname.getValue(), sizeof(config.hostname));
#ifdef ESP8266
            WiFi.hostname(config.hostname);
#elif defined ESP32
            WiFi.setHostname(config.hostname);
#endif
            config.nodrd = true;
            saveConfig();
            ESP.reset();
        }
    }

    if (doNonBlock)
    {
        // Turn off nonblocking portal (if configured)
        Log.notice(F("Returning after non-blocking reconnect." LF));
        doNonBlock = false;
        mdnsreset();
    }

    Log.notice(F("Connected. IP address: %s." LF), WiFi.localIP().toString().c_str());
    blinker.detach();        // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
}

void resetWifi()
{ // Wipe WiFi settings and reset controller
    wm.resetSettings();
    blinker.detach();       // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    Log.notice(F("Restarting after clearing WiFi settings." LF));
    config.nodrd = true;
    saveConfig();
    _delay(100);
    ESP.restart();
}

void wifiBlinker()
{ // Invert Current State of LED
    digitalWrite(LED, !(digitalRead(LED)));
}

// WiFiManager Callbacks

void apCallback(WiFiManager *asyncWiFiManager)
{ // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." LF));
    blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." LF),
               asyncWiFiManager->getConfigPortalSSID().c_str(),
               WiFi.localIP().toString().c_str());
}

void configResetCallback()
{
    Log.verbose(F("[CALLBACK]: setConfigResetCallback fired." LF));
}

void saveConfigCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." LF));
    shouldSaveConfig = true;
}

void tcpCleanup(void)
{
    // Supposedly not needed, but we still get -8 errors on occasion
    // https://github.com/esp8266/Arduino/tree/master/doc/faq#how-to-clear-tcp-pcbs-in-time-wait-state-
    while (tcp_tw_pcbs)
        tcp_abort(tcp_tw_pcbs);
}
