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

#ifndef _NTPHANDLER_H
#define _NTPHANDLER_H

#include "config.h"
#include <TimeLib.h>
#include <NtpClientLib.h>
#include <ArduinoLog.h>

#define NTP_TIMEOUT 1500

class NtpHandler {
    private:
        // Singleton Declarations
        static bool instanceFlag;
        static NtpHandler *single;
        NtpHandler() {}
        // Other Declarations
        boolean syncEventTriggered;
        NTPSyncEvent_t ntpEvent;
        void setup();
        void processSyncEvent(NTPSyncEvent_t);

    public:
        // Singleton Declarations
        static NtpHandler* getInstance();
        ~NtpHandler() {instanceFlag = false;}
        // Other Declarations
        void start();
        void update();
        char* getJsonTime();
        bool hasBeenSet;
};

#endif // _NTPHANDLER_H
