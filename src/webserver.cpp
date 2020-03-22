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

#include "webserver.h"

AsyncWebServer server(PORT);

void initWebServer() {
    setRegPageAliases();
    setActionPageHandlers();
    setJsonHandlers();
    setSettingsAliases();
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();
    Log.notice(F("Async HTTP server started on port %l." CR) , PORT);
    Log.verbose(F("Open: http://%s.local to view controller application." CR), WiFi.hostname().c_str());
}

void setRegPageAliases() {
    // Regular page aliases

    server.on("/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/'." CR));
        request->send(SPIFFS, "/index.htm");
    });

    server.on("/index.htm", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/index.htm'." CR));
        request->send(SPIFFS, "/index.htm");
    });

    server.on("/about/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/about.htm'." CR));
        request->send(SPIFFS, "/about.htm");
    });

    server.on("/help/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/help.htm'." CR));
        request->send(SPIFFS, "/help.htm");
    });

    server.on("/ota/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/ota.htm'." CR));
        request->send(SPIFFS, "/ota.htm");
    });

    server.on("/ota2/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/ota2.htm'." CR));
        request->send(SPIFFS, "/ota2.htm");
    });

    server.on("/settings/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/settings.htm'." CR));
        request->send(SPIFFS, "/settings.htm");
    });

    server.on("/wifi/", HTTP_ANY, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving request for '/wifi.htm'." CR));
        request->send(SPIFFS, "/wifi.htm");
    });

    // File not found handler

    server.onNotFound([] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving 404." CR));
        request->send(404, F("text/plain"), F("404: File not found."));
    });
}

void setActionPageHandlers() {
    // Action Page Handlers

    server.on("/heap/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    server.on("/wifi2/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /wifi2/." CR));
        request->send(SPIFFS, "/wifi2.htm");
        resetWifi(); // Wipe settings, reset controller
    });

    server.on("/otastart/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /otastart/." CR));
        request->send(200, F("text/plain"), F("200: OTA started."));
        execfw(); // Trigger the OTA update
    });

    server.on("/clearupdate/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /clearupdate/." CR));
        Log.verbose(F("Clearing any update flags." CR));
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        saveConfig();
        request->send(200, F("text/plain"), F("200: OK."));
    });
}

void setJsonHandlers() {
    // JSON Handlers

    server.on("/bubble/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // Used to provide the Bubbles json
        Log.verbose(F("Sending /bubble/." CR));

        Bubbles *bubble = Bubbles::getInstance();

        //const size_t capacity = JSON_OBJECT_SIZE(8);
        const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
        StaticJsonDocument<capacity> doc;

        doc[F("api_key")] = F(API_KEY);
        doc[F("device_source")] = F(SOURCE);
        doc[F("name")] = config.bubble.name;
        doc[F("bpm")] = bubble->getAvgBpm();
        doc[F("ambient")] = bubble->getAvgAmbient();
        doc[F("temp")] = bubble->getAvgVessel();
        if (config.bubble.tempinf == true)
            doc[F("temp_unit")] = F("F");
        else
            doc[F("temp_unit")] = F("C");
        doc[F("datetime")] = bubble->lastTime;

        String json;
        serializeJsonPretty(doc, json);

        request->send(200, F("application/json"), json);
    });

    server.on("/thisVersion/",  HTTP_GET, [] (AsyncWebServerRequest *request) { 
        Log.verbose(F("Serving /thisVersion/." CR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        doc["version"] = version();

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/thatVersion/", HTTP_GET,[] (AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thatVersion/." CR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        const char * version = thatVersion.version;
        doc["version"] = version;

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/config/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // Used to provide the Config json
        Log.verbose(F("Serving /config/." CR));

        // Serialize configuration
        const size_t capacity = 5 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(9);
        DynamicJsonDocument doc(capacity);

        // Create an object at the root
        JsonObject root = doc.to<JsonObject>();

        // Fill the object
        config.save(root);

        // Serialize JSON to String
        String json;
        serializeJsonPretty(doc, json);

        request->send(200, F("application/json"), json);
    });
}

void setSettingsAliases() {
    // Settings Update Handler

    server.on("/settings/update/", HTTP_POST, [] (AsyncWebServerRequest *request) {
        // Process POST configuration changes
        Log.verbose(F("Processing post to /settings/update/." CR));

        char redirect[66];
        strcpy(redirect, "/settings/");

        if (request->hasParam(F("mdnsID"), true)) { // Change Hostname
            if ((request->arg("mdnsID").length() > 32) || (request->arg("mdnsID").length() < 3)) {
                Log.warning(F("Settings update error." CR));
            } else {
                strlcpy(config.hostname, request->arg("mdnsID").c_str(), sizeof(config.hostname));
                saveConfig();

                // Reset hostname
                wifi_station_set_hostname(config.hostname);
                MDNS.setHostname(config.hostname);
                MDNS.notifyAPChange();
                MDNS.announce();

                // Creeate a full URL for redirection
                char hostname[45];
                strcpy(hostname, "http://");
                strcat(hostname, config.hostname);
                strcat(hostname, ".local");
                strcpy(redirect, hostname);
                strcat(redirect, "/settings/");
                strcat(redirect, "#controller"); // Redirect to Controller box
                Log.verbose(F("POSTed mdnsID, redirecting to %s." CR), redirect);
            }

        } else if (request->hasParam(F("bubname"), true)) { // Change Bubble ID
            if ((request->arg("bubname").length() > 32) || (request->arg("bubname").length() < 3)) {
                Log.warning(F("Settings update error." CR));
            } else {
                strlcpy(config.bubble.name, request->arg("bubname").c_str(), sizeof(config.bubble.name));
                saveConfig();
            }
            strcat(redirect, "#controller"); // Redirect to Controller box
            Log.notice(F("POSTed bubname, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("tempInF"), true)) { // Change Temp in F
            char option[8];
            strcpy(option, request->arg("tempInF").c_str());
            if (strcmp(option, "option0") == 0) {
                config.bubble.tempinf = false;
            } else {
                config.bubble.tempinf = true;
                saveConfig();
            }
            strcat(redirect, "#temp"); // Redirect to Temp Control
            Log.notice(F("POSTed tempInF, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("calRoom"), true)) { // Change Room temp calibration
            if ((request->arg("calRoom").toDouble() < -25) || (request->arg("calRoom").toDouble() > 25)) {
                Log.warning(F("Settings update error." CR));
            } else {
                config.calibrate.room = request->arg("calRoom").toDouble();
                saveConfig();
            }
            strcat(redirect, "#temp"); // Redirect to Temp Control
            Log.notice(F("POSTed calRoom, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("calVessel"), true)) { // Change Vessel temp calibration
            if ((request->arg("calVessel").toDouble() < -25) || (request->arg("calVessel").toDouble() > 25)) {
                Log.warning(F("Settings update error." CR));
            } else {
                config.calibrate.vessel = request->arg("calVessel").toDouble();
                saveConfig();
            }
            strcat(redirect, "#temp"); // Redirect to Temp Control
            Log.notice(F("POSTed calVessel, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("target"), true)) { // Change Target URL
            if (request->arg("target").length() > 128) {
                Log.warning(F("Settings update error." CR));
            } else {
                strlcpy(config.urltarget.url, request->arg("target").c_str(), sizeof(config.urltarget.url));
                saveConfig();
            }
            strcat(redirect, "#target"); // Redirect to Target Control
            Log.notice(F("POSTed target, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("tfreq"), true)) { // Change Vessel temp calibration
            if ((request->arg("tfreq").toInt() < 1) || (request->arg("tfreq").toInt() > 60)) {
                Log.warning(F("Settings update error." CR));
            } else {
                config.urltarget.freq = request->arg("tfreq").toInt();
                config.urltarget.update = true;
                saveConfig();
            }
            strcat(redirect, "#target"); // Redirect to Target Control
            Log.notice(F("POSTed tfreq, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("bfkey"), true)) { // Change Brewer's Friend key
            if ((request->arg("bfkey").length() > 64) || (request->arg("bfkey").length() < 20)) {
                Log.warning(F("Settings update error." CR));
            } else {
                strlcpy(config.brewersfriend.key, request->arg("bfkey").c_str(), sizeof(config.brewersfriend.key));
                saveConfig();
            }
            strcat(redirect, "#bf"); // Redirect to Brewer's Friend Control
            Log.notice(F("POSTed bfkey, redirecting to %s." CR), redirect);

        } else if (request->hasParam(F("bfreq"))) { // Change Vessel temp calibration
            if ((request->arg("bfreq").toInt() < 15) || (request->arg("bfreq").toInt() > 120)) {
                Log.warning(F("Settings update error." CR));
            } else {
                config.brewersfriend.freq = request->arg("bfreq").toInt();
                config.brewersfriend.update = true;
                saveConfig();
            }
            strcat(redirect, "#bf"); // Redirect to Brewer's Friend Control
            Log.notice(F("POSTed bfreq, redirecting to %s." CR), redirect);
        }

        // Redirect to Settings page
        request->redirect(redirect);
    });

    server.on("/config/apply/", HTTP_POST, [] (AsyncWebServerRequest *request) {  // Process JSON POST configuration changes
        Log.verbose(F("Processing post to /config/apply/." CR));
        String input = request->arg(F("plain"));
        const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7);
        StaticJsonDocument<capacity> doc;
        DeserializationError err = deserializeJson(doc, input);
        if (!err) {
            bool updated = false;

            // Parse JSON

            // TODO:  Use NULL checks from jsonconfig
            // TODO:  Can I use jsonconfig to handle this?

            // Parse Access Point Settings Object
            const char* ssid = doc["apconfig"]["ssid"];
            if ((ssid) && (strcmp (ssid, config.apconfig.ssid) != 0)) {
                updated = true;
                strlcpy(config.apconfig.ssid, ssid, sizeof(config.apconfig.ssid));
            }
            const char* appwd = doc["apconfig"]["appwd"];
            if ((appwd) && (strcmp (appwd, config.apconfig.passphrase) != 0)) {
                updated = true;
                strlcpy(config.apconfig.passphrase, appwd, sizeof(config.apconfig.passphrase));
            }

            // Parse Hostname Settings Object
            const char* hostname = doc["hostname"];
            bool hostNameChanged = false;
            if ((hostname) && (strcmp (hostname, config.hostname) != 0)) {
                updated = true;
                hostNameChanged = true;
                strlcpy(config.hostname, hostname, sizeof(config.hostname));
            }

            // Parse Bubble Settings Object
            const char* bubname = doc["bubbleconfig"]["name"];
            if ((bubname) && (strcmp (bubname, config.bubble.name) != 0)) {
                updated = true;
                strlcpy(config.bubble.name, bubname, sizeof(config.bubble.name));
            }

            JsonVariant tempinf = doc["bubbleconfig"]["tempinf"];
            if ((!tempinf.isNull()) && (!config.bubble.tempinf == tempinf)) {
                updated = true;
                config.bubble.tempinf = tempinf;
            }

            // Parse temperature calibration
            double calAmbient = doc["calibrate"]["room"];
            if ((calAmbient) && (!calAmbient == config.calibrate.room)) {
                updated = true;
                config.calibrate.room = calAmbient;
            }

            double calVessel = doc["calibrate"]["vessel"];
            if ((calVessel) && (!calVessel == config.calibrate.vessel)) {
                updated = true;
                config.calibrate.vessel =  calVessel;
            }

            // Parse Target Settings Object
            const char* targeturl = doc["targetconfig"]["targeturl"];
            if ((targeturl) && (strcmp (targeturl, config.urltarget.url) != 0)) {
                updated = true;
                strlcpy(config.urltarget.url, doc["targetconfig"]["targeturl"], sizeof(config.urltarget.url));
            }

            unsigned long targetfreq = doc["targetconfig"]["targetfreq"];
            if ((targetfreq) && (!targetfreq == config.urltarget.freq)) {
                updated = true;
                config.urltarget.freq = targetfreq;
            }

            // Parse Brewer's Friend Settings Object
            const char* bfkey = doc["bfconfig"]["bfkey"];
            if ((bfkey) && (strcmp (bfkey, config.brewersfriend.key) != 0)) {
                updated = true;
                strlcpy(config.brewersfriend.key, bfkey, sizeof(config.brewersfriend.key));
            }

            unsigned long bffreq = doc["bfconfig"]["freq"];
            if ((bffreq) && (!bffreq == config.brewersfriend.freq)) {
                updated = true;
                config.brewersfriend.freq = bffreq;
            }

            // Parse SPIFFS OTA update choice
            JsonVariant dospiffs1 = doc["dospiffs1"];
            if ((!dospiffs1.isNull()) && (!dospiffs1 == config.dospiffs1)) {
                updated = true;
                config.dospiffs1 = dospiffs1;
            }

            // Parse SPIFFS OTA update choice
            JsonVariant dospiffs2 = doc["dospiffs2"];
            if ((!dospiffs2.isNull()) && (!dospiffs2 == config.dospiffs2)) {
                updated = true;
                config.dospiffs2 = dospiffs2;
            }

            // Parse OTA update semaphore choice
            JsonVariant didupdate = doc["didupdate"];
            if ((!didupdate.isNull()) && (!didupdate == config.didupdate)) {
                updated = true;
                config.didupdate = didupdate;
            }

            if (updated) {

                // Save configuration to file
                saveConfig();

                // Reset hostname
                if (hostNameChanged) {
                    wifi_station_set_hostname(hostname);
                    MDNS.setHostname(hostname);
                    MDNS.notifyAPChange();
                    MDNS.announce();

                    char hostredirect[39];
                    strcpy(hostredirect, config.hostname);
                    strcat(hostredirect, ".local");
                    Log.notice(F("Redirecting to new URL: http://%s.local/" CR), hostname);

                    // Send redirect information
                    Log.verbose(F("Sending %s for redirect." CR), hostredirect);
                    request->redirect(hostredirect);
                } else {
                    request->send(200, F("text/html"), F("Ok."));
                }
            }

        } else {
            request->send(500, F("text/json"), err.c_str());
        }
    });
}

void stopWebServer() {
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}
