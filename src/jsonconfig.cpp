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
    if(! instanceFlag)
    {
        single = new JsonConfig();
        instanceFlag = true;
        single->Parse(true);
        return single;
    }
    else
    {
        return single;
    }
}

bool JsonConfig::Parse(bool reset = false) {
    const size_t capacity = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + 500;
    DynamicJsonDocument doc(capacity);

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error("Failed to mount SPIFFS." CR);
        return false;
    }

    if (reset == true) {
        Log.verbose("Deleted %s." CR, filename);
        SPIFFS.remove(filename); // DEBUG use
    }

    // Open file for reading
    bool loaded;
    File file = SPIFFS.open(filename, "r");
    // This may fail if the file is missing
    if (!file) {
        Log.error("Failed to open configuration file." CR);
        loaded = false;
    } else {
        // Parse the JSON object in the file
        //bool success = deserializeJson(doc, file);
        DeserializationError err = deserializeJson(doc, file);
        if (err) {
            Log.notice("Failed to deserialize configuration." CR);
            Serial.println(err.c_str());         
            loaded = false;
        } else {
            loaded = true;
        }
    }

    if(loaded == false){

        Log.notice("Using default configuration." CR);

        // Set defaults for Access Point Config Object
        strlcpy(ssid, APNAME, sizeof(ssid));
        strlcpy(appwd, AP_PASSWD, sizeof(appwd));
        
        // Set defaults for Hostname Config Object
        strlcpy(hostname, HOSTNAME, sizeof(hostname));
        
        // Set defaults for Bubble Config Object
        strlcpy(bubname, BUBNAME, sizeof(bubname));
        tempinf = TEMPFORMAT;
        strlcpy(tz, TIMEZONE, sizeof(tz));

        // Set defaults for Target Config Object
        strlcpy(targeturl, TARGETURL, sizeof(targeturl));
        targetfreq = TARGETFREQ;

        // Set defaults for Brewer's Friend Config Object
        strlcpy(bfkey, "", sizeof(bfkey));
        bffreq = BFFREQ;

        // We created default configuration, save it
        Serialize();

    } else {

        Log.notice("Parsing configuration data." CR);

        // Parse Access Point Config Object
        strlcpy(ssid, doc["apconfig"]["ssid"] | "", sizeof(ssid));
        strlcpy(appwd, doc["apconfig"]["appwd"] | "", sizeof(appwd));

        // Parse Hostname Config Object
        strlcpy(hostname, doc["hostname"] | "", sizeof(hostname));

        // Parse Bubble Config Object
        strlcpy(bubname, doc["bubbleconfig"]["name"] | "", sizeof(bubname));
        tempinf = doc["bubbleconfig"]["tempinf"];
        strlcpy(tz, doc["bubbleconfig"]["tz"] | "", sizeof(tz));

        // Parse Target Config Object
        strlcpy(targeturl, doc["targetconfig"]["targeturl"] | "", sizeof(targeturl));
        targetfreq = doc["targetconfig"]["freq"];

        // Parse Brewer's Friend Config Object
        strlcpy(bfkey, doc["bfconfig"]["bfkey"] | "", sizeof(bfkey));
        bffreq = doc["bfconfig"]["freq"];

    }
    return true;
}

bool JsonConfig::Serialize() {
    const size_t capacity = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);

    JsonObject apconfig = doc.createNestedObject("apconfig");
    apconfig["ssid"] = ssid;
    apconfig["appwd"] = appwd;

    doc["hostname"] = hostname;

    JsonObject bubbleconfig = doc.createNestedObject("bubbleconfig");
    bubbleconfig["name"] = bubname;
    bubbleconfig["tempinf"] = tempinf;
    bubbleconfig["tz"] = "EST";

    JsonObject target = doc.createNestedObject("target");
    Log.verbose("Saving target as %s." CR, targeturl);
    target["targeturl"] = targeturl;
    Log.verbose("Saved target as %s." CR, target["targeturl"]);
    target["freq"] = targetfreq;

    JsonObject bfconfig =  doc.createNestedObject("bfconfig");
    bfconfig["bfkey"] = bfkey;
    bfconfig["freq"] = bffreq;

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error("Failed to mount SPIFFS." CR);
        return false;
    }

    // Open file for writing
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Log.error("Failed to open configuration file." CR);
        return false;
    } else {
        // Serialize the JSON object to the file
        bool success = serializeJson(doc, file);
        // This may fail if the JSON is invalid
        if (!success) {
            Log.error("Failed to serialize configuration." CR);
            return false;
        } else {
            Log.notice("Saved configuration as %s." CR, filename);
            return true;
        }
    }
}
