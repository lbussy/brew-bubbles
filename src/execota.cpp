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

#include "execota.h"

const char firmwareUrlBase[33] = "http://firmware.brewbubbles.com/";
const char spiffsUrlBase[31] = "http://spiffs.brewbubbles.com/";

void execfw() {
    t_httpUpdate_return ret = ESPhttpUpdate.update(firmwareUrlBase);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F("HTTP Firmware Update failed error (%d): %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F("HTTP Firmware Update: No updates." CR));
            break;
        
        case HTTP_UPDATE_OK:
            // This is just gere to get rid of a compiler warning, since
            // the system will reset after OTA, we will never hit this.
            break;
    }
}

void execspiffs() {
    t_httpUpdate_return ret = ESPhttpUpdate.update(firmwareUrlBase);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F("HTTP SPIFFS Update failed error (%d): %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F("HTTP SPIFFS Update: No updates." CR));
            break;

        case HTTP_UPDATE_OK:
            // This is just gere to get rid of a compiler warning, since
            // the system will reset after OTA, we will never hit this.
            break;
    }
}
