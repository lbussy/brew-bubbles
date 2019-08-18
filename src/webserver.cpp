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

ESP8266WebServer server(PORT); // Create a webserver object that listens for HTTP requests

void webserversetup() {
    SPIFFS.begin(); // Start the SPI Flash Files System

    server.onNotFound([]() { // If the client requests any URI
        if (!handleFileRead(server.uri())) // send it if it exists
            server.send(500, "text/plain", "500: Internal Server Error, unable to process request"); // No idea ...
    });

    server.onNotFound(handleNotFound); // Attach a 404 handler
    setAliases();

    server.begin(); // Start the web server
    Log.notice(F("HTTP server started." CR));
}

void webserverloop() {
    server.handleClient();
}

String getContentType(String path){
    int n = path.length();
    char p[n + 1]; 
    strcpy(p, path.c_str()); 
    Log.verbose(F("In getContentType, looking for %s." CR), p);
    if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if(path.endsWith(".htm")) return "text/html";
    else if(path.endsWith(".html")) return "text/html";
    else if(path.endsWith(".css")) return "text/css";
    else if(path.endsWith(".js")) return "application/javascript";
    else if(path.endsWith(".png")) return "image/png";
    else if(path.endsWith(".gif")) return "image/gif";
    else if(path.endsWith(".jpg")) return "image/jpeg";
    else if(path.endsWith(".ico")) return "image/x-icon";
    else if(path.endsWith(".xml")) return "text/xml";
    else if(path.endsWith(".pdf")) return "application/x-pdf";
    else if(path.endsWith(".zip")) return "application/x-zip";
    else if(path.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path) {  // send the right file to the client (if it exists)
    // Convert String to char array
    int n = path.length();
    char p[n + 1]; 
    strcpy(p, path.c_str()); 
    Log.verbose(F("Handle file read: %s" CR), p);
    String contentType = getContentType(path);             // Get the MIME type
    String pathWithGz = path + ".gz";
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
        if(SPIFFS.exists(pathWithGz))                      // If there's a compressed version available
            path += ".gz";                                 // Use the compressed version
        File file = SPIFFS.open(path, "r");                // Open the file
        if (!file)                                          // Unsuccessful open
            return false;
        server.streamFile(file, contentType);              // Send it to the client
        file.close();                                      // Close the file again
        Log.notice(F("Sent file: %s" CR), p);
        return true;
    } else {
        Log.error(F("File Not Found: %s" CR), p);
        handleNotFound();
        return true;
    }
}

/////////////////////////////////////////////////
//
// Page Action Handlers
//
/////////////////////////////////////////////////

void trigger_OTA() {
    handleFileRead("/updating.htm");    // Send an "are you sure?" message
}

void trigger_OTA2() {
    handleFileRead("/updating2.htm");   // Send a message to the user to let them know what is going on
    JsonConfig *config = JsonConfig::getInstance();
    config->dospiffs = true;            // Set config to update SPIFFS on restart
    config->Serialize();
    execfw();                           // Trigger the OTA update
}

void trigger_wifi_reset() {
    handleFileRead("/wifi_reset.htm");  // Send an "are you sure?" message
}

void trigger_wifi_reset2() {
    handleFileRead("/wifi_reset2.htm"); // Send a message to the user to let them know what is going on
    delay(1000);                        // Wait 3 (safe) seconds to let everything load
    yield();
    delay(1000);
    yield();
    delay(1000);
    disco_restart();                    // Reset the wifi settings
}

void http_json() {
    // I probably don't want this inline so that I can add the Allow-Origin
    // header (in case anyone wants to build scripts that pull this data)
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String message = "TODO: Should be sending json.\n\n"; // TODO: Temp message only, remove this
    server.send(200, "text/plain", message); // TODO: Temp message only, remove this
    // server.send(200, "application/json", tilt_scanner.tilt_to_json().dump().c_str());
}

void settings_json() {
    String message = "TODO: Should be loading settings.\n\n"; // TODO: Temp message only, remove this
    server.send(200, "text/plain", message); // TODO: Temp message only, remove this
    // settings_json is intended to be used to build the "Change Settings"
    // page. Not sure if I want to leave allow-origin here, but for now 
    // it's OK.
    //server.sendHeader("Access-Control-Allow-Origin", "*"); // TODO: Fix this
    //server.send(200, "application/json", app_config.config.dump().c_str()); // TODO: Fix this
}

/////////////////////////////////////////////////
//
// Page Aliases
//
/////////////////////////////////////////////////

void setAliases() { // Aliases for pages
    server.on("/", root_from_spiffs);
    server.on("/settings/", settings_from_spiffs);
    server.on("/help/", help_from_spiffs);
    server.on("/about/", about_from_spiffs);
    // server.on("/settings/update/", processConfig);  // TODO: Fix this - should reset all settings
    server.on("/json/", http_json);
    server.on("/settings/json/", settings_json);
    server.on("/ota/", trigger_OTA);
    server.on("/ota2/", trigger_OTA2);
    server.on("/wifi/", trigger_wifi_reset);
    server.on("/wifi2/", trigger_wifi_reset2);

    server.on("/favicon.ico", favicon_from_spiffs);

    server.on("/android-chrome-192x192.png", android_chrome_192x192png_from_spiffs);
    server.on("/android-chrome-512x512.png", android_chrome_512x512png_from_spiffs);
    server.on("/apple-touch-icon.png", apple_touch_iconpng_from_spiffs);
    server.on("/apple-touch-icon-114x114.png", apple_touch_icon_114x114png_from_spiffs);
    server.on("/apple-touch-icon-120x120.png", apple_touch_icon_120x120png_from_spiffs);
    server.on("/apple-touch-icon-144x144.png", apple_touch_icon_144x144png_from_spiffs);
    server.on("/apple-touch-icon-152x152.png", apple_touch_icon_152x152png_from_spiffs);
    server.on("/apple-touch-icon-180x180.png", apple_touch_icon_180x180png_from_spiffs);
    server.on("/apple-touch-icon-57x57.png", apple_touch_icon_57x57png_from_spiffs);
    server.on("/apple-touch-icon-60x60.png", apple_touch_icon_60x60png_from_spiffs);
    server.on("/apple-touch-icon-72x72.png", apple_touch_icon_72x72png_from_spiffs);
    server.on("/apple-touch-icon-76x76.png", apple_touch_icon_76x76png_from_spiffs);
    server.on("/favicon-16x16.png", favicon_16x16png_from_spiffs);
    server.on("/favicon-32x32.png", favicon_32x32png_from_spiffs);
    server.on("/mstile-144x144.png", mstile_144x144png_from_spiffs);
    server.on("/mstile-150x150.png", mstile_150x150png_from_spiffs);
    server.on("/mstile-310x310.png", mstile_310x310png_from_spiffs);

    server.on("/safari-pinned-tab.svg", safari_pinned_tabsvg_from_spiffs);

    server.on("/manifest.json", manifest_json_from_spiffs);
    server.on("/config.json", config_json_from_spiffs); // TODO: This should be temp
    server.on("/bubbles.json", bubbles_json_from_spiffs); // TODO: This should be temp
}

void root_from_spiffs() {
    handleFileRead("/index.htm");
}

void settings_from_spiffs() {
    handleFileRead("/settings.htm");
}

void help_from_spiffs() {
    handleFileRead("/help.htm");
}

void about_from_spiffs() {
    handleFileRead("/about.htm");
}

void favicon_from_spiffs() {
    handleFileRead("/favicon.ico");
}

void android_chrome_192x192png_from_spiffs() {
    handleFileRead("/android-chrome-192x192.png");
}

void android_chrome_512x512png_from_spiffs() {
    handleFileRead("/android-chrome-512x512.png");
}

void apple_touch_iconpng_from_spiffs() {
    handleFileRead("/apple-touch-icon.png");
}

void apple_touch_icon_114x114png_from_spiffs() {
    handleFileRead("/apple-touch-icon-114x114.png");
}

void apple_touch_icon_120x120png_from_spiffs() {
    handleFileRead("/apple-touch-icon-120x120.png");
}

void apple_touch_icon_144x144png_from_spiffs() {
    handleFileRead("/apple-touch-icon-144x144.png");
}

void apple_touch_icon_152x152png_from_spiffs() {
    handleFileRead("/apple-touch-icon-152x152.png");
}

void apple_touch_icon_180x180png_from_spiffs() {
    handleFileRead("/apple-touch-icon-180x180.png");
}

void apple_touch_icon_57x57png_from_spiffs() {
    handleFileRead("/apple-touch-icon-57x57.png");
}

void apple_touch_icon_60x60png_from_spiffs() {
    handleFileRead("/apple-touch-icon-60x60.png");
}

void apple_touch_icon_72x72png_from_spiffs() {
    handleFileRead("/apple-touch-icon-72x72.png");
}

void apple_touch_icon_76x76png_from_spiffs() {
    handleFileRead("/apple-touch-icon-76x76.png");
}

void favicon_16x16png_from_spiffs() {
    handleFileRead("/favicon-16x16.png");
}

void favicon_32x32png_from_spiffs() {
    handleFileRead("/favicon-32x32.png");
}

void mstile_144x144png_from_spiffs() {
    handleFileRead("/mstile-144x144.png");
}

void mstile_150x150png_from_spiffs() {
    handleFileRead("/mstile-150x150.png");
}

void mstile_310x310png_from_spiffs() {
    handleFileRead("/mstile-310x310.png");
}

void safari_pinned_tabsvg_from_spiffs() {
    handleFileRead("/safari-pinned-tab.svg");
}

void manifest_json_from_spiffs() {
    handleFileRead("/manifest.json");
}

void config_json_from_spiffs() {
    handleFileRead("/config.json"); // TODO: This should be temp
}

void bubbles_json_from_spiffs() {
    handleFileRead("/bubbles.json"); // TODO: This should be temp
}

void handleNotFound() {
    String message = "404: File Not Found\n\n";
    server.send(404, "text/plain", message);
}
