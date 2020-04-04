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

#ifndef _PUSHHELPER_H
#define _PUSHHELPER_H

#define PH_DEBUG // Control debug printing for Puch Helper (uses ArduinoLog)

#include "bubbles.h"
#include "pushtarget.h"
#include "target.h"
#include "bftarget.h"
#include "brewftarget.h"
#include <ESP8266WiFi.h>

IPAddress resolveHost(const char hostname[129]);
bool pushToTarget(PushTarget*, IPAddress, int);
void tickerLoop();
void updateLoop();
void setDoURLTarget();
void setDoBFTarget();
void setDoBrewfTarget();
extern volatile bool dobubble;
extern struct Bubbles bubbles;

#ifdef PH_DEBUG
    #include <ArduinoLog.h>
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
    #define DVER(...)   Log.verbose(F("[%s] in %s(): %s." CR), __FILENAME__, __func__, __VA_ARGS__);
    #define DNOT(...)   Log.notice(F("[%s] in %s(): %s." CR), __FILENAME__, __func__, __VA_ARGS__);
    #define DERR(...)   Log.error(F("[%s] in %s(): %s." CR), __FILENAME__, __func__, __VA_ARGS__);
#else
    #define DVER(...)
    #define DNOT(...)
    #define DERR(...)
#endif // End control debug printing

static bool __attribute__((unused)) doURLTarget = false;    // Semaphore for Target timer
static bool __attribute__((unused)) doBFTarget = false;     // Semaphore for BF timer
static bool __attribute__((unused)) doBrewfTarget = false;    // Semaphore for BRF timer

#endif // _PUSHHELPER_H
