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

void presentPortal(bool ignore = false) { // Present AP and captive portal to allow new settings
    JsonConfig *config = JsonConfig::getInstance();
    WiFiManager wifiManager;
    wifiManager.setSaveConfigCallback(saveConfigCallback); // Set callback to save settings
    wifiManager.setAPCallback(configModeCallback); // Give some basic instructions via Serial

    //wifiManager.resetSettings(); // DEBUG

    wifi_station_set_hostname(config->hostname);

    if (ignore) {
        blinker.attach_ms(APBLINK, wifiBlinker);
        wifiManager.setTimeout(120);
        if (wifiManager.startConfigPortal(config->ssid, config->appwd)) {
            if (shouldSaveConfig) { // Save configuration
                config->Save();
            }
        } else {
            // Hit timeout on voluntary portal
            if (blinker.active()) blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            ESP.restart();
        }
    } else { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        if (!wifiManager.autoConnect(config->ssid, config->appwd)) {
            Log.warning(F("Failed to connect and hit timeout."));
            if (blinker.active()) blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            ESP.restart();
            _delay(1000); // Just a hack to allow it to reset
        }
    }
    if (blinker.active()) blinker.detach(); // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED

    wifi_station_set_hostname(config->hostname);

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
    ESP.restart();
    _delay(1000);
}

void saveConfigCallback() { // Set flag to save config
    Log.verbose(F("Callback: Saving config."));
    shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager) {
    if (blinker.active()) blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
        myWiFiManager->getConfigPortalSSID().c_str(),
        WiFi.localIP().toString().c_str());
}

void wifiBlinker() {
  digitalWrite(LED, !(digitalRead(LED)));  // Invert Current State of LED  
}
