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
        const char * filename = "/config.json";
        static bool instanceFlag;
        static JsonConfig *single;
        JsonConfig() {} 

    public:
        char ssid[33];              // Ap SSID
        char appwd[65];             // Ap Pwd
        char hostname[33];          // Hostname
        char bubname[33];           // This device
        bool tempinf;               // Is temp in F
        char tz[4];                 // Timezone
        char targeturl[129];        // Target host
        unsigned long targetfreq;   // Target freqency
        char bfkey[41];             // Brewer's Friend key
        unsigned long bffreq;       // Brewer's Friend frequency
        bool dospiffs;              // Update SPIFFS on reboot

        // Methods
        bool Parse(bool reset);
        bool Serialize();
        static JsonConfig* getInstance();
        ~JsonConfig() {instanceFlag = false;}
};

#endif // _JSONCONFIG_H
