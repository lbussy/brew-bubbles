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

#ifndef _URLTARGET_H
#define _URLTARGET_H

#include "pushtarget.h"
#include "jsonconfig.h"
// #include "bubbles.h"
#include <LCBUrl.h>
// #include <WiFiClient.h>
// #include <ArduinoLog.h>
#include <Arduino.h>

class URLTarget {
    private:
        // Singleton Declarations
        URLTarget() {}
        static URLTarget *single;
        // External Declarations
        PushTarget *target;
        JsonConfig *config;
        // Private Methods

        // Private Properties


    public:
        // Singleton Declarations
        static URLTarget* getInstance();
        ~URLTarget() {single = NULL;}
        // External Declarations

        // Public Methods
        bool push();
        // Public Properties

};

#endif // _URLTARGET_H
