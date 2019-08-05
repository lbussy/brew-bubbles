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
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin(); // Actually start the server
    Log.notice("HTTP server started." CR);
}

void webserverloop() {
    server.handleClient();
}

String getContentType(String filename){
    if(filename.endsWith(".htm")) return "text/html";
    else if(filename.endsWith(".html")) return "text/html";
    else if(filename.endsWith(".css")) return "text/css";
    else if(filename.endsWith(".js")) return "application/javascript";
    else if(filename.endsWith(".png")) return "image/png";
    else if(filename.endsWith(".gif")) return "image/gif";
    else if(filename.endsWith(".jpg")) return "image/jpeg";
    else if(filename.endsWith(".ico")) return "image/x-icon";
    else if(filename.endsWith(".xml")) return "text/xml";
    else if(filename.endsWith(".pdf")) return "application/x-pdf";
    else if(filename.endsWith(".zip")) return "application/x-zip";
    else if(filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path) {  // send the right file to the client (if it exists)
    // Convert String to char array
    int n = path.length();
    char p[n + 1]; 
    strcpy(p, path.c_str()); 
    Log.verbose("handleFileRead: %s" CR, p);
    if(path.endsWith("/")) {
        if(SPIFFS.exists(path + "index.html")) 
            path += "index.html"; // If a folder is requested, send the index file
        else if(SPIFFS.exists(path + "index.htm"))
            path += "index.htm"; // If a folder is requested, send the index file
    }
    String contentType = getContentType(path);              // Get the MIME type
    String pathWithGz = path + ".gz";
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {   // If the file exists, either as a compressed archive, or normal
        if(SPIFFS.exists(pathWithGz))                       // If there's a compressed version available
            path += ".gz";                                  // Use the compressed version
        File file = SPIFFS.open(path, "r");                 // Open the file
        //size_t sent = 
        server.streamFile(file, contentType); // Send it to the client
        file.close();                                       // Close the file again
        Log.notice("Sent file: %s" CR, p);
        return true;
    }
    Log.error("File Not Found: %s" CR, p);
    return false;                                           // If the file doesn't exist, return false
}
