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

WebServer *server = WebServer::getInstance();
Bubbles *bubble = Bubbles::getInstance();
NtpHandler *ntpTime = NtpHandler::getInstance();

void setup(void)
{
    serial();

    JsonConfig *config = JsonConfig::getInstance();
    wifi_station_set_hostname(config->hostname);

    Log.notice(F("Connecting to %s." CR), WiFi.SSID().c_str());
    if (String(WiFi.SSID()) != String(config->ssid)) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        _delay(500);
        #ifdef LOG_LEVEL
        Serial.print(F("."));
        #endif
    }
    #ifdef LOG_LEVEL
    Serial.println();
    #endif

    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());

    if (!MDNS.begin(config->hostname)) {
        Log.error(F("Error setting up MDNS responder."));
    } else {
        Log.notice(F("mDNS responder started, hostname %s.local." CR), WiFi.hostname().c_str());
    }
    server->initialize(PORT);
    ntpTime->start();
}

void loop(void)
{
    bubble->Update();
    server->handleLoop();
    MDNS.update();
}
