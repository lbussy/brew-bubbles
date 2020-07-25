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
extern const size_t capacityDeserial = 3*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10) + 680;
extern const size_t capacitySerial = 3*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10);

bool deleteConfigFile() {
    if (!LittleFS.begin()) {
        return false;
    }
    return LittleFS.remove(filename);
}

bool loadConfig()
{
    // Manage loading the configuration
    if (!loadFile()) {
        return false;
    } else {
        saveFile();
        return true;
    }
}

bool loadFile()
{
    if (!LittleFS.begin()) {
        return false;
    }
    // Loads the configuration from a file on File System
    File file = LittleFS.open(filename, "r");
    if (!LittleFS.exists(filename) || !file) {
        // File does not exist or unable to read file
    } else {
        // Existing configuration present
    }

    if (!deserializeConfig(file)) {
        file.close();
        return false;
    } else {
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
    // Saves the configuration to a file on File System
    File file = LittleFS.open(filename, "w");
    if (!file) {
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeConfig(file)) {
        file.close();
        return false;
    }
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
        config.load(doc.as<JsonObject>());
        return true;
    } else {
        config.load(doc.as<JsonObject>());
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
    config.save(root); // TODO:  I think this is why it's not working right, it's reloading the config

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printFile()
{
    // Prints the content of a file to the Serial
    File file = LittleFS.open(filename, "r");
    if (!file)
        return false;

    while (file.available())
        Serial.print((char)file.read());

    Serial.println();
    file.close();
    return true;
}

bool printConfig()
{
    // Serialize configuration
    DynamicJsonDocument doc(capacitySerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    bool retval = serializeJsonPretty(doc, Serial) > 0;
    Serial.println();
    return retval;
}

bool mergeJsonString(String newJson)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityDeserial);

    // Parse directly from file
    DeserializationError err = deserializeJson(doc, newJson);
    if (err)
        Serial.println(err.c_str());

    return mergeJsonObject(doc);
}

bool mergeJsonObject(JsonVariantConst src)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityDeserial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Merge in the configuration
    if (merge(root, src))
    {
        // Move new object to config
        config.load(root);
        saveFile();
        return true;
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
        strlcpy(ssid, APNAME, sizeof(ssid));
    } else {
        const char* sd = obj["ssid"];
        strlcpy(ssid, sd, sizeof(ssid));
    }

    if (obj["passphrase"].isNull()) {
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase));
    } else {
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
        strlcpy(name, BUBNAME, sizeof(name));
    } else {
        const char* nm = obj["name"];
        strlcpy(name, nm, sizeof(name));
    }

    if (obj["tempinf"].isNull()) {
        tempinf = TEMPFORMAT;
    } else {
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
        room = 0.0;
    } else {
        float rc = obj["room"];
        room = rc;
    }

    if (obj["vessel"].isNull()) {
        vessel = 0.0;
    } else {
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
        strlcpy(url, "", sizeof(url));
    } else {
        const char* tu = obj["url"];
        strlcpy(url, tu, sizeof(url));
    }

    if (obj["freq"].isNull()) {
        freq = 2;
    } else {
        int f = obj["freq"];
        freq = f;
    }

    if (obj["update"].isNull()) {
        update = false;
    } else {
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
        strlcpy(key, "", sizeof(key));
    } else {
        const char* k = obj["key"];
        strlcpy(key, k, sizeof(key));
    }

    if (obj["freq"].isNull()) {
        freq = 15;
    } else {
        int f = obj["freq"];
        freq = f;
    }

    if (obj["update"].isNull()) {
        update = false;
    } else {
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
        strlcpy(hostname, APNAME, sizeof(hostname));
    } else {
        const char* hn = obj["hostname"];
        strlcpy(hostname, hn, sizeof(hostname));
    }

    bubble.load(obj["bubble"]);
    calibrate.load(obj["calibrate"]);
    urltarget.load(obj["urltarget"]);
    brewersfriend.load(obj["brewersfriend"]);
    brewfather.load(obj["brewfather"]);

    if (obj["dospiffs1"].isNull()) {
        dospiffs1 = false;
    } else {
        dospiffs1 = obj["dospiffs1"];
    }

    if (obj["dospiffs2"].isNull()) {
        dospiffs2 = false;
    } else {
        dospiffs2 = obj["dospiffs2"];
    }

    bool firstrun = obj["didupdate"].isNull();
    if (firstrun) {
        didupdate = false;
    } else {
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
