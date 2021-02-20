/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#ifndef _WIFIHANDLER_H
#define _WIFIHANDLER_H

#define WM_ASYNC

#include "config.h"
#include "jsonconfig.h"
#include "tools.h"
#include "mdns.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif
#include <AsyncWiFiManager.h>

#include <Ticker.h>
#include <ArduinoLog.h>

void wifiBlinker();
void doWiFi();
void doWiFi(bool);
void resetWifi();

// WiFiManager Callbacks
void apCallback(AsyncWiFiManager *myWiFiManager);
void configResetCallback();
void preSaveConfigCallback();
void saveConfigCallback();
void saveParamsCallback();
void webServerCallback();

extern struct Config config;
extern const size_t capacitySerial;
extern const size_t capacityDeserial;

struct tcp_pcb;
extern struct tcp_pcb *tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb *pcb);
void tcpCleanup(void);

#endif // _WIFIHANDLER_H
