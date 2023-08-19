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

#include "webserver.h"
#include "resetreasons.h"
#include "wifihandler.h"
#include "execota.h"
#include "version.h"
#include "config.h"
#include "thatVersion.h"
#include "pushhelper.h"
#include "tools.h"
#include "uptime.h"
#include "mdns.h"
#include <SPIFFSEditor.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>

AsyncWebServer server(HTTPPORT);

enum BB_METHODS
{
    BB_HTTP_GET = 0b00000001,
    BB_HTTP_POST = 0b00000010,
    BB_HTTP_DELETE = 0b00000100,
    BB_HTTP_PUT = 0b00001000,
    BB_HTTP_PATCH = 0b00010000,
    BB_HTTP_HEAD = 0b00100000,
    BB_HTTP_OPTIONS = 0b01000000,
    BB_HTTP_ANY = 0b01111111,
};

void initWebServer()
{
    setRegPageAliases();
    setActionPageHandlers();
    setJsonHandlers();
    setSettingsAliases();
    setEditor();

    // File not found handler

    server.onNotFound([](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving 404." CR));
        request->send(404, F("text/plain"), F("404: File not found."));
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), HTTPPORT);
    Log.verbose(F("Open: http://%s.local to view controller application." CR), WiFi.hostname().c_str());
}

void setRegPageAliases()
{
    // Regular page aliases

    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/index.htm", LittleFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/about/", LittleFS, "/").setDefaultFile("about.htm").setCacheControl("max-age=600");
    server.serveStatic("/help/", LittleFS, "/").setDefaultFile("help.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota/", LittleFS, "/").setDefaultFile("ota.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota2/", LittleFS, "/").setDefaultFile("ota2.htm").setCacheControl("max-age=600");
    server.serveStatic("/settings/", LittleFS, "/").setDefaultFile("settings.htm").setCacheControl("max-age=600");
    server.serveStatic("/wifi/", LittleFS, "/").setDefaultFile("wifi.htm").setCacheControl("max-age=600");
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/wifi2/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /wifi2/." CR));
        request->send(LittleFS, "/wifi2.htm");
        resetWifi(); // Wipe settings, reset controller
    });

    server.on("/reset/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /reset/." CR));
        // Redirect to Reset page
        request->send(LittleFS, "/reset.htm");
        setDoReset();
    });

    server.on("/otastart/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /otastart/." CR));
        request->send(200, F("text/plain"), F("200: OTA queued."));
        setDoOTA();
    });

    server.on("/clearupdate/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /clearupdate/." CR));
        Log.verbose(F("Clearing any update flags." CR));
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        saveConfig();
        request->send(200, F("text/plain"), F("200: OK."));
    });
}

void setJsonHandlers()
{
    // JSON Handlers

    server.on("/resetreason/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the reset reason json
        Log.verbose(F("Sending /resetreason/." CR));

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> doc;
        JsonObject r = doc.createNestedObject("r");

        rst_info *_reset = ESP.getResetInfoPtr();
        unsigned int reset = (unsigned int)(*_reset).reason;

        r["reason"] = resetReason[reset];
        r["description"] = resetDescription[reset];

        String resetreason;
        serializeJson(doc, resetreason);
        request->send(200, F("text/plain"), resetreason);
    });

    server.on("/heap/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the heap json
        Log.verbose(F("Sending /heap/." CR));

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
        StaticJsonDocument<capacity> doc;
        JsonObject h = doc.createNestedObject("h");

        uint32_t free;
        uint32_t max;
        uint8_t frag;
        ESP.getHeapStats(&free, &max, &frag);

        h["free"] = free;
        h["max"] = max;
        h["frag"] = frag;

        String heap;
        serializeJson(doc, heap);
        request->send(200, F("text/plain"), heap);
    });

    server.on("/uptime/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the uptime json
        Log.verbose(F("Sending /uptime/." CR));

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5);
        StaticJsonDocument<capacity> doc;
        JsonObject u = doc.createNestedObject("u");

        const int days = uptimeDays();
        const int hours = uptimeHours();
        const int minutes = uptimeMinutes();
        const int seconds = uptimeSeconds();
        //const int millis = uptimeMillis();

        u["days"] = days;
        u["hours"] = hours;
        u["minutes"] = minutes;
        u["seconds"] = seconds;

        String ut = "";
        serializeJson(doc, ut);
        request->send(200, F("text/plain"), ut);
    });

    server.on("/bubble/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the Bubbles json
        Log.verbose(F("Sending /bubble/." CR));

        const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
        StaticJsonDocument<capacity> doc;

        doc[F("api_key")] = F(API_KEY);
        doc[F("device_source")] = F(SOURCE);
        doc[F("name")] = config.bubble.name;
        doc[F("bpm")] = bubbles.getAvgBpm();
        doc[F("ambient")] = bubbles.getAvgAmbient();
        doc[F("temp")] = bubbles.getAvgVessel();
        if (config.bubble.tempinf == true)
            doc[F("temp_unit")] = F("F");
        else
            doc[F("temp_unit")] = F("C");
        doc[F("datetime")] = bubbles.lastTime;

        String json;
        serializeJsonPretty(doc, json);

        request->send(200, F("application/json"), json);
    });

    server.on("/thisVersion/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thisVersion/." CR));
        StaticJsonDocument<192> doc;

        doc["fw_version"] = fw_version();
        doc["fs_version"] = fs_version();
        doc["branch"] = branch();
        doc["build"] = build();

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/thatVersion/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thatVersion/." CR));
        StaticJsonDocument<96> doc;

        const char *fw_version = thatVersion.fw_version;
        const char *fs_version = thatVersion.fs_version;
        doc["fs_version"] = fw_version;
        doc["fw_version"] = fs_version;

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/config/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the Config json
        Log.verbose(F("Serving /config/." CR));

        // Serialize configuration
        DynamicJsonDocument doc(capacitySerial); // Create doc
        JsonObject root = doc.to<JsonObject>();  // Create JSON object
        config.save(root);                       // Fill the object with current config
        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String

        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });
}

void setSettingsAliases()
{
    server.on("/settings/controller/", BB_HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/controller/." CR));
        if (handleControllerPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/controller/", BB_HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/temperature/", BB_HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/tempcontrol/." CR));
        if (handleTemperaturePost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/temperature/", BB_HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/urltarget/", BB_HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/urltarget/." CR));
        if (handleURLTargetPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/urltarget/", BB_HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/brewersfriendtarget/", BB_HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/brewersfriendtarget/." CR));
        if (handleBrewersFriendTargetPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/brewersfriendtarget/", BB_HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/brewersfriendtarget/." CR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/brewfathertarget/", BB_HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/brewersfriendtarget/." CR));
        if (handleBrewfatherTargetPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/brewfathertarget/", BB_HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/brewersfriendtarget/." CR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/thingspeaktarget/", BB_HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/thingspeaktarget/." CR));
        if (handleThingSpeakTargetPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/thingspeaktarget/", BB_HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/thingspeaktarget/." CR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });
}

bool handleControllerPost(AsyncWebServerRequest *request) // Handle controller settings
{
    bool hostnamechanged = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Controller settings
            //
            if (strcmp(name, "mdnsid") == 0) // Set hostname
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    if (!strcmp(config.hostname, value) == 0)
                    {
                        hostnamechanged = true;
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.hostname, value, sizeof(config.hostname));
                }
            }
            if (strcmp(name, "bubname") == 0) // Set brew bubbles name
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.bubble.name, value, sizeof(config.bubble.name));
                }
            }
        }
        if (hostnamechanged)
        { // We reset hostname, process
            hostnamechanged = false;
#ifdef ESP8266
            wifi_station_set_hostname(config.hostname);
#elif defined ESP32
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, config.hostname);
#endif
            mdnsreset();
            Log.verbose(F("POSTed new mDNSid, reset mDNS stack." CR));
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return false;
    }
}

bool handleTemperaturePost(AsyncWebServerRequest *request) // Handle Temperature post
{
    bool tempinf = config.bubble.tempinf;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Target url settings
            //
            if (strcmp(name, "calroom") == 0) // Set room calibration
            {
                const double val = atof(value);
                if ((val >= -25) || (val <= 25))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.calibrate.room = val;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calvessel") == 0) // Set vessel calibration
            {
                const double val = atof(value);
                if ((val >= -25) || (val <= 25))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.calibrate.vessel = val;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "tempformat") == 0) // Set temperature format
            {
                if (strcmp(value, "celsius") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.bubble.tempinf = false;
                }
                else if (strcmp(value, "fahrenheit") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.bubble.tempinf = true;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (!tempinf == config.bubble.tempinf)
                bubbles.wipeArray(); // Clear temp array out in case we changed format
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return false;
    }
}

bool handleURLTargetPost(AsyncWebServerRequest *request) // Handle URL Target Post
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // URL Target settings
            //
            if (strcmp(name, "urltargeturl") == 0) // Change Target URL
            {
                if (strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied.  Disabling Url Target." CR), name, value);
                    strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
                }
                else if ((strlen(value) < 3) || (strlen(value) > 128))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
                }
            }
            if (strcmp(name, "urlfreq") == 0) // Change Target URL frequency
            {
                if ((atoi(value) < 1) || (atoi(value) > 60))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.urltarget.freq = atoi(value);
                    config.urltarget.update = true;
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return false;
    }
}

bool handleBrewersFriendTargetPost(AsyncWebServerRequest *request) // Handle Brewer's Friend Target Post
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Brewer's Friend target settings
            //
            if (strcmp(name, "brewersfriendkey") == 0) // Change Brewer's Friend key
            {
                if (strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied. Disabling Brewer's Friend target." CR), name, value);
                    strlcpy(config.brewersfriend.key, value, sizeof(config.brewersfriend.key));
                }
                else if ((strlen(value) < 20) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.brewersfriend.key, value, sizeof(config.brewersfriend.key));
                }
            }
            if (strcmp(name, "brewersfriendfreq") == 0) // Change Brewer's Friend frequency
            {
                if ((atoi(value) < 15) || (atoi(value) > 120))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.brewersfriend.freq = atoi(value);
                    config.brewersfriend.update = true;
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return false;
    }
}

bool handleBrewfatherTargetPost(AsyncWebServerRequest *request) // Handle Brewfather Target Post
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Brewer's Friend target settings
            //
            if (strcmp(name, "brewfatherkey") == 0) // Change Brewfather key
            {
                if (strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied. Disabling Brewfather target." CR), name, value);
                    strlcpy(config.brewfather.key, value, sizeof(config.brewfather.key));
                }
                else if ((strlen(value) < 10) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.brewfather.key, value, sizeof(config.brewfather.key));
                }
            }
            if (strcmp(name, "brewfatherfreq") == 0) // Change Brewfather frequency
            {
                if ((atoi(value) < 15) || (atoi(value) > 120))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.brewfather.freq = atoi(value);
                    config.brewfather.update = true;
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return false;
    }
}

bool handleThingSpeakTargetPost(AsyncWebServerRequest *request) // Handle ThingSpeak Target Post
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // ThingSpeak target settings
            //
            if (strcmp(name, "thingspeakchannel") == 0) // Change ThingSpeak channel
            {
                if ((atoi(value) == 0))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied. Disabling ThingSpeak target." CR), name, value);
                    config.thingspeak.channel = atoi(value);
                }
                else if ((atoi(value) < 1000) || (atoi(value) > 9999999999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.thingspeak.channel = atoi(value);
                }
            }
            if (strcmp(name, "thingspeakkey") == 0) // Change    key
            {
                if (strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied. Disabling ThingSpeak target." CR), name, value);
                    strlcpy(config.thingspeak.key, value, sizeof(config.thingspeak.key));
                }
                else if ((strlen(value) < 10) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.thingspeak.key, value, sizeof(config.thingspeak.key));
                }
            }
            if (strcmp(name, "thingspeakfreq") == 0) // Change ThingSpeak frequency
            {
                if ((atoi(value) < 1) || (atoi(value) > 120))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.thingspeak.freq = atoi(value);
                    config.thingspeak.update = true;
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return false;
    }
}

void setEditor()
{
#ifdef SPIFFSEDIT
    // Setup SPIFFS editor
#ifdef ESP32
    server.addHandler(new SPIFFSEditor(SPIFFS, SPIFFSEDITUSER, SPIFFSEDITPW));
#elif defined(ESP8266)
    server.addHandler(new SPIFFSEditor(SPIFFSEDITUSER, SPIFFSEDITPW));
#endif
    server.on("/edit/", BB_HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/edit");
    });
#endif
}

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}
