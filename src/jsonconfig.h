#ifndef _JSONCONFIG_H
#define _JSONCONFIG_H

#define JC_DEBUG // Control debug printing for JsonConfig

#include "config.h"
#include <ArduinoJson.h>
/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#include <FS.h>

struct ApConfig
{
    // Stores Access Point configuration
    char ssid[32];
    char passphrase[64];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Bubble
{
    // Stores Bubble configuration
    char name[32];
    bool tempinf;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Calibrate
{
    // Stores Temp Probe configuration
    float room;
    float vessel;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct URLTarget
{
    // Stores URL Target configuration
    char url[128];
    int freq;
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct KeyTarget
{
    // Stored Brewer's Friend configuration
    char key[64];
    int freq;
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Config
{
    // Stores the complete configuration
    ApConfig apconfig;
    char hostname[32];
    Bubble bubble;
    Calibrate calibrate;
    URLTarget urltarget;
    KeyTarget brewersfriend;
    bool dospiffs1;
    bool dospiffs2;
    bool didupdate;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

bool loadConfig();
bool saveConfig();
bool loadFile(const char *filename, Config &config);
bool saveFile(const char *filename, const Config &config);
bool printFile(const char *filename);
bool serializeConfig(const Config &config, Print &dst);
bool deserializeConfig(Stream &src, Config &config);

#ifdef JC_DEBUG
    #include <ArduinoLog.h>
    #define DNOT(...)   Log.notice(F("[JsonConfig Debug] in %s(): %s." CR), __func__, __VA_ARGS__);
    #define DERR(...)   Log.error(F("[JsonConfig Debug] in %s(): %s." CR), __func__, __VA_ARGS__);
#else
    #define DNOT(...)
    #define DERR(...)
#endif              // End control debug printing

#endif // _JSONCONFIG_H
