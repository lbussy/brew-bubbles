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

#include "mdns.h"

void mdnssetup() {
    if (!MDNS.begin(WiFi.hostname())) { // Start the mDNS responder for esp8266.local
        Log.error("Error setting up MDNS responder." CR);
    } else {
        Log.notice("mDNS responder started for %s.local." CR, HOSTNAME); // TODO:  Get from configuration 
        if (!MDNS.addService("http", "tcp", PORT)) {
            Log.error("Failed to register MDNS service." CR);
        } else {
            Log.notice("HTTP registered via MDNS on port %i." CR, PORT); // TODO:  Get from configuration
        }
    }
}

IPAddress mdnsquery(char hostname[63]) {
    // char* hostname = "brewpi.local";
    // IPAddress ip = mdnsquery(hostname);
    // Log.notice("mDNS Lookup - %s: %d.%d.%d.%d." CR, hostname, ip[0], ip[1], ip[2], ip[3]);
    Log.notice("Sending mDNS query." CR);
    int n = MDNS.queryService("workstation", "tcp");
    Log.notice("mDNS query complete." CR);
    if (n == 0) {
        Log.notice("No services found." CR);
        return {0, 0, 0, 0};
    } else {
        for (int i = 0; i < n; ++i) {
            char foundhost[63];
            MDNS.hostname(i).toCharArray(foundhost, 63);
            if(strcmp(foundhost, hostname) == 0) {
                IPAddress ip = MDNS.IP(i);
                return ip;
            }
        }
        return {0, 0, 0, 0};
    }
}
