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

#ifndef _WIFI_H
#define _WIFI_H

#include "main.h"
#include <ESP8266WiFi.h>        //ESP8266 Core WiFi Library
#include <DNSServer.h>          //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>   //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

void wifisetup(bool reset);
void configModeCallback();
void presentPortal();
void resetWifi();

#endif // _WIFI_H
