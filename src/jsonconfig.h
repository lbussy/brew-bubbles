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
        // Other Declarations
        bool parse();
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

        // Methods
        bool save();
};

#endif // _JSONCONFIG_H
