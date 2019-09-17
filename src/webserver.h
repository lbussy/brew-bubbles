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

#ifndef _WEBSERVER_H
#define _WEBSERVER_H

#include "wifi.h"
#include "execota.h"
#include "bubbles.h"
#include "tools.h"
#include "jsonconfig.h"
#include "serial.h"
#include "version.h"
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

class WebServer {
    private:
        // Singleton Declarations
        static bool instanceFlag;
        static WebServer *single;
        WebServer() {}
        // External Declarations
        FS *filesystem;
        ESP8266WebServer *server;
        // Private Methods
        String getContentType(String);
        bool handleFileRead(String);
        void aliases();
        bool running;

    public:
        // Singleton Declarations
        static WebServer* getInstance();
        ~WebServer() {instanceFlag = false;}
        // Other Declarations
        void initialize(int port);
        void handleLoop();
        void stop();
};

#endif // _WEBSERVER_H
