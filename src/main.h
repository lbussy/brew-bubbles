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
#include <DoubleResetDetect.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

//#include "tools.h"
//#include "execota.h"
//#include "mdns.h"
//#include "bubbles.h"
//#include "wifi.h"
//#include "targethandler.h"
//#include <EEPROM.h>
//#include <WiFiClient.h>
//#include <Arduino.h>

#ifndef _WIFI_CONFIG_H
#define _WIFI_CONFIG_H

#define WIFI_SSID "Bohica"
#define WIFI_PASSWD "idoxlr8!"

#endif // _WIFI_CONFIG_H

// DRD_TIMEOUT =    Maximum number of seconds between resets that counts
//                  as a double reset
// DRD_ADDRESS =    Address to the block in the RTC user memory change it
//                  if it collides with another usage of the address block
// Max time between resets to count as a double 
#define DRD_TIMEOUT 2.0
// Address within RTC memory to contain flag
#define DRD_ADDRESS 0x00
// EEPROM address at which to store the IPL flag.  Edit accordingly.
#define EEPROM_ADDRESS 0x00

// Have to use two levels of macro expansion to convert a symbol to
// a string. See http://gcc.gnu.org/onlinedocs/cpp/Stringification.html

#define stringify(s) _stringifyDo(s)
#define _stringifyDo(s) #s

#endif // _MAIN_H
