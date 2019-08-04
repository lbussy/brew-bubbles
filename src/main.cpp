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

#include "main.h"

void setup() {
    serial();
    wifisetup();
    mdnssetup();
    webserversetup();
    char hostname[39] = "brewpi.local";
    mdnsquery(hostname);
    yield();
}

void loop() {
    MDNS.update();
    webserverloop();
    Bubbles();
    yield();
}

void mdnssetup() {
    if (!MDNS.begin(WiFi.hostname())) { // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder.");
    } else {
        Serial.print("mDNS responder started for ");
        // Serial.print(WiFi.hostname()); // TODO:  Get from configuration
        Serial.print(HOSTNAME);
        Serial.println(".local.");
        if (!MDNS.addService("http", "tcp", PORT)) {
            Serial.println("Failed to register MDNS service.");
        } else {
            Serial.print("HTTP registered via MDNS on port ");
            Serial.print(PORT);
            Serial.println(".");
        }
    }
}

void mdnsquery(char hostname[39]) {
        Serial.println("Sending mDNS query");
    int n = MDNS.queryService("workstation", "tcp");
    Serial.println("mDNS query done");
    if (n == 0) {
        Serial.println("No services found.");
    } else {
        for (int i = 0; i < n; ++i) {
            char foundhost[39];
            MDNS.hostname(i).toCharArray(foundhost, 39);
            if(strcmp(foundhost, hostname) == 0) {
                Serial.print("FOUND: ");
                Serial.print(foundhost);
                Serial.print(": ");
                Serial.print(MDNS.IP(i));
                Serial.println(".");
            }
        }
    }
}
