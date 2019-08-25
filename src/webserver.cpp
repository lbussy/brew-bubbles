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
            config->dospiffs = true;    // Set config to update SPIFFS on restart
            config->Save();
            execfw();                   // Trigger the OTA update
        });

    single->server->on(
        F("/wifi2/"),
        HTTP_GET,
        []() {
            single->handleFileRead(F("/wifi2.htm"));
            _delay(3000);       // Wait for 3 (safe) seconds before resetting to let page load
            // disco_restart();    // Reset the wifi settings // TODO
        });


    // JSON Handlers

    single->server->on(
        F("/bubble/"),
        HTTP_GET,
        []() {
            // Used to provide the Bubbles json
            Bubbles *bubble = Bubbles::getInstance();
            single->server->sendHeader("Access-Control-Allow-Origin", "*");
            char* json = bubble->CreateBubbleJson();
            single->server->send(200, "application/json", json);
        });

    single->server->on(
        F("/config/"),
        HTTP_GET,
        []() {
            // Used to build the "Change Settings" page
            JsonConfig *config = JsonConfig::getInstance();
            single->server->sendHeader("Access-Control-Allow-Origin", "*");
            char* json = config->CreateSettingsJson();
            single->server->send(200, "application/json", json);
        });

    single->server->on("/stats/", HTTP_GET, []() {
        // Get heap status, analog input value and GPIO statuses
        // TODO: Make sure DI's actually display properly
        const size_t capacity = 65;
        DynamicJsonDocument doc(capacity);

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
    if (single->server->hasArg("download")) 
        return F("application/octet-stream");
    else if (filename.endsWith(".src"))
        filename = filename.substring(0, filename.lastIndexOf("."));
    else if (filename.endsWith(".htm")) 
        return F("text/html");
    else if (filename.endsWith(".html")) 
        return F("text/html");
    else if (filename.endsWith(".css")) 
        return F("text/css");
    else if (filename.endsWith(".txt")) 
        return F("text/plain");
    else if (filename.endsWith(".js")) 
        return F("application/javascript");
    else if (filename.endsWith(".png")) 
        return F("image/png");
    else if (filename.endsWith(".gif")) 
        return F("image/gif");
    else if (filename.endsWith(".jpg")) 
        return F("image/jpeg");
    else if (filename.endsWith(".ico")) 
        return F("image/x-icon");
    else if (filename.endsWith(".svg")) 
        return F("image/svg+xml");
    else if (filename.endsWith(".xml")) 
        return F("text/xml");
    else if (filename.endsWith(".pdf")) 
        return F("application/x-pdf");
    else if (filename.endsWith(".zip")) 
        return F("application/x-zip");
    else if (filename.endsWith(".gz")) 
        return F("application/x-gzip");
    else if (filename.endsWith(".zip")) 
        return F("application/zip");
    else if(filename.endsWith(".json")) 
        return F("application/json");
    else if(filename.endsWith(".ttf"))
        return F("application/x-font-ttf");
    else if(filename.endsWith(".otf"))
        return F("application/x-font-opentype");
    else if(filename.endsWith(".woff"))
        return F("application/font-woff");
    else if(filename.endsWith(".woff2"))
        return F("application/font-woff2");
    else if(filename.endsWith(".eot"))
        return F("application/vnd.ms-fontobject");
    else if(filename.endsWith(".sfnt"))
        return F("application/font-sfnt");
    else if(filename.endsWith(".appcache"));
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
            path += ".gz";
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
