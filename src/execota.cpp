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

void execfw() {
    const char* firmwareUrlBase = FIRMWAREURL;
    Log.verbose(F("Starting the Firmware OTA pull." CR));
    t_httpUpdate_return ret = ESPhttpUpdate.update(firmwareUrlBase);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F("HTTP Firmware Update failed error (%d): %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F("HTTP Firmware Update: No updates." CR));
            break;
        
        case HTTP_UPDATE_OK:
            // This is just here to get rid of a compiler warning, since
            // the system will reset after OTA, we will never hit this.
            break;
    }
}

void execspiffs() {
    const char* spiffsUrlBase = SPIFFSURL;
    Log.verbose(F("Starting the SPIFFS OTA pull." CR));

    // Reset SPIFFS update flag
    JsonConfig *config = JsonConfig::getInstance();
    config->dospiffs = false;
    config->Serialize();

    t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(spiffsUrlBase);

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
            Log.notice(F("HTTP SPIFFS Update complete, restarting." CR));
            ESP.reset();
            delay(1000);
            break;
    }
}
