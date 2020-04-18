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

#include "bubbles.h"
#include "pushtarget.h"
#include "target.h"
#include "brewfather.h"
#include "brewersfriend.h"
#include <ESP8266WiFi.h>

IPAddress resolveHost(const char hostname[129]);
bool pushToTarget(PushTarget*, IPAddress, int);
void tickerLoop();
void updateLoop();
void setDoURLTarget();
void setDoBFTarget();
void setDoBrewfTarget();
void setDoReset();
extern volatile bool doBubble;
extern struct Bubbles bubbles;

static bool __attribute__((unused)) doURLTarget = false;    // Semaphore for Target timer
static bool __attribute__((unused)) doBFTarget = false;     // Semaphore for BF timer
static bool __attribute__((unused)) doBrewfTarget = false;  // Semaphore for BRF timer
static bool __attribute__((unused)) doReset = false;        // Semahore for reset

#endif // _PUSHHELPER_H
