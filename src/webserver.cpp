/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

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

#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>

#include "bubbles.h"
#include "version"
#include "config.h"
#include "editfs.h"
#include "jsonconfig.h"
#include "resetreasons.h"
#include "thatVersion.h"
#include "uptime.h"
#include "mdns.h"
#include "wifihandler.h"
#include "version.h"
#include "tools.h"

extern const size_t capacitySerial;
extern const size_t capacityDeserial;
extern struct Config config;

extern FS *fileSystem;
extern bool fsOK;

extern ThatVersion thatVersion;
extern Bubbles bubbles;

ESP8266WebServer webserver(HTTPPORT);

static const char TEXT_PLAIN[] PROGMEM = "text/plain";

////////////////////////////////
// Utils to return HTTP codes, and determine content-type

void replyOK()
{
    webserver.send(200, FPSTR(TEXT_PLAIN), "");
}

void replyOKWithMsg(String msg)
{
    webserver.send(200, FPSTR(TEXT_PLAIN), msg);
}

void replyNotFound(String msg)
{
    webserver.send(404, FPSTR(TEXT_PLAIN), msg);
}

void replyBadRequest(String msg)
{
    Serial.println(msg);
    webserver.send(400, FPSTR(TEXT_PLAIN), msg);
}

void replyServerError(String msg)
{
    Serial.println(msg);
    webserver.send(500, FPSTR(TEXT_PLAIN), msg);
}

/*
   Read the given file from the filesystem and stream it back to the client
*/
bool handleFileRead(String path)
{
    Log.verbose(F("handleFileRead: %s" LF), path.c_str());
    if (!fsOK)
    {
        replyServerError(FPSTR(FS_INIT_ERROR));
        return false;
    }

    if (path.endsWith("/"))
    {
        path += "index.htm";
    }

    if (!fileSystem->exists(path))
    {
        // File not found, try gzip version
        path = path + ".gz";
    }

    if (fileSystem->exists(path))
    {
        String contentType;
        if (webserver.hasArg("download"))
        {
            contentType = F("application/octet-stream");
        }
        else
        {
            contentType = mime::getContentType(path);
        }

        File file = fileSystem->open(path, "r");
        if (webserver.streamFile(file, contentType) != file.size())
        {
            Log.warning(F("Sent less data than expected." LF));
        }
        file.close();
        return true;
    }
    else
    {
        return false;        
    }
}

/*
   The "Not Found" handler catches all URI not explicitly declared in code
   First try to find and return the requested file from the filesystem, and
   if it fails, return a 404 page with debug information
*/
void handleNotFound()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String uri = ESP8266WebServer::urlDecode(webserver.uri()); // Required to read paths with blanks

    // This will load files by explicit name
    if (handleFileRead(uri))
    {
        return;
    }

    // Dump debug data
    String message;
    message.reserve(100);
    message = F("Error: File not found\nURI: ");
    message += uri;
    message += F("\nMethod: ");
    message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
    message += F("\nArguments: ");
    message += webserver.args();
    message += '\n';

    for (uint8_t i = 0; i < webserver.args(); i++)
    {
        message += F("\nName: ");
        message += webserver.argName(i);
        message += F(", Value: ");
        message += webserver.arg(i);
        message += '\n';
    }
    message += "Path=";
    message += webserver.arg("path");
    message += "\n";

    Log.error(F("404: [%s] URI: %s%s not found." LF),
              (webserver.method() == HTTP_GET) ? "GET" : "POST",
              uri.c_str(),
              webserver.arg("path").c_str());

    return replyNotFound(message);
}

void setRegPageAliases()
{
    // Regular page aliases

    webserver.on("/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/index.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/home/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/index.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/about/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/about.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/help/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/help.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/ota/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/ota.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/ota2/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/ota2.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/settings/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/settings.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });

    webserver.on("/wifi/", HTTP_ANY, []()
                 {
        if (handleFileRead(F("/wifi.htm")))
        {
            return;
        }
        replyNotFound(FPSTR(FILE_NOT_FOUND)); });
}

void setJsonHandlers()
{
    // JSON Handlers

    webserver.on("/resetreason/", HTTP_GET, []()
                 {
            // Used to provide the reset reason json
            Log.verbose(F("Sending /resetreason/." LF));

            StaticJsonDocument<192> doc;
            JsonObject r = doc.createNestedObject("r");

            rst_info *_reset = ESP.getResetInfoPtr();
            unsigned int reset = (unsigned int)(*_reset).reason;

            r["reason"] = resetReason[reset];
            r["description"] = resetDescription[reset];

            String resetreason;
            serializeJson(doc, resetreason);
            webserver.send(200, F("application/json"), resetreason); });

    webserver.on("/heap/", HTTP_GET, []()
                 {
            // Used to provide the heap json
            Log.verbose(F("Sending /heap/." LF));

            StaticJsonDocument<64> doc;
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
            webserver.send(200, F("application/json"), heap); });

    webserver.on("/uptime/", HTTP_GET, []()
                 {
            // Used to provide the uptime json
            Log.verbose(F("Sending /uptime/." LF));

            StaticJsonDocument<96> doc;
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
            webserver.send(200, F("application/json"), ut); });

    webserver.on("/bubble/", HTTP_GET, []()
                 {
            // Used to provide the Bubbles json
            Log.verbose(F("Sending /bubble/." LF));

            StaticJsonDocument<384> doc;

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
            serializeJson(doc, bubble);

            webserver.send(200, F("application/json"), bubble); });

    webserver.on("/thisVersion/", HTTP_GET, []()
                 {
            Log.verbose(F("Serving /thisVersion/." LF));
            StaticJsonDocument<192> doc;

            doc["fw_version"] = fw_version();
            doc["fs_version"] = fs_version();
            doc["branch"] = branch();
            doc["build"] = build();

            String version;
            serializeJson(doc, version);
            webserver.send(200, F("application/json"), version); });

    webserver.on("/thatVersion/", HTTP_GET, []()
                 {
            Log.verbose(F("Serving /thatVersion/." LF));
            StaticJsonDocument<192> doc;

            const char *fw_version = thatVersion.fw_version;
            const char *fs_version = thatVersion.fs_version;
            doc["fs_version"] = fw_version;
            doc["fw_version"] = fs_version;

            String version;
            serializeJson(doc, version);
            webserver.send(200, F("application/json"), version); });

    webserver.on("/config/", HTTP_GET, []()
                 {
            // Used to provide the Config json
            Log.verbose(F("Serving /config/." LF));

            // Serialize configuration
            DynamicJsonDocument doc(capacitySerial);    // Create doc
            JsonObject root = doc.to<JsonObject>();     // Create JSON object
            config.save(root);                          // Fill the object with current config
            String json_config;
            serializeJson(doc, json_config);            // Serialize JSON to String

            //request->header("Cache-Control: no-store");
            webserver.send(200, F("application/json"), json_config); });
}

void setSettingsAliases()
{
    webserver.on("/settings/controller/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/controller/." LF));
        if (handleControllerPost())
        {
            webserver.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            webserver.send(500, F("text/plain"), F("Unable to process data"));
        } });

    webserver.on("/settings/controller/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." LF));
        webserver.send(405, F("text/plain"), F("Method not allowed.")); });

    webserver.on("/settings/temperature/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/tempcontrol/." LF));
        if (handleTemperaturePost())
        {
            webserver.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            webserver.send(500, F("text/plain"), F("Unable to process data"));
        } });

    webserver.on("/settings/temperature/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." LF));
        webserver.send(405, F("text/plain"), F("Method not allowed.")); });

    webserver.on("/settings/urltarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/urltarget/." LF));
        if (handleURLTargetPost())
        {
            webserver.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            webserver.send(500, F("text/plain"), F("Unable to process data"));
        } });

    webserver.on("/settings/urltarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." LF));
        webserver.send(405, F("text/plain"), F("Method not allowed.")); });

    webserver.on("/settings/brewersfriendtarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/brewersfriendtarget/." LF));
        if (handleBrewersFriendTargetPost())
        {
            webserver.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            webserver.send(500, F("text/plain"), F("Unable to process data"));
        } });

    webserver.on("/settings/brewersfriendtarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/brewersfriendtarget/." LF));
        webserver.send(405, F("text/plain"), F("Method not allowed.")); });

    webserver.on("/settings/brewfathertarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/brewersfriendtarget/." LF));
        if (handleBrewfatherTargetPost())
        {
            webserver.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            webserver.send(500, F("text/plain"), F("Unable to process data"));
        } });

    webserver.on("/settings/brewfathertarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/brewersfriendtarget/." LF));
        webserver.send(405, F("text/plain"), F("Method not allowed.")); });

    webserver.on("/settings/thingspeaktarget/", HTTP_POST, []()
              {
        Log.verbose(F("Processing post to /settings/thingspeaktarget/." LF));
        if (handleThingSpeakTargetPost())
        {
            webserver.send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            webserver.send(500, F("text/plain"), F("Unable to process data"));
        } });

    webserver.on("/settings/thingspeaktarget/", HTTP_ANY, []()
              {
        Log.verbose(F("Invalid method to /settings/thingspeaktarget/." LF));
        webserver.send(405, F("text/plain"), F("Method not allowed.")); });
}

bool handleControllerPost() // Handle Controller settings
{
    bool hostnamechanged = false;

    // Loop through all parameters
    for (int i = 0; i < webserver.args(); i++)
    {
        const char *name = webserver.argName(i).c_str();
        const char *value = webserver.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." LF), name, value);

        // Controller settings
        //
        if (strcmp(name, "mdnsid") == 0) // Set hostname
        {
            if ((strlen(value) < 3) || (strlen(value) > 32))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not valid." LF), name, value);
            }
            else
            {
                if (!strcmp(config.hostname, value) == 0)
                {
                    hostnamechanged = true;
                }
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                strlcpy(config.hostname, value, sizeof(config.hostname));
            }
        }
        if (strcmp(name, "bubname") == 0) // Set brew bubbles name
        {
            if ((strlen(value) < 3) || (strlen(value) > 32))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not valid." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
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
        Log.verbose(F("POSTed new mDNSid, reset mDNS stack." LF));
    }

    setDoSaveConfig();
    return true;
}

bool handleTemperaturePost() // Handle Temperature Post
{
    bool tempinf = config.bubble.tempinf;

    // Loop through all parameters
    for (int i = 0; i < webserver.args(); i++)
    {
        const char *name = webserver.argName(i).c_str();
        const char *value = webserver.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." LF), name, value);

        // Temperature settings
        //
        if (strcmp(name, "calroom") == 0) // Set room calibration
        {
            const double val = atof(value);
            if ((val >= -25) || (val <= 25))
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.calibrate.room = val;
            }
            else
            {
                Log.warning(F("Settings update error, [%s]:(%s) not valid." LF), name, value);
            }
        }
        if (strcmp(name, "calvessel") == 0) // Set vessel calibration
        {
            const double val = atof(value);
            if ((val >= -25) || (val <= 25))
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.calibrate.vessel = val;
            }
            else
            {
                Log.warning(F("Settings update error, [%s]:(%s) not valid." LF), name, value);
            }
        }
        if (strcmp(name, "tempformat") == 0) // Set temperature format
        {
            if (strcmp(value, "celsius") == 0)
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.bubble.tempinf = false;
            }
            else if (strcmp(value, "fahrenheit") == 0)
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.bubble.tempinf = true;
            }
            else
            {
                Log.warning(F("Settings update error, [%s]:(%s) not valid." LF), name, value);
            }
        }
        if (!tempinf == config.bubble.tempinf)
            bubbles.wipeArray(); // Clear temp array out in case we changed format
    }

    setDoSaveConfig();
    return true;
}

bool handleURLTargetPost() // Handle URL Target Post
{
    // Loop through all parameters
    for (int i = 0; i < webserver.args(); i++)
    {
        const char *name = webserver.argName(i).c_str();
        const char *value = webserver.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." LF), name, value);

        // URL Target settings
        //
        if (strcmp(name, "urltargeturl") == 0) // Change Target URL
        {
            if (strlen(value) == 0)
            {
                Log.notice(F("Settings update, [%s]:(%s) applied.  Disabling Url Target." LF), name, value);
                strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
            }
            else if ((strlen(value) < 3) || (strlen(value) > 128))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
            }
        }
        if (strcmp(name, "urlfreq") == 0) // Change Target URL frequency
        {
            if ((atoi(value) < 1) || (atoi(value) > 60))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.urltarget.freq = atoi(value);
                config.urltarget.update = true;
            }
        }
    }

    setDoSaveConfig();
    return true;
}

bool handleBrewersFriendTargetPost() // Handle Brewer's Friend Target Post
{
    // Loop through all parameters
    for (int i = 0; i < webserver.args(); i++)
    {
        const char *name = webserver.argName(i).c_str();
        const char *value = webserver.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." LF), name, value);

        // Brewer's Friend target settings
        //
        if (strcmp(name, "brewersfriendkey") == 0) // Change Brewer's Friend key
        {
            if (strlen(value) == 0)
            {
                Log.notice(F("Settings update, [%s]:(%s) applied. Disabling Brewer's Friend target." LF), name, value);
                strlcpy(config.brewersfriend.key, value, sizeof(config.brewersfriend.key));
            }
            else if ((strlen(value) < 20) || (strlen(value) > 64))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                strlcpy(config.brewersfriend.key, value, sizeof(config.brewersfriend.key));
            }
        }
        if (strcmp(name, "brewersfriendfreq") == 0) // Change Brewer's Friend frequency
        {
            if ((atoi(value) < 15) || (atoi(value) > 120))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.brewersfriend.freq = atoi(value);
                config.brewersfriend.update = true;
            }
        }
    }

    setDoSaveConfig();
    return true;
}

bool handleBrewfatherTargetPost() // Handle Brewfather Target Pos
{
    // Loop through all parameters
    for (int i = 0; i < webserver.args(); i++)
    {
        const char *name = webserver.argName(i).c_str();
        const char *value = webserver.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." LF), name, value);

        // Brewer's Friend target settings
        //
        if (strcmp(name, "brewfatherkey") == 0) // Change Brewfather key
        {
            if (strlen(value) == 0)
            {
                Log.notice(F("Settings update, [%s]:(%s) applied. Disabling Brewfather target." LF), name, value);
                strlcpy(config.brewfather.key, value, sizeof(config.brewfather.key));
            }
            else if ((strlen(value) < 10) || (strlen(value) > 64))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                strlcpy(config.brewfather.key, value, sizeof(config.brewfather.key));
            }
        }
        if (strcmp(name, "brewfatherfreq") == 0) // Change Brewfather frequency
        {
            if ((atoi(value) < 15) || (atoi(value) > 120))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.brewfather.freq = atoi(value);
                config.brewfather.update = true;
            }
        }
    }

    setDoSaveConfig();
    return true;
}

bool handleThingSpeakTargetPost() // Handle ThingSpeak Target Post
{
    // Loop through all parameters
    for (int i = 0; i < webserver.args(); i++)
    {
        const char *name = webserver.argName(i).c_str();
        const char *value = webserver.arg(i).c_str();
        Log.verbose(F("Processing [%s]:(%s) pair." LF), name, value);

        // ThingSpeak target settings
        //
        if (strcmp(name, "thingspeakchannel") == 0) // Change ThingSpeak channel
        {
            if ((atoi(value) == 0))
            {
                Log.notice(F("Settings update, [%s]:(%s) applied. Disabling ThingSpeak target." LF), name, value);
                config.thingspeak.channel = atoi(value);
            }
            else if ((atoi(value) < 1000) || (atoi(value) > 9999999999))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.thingspeak.channel = atoi(value);
            }
        }
        if (strcmp(name, "thingspeakkey") == 0) // Change    key
        {
            if (strlen(value) == 0)
            {
                Log.notice(F("Settings update, [%s]:(%s) applied. Disabling ThingSpeak target." LF), name, value);
                strlcpy(config.thingspeak.key, value, sizeof(config.thingspeak.key));
            }
            else if ((strlen(value) < 10) || (strlen(value) > 64))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                strlcpy(config.thingspeak.key, value, sizeof(config.thingspeak.key));
            }
        }
        if (strcmp(name, "thingspeakfreq") == 0) // Change ThingSpeak frequency
        {
            if ((atoi(value) < 1) || (atoi(value) > 120))
            {
                Log.warning(F("Settings update error, [%s]:(%s) not applied." LF), name, value);
            }
            else
            {
                Log.notice(F("Settings update, [%s]:(%s) applied." LF), name, value);
                config.thingspeak.freq = atoi(value);
                config.thingspeak.update = true;
            }
        }
    }

    setDoSaveConfig();
    return true;
}

void setActionPageHandlers()
{
    // Action Page Handlers

    webserver.on("/wifi2/", HTTP_ANY, []()
              {
        Log.verbose(F("Processing /wifi2/." LF));
        handleFileRead("/wifi2.htm");
        setDoReset(); // Wipe settings, reset controller
        });

    webserver.on("/reset/", HTTP_GET, []()
              {
        Log.verbose(F("Processing /reset/." LF));
        // Redirect to Reset page
        handleFileRead("/reset.htm");
        setDoReset();
         });

    webserver.on("/otastart/", HTTP_GET, []()
              {
        Log.verbose(F("Processing /otastart/." LF));
        webserver.send(200, F("text/plain"), F("200: OTA queued."));
        setDoOTA();
        });

    webserver.on("/clearupdate/", HTTP_GET, []()
              {
        Log.verbose(F("Processing /clearupdate/." LF));
        Log.verbose(F("Clearing any update flags." LF));
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        setDoSaveConfig();
        webserver.send(200, F("text/plain"), F("200: OK.")); });
}

void startWebServer()
{
    // Standard routes
    setRegPageAliases();

    // Default handler for all URIs not defined above
    // Use it to read files from filesystem
    webserver.onNotFound(handleNotFound);

    // Add FS Editor
    editPagesInit();

    // Add working handlers
    setJsonHandlers();
    setActionPageHandlers();
    setSettingsAliases();

    // Start webserver
    webserver.begin();

    Log.notice(F("HTTP webserver started on port %d." LF), HTTPPORT);
}

void stopWebServer()
{
    webserver.close();
    webserver.stop();
    Log.notice(F("Web webserver stopped." LF));
}
