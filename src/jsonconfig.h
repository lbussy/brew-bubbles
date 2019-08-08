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

#ifndef JSONCONFIG_H
#define JSONCONFIG_H

#include "config.h"
#include <ArduinoLog.h>
#include <FS.h>
#include <ArduinoJson.h>

// Stores access point configuration
struct ApConfig {
    char ssid[33];
    char appwd[65];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Stores wireless configuration
struct WiFiConfig {
    char hostname[33];
    char wifipwd[65];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Stores Brew Bubbles configuration
struct BubbleConfig {
    char name[33];
    bool tempinf;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Stores NTP configuration
struct NTPConfig {
    char ntphost[65];
    char tz[33];
    int freq;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Stores Target configuration
struct TargetConfig {
    char targethost[65];
    char brewfkey[41];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Stores the complete configuration
struct Config {
    ApConfig apconfig;
    WiFiConfig wificonfig;
    BubbleConfig bubbleconfig;
    NTPConfig ntpconfig;
    TargetConfig targetconfig;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

bool serializeConfig(const Config & config, Print & dst);
bool deserializeConfig(Stream & src, Config & config);
bool loadwithdefaults();
bool loadFile(const char * filename, Config & config);
bool saveFile(const char * filename, const Config & config);
bool printFile(const char * filename);

#endif // JSONCONFIG_H
