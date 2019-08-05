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

#include "ota.h"

void otasetup() {
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        if (ArduinoOTA.getCommand() == U_FLASH) {
            char* type = "sketch";
            Log.notice("OTA start updating %s." CR, type);
        } else { // U_FS
            char* type = "filesystem";
            Log.notice("OTA start updating %s." CR, type);
        }
        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    });

    ArduinoOTA.onEnd([]() {
        Log.notice("OTA update complete." CR);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Log.notice("OTA progress: %u%%" CR, (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Log.error("OTA Error[%u]: " CR, error);
        if (error == OTA_AUTH_ERROR) {
            Log.error("OTA authorization failed." CR);
        } else if (error == OTA_BEGIN_ERROR) {
            Log.error("OTA begin failed." CR);
        } else if (error == OTA_CONNECT_ERROR) {
            Log.error("OTA connect failed." CR);
        } else if (error == OTA_RECEIVE_ERROR) {
            Log.error("OTA receive failed." CR);
        } else if (error == OTA_END_ERROR) {
            Log.error("OTA end failed." CR);
        }
        ESP.restart();
    });

    ArduinoOTA.begin();
}
