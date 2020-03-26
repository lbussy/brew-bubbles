/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#include "serial_setup.h"
#include "config.h"
#include "execota.h"
#include "jsonconfig.h"
#include "webserver.h"
#include "wifi.h"
#include "version.h"
#include "pushtarget.h"
#include "target.h"
#include "bftarget.h"
#include "brewftarget.h"
#include "pushhelper.h"
#include "bubbles.h"
#include "mdns.h"
#include "ntp.h"
#include "thatVersion.h"
#include <DoubleResetDetect.h>
#include <ArduinoLog.h>
#include <Arduino.h>

// DRD_TIMEOUT =    Maximum number of seconds between resets that counts
//                  as a double reset
// DRD_ADDRESS =    Address to the block in the RTC user memory change it
//                  if it collides with another usage of the address block
//
#define DRD_TIMEOUT 3.0
#define DRD_ADDRESS 0x00

extern struct Config config;
extern bool loadConfig();
extern const char *filename;
extern struct ThatVersion thatVersion;

#endif // _MAIN_H
