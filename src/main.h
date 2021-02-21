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

#ifndef _MAIN_H
#define _MAIN_H

// Use LittleFS for DRD storage
#define ESP_DRD_USE_LITTLEFS true
// #define DOUBLERESETDETECTOR_DEBUG true
// Number of seconds to consider as considered a double reset.
#define DRD_TIMEOUT 3
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

#include "serialhandler.h"
#include "config.h"
#include "execota.h"
#include "jsonconfig.h"
#include "webserver.h"
#include "wifihandler.h"
#include "version.h"
#include "pushtarget.h"
#include "target.h"
#include "brewfather.h"
#include "brewersfriend.h"
#include "pushhelper.h"
#include "bubbles.h"
#include "mdns.h"
#include "ntp.h"
#include "thatVersion.h"
#include <ESP_DoubleResetDetector.h>
#include <ArduinoLog.h>
#include <Arduino.h>

extern struct Config config;
extern bool loadConfig();
extern const char *filename;
extern struct ThatVersion thatVersion;
extern struct Bubbles bubbles;
extern volatile bool doBubble;

#endif // _MAIN_H
