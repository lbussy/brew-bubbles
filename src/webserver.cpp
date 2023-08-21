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
#include "jsonconfig.h"
#include "bubbles.h"
#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>

extern struct Config config;
extern struct ThatVersion thatVersion;
extern struct Bubbles bubbles;
extern const size_t capacityDeserial;
extern const size_t capacitySerial;
extern const char *resetReason[7];
extern const char *resetDescription[7];

ESP8266WebServer server(HTTPPORT);

void initWebServer()
{
    setRegPageAliases();
    setActionPageHandlers();
    setJsonHandlers();
    setSettingsAliases();

    // File not found handler

    server.onNotFound(handleNotFound);

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), HTTPPORT);
    Log.verbose(F("Open: http://%s.local to view controller application." CR), WiFi.hostname().c_str());
}

void handleNotFound()
{
    Log.verbose(F("Serving 404." CR));
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setRegPageAliases()
{
    // Regular page aliases

    server.on("/", HTTP_ANY, []()
              { loadFromFS("index.html"); });
    server.on("/home/", HTTP_ANY, []()
              { loadFromFS("index.html"); });
    server.on("/about/", HTTP_ANY, []()
              { loadFromFS("about.html"); });
    server.on("/help/", HTTP_ANY, []()
              { loadFromFS("help.html"); });
    server.on("/ota/", HTTP_ANY, []()
              { loadFromFS("ota.html"); });
    server.on("/ota2/", HTTP_ANY, []()
              { loadFromFS("ota2.html"); });
    server.on("/settings/", HTTP_ANY, []()
              { loadFromFS("settings.html"); });
    server.on("/wifi/", HTTP_ANY, []()
              { loadFromFS("wifi.html"); });
}

bool loadFromFS(String path)
{
    String dataType = "text/plain";
    if (path.endsWith("/"))
        path += "index.htm";

    if (path.endsWith(".src"))
        path = path.substring(0, path.lastIndexOf("."));
    else if (path.endsWith(".htm"))
        dataType = "text/html";
    else if (path.endsWith(".css"))
        dataType = "text/css";
    else if (path.endsWith(".js"))
        dataType = "application/javascript";
    else if (path.endsWith(".png"))
        dataType = "image/png";
    else if (path.endsWith(".gif"))
        dataType = "image/gif";
    else if (path.endsWith(".jpg"))
        dataType = "image/jpeg";
    else if (path.endsWith(".ico"))
        dataType = "image/x-icon";
    else if (path.endsWith(".xml"))
        dataType = "text/xml";
    else if (path.endsWith(".pdf"))
        dataType = "application/pdf";
    else if (path.endsWith(".zip"))
        dataType = "application/zip";

    File dataFile = LittleFS.open(path, "r");

    if (!dataFile)
        return false;

    if (server.hasArg("download"))
        dataType = "application/octet-stream";

    if (server.streamFile(dataFile, dataType) != dataFile.size())
    {
        Log.warning(F("Sent less data than expected!" CR));
    }

    dataFile.close();
    return true;
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/wifi2/", HTTP_ANY, []()
              {
        Log.verbose(F("Processing /wifi2/." CR));
        loadFromFS("/wifi2.htm");
        resetWifi(); // Wipe settings, reset controller // TODO:  Make this async 
        });

    server.on("/reset/", HTTP_GET, []()
              {
        Log.verbose(F("Processing /reset/." CR));
        // Redirect to Reset page
        loadFromFS("/reset.htm");
        setDoReset(); });

    server.on("/otastart/", HTTP_GET, []()
              {
        Log.verbose(F("Processing /otastart/." CR));
        server.send(200, F("text/plain"), F("200: OTA queued."));
        setDoOTA(); });

    server.on("/clearupdate/", HTTP_GET, []()
              {
        Log.verbose(F("Processing /clearupdate/." CR));
        Log.verbose(F("Clearing any update flags." CR));
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        saveConfig(); // TODO: Make Async
        server.send(200, F("text/plain"), F("200: OK.")); });
}

void setJsonHandlers()
{
    // JSON Handlers

        server.on("/resetreason/", HTTP_GET, []() {
            // Used to provide the reset reason json
            Log.verbose(F("Sending /resetreason/." CR));

            const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2); // TODO: Size this
            StaticJsonDocument<capacity> doc;
            JsonObject r = doc.createNestedObject("r");

            rst_info *_reset = ESP.getResetInfoPtr();
            unsigned int reset = (unsigned int)(*_reset).reason;

            r["reason"] = resetReason[reset];
            r["description"] = resetDescription[reset];

            String resetreason;
            serializeJson(doc, resetreason);
            server.send(200, F("text/plain"), resetreason);
        });

        server.on("/heap/", HTTP_GET, []() {
            // Used to provide the heap json
            Log.verbose(F("Sending /heap/." CR));

            const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3); // TODO: Size this
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
            server.send(200, F("text/plain"), heap);
        });

        server.on("/uptime/", HTTP_GET, []() {
            // Used to provide the uptime json
            Log.verbose(F("Sending /uptime/." CR));

            const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5); // TODO: Size this
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
            server.send(200, F("text/plain"), ut);
        });

        server.on("/bubble/", HTTP_GET, []() {
            // Used to provide the Bubbles json
            Log.verbose(F("Sending /bubble/." CR));

            const size_t capacity = JSON_OBJECT_SIZE(8) + 210; // TODO: Size this
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

            String bubble;
            serializeJsonPretty(doc, bubble);

            server.send(200, F("text/plain"), bubble);
        });

        server.on("/thisVersion/", HTTP_GET, []() {
            Log.verbose(F("Serving /thisVersion/." CR));
            StaticJsonDocument<256> doc; // TODO: Size this

            doc["fw_version"] = fw_version();
            doc["fs_version"] = fs_version();
            doc["branch"] = branch();
            doc["build"] = build();

            String version;
            serializeJsonPretty(doc, version);
            server.send(200, F("text/plain"), version);
        });

        server.on("/thatVersion/", HTTP_GET, []() {
            Log.verbose(F("Serving /thatVersion/." CR));
            StaticJsonDocument<192> doc; // TODO: Size this

            const char *fw_version = thatVersion.fw_version;
            const char *fs_version = thatVersion.fs_version;
            doc["fs_version"] = fw_version;
            doc["fw_version"] = fs_version;

            String version;
            serializeJsonPretty(doc, version);
            server.send(200, F("text/plain"), version);
        });

        server.on("/config/", HTTP_GET, []() {
            // Used to provide the Config json
            Log.verbose(F("Serving /config/." CR));

            // Serialize configuration
            DynamicJsonDocument doc(capacitySerial); // Create doc
            JsonObject root = doc.to<JsonObject>();  // Create JSON object
            config.save(root);                       // Fill the object with current config
            String json_config;
            serializeJsonPretty(doc, json_config); // Serialize JSON to String

            //request->header("Cache-Control: no-store");
            server.send(200, F("text/plain"), json_config);
        });
}

void setSettingsAliases()
{
    server.on("/settings/controller/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/controller/." CR));
        if (handleControllerPost())
        {
            server.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            server.send(500, F("text/plain"), F("Unable to process data"));
        } });

    server.on("/settings/controller/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CR));
        server.send(405, F("text/plain"), F("Method not allowed.")); });

    server.on("/settings/temperature/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/tempcontrol/." CR));
        if (handleTemperaturePost())
        {
            server.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            server.send(500, F("text/plain"), F("Unable to process data"));
        } });

    server.on("/settings/temperature/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CR));
        server.send(405, F("text/plain"), F("Method not allowed.")); });

    server.on("/settings/urltarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/urltarget/." CR));
        if (handleURLTargetPost())
        {
            server.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            server.send(500, F("text/plain"), F("Unable to process data"));
        } });

    server.on("/settings/urltarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CR));
        server.send(405, F("text/plain"), F("Method not allowed.")); });

    server.on("/settings/brewersfriendtarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/brewersfriendtarget/." CR));
        if (handleBrewersFriendTargetPost())
        {
            server.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            server.send(500, F("text/plain"), F("Unable to process data"));
        } });

    server.on("/settings/brewersfriendtarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/brewersfriendtarget/." CR));
        server.send(405, F("text/plain"), F("Method not allowed.")); });

    server.on("/settings/brewfathertarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/brewersfriendtarget/." CR));
        if (handleBrewfatherTargetPost())
        {
            server.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            server.send(500, F("text/plain"), F("Unable to process data"));
        } });

    server.on("/settings/brewfathertarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/brewersfriendtarget/." CR));
        server.send(405, F("text/plain"), F("Method not allowed.")); });

    server.on("/settings/thingspeaktarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/thingspeaktarget/." CR));
        if (handleThingSpeakTargetPost())
        {
            server.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            server.send(500, F("text/plain"), F("Unable to process data"));
        } });

    server.on("/settings/thingspeaktarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/thingspeaktarget/." CR));
        server.send(405, F("text/plain"), F("Method not allowed.")); });
}

bool handleControllerPost() // Handle Controller settings
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    bool hostnamechanged = false;
    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
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
    if (hostnamechanged) // TODO: Make async
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

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save Controler configuration data." CR));
        return false;
    }
}

bool handleTemperaturePost() // Handle Temperature Post
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    bool tempinf = config.bubble.tempinf;

    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

        // Temperature settings
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

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save temperature configuration data." CR));
        return false;
    }
}

bool handleURLTargetPost() // Handle URL Target Post
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
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

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save URL Target data." CR));
        return false;
    }
}

bool handleBrewersFriendTargetPost() // Handle Brewer's Friend Target Post
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
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

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save Brewer's Friend configuration data." CR));
        return false;
    }
}

bool handleBrewfatherTargetPost() // Handle Brewfather Target Pos
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
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

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save Brewfather configuration data." CR));
        return false;
    }
}

bool handleThingSpeakTargetPost() // Handle ThingSpeak Target Post
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
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

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save ThingSpeak configuration data." CR));
        return false;
    }
}

void stopWebServer()
{
    server.stop();
    Log.notice(F("Web server stopped." CR));
}

bool handleGenericTargetPost() // Handle Generic Post
{
    // TODO: Make sure it is a post
    // TODO: Save only if changed

    // Loop through all parameters
    for (int i = 0; i < server.args(); i++)
    {
        const char *name = server.argName(i).c_str();
        const char *value = server.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

        // Handle target settings: 
        //

    }

    if (saveConfig()) // TODO: Make async
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save Generic configuration data." CR));
        return false;
    }
}
