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

const char __attribute__((unused)) *filename = "/config.json";
Config __attribute__((unused)) config;

bool loadConfig()
{
    // Manage loading the configuration
    if (!loadFile(filename, config)) {
        DERR(F("failed to load configuration"));
        return false;
    } else {
        saveFile(filename, config);
        DNOT(F("configuration loaded"));
        Log.verbose(F("Hostname = %s." CR), config.hostname);
        return true;
    }
}

bool saveConfig() {
    return saveFile(filename, config);
}

bool loadFile(const char *filename, Config &config)
{
    if (!SPIFFS.begin()) {
        DERR(F("unable to start SPIFFS"));
        return false;
    }
    // Loads the configuration from a file on SPIFFS
    File file = SPIFFS.open(filename, "r");
    if (!SPIFFS.exists(filename) || !file) {
        DNOT(F("configuration does not exist, creating blank file"));
    } else {
        DNOT(F("existing configuration present"));
    }

    if (!deserializeConfig(file, config)) {
        DERR(F("failed to deserialize configuration"));
        file.close();
        return false;
    } else {
        DNOT(F("configuration deserialized"));
        file.close();
        return true;
    }
}

bool saveFile(const char *filename, const Config &config)
{
    // Saves the configuration to a file on SPIFFS
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        DERR(F("unable to open SPIFFS"));
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeConfig(config, file)) {
        DERR(F("unable to serialize JSON"));
        file.close();
        return false;
    }
    DNOT(F("saved configuration to file"));
    file.close();
    return true;
}

bool printFile(const char *filename)
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

bool serializeConfig(const Config &config, Print &dst)
{
    // Serialize configuration
    const size_t capacity = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8);
    DynamicJsonDocument doc(capacity);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool deserializeConfig(Stream &src, Config &config)
{
    // Deserialize configuration
    const size_t capacity = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 690;
    DynamicJsonDocument doc(capacity);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);
    if (err) {
        DNOT(F("no existing configuration"));
        config.load(doc.as<JsonObject>());
        DNOT(F("loaded default configuration"));
        return true;
    } else {
        config.load(doc.as<JsonObject>());
        DNOT(F("loaded existing configuration"));
        return true;        
    }
    // TODO:  Can I return false here somehow?
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
    // Load SSID config, check for null
    const char* sd = obj["ssid"];
    if (sd)
        strlcpy(ssid, sd, sizeof(ssid));
    else
        strlcpy(ssid, APNAME, sizeof(ssid)); // Default
    // Load SSID config, check for null
    const char* ps = obj["passphrase"];
    if (ps)
        strlcpy(passphrase, ps, sizeof(passphrase));
    else
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase)); // Default
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
    // Load SSID config, check for null
    const char* nm = obj["name"];
    if (nm)
        strlcpy(name, nm, sizeof(name));
    else
        strlcpy(name, BUBNAME, sizeof(name)); // Default
    // Load Temp Format config, check for null
    JsonVariantConst tf = obj["tempinf"];
    if (!tf.isNull())
        tempinf = tf;
    else
        tempinf = TEMPFORMAT; // Default
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
    // Load Room Sensor Calibration configuration
    JsonVariantConst rc = obj["room"];
    if (rc.isNull())
        room = 0.0;
    else
        room = obj["room"];;
    // Load Room Sensor Calibration configuration
    JsonVariantConst vc = obj["vessel"];
    if (vc.isNull())
        vessel = 0.0;
    else
        vessel = obj["vessel"];
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
    // Load Target URL config, check for null
    const char* tu = obj["url"];
    if (tu)
        strlcpy(url, tu, sizeof(url));
    else
        strlcpy(url, "", sizeof(url)); // Default
    // Load Target URL Frequency configuration
    JsonVariantConst f = obj["freq"];
    if (f.isNull())
        freq = 2;
    else
        freq = obj["freq"];;
    // Load Update semaphore configuration
    JsonVariantConst u = obj["update"];
    if (u.isNull())
        update = false;
    else
        update = obj["update"];;
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
    // Load Key configuration
    const char* k = obj["key"];
    if (k)
        strlcpy(key, k, sizeof(key));
    else
        strlcpy(key, "", sizeof(key)); // Default
    // Load Key-type Frequency configuration
    JsonVariantConst f = obj["freq"];
    if (f.isNull())
        freq = 15;
    else
        freq = obj["freq"];;
    // Load Update semaphore configuration
    JsonVariantConst u = obj["update"];
    if (u.isNull())
        update = false;
    else
        update = obj["update"];;
}

void Config::load(JsonObjectConst obj)
{
    // Load all config objects
    //
    // Load Access Point config object
    apconfig.load(obj["apconfig"]);
    // Load Hostname config, check for null
    const char* hn = obj["hostname"];
    if (hn)
        strlcpy(hostname, hn, sizeof(hostname));
    else
        strlcpy(hostname, HOSTNAME, sizeof(hostname)); // Default
    // Load Bubble config object
    bubble.load(obj["bubble"]);
    // Load Calibration config object
    calibrate.load(obj["calibrate"]);
    // Load Target config object
    urltarget.load(obj["urltarget"]);
    // Load Brewer's Friend config object
    brewersfriend.load(obj["brewersfriend"]);
    // Load Brewfather config object
    brewfather.load(obj["brewfather"]);
    // Load dospiffs1, check for null
    JsonVariantConst ds1 = obj["dospiffs1"];
    if (!ds1.isNull())
        dospiffs1= ds1;
    else
        dospiffs1= false; // Default
    // Load dospiffs2, check for null
    JsonVariantConst ds2 = obj["dospiffs2"];
    if (!ds2.isNull())
        dospiffs2= ds2;
    else
        dospiffs2= false; // Default
    // Load didupdate, check for null
    JsonVariantConst du = obj["didupdate"];
    if (!du.isNull())
        didupdate= du;
    else
        didupdate= false; // Default
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
