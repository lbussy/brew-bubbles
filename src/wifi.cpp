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

#include "wifi.h"

bool shouldSaveConfig = false;
Ticker blinker;

void doWiFi() {
    doWiFi(false);
}

void doWiFi(bool ignore = false) { // Handle WiFi and optionally ignore current config
    AsyncWiFiManager myAsyncWifiManager;
    JsonConfig *config = JsonConfig::getInstance();

    // AsyncWiFiManager Callbacks
    myAsyncWifiManager.setAPCallback(apCallback); // Called after AP has started
    myAsyncWifiManager.setConfigResetCallback(configResetCallback); // Called after settings are reset
    myAsyncWifiManager.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving wifi creds 
    myAsyncWifiManager.setSaveConfigCallback(saveConfigCallback);
    myAsyncWifiManager.setSaveParamsCallback(saveParamsCallback); // Called after parameters are saved via params menu or wifi config
    myAsyncWifiManager.setWebServerCallback(webServerCallback); // Called after webserver is setup

#ifndef DISABLE_LOGGING
    myAsyncWifiManager.setDebugOutput(true); // Verbose debug is enabled by default
#else
    myAsyncWifiManager.setDebugOutput(false);
#endif

    std::vector<const char *> _wfmPortalMenu = {
        "wifi",
        "wifinoscan",
        "sep",
        "info",
        //"param",
        //"close",
        //"sep",
        "erase",
        "restart",
        "exit"
    };

    myAsyncWifiManager.setMenu(_wfmPortalMenu);   // Set menu items
    myAsyncWifiManager.setClass(F("invert"));     // Set dark theme

    myAsyncWifiManager.setCountry(WIFI_COUNTRY); // Setting wifi country seems to improve OSX soft ap connectivity
    myAsyncWifiManager.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel

    myAsyncWifiManager.setShowStaticFields(true); // force show static ip fields
    myAsyncWifiManager.setShowDnsFields(true);    // force show dns field always

    if (ignore) { // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        myAsyncWifiManager.setConfigPortalTimeout(120);
        if (myAsyncWifiManager.startConfigPortal(config->ssid, config->appwd)) {
            // We finished with portal, do we need this?
        } else {
            // Hit timeout on voluntary portal
            if (blinker.active()) blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.notice(F("Hit timeout for on-demand portal, exiting." CR));
        }
    } else { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        myAsyncWifiManager.setConnectTimeout(30);
        myAsyncWifiManager.setConfigPortalTimeout(120);
        if (!myAsyncWifiManager.autoConnect(config->ssid, config->appwd)) {
            Log.warning(F("Failed to connect and hit timeout."));
            if (blinker.active()) blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Hit timeout on connect, restarting." CR));
            ESP.restart();
            _delay(1000); // Just a hack to give it time to reset
        } else {
            // We finished with portal (configured), do we need this?
        }
    }

    WiFi.mode(WIFI_STA); // Explicitly set mode, esp defaults to STA+AP
    WiFi.setSleepMode(WIFI_NONE_SLEEP); // Make sure sleep is disabled

    if (blinker.active()) blinker.detach(); // Turn off blinker
        digitalWrite(LED, HIGH); // Turn off LED

    if (shouldSaveConfig) { // Save configuration
        Log.notice(F("Saving configuration." CR));
        _delay(2000);
        Log.notice(F("Resetting." CR));
        ESP.restart(); // If we don't reset, it hangs connecting to AP
    }

    WiFi.hostname(config->hostname);

    Log.notice(F("Connecting to access point: %s."), WiFi.SSID().c_str());
    while (WiFi.status() != WL_CONNECTED) {
        blinker.attach_ms(STABLINK, wifiBlinker);
        _delay(500);
#ifdef LOG_LEVEL
        Serial.print(F("."));
#endif
    }
#ifdef LOG_LEVEL
    Serial.println();
#endif
    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());
    if (blinker.active()) blinker.detach(); // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
}

void resetWifi() { // Wipe wifi settings and reset controller
    AsyncWiFiManager myAsyncWifiManager;
    _delay(3000); // Allow page to load
    myAsyncWifiManager.resetSettings();
    if (blinker.active()) blinker.detach(); // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    _delay(3000);
    Log.warning(F("Restarting after clearing wifi settings." CR));
    ESP.restart();
    _delay(1000);
}

void wifiBlinker() { // Invert Current State of LED
    digitalWrite(LED, !(digitalRead(LED)));
}

// AsyncWiFiManager Callbacks

void apCallback(AsyncWiFiManager *asyncWiFiManager) { // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
    if (blinker.active()) blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
        asyncWiFiManager->getConfigPortalSSID().c_str(),
        WiFi.localIP().toString().c_str());
}

void configResetCallback() {
    Log.verbose(F("[CALLBACK]: setConfigResetCallback fired." CR));
}

void preSaveConfigCallback() {
    Log.verbose(F("[CALLBACK]:  " CR));
}

void saveConfigCallback() {
    Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." CR));
}

void saveParamsCallback() { // Set flag to save config
    Log.verbose(F("[CALLBACK]: setSaveParamsCallback fired." CR));
    shouldSaveConfig = true;
}

void webServerCallback() {
    Log.verbose(F("[CALLBACK]: setWebServerCallback fired." CR));
}
