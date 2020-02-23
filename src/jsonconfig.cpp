/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

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

JsonConfig* JsonConfig::single = NULL;

JsonConfig* JsonConfig::getInstance() {
    if (!single) {
        single = new JsonConfig();
        single->parse(); // True to wipe config.json for testing
    }
    return single;
}

bool JsonConfig::parse() {
    single->updateBFFreq = false;
    single->updateTargetFreq = false;
    const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7) + 481;
    StaticJsonDocument<capacity> doc;

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("Failed to mount SPIFFS." CR));
        return false;
    }

    // Open file for reading
    bool loaded;
    File file = SPIFFS.open(filename, "r");
    // This may fail if the file is missing
    if (!file) {
        Log.error(F("Failed to open configuration file." CR));
        loaded = false;
    } else {
        // Parse the JSON object in the file
        //bool success = deserializeJson(doc, file);
        DeserializationError err = deserializeJson(doc, file);
        if (err) {
            Log.error(F("Failed to deserialize configuration." CR));
            Log.error(err.c_str());         
            loaded = false;
        } else {
            loaded = true;
        }
    }

    if(loaded == false) { // Load defaults

        Log.notice(F("Using default configuration." CR));

        // Set defaults for Access Point Settings Object
        strlcpy(single->ssid, APNAME, sizeof(single->ssid));
        strlcpy(single->appwd, AP_PASSWD, sizeof(single->appwd));
        
        // Set defaults for Hostname Settings Object
        strlcpy(single->hostname, HOSTNAME, sizeof(single->hostname));
        
        // Set defaults for Bubble Settings Object
        strlcpy(single->bubname, BUBNAME, sizeof(single->bubname));
        single->tempinf = TEMPFORMAT;

        // Set defaults for temperature calibration
        single->calAmbient = 0.0;
        single->calVessel = 0.0;
        
        // Set defaults for Target Settings Object
        strlcpy(single->targeturl, "", sizeof(single->targeturl));
        single->targetfreq = URLTARGETFREQ;

        // Set defaults for Brewer's Friend Settings Object
        strlcpy(single->bfkey, "", sizeof(single->bfkey));
        single->bffreq = BFFREQ;

        // Set defaults for SPIFFS OTA update
        single->dospiffs1 = false;
        single->dospiffs2 = false;

        // Set semaphore for OTA update
        single->didupdate = false;

        // We created default configuration, save it
        single->save();

    } else { // Parse from file

        Log.notice(F("Parsing configuration data." CR));

        // Parse Access Point Settings Object
        strlcpy(single->ssid, doc["apconfig"]["ssid"] | APNAME, sizeof(single->ssid));
        strlcpy(single->appwd, doc["apconfig"]["appwd"] | AP_PASSWD, sizeof(single->appwd));

        // Parse Hostname Settings Object
        strlcpy(single->hostname, doc["hostname"] | HOSTNAME, sizeof(single->hostname));

        // Parse Bubble Settings Object
        strlcpy(single->bubname, doc["bubbleconfig"]["name"] | BUBNAME, sizeof(single->bubname));
        single->tempinf = doc["bubbleconfig"]["tempinf"] | TEMPFORMAT;

        // Parse temperature calibration
        single->calAmbient = doc["calibrate"]["room"] | 0.0;
        single->calVessel = doc["calibrate"]["vessel"] | 0.0;

        // Parse Target Settings Object
        strlcpy(single->targeturl, doc["targetconfig"]["targeturl"] | "", sizeof(single->targeturl));
        single->targetfreq = doc["targetconfig"]["freq"] | URLTARGETFREQ;

        // Parse Brewer's Friend Settings Object
        strlcpy(single->bfkey, doc["bfconfig"]["bfkey"] | "", sizeof(single->bfkey));
        single->bffreq = doc["bfconfig"]["freq"] | BFFREQ;

        // Parse SPIFFS OTA update choice
        single->dospiffs1 = doc["dospiffs1"] | false;
        single->dospiffs2 = doc["dospiffs2"] | false;

        // Parse semaphore for OTA update choice
        single->didupdate = doc["didupdate"] | false;
    }
    return true;
}

bool JsonConfig::save() {
    const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7) + 481;
    StaticJsonDocument<capacity> doc;

    // Serialize Access Point Settings Object
    JsonObject apconfig = doc.createNestedObject("apconfig");
    apconfig["ssid"] = single->ssid;
    apconfig["appwd"] = single->appwd;

    // Serialize Hostname Settings Object
    doc["hostname"] = single->hostname;

    // Serialize Bubble Settings Object
    JsonObject bubbleconfig = doc.createNestedObject("bubbleconfig");
    bubbleconfig["name"] = single->bubname;
    bubbleconfig["tempinf"] = single->tempinf;

    // Serialize temperature calibration
    JsonObject calibrate = doc.createNestedObject("calibrate");
    calibrate["room"] = single->calAmbient;
    calibrate["vessel"] = single->calVessel;

    // Serialize Target Settings Object
    JsonObject targetconfig = doc.createNestedObject("targetconfig");
    targetconfig["targeturl"] = single->targeturl;
    targetconfig["freq"] = single->targetfreq;

    // Serialize Brewer's Friend Settings Object
    JsonObject bfconfig = doc.createNestedObject("bfconfig");
    bfconfig["bfkey"] = single->bfkey;
    bfconfig["freq"] = single->bffreq;

    // Serialize SPIFFS OTA update choice
    doc["dospiffs1"] = single->dospiffs1;
    doc["dospiffs2"] = single->dospiffs2;

    // Parse semaphore for OTA update choice
    doc["didupdate"] = single->didupdate;

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("Failed to mount SPIFFS." CR));
        return false;
    }

    // Open file for writing
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Log.error(F("Failed to open configuration file." CR));
        return false;
    } else {
        // Serialize the JSON object to the file
        bool success = serializeJson(doc, file);
        // This may fail if the JSON is invalid
        if (!success) {
            Log.error(F("Failed to serialize configuration." CR));
            return false;
        } else {
            Log.verbose(F("Saved configuration as %s." CR), filename);
            return true;
        }
    }
}
