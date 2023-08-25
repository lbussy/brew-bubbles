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

#include "mdns.h"

#include "config.h"
#include "jsonconfig.h"
#include <ArduinoLog.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

extern struct Config config;

void mdnssetup()
{
    if (!MDNS.begin(config.hostname))
    { // Start the mDNS responder
        Log.error(F("Error setting up mDNS responder." LF));
    }
    else
    {
        Log.notice(F("mDNS responder started for %s.local." LF), config.hostname);
        if (!MDNS.addService("http", "tcp", HTTPPORT))
        {
            Log.error(F("Failed to register Web mDNS service." LF));
        }
        else
        {
            Log.notice(F("HTTP registered via mDNS on port %i." LF), HTTPPORT);
        }
        if (!MDNS.addService(config.hostname, "tcp", HTTPPORT))
        {
            Log.error(F("Failed to register %s mDNS service." LF), API_KEY);
        }
        else
        {
            Log.notice(F("%s registered via mDNS on port %i." LF), API_KEY, HTTPPORT);
        }
    }
}

void mdnsreset()
{
    MDNS.end();
    if (!MDNS.begin(config.hostname))
    {
        Log.error(F("Error resetting MDNS responder."));
    }
    else
    {
#ifdef ESP32
        Log.notice(F("mDNS responder restarted, hostname: %s.local." LF), WiFi.getHostname());
#elif ESP8266
        Log.notice(F("mDNS responder restarted, hostname: %s.local." LF), WiFi.hostname().c_str());
#endif
        MDNS.addService("http", "tcp", HTTPPORT);
        MDNS.addService(config.hostname, "tcp", HTTPPORT);
#if DOTELNET == true
        MDNS.addService("telnet", "tcp", TELNETPORT);
#endif
    }
}
