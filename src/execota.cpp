/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

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

#include "execota.h"

#include "config.h"
#include "tools.h"
#include "webserver.h"
#include "version.h"
#include "wifihandler.h"
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoLog.h>

#if LWIP_VERSION_MAJOR == 2
#warning "Remember: You are using lwIP v2.x and this causes filesystem OTA to act weird."
#endif

void execfw()
{
    Log.notice(F("Starting the Firmware OTA pull, will reboot without notice." LF));

    // Stop web server before OTA update - will restart on reset
    stopWebServer();

    // Have to set this here because we have no chance after update
    config.dospiffs1 = true;
    config.dospiffs2 = false;
    config.didupdate = false;
    saveConfig();

    ESPhttpUpdate.setLedPin(LED, LOW);
    WiFiClient _client;
    char url[128];
#ifdef DOBETA
    strcpy(url, UPGRADEURL);
#else
    strcpy(url, UPGRADEURL);
#endif
    strcat(url, "/");
    strcat(url, board());
    strcat(url, "_firmware.bin");
    Log.verbose(F("Pulling Firmware from: %s" LF), url);
    config.nodrd = true;
    saveConfig();
    t_httpUpdate_return ret = ESPhttpUpdate.update(_client, url, "0");

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Log.error(F("HTTP Firmware OTA Update failed error (%d): %s" LF), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        // Don't allow anything to proceed
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        config.nodrd = true;
        saveConfig();
        _delay(100);
        ESP.restart();
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Log.notice(F("HTTP Firmware OTA Update: No updates." LF));
        // Don't allow anything to proceed
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        config.nodrd = true;
        saveConfig();
        _delay(100);
        ESP.restart();
        break;

    case HTTP_UPDATE_OK:
        // We should never actually reach this as the controller
        // resets after OTA
        Log.notice(F("HTTP Firmware OTA Update complete, restarting." LF));
        config.nodrd = true;
        saveConfig();
        _delay(100);
        ESP.restart();
        break;
    }
}

void execspiffs()
{
    if (config.dospiffs1)
    {
        Log.notice(F("Rebooting a second time before File System OTA pull." LF));
        config.dospiffs1 = false;
        config.dospiffs2 = true;
        config.didupdate = false;
        config.nodrd = true;
        saveConfig();
        _delay(100);
        ESP.restart();
    }
    else if (config.dospiffs2)
    {
        Log.notice(F("Starting the File System OTA pull." LF));

        // Stop web server before OTA update - will restart on reset
        stopWebServer();

        ESPhttpUpdate.setLedPin(LED, LOW);
        // "http://www.brewbubbles.com/firmware/spiffs.bin"
        WiFiClient client;
        char url[128];
        strcpy(url, UPGRADEURL);
        strcat(url, "/");
        strcat(url, board());
        strcat(url, "_littlefs.bin");
        Log.verbose(F("Pulling Filesystem from: %s" LF), url);
        t_httpUpdate_return ret = ESPhttpUpdate.updateFS(client, url, "");

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            Log.error(F("HTTP File System OTA Update failed error (%d): %s" LF), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F("HTTP File System OTA Update: No updates." LF));
            break;

        case HTTP_UPDATE_OK:
            // Reset File System update flag
            config.dospiffs1 = false;
            config.dospiffs2 = false;
            config.didupdate = true;
            config.nodrd = true;
            Log.notice(F("HTTP File System OTA Update complete, restarting." LF));
            saveConfig();  // This not only saves the flags, it (re)saves the whole config after File System wipes it
            _delay(100);
            ESP.restart();
            break;
        }
    }
    else
    {
        Log.verbose(F("No OTA pending." LF));
    }
}
