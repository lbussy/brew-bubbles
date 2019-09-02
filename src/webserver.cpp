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

#include "webserver.h"

bool WebServer::instanceFlag = false;
WebServer* WebServer::single = NULL;

WebServer* WebServer::getInstance() {
    if (!instanceFlag) {
        single = new WebServer();
        instanceFlag = true;
        return single;
    } else {
        return single;
    }
}

void WebServer::initialize (int port) {
    server = new ESP8266WebServer(port);
    filesystem = &SPIFFS;
    filesystem->begin();

    single->aliases();

    single->server->begin();
    Log.verbose(F("HTTP server started on port %l." CR) , port);
    Log.verbose(F("Open: http://%s.local." CR), WiFi.hostname().c_str());
}

void WebServer::aliases() {

    // Regular page aliases

    single->server->on(
        F("/about/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/about.htm")))
             {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/debug/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/debug.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/help/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/help.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/ota/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/ota.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/settings/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/settings.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/wifi/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/wifi.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    // Action Page Handlers

    single->server->on(
        F("/ota2/"),
        HTTP_GET,
        []() {
            single->handleFileRead(F("/ota2.htm"));
            JsonConfig *config = JsonConfig::getInstance();
            config->dospiffs = true; // Set config to update SPIFFS on restart
            config->Save();
            execfw();                // Trigger the OTA update
        });

    single->server->on(
        F("/wifi2/"),
        HTTP_GET,
        []() {
            single->handleFileRead(F("/wifi2.htm"));
            _delay(5000);   // Let page load
            resetWifi();    // Wipe settings, reset controller
        });

    // JSON Handlers

    single->server->on(
        F("/bubble/"),
        HTTP_GET,
        []() {
            // Used to provide the Bubbles json
            Bubbles *bubble = Bubbles::getInstance();
            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            char* json = bubble->CreateBubbleJson();
            single->server->send(200, F("application/json"), json);
        });

    single->server->on(
        F("/config/"),
        HTTP_GET,
        []() {
            // Used to build the "Change Settings" page
            JsonConfig *config = JsonConfig::getInstance();
            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            char* json = config->CreateSettingsJson();
            single->server->send(200, F("application/json"), json);
        });

    single->server->on(
        F("/config/update/"),
        HTTP_POST,
        []() {
            Log.verbose(F("Processing post to /config/update/." CR));
            String input = single->server->arg(F("plain"));
            const size_t capacity = CONFIG_CAP;
            StaticJsonDocument<capacity> doc;
            DeserializationError err = deserializeJson(doc, input);
            if (!err) {
                // Parse JSON
                JsonConfig *config = JsonConfig::getInstance();

                // Parse Access Point Settings Object
                const char* ssid = doc["apconfig"]["ssid"];
                if (ssid)
                    strlcpy(config->ssid, ssid, sizeof(config->ssid));
                const char* appwd = doc["apconfig"]["appwd"];
                if (appwd)
                    strlcpy(config->appwd, appwd, sizeof(config->appwd));

                // Parse Hostname Settings Object
                const char* hostname = doc["hostname"];
                bool hostNameChanged = false;
                if (hostname)
                    hostNameChanged = true;
                    strlcpy(config->hostname, hostname, sizeof(config->hostname));

                // Parse Bubble Settings Object
                const char* bubname = doc["bubbleconfig"]["name"];
                if (hostname)
                    strlcpy(config->bubname, bubname, sizeof(config->bubname));
                input.toLowerCase();
                if (input.indexOf("tempinf") >= 0)
                    config->tempinf = doc["tempinf"];

                // Parse temperature calibration
                double calAmbient = doc["calibrate"]["room"];
                if (calAmbient)
                    config->calAmbient = calAmbient;
                double calVessel = doc["calibrate"]["vessel"];
                if (calVessel)
                    config->calVessel =  calVessel;

                // Parse Target Settings Object
                const char* targeturl = doc["targetconfig"]["targeturl"];
                if (targeturl)
                    strlcpy(config->targeturl, targeturl, sizeof(config->targeturl));
                unsigned long targetfreq = doc["targetconfig"]["freq"];
                if (targetfreq)
                    config->targetfreq = targetfreq;

                // Parse Brewer's Friend Settings Object
                const char* bfkey = doc["bfconfig"]["bfkey"];
                if (bfkey)
                    strlcpy(config->bfkey, bfkey, sizeof(config->bfkey));
                unsigned long bffreq = doc["bfconfig"]["freq"];
                if (bffreq)
                    config->bffreq = bffreq;

                // Parse SPIFFS OTA update choice
                input.toLowerCase();
                if (input.indexOf("dospiffs") >= 0)
                    config->dospiffs = doc["dospiffs"];

                // Save configuration to file
                config->Save();

                // Reset hostname
                if (hostNameChanged) {
                    wifi_station_set_hostname(hostname);
                    MDNS.setHostname(hostname);
                    MDNS.notifyAPChange();
                    MDNS.announce();
                }

                // Redirect to Settings page
                single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                single->server->sendHeader(F("Location"),F("/settings/"));
                single->server->send(303); 
            } else {
                single->server->send(500, F("text/json"), err.c_str());
            }
        });

    single->server->on(
        F("/stats/"),
        HTTP_GET,
        []() {
            // Get heap status, analog input value and GPIO statuses
            // TODO: Make sure DI's actually display properly
            const size_t capacity = 65;
            StaticJsonDocument<capacity> doc;

            doc["heap"] = String(ESP.getFreeHeap());
            doc["analog"] = String(analogRead(A0));
            doc["gpio"] = String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));

            char json[capacity] = {};
            serializeJson(doc, json, capacity);
            single->server->send(200, F("application/json"), json);
        });

    // File not found handler

    single->server->onNotFound(
        []() {
            if (!single->handleFileRead(single->server->uri()))
                {single->server->send(404, F("text/plain"), F("404: File not found."));}});
}

String WebServer::getContentType(String filename) {
    if (single->server->hasArg(F("download"))) 
        return F("application/octet-stream");
    else if (filename.endsWith(F(".src")))
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

bool WebServer::handleFileRead(String path) {
    Log.verbose(F("Handle File Read: %s" CR), path.c_str());
    if (path.endsWith(F("/"))) {
        path += F("index.htm");
    }
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (filesystem->exists(pathWithGz) || filesystem->exists(path)) {
        if (filesystem->exists(pathWithGz)) {
            path += F(".gz");
        }
        File file = filesystem->open(path, "r");
        single->server->streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void WebServer::handleLoop() {
    single->server->handleClient();
}
