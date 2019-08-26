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

void configModeCallback (WiFiManager *myWiFiManager) {
#ifndef DISABLE_LOGGING
    // If you used auto generated SSID, print it
    String ssid = myWiFiManager->getConfigPortalSSID();
     // Convert String to char array
    int n = ssid.length();
    char ap[n + 1]; 
    strcpy(ap, ssid.c_str()); 
    IPAddress myIP = WiFi.softAPIP();
    Log.notice(F("Entered AP configuration mode, SSID: %s, IP: %d.%d.%d.%d." CR), ap, myIP[0], myIP[1], myIP[2], myIP[3]);
#endif //DISABLE_LOGGING
}

void wifisetup(bool reset) {
    WiFiManager wifiManager; // Local init only
    if(reset == true) wifiManager.resetSettings(); // Reset wifi

#ifndef DISABLE_LOGGING
    wifiManager.setDebugOutput(true); // Turn on Debug (default off)
    WiFi.printDiag(Serial);
#else
    wifiManager.setDebugOutput(false);
#endif //DISABLE_LOGGING

    wifiManager.setTimeout(180); // Timeout for config portal
    wifiManager.setAPCallback(configModeCallback); // Set callback for when entering AP

    JsonConfig *config = JsonConfig::getInstance(); // Get config

    if(!wifiManager.autoConnect(config->ssid, config->appwd)) {
#ifndef DISABLE_LOGGING
        Log.warning(F("Timed out waiting for connection to AP, resetting." CR));
#endif // DISABLE_LOGGING
        _delay(1000);
        ESP.reset();  // Reset and try again
        _delay(1000);
    }

    // Connected
    WiFi.hostname(config->hostname);
    
#ifndef DISABLE_LOGGING
    Log.notice(F("WiFi connected." CR));
    String host = WiFi.hostname();
    Log.notice(F("Hostname: %s" CR),  &host);
    IPAddress myIP = WiFi.localIP();
    Log.notice(F("IP address: %d.%d.%d.%d" CR), myIP[0], myIP[1], myIP[2], myIP[3]);
#endif // DISABLE_LOGGING

    digitalWrite(LED, HIGH); // Turn off LED
}

void presentPortal() { // Present AP and captive portal to allow new settings
    JsonConfig *config = JsonConfig::getInstance();
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(120); // Reset controller after 120 secs
    wifiManager.startConfigPortal(config->ssid, config->appwd);
}

void resetWifi() { // Wipe wifi settings and reset controller
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    _delay(3000);
    ESP.reset();
}
