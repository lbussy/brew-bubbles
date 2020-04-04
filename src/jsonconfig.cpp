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

#include "jsonconfig.h"

const char *filename = "/config.json";
Config config;
extern const size_t capacityDeserial = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 690;
extern const size_t capacitySerial = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8);

bool deleteConfig() {
    if (!SPIFFS.begin()) {
        DERR("An error has occurred while mounting SPIFFS");
        return false;
    }
    return SPIFFS.remove(filename);
}

bool loadConfig()
{
    // Manage loading the configuration
    if (!loadFile()) {
        DERR(F("Failed to load configuration"));
        return false;
    } else {
        saveFile();
        DNOT(F("Configuration loaded"));
        return true;
    }
}

bool loadFile()
{
    if (!SPIFFS.begin()) {
        DERR(F("Unable to start SPIFFS"));
        return false;
    }
    // Loads the configuration from a file on SPIFFS
    File file = SPIFFS.open(filename, "r");
    if (!SPIFFS.exists(filename) || !file) {
        DNOT(F("Configuration does not exist, creating blank file"));
    } else {
        DNOT(F("Existing configuration present"));
    }

    if (!deserializeConfig(file)) {
        DERR(F("Failed to deserialize configuration"));
        file.close();
        return false;
    } else {
        DNOT(F("Configuration deserialized"));
        file.close();
        return true;
    }
}

bool saveConfig()
{
    return saveFile();
}

bool saveFile()
{
    // Saves the configuration to a file on SPIFFS
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        DERR(F("Unable to open SPIFFS"));
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeConfig(file)) {
        DERR(F("Unable to serialize JSON"));
        file.close();
        return false;
    }
    DNOT(F("Saved configuration to file"));
    file.close();
    return true;
}

bool deserializeConfig(Stream &src)
{
    // Deserialize configuration
    DynamicJsonDocument doc(capacityDeserial);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err) {
        DNOT(F("No existing configuration"));
        config.load(doc.as<JsonObject>());
        DNOT(F("Loaded default configuration"));
        return true;
    } else {
        DNOT("Configuration exists");
        config.load(doc.as<JsonObject>());
        DNOT(F("Loaded existing configuration"));
        return true;
    }
    // TODO:  Can I return false here somehow?
}

bool serializeConfig(Print &dst)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacitySerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printFile()
{
    // Prints the content of a file to the Serial
    File file = SPIFFS.open(filename, "r");
    if (!file)
        return false;

    while (file.available())
        Serial.print((char)file.read());

    Serial.println();
    file.close();
    return true;
}

bool printConfig(const Config &config)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacitySerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, Serial) > 0;
}

bool mergeConfig(JsonVariantConst src) {
    // Serialize configuration
    DynamicJsonDocument doc(capacitySerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Merge in the configuration
    if (merge(root, src)) {
        // Move new configuration to Config object and save
        config.load(root);
        if (saveConfig()) {
            return true;
        }
    }

    return false;
}

bool merge(JsonVariant dst, JsonVariantConst src)
{
    if (src.is<JsonObject>())
    {
        for (auto kvp : src.as<JsonObject>())
        {
            merge(dst.getOrAddMember(kvp.key()), kvp.value());
        }
    }
    else
    {
        dst.set(src);
    }
    return true;
}

void ApConfig::save(JsonObject obj) const
{
    obj["ssid"] = ssid;
    obj["passphrase"] = passphrase;
}

void ApConfig::load(JsonObjectConst obj)
{
    // Load Access Point configuration
    //
    if (obj["ssid"].isNull()) {
        DNOT("Added default config for SSID");
        strlcpy(ssid, APNAME, sizeof(ssid));
    } else {
        DNOT("SSID JSON object loaded");
        const char* sd = obj["ssid"];
        strlcpy(ssid, sd, sizeof(ssid));
    }

    if (obj["passphrase"].isNull()) {
        DNOT("Added default config for Passphrase");
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase));
    } else {
        DNOT("Passphrase loaded");
        const char* ps = obj["passphrase"];
        strlcpy(passphrase, ps, sizeof(passphrase));
    }

}

void Bubble::save(JsonObject obj) const
{
    obj["name"] = name;
    obj["tempinf"] = tempinf;
}

void Bubble::load(JsonObjectConst obj)
{
    // Load Bubble configuration
    //
    if (obj["name"].isNull()) {
        DNOT("Added default config for Bubname");
        strlcpy(name, BUBNAME, sizeof(name));
    } else {
        DNOT("Bubname JSONloaded");
        const char* nm = obj["name"];
        strlcpy(name, nm, sizeof(name));
    }

    if (obj["tempinf"].isNull()) {
        DNOT("Added default config for Temp in F");
        tempinf = TEMPFORMAT;
    } else {
        DNOT("Temp in F JSON object loaded");
        bool tf = obj["tempinf"];
        tempinf = tf;
    }
}

void Calibrate::save(JsonObject obj) const
{
    obj["room"] = room;
    obj["vessel"] = vessel;
}

void Calibrate::load(JsonObjectConst obj)
{
    // Load Temp Sensor Calibration configuration
    //
    if (obj["room"].isNull()) {
        DNOT("Added default config for Room Cal");
        room = 0.0;
    } else {
        DNOT("Room Cal JSON object loaded");
        bool rc = obj["room"];
        room = rc;
    }

    if (obj["vessel"].isNull()) {
        DNOT("Added default config for Vessel Cal");
        vessel = 0.0;
    } else {
        DNOT("Vessel Cal JSON object loaded");
        float vc = obj["vessel"];
        vessel = vc;
    }
}

void URLTarget::save(JsonObject obj) const
{
    obj["url"] = url;
    obj["freq"] = freq;
    obj["update"] = update;
}

void URLTarget::load(JsonObjectConst obj)
{
    // Load URL Target configuration
    //
    if (obj["url"].isNull()) {
        DNOT("Added default config for URL");
        strlcpy(url, "", sizeof(url));
    } else {
        DNOT("URL JSON object loaded");
        const char* tu = obj["url"];
        strlcpy(url, tu, sizeof(url));
    }

    if (obj["freq"].isNull()) {
        DNOT("Added default config for Frequency");
        freq = 2;
    } else {
        DNOT("Frequency JSON object loaded");
        int f = obj["freq"];
        freq = f;
    }

    if (obj["update"].isNull()) {
        DNOT("Added default config for Update");
        update = false;
    } else {
        DNOT("Update JSON object loaded");
        bool u = obj["update"];
        update = u;
    }
}

void KeyTarget::save(JsonObject obj) const
{
    obj["key"] = key;
    obj["freq"] = freq;
    obj["update"] = update;
}

void KeyTarget::load(JsonObjectConst obj)
{
    // Load Key-type configuration
    //
    if (obj["key"].isNull()) {
        DNOT("Added default config for Key");
        strlcpy(key, "", sizeof(key));
    } else {
        DNOT("Key JSON object loaded");
        const char* k = obj["key"];
        strlcpy(key, k, sizeof(key));
    }

    if (obj["freq"].isNull()) {
        DNOT("Added default config for Frequency");
        freq = 15;
    } else {
        DNOT("Frequency JSON object loaded");
        int f = obj["freq"];
        freq = f;
    }

    if (obj["update"].isNull()) {
        DNOT("Added default config for Update");
        update = false;
    } else {
        DNOT("Update JSON object loaded");
        bool u = obj["update"];
        update = u;
    }
}

void Config::load(JsonObjectConst obj)
{
    // Load all config objects
    //

    apconfig.load(obj["apconfig"]);

    if (obj["hostname"].isNull()) {
        DNOT("Added default config for Hostname");
        strlcpy(hostname, APNAME, sizeof(hostname));
    } else {
        DNOT("Hostname JSON object loaded");
        const char* hn = obj["hostname"];
        strlcpy(hostname, hn, sizeof(hostname));
    }

    bubble.load(obj["bubble"]);
    calibrate.load(obj["calibrate"]);
    urltarget.load(obj["urltarget"]);
    brewersfriend.load(obj["brewersfriend"]);
    brewfather.load(obj["brewfather"]);

    if (obj["dospiffs1"].isNull()) {
        DNOT("Added default config for DoSpiffs1");
        dospiffs1 = false;
    } else {
        DNOT("DoSpiffs1 JSON object loaded");
        dospiffs1 = obj["dospiffs1"];
    }

    if (obj["dospiffs2"].isNull()) {
        DNOT("Added default config for DoSpiffs2");
        dospiffs2 = false;
    } else {
        DNOT("DoSpiffs2 JSON object loaded");
        dospiffs2 = obj["dospiffs2"];
    }

    bool firstrun = obj["didupdate"].isNull();
    if (firstrun) {
        DNOT("Added default config for DidUpdate");
        didupdate = false;
    } else {
        DNOT("DidUpdate JSON object loaded");
        didupdate = obj["didupdate"];
    }
}

void Config::save(JsonObject obj) const
{
    // Add Access Point object
    apconfig.save(obj.createNestedObject("apconfig"));
    // Add Hostname object
    obj["hostname"] = hostname;
    // Add Bubble object
    bubble.save(obj.createNestedObject("bubble"));
    // Add Calibration object
    calibrate.save(obj.createNestedObject("calibrate"));
    // Add Target object
    urltarget.save(obj.createNestedObject("urltarget"));
    // Add Brewer's Friend object
    brewersfriend.save(obj.createNestedObject("brewersfriend"));
    // Add Brewfather object
    brewfather.save(obj.createNestedObject("brewfather"));
    // Add dospiffs1 object
    obj["dospiffs1"] = dospiffs1;
    // Add dospiffs2 object
    obj["dospiffs2"] = dospiffs2;
    // Add dospiffs1 object
    obj["didupdate"] = didupdate;
}
