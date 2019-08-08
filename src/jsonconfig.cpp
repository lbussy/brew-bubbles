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

#include "jsonconfig.h"

const char * filename = "config.json";
Config config;

void ApConfig::save(JsonObject obj) const {
    obj["ssid"] = ssid;
    obj["appwd"] = appwd;
}

void ApConfig::load(JsonObjectConst obj) {
    strlcpy(ssid, obj["ssid"] | "", sizeof(ssid));
    strlcpy(appwd, obj["appwd"] | "", sizeof(appwd));
}

void WiFiConfig::save(JsonObject obj) const {
    obj["hostname"] = hostname;
    obj["wifipwd"] = wifipwd;
}

void BubbleConfig::load(JsonObjectConst obj) {
    strlcpy(name, obj["name"] | "", sizeof(name));
    tempinf = obj["tempinf"];
}

void BubbleConfig::save(JsonObject obj) const {
    obj["name"] = name;
    obj["tempinf"] = tempinf;
}

void WiFiConfig::load(JsonObjectConst obj) {
    strlcpy(hostname, obj["hostname"] | "", sizeof(hostname));
    strlcpy(wifipwd, obj["wifipwd"] | "", sizeof(wifipwd));
}

void NTPConfig::save(JsonObject obj) const {
    obj["ntphost"] = ntphost;
    obj["tz"] = tz;
    obj["freq"] = freq;
}

void NTPConfig::load(JsonObjectConst obj) {
    strlcpy(ntphost, obj["ntphost"] | "", sizeof(ntphost));
    strlcpy(tz, obj["tz"] | "", sizeof(tz));
    freq = obj["freq"];
}

void TargetConfig::save(JsonObject obj) const {
    obj["targethost"] = targethost;
    obj["brewfkey"] = brewfkey;
}

void TargetConfig::load(JsonObjectConst obj) {
    strlcpy(targethost, obj["targethost"] | "", sizeof(targethost));
    strlcpy(brewfkey, obj["brewfkey"] | "", sizeof(brewfkey));
}

void Config::load(JsonObjectConst obj) {
    // Read "Access Point" object
    apconfig.load(obj["apconfig"]);

    // Read "WAP Config" object
    wificonfig.load(obj["wificonfig"]);

    // Read "Bubble Config" object
    bubbleconfig.load(obj["bubbleconfig"]);

    // Read "NTP Config" object
    ntpconfig.load(obj["ntpconfig"]);

    // Read "Target Config" object
    targetconfig.load(obj["targetconfig"]);
}

void Config::save(JsonObject obj) const {
    // Add "Access Point" object
    apconfig.save(obj.createNestedObject("apconfig"));

    // Add "WAP Config" object
    wificonfig.save(obj.createNestedObject("wificonfig"));

    // Add "Bubble Config" object
    bubbleconfig.save(obj.createNestedObject("bubbleconfig"));

    // Add "NTP Config" object
    ntpconfig.save(obj.createNestedObject("ntpconfig"));

    // Add "Target Config" object
    targetconfig.save(obj.createNestedObject("targetconfig"));
}

bool serializeConfig(const Config & config, Print & dst) {
    const size_t capacity = 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);

    // Create an object at the root
    JsonObject root = doc.to < JsonObject > ();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool deserializeConfig(Stream & src, Config & config) {
    const size_t capacity = 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + 600;
    DynamicJsonDocument doc(capacity);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);
    if (err)
        return false;

    config.load(doc.as < JsonObject > ());
    return true;
}

bool loadwithdefaults() {
    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error("Failed to mount SPIFFS." CR);
        return false;
    }

    // SPIFFS.remove(filename); // DEBUG use

    // Load configuration
    bool loaded = loadFile(filename, config);

    if (!loaded) {
        Log.notice("Using default configuration." CR);
        // Defaults for APConfig Object
        strcpy(config.apconfig.ssid, HOSTNAME);
        strcpy(config.apconfig.appwd, AP_PASSWD);
        // Defaults for WiFiConfig Object
        strcpy(config.wificonfig.hostname, HOSTNAME);
        strcpy(config.wificonfig.wifipwd, "MyPassword");
        // Defaults for BubbleConfig Object
        strcpy(config.bubbleconfig.name, "BrewBubbles");
        config.bubbleconfig.tempinf = TEMPFORMAT;
        // Defaults for NTPConfig Object
        strcpy(config.ntpconfig.ntphost, NTP_SERVER);
        strcpy(config.ntpconfig.tz, TIMEZONE);
        config.ntpconfig.freq = NTP_INTERVAL;
        // Defaults for TargetConfig Object
        strcpy(config.targetconfig.targethost, HTTPTARGET);
        strcpy(config.targetconfig.brewfkey, BFKEY);
    }

    // Save configuration
    saveFile(filename, config);

    // Dump config file
    printFile(filename);

    if (!loaded) {
        Log.notice("Loaded configuration defaults." CR);
    } else {
        Log.notice("Configuration load complete." CR);
    }
    return true;
}

// Loads the configuration from a file on SPIFFS
bool loadFile(const char * filename, Config & config) {
    // Open file for reading
    File file = SPIFFS.open(filename, "r");

    // This may fail if the file is missing
    if (!file) {
        Log.error("Failed to open configuration file." CR);
        return false;
    }

    // Parse the JSON object in the file
    bool success = deserializeConfig(file, config);

    // This may fail if the JSON is invalid
    if (!success) {
        Log.notice("Failed to deserialize configuration." CR);
        return false;
    }

    return true;
}

// Saves the configuration to a file on SPIFFS
bool saveFile(const char * filename,
    const Config & config) {
    // Open file for writing
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Log.error("Failed to create configuration file." CR);
        return false;
    }

    // Serialize JSON to file
    bool success = serializeConfig(config, file);
    if (!success) {
        Log.error("Failed to serialize configuration." CR);
        return false;
    }
    return true;
}

// Prints the content of a file to the Serial
bool printFile(const char * filename) {
    // Open file for reading
    File file = SPIFFS.open(filename, "r");
    if (!file) {
        Log.error("Failed to open configuration file." CR);
        return false;
    }

    // Extract each by one by one
    //while (file.available()) {
        //Log.verbose((char) file.read());
    //}
    //Log.verbose(CR);
    return true;
}
