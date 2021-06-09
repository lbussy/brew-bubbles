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

#include "wifihandler.h"

AsyncWiFiManager awm;
bool shouldSaveConfig = false;
Ticker blinker;

void doWiFi()
{
    doWiFi(false);
}

void doWiFi(bool dontUseStoredCreds)
{
    // Eliminate 4-way handshake errors
    // WiFi.disconnect(); // Don't think I should use this
    WiFi.enableSTA(true);
#ifdef ESP8266
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#elif ESP32
    WiFi.setSleep(false);
#endif
    awm.setCleanConnect(true); // Always disconnect before connecting

    // AsyncWiFiManager Callbacks
    awm.setAPCallback(apCallback);                       // Called after AP has started
    awm.setWebServerCallback(webServerCallback);         // Called after webserver is setup
    awm.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving WiFi creds or parameters
    awm.setSaveParamsCallback(saveParamsCallback);       // Called after WiFi parameters are saved
    awm.setSaveConfigCallback(saveConfigCallback);       // Called only if optional parameters are saved, or setBreakAfterConfig(true)
    awm.setConfigResetCallback(configResetCallback);     // Called after settings are reset

#ifndef DISABLE_LOGGING
    if (Log.getLevel())
        awm.setDebugOutput(true); // Verbose debug is enabled by default
    else
        awm.setDebugOutput(false);
#else
    awm.setDebugOutput(false);
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

    awm.setMenu(_wfmPortalMenu); // Set menu items
    // awm.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");
    // awm.setClass(F("invert"));   // Set dark theme

    awm.setCountry(WIFI_COUNTRY);    // Setting WiFi country seems to improve OSX soft ap connectivity
    awm.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel

    awm.setShowStaticFields(true); // Force show static ip fields
    awm.setShowDnsFields(true);    // Force show dns field always

    // Allow non-default host name
    AsyncWiFiManagerParameter hostname("hostname", "Custom Hostname", HOSTNAME, 32);
    awm.addParameter(&hostname);

    if (doNonBlock)
    {
        // Enable nonblocking portal (if configured)
        awm.setConfigPortalBlocking(false);
    }

    if (dontUseStoredCreds)
    {
        // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        awm.setConfigPortalTimeout(120);

        if (awm.startConfigPortal(config.apconfig.ssid, config.apconfig.passphrase))
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
            Log.notice(F("Hit timeout for on-demand portal, exiting." CR));
            setDoReset();
        }
    }
    else
    { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        awm.setConnectTimeout(30);
        awm.setConfigPortalTimeout(120);
        if (!awm.autoConnect(config.apconfig.ssid, config.apconfig.passphrase))
        {
            Log.warning(F("Failed to connect and/or hit timeout." CR));
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Restarting." CR));
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
            Log.notice(F("Saving custom hostname configuration: %s." CR), hostname.getValue());
            strlcpy(config.hostname, hostname.getValue(), sizeof(config.hostname));
            saveConfig();
#ifdef ESP8266
            WiFi.hostname(config.hostname);
#elif defined ESP32
            WiFi.setHostname(config.hostname);
#endif
        }
    }

    if (doNonBlock)
    {
        // Turn off nonblocking portal (if configured)
        Log.notice(F("Returning after non-blocking reconnect." CR));
        doNonBlock = false;
        mdnsreset();
    }

    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());
    blinker.detach();        // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
}

void resetWifi()
{ // Wipe WiFi settings and reset controller
    awm.resetSettings();
    blinker.detach();       // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    Log.notice(F("Restarting after clearing WiFi settings." CR));
    config.nodrd = true;
    saveConfig();
    _delay(100);
    ESP.restart();
}

void wifiBlinker()
{ // Invert Current State of LED
    digitalWrite(LED, !(digitalRead(LED)));
}

// AsyncWiFiManager Callbacks

void apCallback(AsyncWiFiManager *asyncWiFiManager)
{ // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
    blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
               asyncWiFiManager->getConfigPortalSSID().c_str(),
               WiFi.localIP().toString().c_str());
}

void configResetCallback()
{
    Log.verbose(F("[CALLBACK]: setConfigResetCallback fired." CR));
}

void preSaveConfigCallback()
{
    Log.verbose(F("[CALLBACK]: preSaveConfigCallback fired." CR));
}

void saveConfigCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." CR));
    shouldSaveConfig = true;
}

void saveParamsCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveParamsCallback fired." CR));
    shouldSaveConfig = true;
}

void webServerCallback()
{
    Log.verbose(F("[CALLBACK]: setWebServerCallback fired." CR));
}

void tcpCleanup(void)
{
    // Supposedly not needed, but we still get -8 errors on occasion
    // https://github.com/esp8266/Arduino/tree/master/doc/faq#how-to-clear-tcp-pcbs-in-time-wait-state-
    while (tcp_tw_pcbs)
        tcp_abort(tcp_tw_pcbs);
}
