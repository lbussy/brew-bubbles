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

bool wifiFirstConnected = false;

//DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

void onSTAConnected(WiFiEventStationModeConnected ipInfo) {
    Log.notice(F("Connected to %s." CR), ipInfo.ssid.c_str());
}

// Start NTP only after IP network is connected
void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
    Log.notice(F("Received IP: %s" CR), ipInfo.ip.toString().c_str());
    Log.notice(F("Connected: %s" CR), WiFi.status() == WL_CONNECTED ? "Yes" : "No");
    wifiFirstConnected = true;
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
    Log.warning(F("Disconnected from SSID: %s" CR), event_info.ssid.c_str());
    Log.warning(F("Reason: %d" CR), event_info.reason);
    WiFi.reconnect();
}

void setup() {
    //bool rst = drd.detect(); // Check for double-reset
    //serial();
    // _delay(200); // Let pins settle, else detect is inconsistent
    // pinMode(RESETWIFI, INPUT_PULLUP);
    // if (digitalRead(RESETWIFI) == LOW) {
    //     Log.notice(F("%s low, resetting wifi and restarting." CR), RESETWIFI);
    //     //disco_restart(); // DEBUG
    //     wifisetup(false); // DEBUG
    // } else if (ipl() == true) {
    //     Log.verbose(F("DRD: Initial program load." CR));
    // } else if (rst == true) {
    //     Log.notice(F("DRD: Double reset boot, resetting wifi and restarting." CR));
    //     //disco_restart(); // DEBUG
    //     wifisetup(false); // DEBUG
    // } else {
    //     Log.verbose(F("DRD: Normal boot, re-using WiFi values." CR));
    //     wifisetup(false);
    // }
    //wifisetup(false); // TODO // DEBUG

    wifiFirstConnected = false;

    static WiFiEventHandler e1, e2, e3;

    serial();

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Log.verbose(".");
    // }
    // Log.verbose(CR);

    e1 = WiFi.onStationModeGotIP(onSTAGotIP); // As soon WiFi is connected, start NTP Client
    e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
    e3 = WiFi.onStationModeConnected(onSTAConnected);
}

void loop() {
    static int last = 0;
    NtpHandler *time = NtpHandler::getInstance();
    JsonConfig *config = JsonConfig::getInstance();
    WebServer *server = WebServer::getInstance();

    while(true) {
        if (wifiFirstConnected) {
            wifiFirstConnected = false;
            
            MDNS.begin(config->hostname);
            time->start();

            //if (config->dospiffs == true) {execspiffs();} // DEBUG: Remove comment
        }

        if (WiFi.isConnected()) {
            time->update();

            if ((millis() - last) > LOOPTIMER) {
                last = millis();
                if (time->hasBeenSet)
                    Log.verbose(F("%s" CR), time->getJsonTime());
            }

            MDNS.update();
            server->handleLoop();
            //bubble->Update();
            //doTargets();
        }
        yield();
    }
}
