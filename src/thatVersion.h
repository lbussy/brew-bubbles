/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

#ifndef _THATVERSION_H
#define _THATVERSION_H

// #define TV_DEBUG // Enable Debug for module

#include "config.h"
#include <ArduinoLog.h>
#include <ESPAsyncTCP.h>
#include <asyncHTTPrequest.h>
#include <ArduinoJson.h>
#include <Arduino.h>

struct ThatVersion {
    char version[32] = {'0','.','0','.','0'};
    
    void load(JsonObjectConst);
    void save(JsonObject) const;
};

void doPoll();
void sendRequest();
void requestHandler(void*, asyncHTTPrequest*, int);
bool serializeVersion(const ThatVersion &, Print &);
bool deserializeVersion(const char * &, ThatVersion &);

#ifdef TV_DEBUG
    #include <ArduinoLog.h>
    #define TV_NOT(...)   Log.notice(F("[TV Debug] in %s(): %s." CR), __func__, __VA_ARGS__);
    #define TV_ERR(...)   Log.error(F("[TV Debug] in %s(): %s." CR), __func__, __VA_ARGS__);
#else
    #define TV_NOT(...)
    #define TV_ERR(...)
#endif // End control debug printing

#endif // _THATVERSION_H
