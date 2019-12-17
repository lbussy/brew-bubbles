/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

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

#ifndef _JSONCONFIG_H
#define _JSONCONFIG_H

#include "config.h"
#include <ArduinoLog.h>
#include <FS.h>
#include <ArduinoJson.h>

class JsonConfig {
    private:
        // Singleton Declarations
        JsonConfig() {} 
        static JsonConfig *single;
        // Other Declarations
        const char * filename = "/config.json";

    public:
        // Singleton Declarations
        ~JsonConfig() {single = NULL;}
        static JsonConfig* getInstance();

        // Methods
        bool parse();
        bool save();

        // Other Declarations
        char ssid[33];              // AP SSID
        char appwd[65];             // AP Pwd
        char hostname[33];          // Hostname
        char bubname[33];           // This device
        char targeturl[129];        // Target host
        char bfkey[65];             // Brewer's Friend key
        bool tempinf;               // Is temp in F
        bool dospiffs1;             // Reboot one time before SPIFFS OTA
        bool dospiffs2;             // Update SPIFFS on reboot
        bool updateBFFreq;          // Need to update BF Loop timer
        bool updateTargetFreq;      // Need to update Target Loop timer
        bool didupdate;             // Semaphore to indicate OTA complete
        float calAmbient;           // Calibration for Ambient sensor
        float calVessel;            // Calibration for Vessel sensor
        unsigned long targetfreq;   // Target freqency
        unsigned long bffreq;       // Brewer's Friend frequency
};

#endif // _JSONCONFIG_H
