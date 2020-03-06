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
    setWebAliases();
    server.begin();
    Log.notice(F("Async HTTP server started on port %l." CR) , PORT);
    Log.verbose(F("Open: http://%s.local to view controller application." CR), WiFi.hostname().c_str());
}

void setWebAliases() {

    // Regular page aliases


    // Route for root / web page
    server.on(
        "/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/index.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    server.on(
        "/about/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/about.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    server.on(
        "/help/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/help.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    server.on(
        "/ota/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/ota.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    server.on(
        "/ota2/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/ota2.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    server.on(
        "/settings/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/settings.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    server.on(
        "/wifi/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/wifi.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );

    // Action Page Handlers

    server.on(
        "/wifi2/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            if (!handleWebFileRead(request, F("/wifi2.htm"))) {
                request->send(404, F("text/plain"), F("404: File not found."));
            } else {
                resetWifi();    // Wipe settings, reset controller
            }
        }
    );

    server.on(
        "/otastart/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            request->send(200, F("text/plain"), F("200: OTA started."));
            execfw(); // Trigger the OTA update
        }
    );

    // Settings Update Handler

    server.on(
        "/settings/update/",
        HTTP_POST,
        [] (AsyncWebServerRequest *request) {
            // Process POST configuration changes
            Log.verbose(F("Processing post to /settings/update/." CR));

            JsonConfig *config = JsonConfig::getInstance();

            char redirect[66];
            strcpy(redirect, "/settings/");

            if (request->hasParam(F("mdnsID"), true)) { // Change Hostname
                if ((request->arg("mdnsID").length() > 32) || (request->arg("mdnsID").length() < 3)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->hostname, request->arg("mdnsID").c_str(), sizeof(config->hostname));
                    config->save();

                    // Reset hostname
                    wifi_station_set_hostname(config->hostname);
                    MDNS.setHostname(config->hostname);
                    MDNS.notifyAPChange();
                    MDNS.announce();

                    // Creeate a full URL for redirection
                    char hostname[45];
                    strcpy(hostname, "http://");
                    strcat(hostname, config->hostname);
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
                    strlcpy(config->bubname, request->arg("bubname").c_str(), sizeof(config->bubname));
                    config->save();
                }
                strcat(redirect, "#controller"); // Redirect to Controller box
                Log.notice(F("POSTed bubname, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("tempInF"), true)) { // Change Temp in F
                char option[8];
                strcpy(option, request->arg("tempInF").c_str());
                if (strcmp(option, "option0") == 0) {
                    config->tempinf = false;
                } else {
                    config->tempinf = true;
                    config->save();
                }
                strcat(redirect, "#temp"); // Redirect to Temp Control
                Log.notice(F("POSTed tempInF, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("calRoom"), true)) { // Change Room temp calibration
                if ((request->arg("calRoom").toDouble() < -25) || (request->arg("calRoom").toDouble() > 25)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->calAmbient = request->arg("calRoom").toDouble();
                    config->save();
                }
                strcat(redirect, "#temp"); // Redirect to Temp Control
                Log.notice(F("POSTed calRoom, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("calVessel"), true)) { // Change Vessel temp calibration
                if ((request->arg("calVessel").toDouble() < -25) || (request->arg("calVessel").toDouble() > 25)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->calVessel = request->arg("calVessel").toDouble();
                    config->save();
                }
                strcat(redirect, "#temp"); // Redirect to Temp Control
                Log.notice(F("POSTed calVessel, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("target"), true)) { // Change Target URL
                if (request->arg("target").length() > 128) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->targeturl, request->arg("target").c_str(), sizeof(config->targeturl));
                    config->save();
                }
                strcat(redirect, "#target"); // Redirect to Target Control
                Log.notice(F("POSTed target, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("tfreq"), true)) { // Change Vessel temp calibration
                if ((request->arg("tfreq").toInt() < 1) || (request->arg("tfreq").toInt() > 60)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->targetfreq = request->arg("tfreq").toInt();
                    config->updateTargetFreq = true;
                    config->save();
                }
                strcat(redirect, "#target"); // Redirect to Target Control
                Log.notice(F("POSTed tfreq, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("bfkey"), true)) { // Change Brewer's Friend key
                if ((request->arg("bfkey").length() > 64) || (request->arg("bfkey").length() < 20)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->bfkey, request->arg("bfkey").c_str(), sizeof(config->bfkey));
                    config->save();
                }
                strcat(redirect, "#bf"); // Redirect to Brewer's Friend Control
                Log.notice(F("POSTed bfkey, redirecting to %s." CR), redirect);

            } else if (request->hasParam(F("bfreq"))) { // Change Vessel temp calibration
                if ((request->arg("bfreq").toInt() < 15) || (request->arg("bfreq").toInt() > 120)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->bffreq = request->arg("bfreq").toInt();
                    config->updateBFFreq = true;
                    config->save();
                }
                strcat(redirect, "#bf"); // Redirect to Brewer's Friend Control
                Log.notice(F("POSTed bfreq, redirecting to %s." CR), redirect);

            }

            // Redirect to Settings page
            request->redirect(redirect);
        }
    );

    server.on(
        "/clearupdate/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            JsonConfig *config = JsonConfig::getInstance();
            Log.verbose(F("Clearing any update flags." CR));
            config->dospiffs1 = false;
            config->dospiffs2 = false;
            config->didupdate = false;
            config->save();
            request->send(200, F("text/plain"), F("200: OK."));
        }
    );

    // JSON Handlers

    server.on(
        "/bubble/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            // Used to provide the Bubbles json
            Bubbles *bubble = Bubbles::getInstance();
            JsonConfig *config = JsonConfig::getInstance();

            //const size_t capacity = JSON_OBJECT_SIZE(8);
            const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
            StaticJsonDocument<capacity> doc;

            doc[F("api_key")] = F(API_KEY);
            doc[F("device_source")] = F(SOURCE);
            doc[F("name")] = config->bubname;
            doc[F("bpm")] = bubble->getAvgBpm();
            doc[F("ambient")] = bubble->getAvgAmbient();
            doc[F("temp")] = bubble->getAvgVessel();
            if (config->tempinf == true)
                doc[F("temp_unit")] = F("F");
            else
                doc[F("temp_unit")] = F("C");
            doc[F("datetime")] = bubble->lastTime;

            String json;
            serializeJsonPretty(doc, json);

            request->send(200, F("application/json"), json);
        }
    );

    server.on(
        "/config/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            // Used to provide the Config json
            JsonConfig *config = JsonConfig::getInstance();

            const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(8) + 481;
            StaticJsonDocument<capacity> doc;

            // Serialize Access Point Settings Object
            JsonObject apconfig = doc.createNestedObject("apconfig");
            apconfig["ssid"] = config->ssid;
            apconfig["appwd"] = config->appwd;

            // Serialize Hostname Settings Object
            doc["hostname"] = config->hostname;

            // Serialize Bubble Settings Object
            JsonObject bubbleconfig = doc.createNestedObject("bubbleconfig");
            bubbleconfig["name"] = config->bubname;
            bubbleconfig["tempinf"] = config->tempinf;

            // Serialize temperature calibration
            JsonObject calibrate = doc.createNestedObject("calibrate");
            calibrate["room"] = config->calAmbient;
            calibrate["vessel"] = config->calVessel;

            // Serialize Target Settings Object
            JsonObject targetconfig = doc.createNestedObject("targetconfig");
            targetconfig["targeturl"] = config->targeturl;
            targetconfig["freq"] = config->targetfreq;

            // Serialize Brewer's Friend Settings Object
            JsonObject bfconfig = doc.createNestedObject("bfconfig");
            bfconfig["bfkey"] = config->bfkey;
            bfconfig["freq"] = config->bffreq;

            // Serialize SPIFFS OTA update choice
            doc["dospiffs1"] = config->dospiffs1;
            doc["dospiffs2"] = config->dospiffs2;

            // Serialize semaphore for OTA update
            doc["didupdate"] = config->didupdate;

            String json;
            serializeJsonPretty(doc, json);

            request->send(200, F("application/json"), json);
        }
    );

    server.on(
        "/config/apply/",
        HTTP_POST,
        [] (AsyncWebServerRequest *request) {  // Process JSON POST configuration changes
            Log.verbose(F("Processing post to /config/apply/." CR));
            String input = request->arg(F("plain"));
            const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7);
            StaticJsonDocument<capacity> doc;
            DeserializationError err = deserializeJson(doc, input);
            if (!err) {
                bool updated = false;

                // Parse JSON
                JsonConfig *config = JsonConfig::getInstance();

                // Parse Access Point Settings Object
                const char* ssid = doc["apconfig"]["ssid"];
                if ((ssid) && (strcmp (ssid, config->ssid) != 0)) {
                    updated = true;
                    strlcpy(config->ssid, ssid, sizeof(config->ssid));
                }
                const char* appwd = doc["apconfig"]["appwd"];
                if ((appwd) && (strcmp (appwd, config->appwd) != 0)) {
                    updated = true;
                    strlcpy(config->appwd, appwd, sizeof(config->appwd));
                }

                // Parse Hostname Settings Object
                const char* hostname = doc["hostname"];
                bool hostNameChanged = false;
                if ((hostname) && (strcmp (hostname, config->hostname) != 0)) {
                    updated = true;
                    hostNameChanged = true;
                    strlcpy(config->hostname, hostname, sizeof(config->hostname));
                }

                // Parse Bubble Settings Object
                const char* bubname = doc["bubbleconfig"]["name"];
                if ((bubname) && (strcmp (bubname, config->bubname) != 0)) {
                    updated = true;
                    strlcpy(config->bubname, bubname, sizeof(config->bubname));
                }
            
                JsonVariant tempinf = doc["bubbleconfig"]["tempinf"];
                if ((!tempinf.isNull()) && (!config->tempinf == tempinf)) {
                    updated = true;
                    config->tempinf = tempinf;
                }

                // Parse temperature calibration
                double calAmbient = doc["calibrate"]["room"];
                if ((calAmbient) && (!calAmbient == config->calAmbient)) {
                    updated = true;
                    config->calAmbient = calAmbient;
                }

                double calVessel = doc["calibrate"]["vessel"];
                if ((calVessel) && (!calVessel == config->calVessel)) {
                    updated = true;
                    config->calVessel =  calVessel;
                }

                // Parse Target Settings Object
                const char* targeturl = doc["targetconfig"]["targeturl"];
                if ((targeturl) && (strcmp (targeturl, config->targeturl) != 0)) {
                    updated = true;
                    strlcpy(config->targeturl, doc["targetconfig"]["targeturl"], sizeof(config->targeturl));
                }

                unsigned long targetfreq = doc["targetconfig"]["targetfreq"];
                if ((targetfreq) && (!targetfreq == config->targetfreq)) {
                    updated = true;
                    config->targetfreq = targetfreq;
                }

                // Parse Brewer's Friend Settings Object
                const char* bfkey = doc["bfconfig"]["bfkey"];
                if ((bfkey) && (strcmp (bfkey, config->bfkey) != 0)) {
                    updated = true;
                    strlcpy(config->bfkey, bfkey, sizeof(config->bfkey));
                }

                unsigned long bffreq = doc["bfconfig"]["freq"];
                if ((bffreq) && (!bffreq == config->bffreq)) {
                    updated = true;
                    config->bffreq = bffreq;
                }

                // Parse SPIFFS OTA update choice
                JsonVariant dospiffs1 = doc["dospiffs1"];
                if ((!dospiffs1.isNull()) && (!dospiffs1 == config->dospiffs1)) {
                    updated = true;
                    config->dospiffs1 = dospiffs1;
                }

                // Parse SPIFFS OTA update choice
                JsonVariant dospiffs2 = doc["dospiffs2"];
                if ((!dospiffs2.isNull()) && (!dospiffs2 == config->dospiffs2)) {
                    updated = true;
                    config->dospiffs2 = dospiffs2;
                }

                // Parse OTA update semaphore choice
                JsonVariant didupdate = doc["didupdate"];
                if ((!didupdate.isNull()) && (!didupdate == config->didupdate)) {
                    updated = true;
                    config->didupdate = didupdate;
                }

                if (updated) {

                    // Save configuration to file
                    config->save();

                    // Reset hostname
                    if (hostNameChanged) {
                        wifi_station_set_hostname(hostname);
                        MDNS.setHostname(hostname);
                        MDNS.notifyAPChange();
                        MDNS.announce();

                        char hostredirect[39];
                        strcpy(hostredirect, config->hostname);
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
        }
    );

    server.on(
        "/thisVersion/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            Log.verbose(F("Serving /thisVersion/." CR));
            const size_t capacity = JSON_OBJECT_SIZE(3);
            StaticJsonDocument<capacity> doc;

            doc["version"] = version();

            String json;
            serializeJsonPretty(doc, json);

            request->send(200, F("application/json"), json);
        }
    );

    server.on(
        "/thatVersion/",
        HTTP_GET,
        [] (AsyncWebServerRequest *request) {
            Log.verbose(F("Serving /thatVersion/." CR));

            String json = "";
            HTTPClient http;
            WiFiClient client;
            http.begin(client, F(VERSIONJSONLOC));
            http.addHeader(F("Cache-Control"), F("no-cache"));
            if (http.GET() > 0) {
                json = http.getString();
            }
            http.end();

            request->send(200, F("application/json"), json);
        }
    );

    // File not found handler

    server.onNotFound(
        [] (AsyncWebServerRequest *request) {
            request->send(404, F("text/plain"), F("404: File not found."));
        }
    );
}

String getWebContentType(AsyncWebServerRequest *request, String filename) {
    if (request->hasParam(F("download"), true))
        return F("application/octet-stream");
    if (filename.endsWith(F(".src")))
        filename = filename.substring(0, filename.lastIndexOf(F(".")));
    else if (filename.endsWith(F(".htm")))
        return F("text/html");
    else if (filename.endsWith(F(".html")))
        return F("text/html");
    else if (filename.endsWith(F(".css")))
        return F("text/css");
    else if (filename.endsWith(F(".txt")))
        return F("text/plain");
    else if (filename.endsWith(F(".js")))
        return F("application/javascript");
    else if (filename.endsWith(F(".png")))
        return F("image/png");
    else if (filename.endsWith(F(".gif")))
        return F("image/gif");
    else if (filename.endsWith(F(".jpg")))
        return F("image/jpeg");
    else if (filename.endsWith(F(".ico")))
        return F("image/x-icon");
    else if (filename.endsWith(F(".svg")))
        return F("image/svg+xml");
    else if (filename.endsWith(F(".xml")))
        return F("text/xml");
    else if (filename.endsWith(F(".pdf")))
        return F("application/x-pdf");
    else if (filename.endsWith(F(".zip")))
        return F("application/x-zip");
    else if (filename.endsWith(F(".gz")))
        return F("application/x-gzip");
    else if (filename.endsWith(F(".zip")))
        return F("application/zip");
    else if(filename.endsWith(F(".json")))
        return F("application/json");
    else if(filename.endsWith(F(".ttf")))
        return F("application/x-font-ttf");
    else if(filename.endsWith(F(".otf")))
        return F("application/x-font-opentype");
    else if(filename.endsWith(F(".woff")))
        return F("application/font-woff");
    else if(filename.endsWith(F(".woff2")))
        return F("application/font-woff2");
    else if(filename.endsWith(F(".eot")))
        return F("application/vnd.ms-fontobject");
    else if(filename.endsWith(F(".sfnt")))
        return F("application/font-sfnt");
    else if(filename.endsWith(F(".appcache")));
        return F("text/cache-manifest");
    return F("text/plain"); 
}

bool handleWebFileRead(AsyncWebServerRequest *request, String path) {
    // Initialize SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("An Error has occurred while mounting SPIFFS." CR));
        return false;
    }

    Log.verbose(F("Handle File Read: %s" CR), path.c_str());
    if (path.endsWith(F("/"))) {
        path += F("index.htm");
    }

    String contentType = getWebContentType(request, path);
    String pathWithGz = path + F(".gz");
    File file = SPIFFS.open(path, "r");
    if (!SPIFFS.exists(path) || !file) {
        if (!SPIFFS.exists(pathWithGz) || !pathWithGz) {
            return false;
        } else {
            path += F(".gz");
        }
        return true;
    }
    request->send(SPIFFS, path, contentType, false); // Change String() to content type
    return true;
}

void stopWebServer() {
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}
