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

#include "wifi.h"

bool shouldSaveConfig = false;
Ticker blinker;

void doWiFi() {
    doWiFi(false);
}

void doWiFi(bool ignore = false) { // Handle WiFi and optionally ignore current config

    WiFiManager wifiManager;
    JsonConfig *config = JsonConfig::getInstance();

    // WiFiManager Callbacks
    wifiManager.setAPCallback(apCallback); // Called after AP has started
    wifiManager.setConfigResetCallback(configResetCallback); // Called after settings are reset
    wifiManager.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving wifi creds 
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.setSaveParamsCallback(saveParamsCallback); // Called after parameters are saved via params menu or wifi config
    wifiManager.setWebServerCallback(webServerCallback); // Called after webserver is setup

#ifndef DISABLE_LOGGING
    wifiManager.setDebugOutput(true); // Verbose debug is enabled by default
#else
    wifiManager.setDebugOutput(false);
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

    wifiManager.setMenu(_wfmPortalMenu);   // Set menu items
    wifiManager.setClass(F("invert"));     // Set dark theme

    wifiManager.setCountry(WIFI_COUNTRY); // Setting wifi country seems to improve OSX soft ap connectivity
    wifiManager.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel

    wifiManager.setShowStaticFields(true); // force show static ip fields
    wifiManager.setShowDnsFields(true);    // force show dns field always

    if (ignore) { // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        wifiManager.setConfigPortalTimeout(120);
        if (wifiManager.startConfigPortal(config->ssid, config->appwd)) {
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
        wifiManager.setConnectTimeout(30);
        wifiManager.setConfigPortalTimeout(120);
        if (!wifiManager.autoConnect(config->ssid, config->appwd)) {
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
    WiFiManager wifiManager;
    wifiManager.resetSettings();
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

// WiFiManager Callbacks

void apCallback(WiFiManager *myWiFiManager) { // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
    if (blinker.active()) blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
        myWiFiManager->getConfigPortalSSID().c_str(),
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

// Misc helpers

bool isNullField(const char *field) {
    return ((field == NULL) || (field[0] == '\0'));
}
