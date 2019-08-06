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
    Log.notice("Entered AP configuration mode, SSID: %s, IP: %d.%d.%d.%d." CR, ap, myIP[0], myIP[1], myIP[2], myIP[3]);
#endif //DISABLE_LOGGING
}

void wifisetup(bool reset) {
    WiFiManager wifiManager; // Local init only
    if(reset == true) wifiManager.resetSettings(); // Reset wifi
// #if DEBUG > 0
#ifndef DISABLE_LOGGING
    wifiManager.setDebugOutput(true); // Turn on Debug (default off)
#else
    wifiManager.setDebugOutput(false);
//#endif // DEBUG
#endif //DISABLE_LOGGING
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
#ifndef DISABLE_LOGGING
        Log.warning("Timed out trying to connect to AP, resetting" CR);
#endif // DISABLE_LOGGING
        delay(1000);
        ESP.reset();  // Reset and try again
        delay(1000);
    }
    // Connected
    WiFi.hostname(HOSTNAME);
#ifndef DISABLE_LOGGING
    Log.notice("WiFi connected." CR);
    String host = WiFi.hostname();
    Log.notice("Hostname: %s" CR,  &host);
    IPAddress myIP = WiFi.localIP();
    Log.notice("IP address: %d.%d.%d.%d" CR, myIP[0], myIP[1], myIP[2], myIP[3]);
#endif // DISABLE_LOGGING

    // Save custom configuration
    // {parameter} = {parameter_name}.getValue(); // Read and save custom parameters

    // Set custom IP for Station
    // wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
}
