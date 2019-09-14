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

bool JsonConfig::instanceFlag = false;
JsonConfig* JsonConfig::single = NULL;
JsonConfig* JsonConfig::getInstance()
{
    if(! instanceFlag) {
        single = new JsonConfig();
        instanceFlag = true;
        single->Parse(false); // True to wipe config.json for testing
        return single;
    }
    else {
        return single;
    }
}

bool JsonConfig::Parse(bool reset = false) {
    single->updateBFFreq = false;
    single->updateTargetFreq = false;
    //const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7) + 480;
    const size_t capacity = CONFIGJSON;
    StaticJsonDocument<capacity> doc;

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("Failed to mount SPIFFS." CR));
        return false;
    }

    if (reset == true) {
        Log.verbose(F("Deleted %s." CR), filename);
        SPIFFS.remove(filename); // DEBUG use
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

    if(loaded == false){ // Load defaults

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
        strlcpy(single->targeturl, TARGETURL, sizeof(single->targeturl));
        single->targetfreq = TARGETFREQ;

        // Set defaults for Brewer's Friend Settings Object
        strlcpy(single->bfkey, "", sizeof(single->bfkey));
        single->bffreq = BFFREQ;

        // Set defaults for SPIFFS OTA update
        single->dospiffs = false;

        // We created default configuration, save it
        single->Save();

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
        strlcpy(single->targeturl, doc["targetconfig"]["targeturl"] | TARGETURL, sizeof(single->targeturl));
        single->targetfreq = doc["targetconfig"]["freq"] | TARGETFREQ;

        // Parse Brewer's Friend Settings Object
        strlcpy(single->bfkey, doc["bfconfig"]["bfkey"] | "", sizeof(single->bfkey));
        single->bffreq = doc["bfconfig"]["freq"] | BFFREQ;

        // Parse SPIFFS OTA update choice
        single->dospiffs = doc["dospiffs"] | false;
    }
    return true;
}

bool JsonConfig::Save() {
    // const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7);
     const size_t capacity = CONFIGJSON;
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
    doc["dospiffs"] = single->dospiffs;

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
            Log.notice(F("Saved configuration as %s." CR), filename);
            return true;
        }
    }
}

void JsonConfig::CreateSettingsJson() {
    // const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7);
    const size_t capacity = CONFIGJSON;
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
    doc["dospiffs"] = single->dospiffs;

    char output[capacity];
    serializeJson(doc, output, sizeof(output));
    strlcpy(single->Config, output, sizeof(output));
}
