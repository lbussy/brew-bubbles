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
    Serial.print("Entered AP configuration mode. IP:  ");
    Serial.print(WiFi.softAPIP());
    Serial.print(", SSID:  ");
    // If you used auto generated SSID, print it
    Serial.print(myWiFiManager->getConfigPortalSSID());
    Serial.println(".");
}

void wifisetup() {
    WiFiManager wifiManager; // Local init only
    // wifiManager.resetSettings(); // For testing
#if DEBUG > 0
    wifiManager.setDebugOutput(true); // Turn on Debug (default off)
#else
    wifiManager.setDebugOutput(false);
#endif
    wifiManager.setTimeout(180); // Timeout for config portal
    wifiManager.setAPCallback(configModeCallback); // Set callback for when entering AP

    // Custom Parameters
    // {Name}, {Prompt}, {Lamda}, {Length}
    // WiFiManagerParameter {parameter_name}("{name}", "{prompt}", {lamda}, {length (int)});
    // wifiManager.addParameter(&{parameter name});

    // Inject custom head element - overwrites included css
    // wifiManager.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");

    // Custom text for captive portal
    //WiFiManagerParameter custom_text("<p>This is just a text paragraph</p>");
    //wifiManager.addParameter(&custom_text);

    // Set custom ip for portal
    // wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    if(!wifiManager.autoConnect(HOSTNAME, AP_PASSWD)) {
        Serial.println("Timed out trying to connect to AP.");
        Serial.println();
        delay(1000);
        ESP.reset();  // Reset and try again
        delay(1000);
    }
    // Connected
    WiFi.hostname(HOSTNAME); // TODO: Get this from config
    Serial.println("WiFi connected.");
    Serial.print("Hostname: ");
    Serial.println(WiFi.hostname());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Save custom configuration
    // {parameter} = {parameter_name}.getValue(); // Read and save custom parameters

    // Set custom IP for Station
    // wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
}
