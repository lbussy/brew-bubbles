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

#ifndef _MAIN_H
#define _MAIN_H

#include "serial.h"
#include "config.h"
#include "jsonconfig.h"
#include "ntphandler.h"
#include "webserver.h"
#include "wifi.h"
#include "version.h"
#include "targethandler.h"
#include "bubbles.h"
#include <WiFiManager.h>
#include <DoubleResetDetect.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// DRD_TIMEOUT =    Maximum number of seconds between resets that counts
//                  as a double reset
// DRD_ADDRESS =    Address to the block in the RTC user memory change it
//                  if it collides with another usage of the address block
//
#define DRD_TIMEOUT 2.0
#define DRD_ADDRESS 0x00

// DEBUG mDNS issues
#define DEBUG_ESP_MDNS_ERR

void updateBubbles();
void heapPrint();

#endif // _MAIN_H
