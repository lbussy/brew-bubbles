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
    JsonConfig *config = JsonConfig::getInstance();
    Log.notice(F("Starting the Firmware OTA pull, will reboot without notice." CR));

    // Stop web server before OTA update - will restart on reset
    WebServer *server = WebServer::getInstance();
    server->stop();

    // Have to set this here because we have no chance after update
    config->dospiffs1 = true;
    config->dospiffs2 = false;
    config->didupdate = false;
    config->Save();

    ESPhttpUpdate.setLedPin(LED, LOW);
    // "http://www.brewbubbles.com/firmware/firmware.bin"
    t_httpUpdate_return ret = ESPhttpUpdate.update(F(FIRMWAREURL));

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F("HTTP Firmware OTA Update failed error (%d): %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            // Don't allow anything to proceed
            config->dospiffs1 = false;
            config->dospiffs2 = false;
            config->didupdate = false;
            config->Save();
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F("HTTP Firmware OTA Update: No updates." CR));
            // Don't allow anything to proceed
            config->dospiffs1 = false;
            config->dospiffs2 = false;
            config->didupdate = false;
            config->Save();
            break;
        
        case HTTP_UPDATE_OK:
            // We should never actually reach this as the controller
            // resets after OTA
            Log.notice(F("HTTP Firmware OTA Update complete, restarting." CR));
            ESP.restart();
            _delay(1000);
            break;
    }
}

void execspiffs() {
    JsonConfig *config = JsonConfig::getInstance(); // Must instantiate the config to save later
    if (config->dospiffs1) {
        Log.notice(F("Rebooting a second time before SPIFFS OTA pull." CR));
        config->dospiffs1 = false;
        config->dospiffs2 = true;
        config->didupdate = false;
        config->Save();
        // TODO:  Put a delay here to avoid triggering DRD
        ESP.restart();
        _delay(1000);
    } else if (config->dospiffs2) {
        Log.notice(F("Starting the SPIFFS OTA pull." CR));

        // Stop web server before OTA update - will restart on reset
        WebServer *server = WebServer::getInstance();
        server->stop();

        ESPhttpUpdate.setLedPin(LED, LOW);
        // "http://www.brewbubbles.com/firmware/spiffs.bin"
        t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(F(SPIFFSURL));

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Log.error(F("HTTP SPIFFS OTA Update failed error (%d): %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Log.notice(F("HTTP SPIFFS OTA Update: No updates." CR));
                break;

            case HTTP_UPDATE_OK:
                // Reset SPIFFS update flag
                config->dospiffs1 = false;
                config->dospiffs2 = false;
                config->didupdate = true;
                config->Save(); // This not only saves the flags, it (re)saves the whole config after SPIFFS wipes it
                _delay(1000);
                Log.notice(F("HTTP SPIFFS OTA Update complete, restarting." CR));
                ESP.restart();
                _delay(1000);
                break;
        }
    } else {
        Log.verbose(F("No OTA pending." CR));
    }
}
