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
    JsonConfig *config;
    config = JsonConfig::getInstance();
    // Set host name
    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(config->hostname);

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Set OTA password
    // ArduinoOTA.setPassword("esp8266");
    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    ArduinoOTA.setPasswordHash("5f4dcc3b5aa765d61d8327deb882cf99");

    ArduinoOTA.onStart([]() {
        Log.notice(F("OTA started." CR));
    });

    ArduinoOTA.onEnd([]() {
        Log.notice(F("OTA complete." CR));
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Log.notice(F("Progress: %u%%\r"), (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Log.error(F("Error[%u]: " CR), error);
        if (error == OTA_AUTH_ERROR) Log.error(F("OTA auth failed." CR));
        else if (error == OTA_BEGIN_ERROR) Log.error(F("OTA begin failed." CR));
        else if (error == OTA_CONNECT_ERROR) Log.error(F("OTA connect failed." CR));
        else if (error == OTA_RECEIVE_ERROR) Log.error(F("OTA receive failed." CR));
        else if (error == OTA_END_ERROR) Log.error(F("OTA end failed." CR));
        ESP.restart();
    });

    ArduinoOTA.begin();
    Log.notice(F("OTA ready." CR));
}

