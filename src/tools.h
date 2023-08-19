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

#ifndef _TOOLS_H
#define _TOOLS_H

#include "config.h"
#include "bubbles.h"
#include "brewersfriend.h"
#include "brewfather.h"
#include "thingspeaktarget.h"
#include "target.h"
#include <LittleFS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Arduino.h>

#define EEPROM_ADDRESS 0x00
#define LASTBPM_JSON "lastBpm.json"

void _delay(unsigned long);
void resetController();
void loadBpm();
void saveBpm();
void tickerLoop();
void maintenanceLoop();
void setDoURLTarget();
void setDoBFTarget();
void setDoBrewfTarget();
void setDoTSTarget();
void setDoReset();
void setDoOTA();

static bool __attribute__((unused)) doURLTarget = false;   // Semaphore for Target timer
static bool __attribute__((unused)) doBFTarget = false;    // Semaphore for BF timer
static bool __attribute__((unused)) doBrewfTarget = false; // Semaphore for BRF timer
static bool __attribute__((unused)) doTSTarget = false;    // Semaphore for TS timer
static bool __attribute__((unused)) doReset = false;       // Semaphore for reset
static bool __attribute__((unused)) doOTA = false;         // Semaphore for reset
static bool __attribute__((unused)) doNonBlock = false;    // Semaphore for non-blocking portal

extern struct Bubbles bubbles;
extern volatile bool doBubble;
extern volatile bool blip;

#endif
