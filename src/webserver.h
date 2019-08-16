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

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "config.h"
#include "serial.h"
#include "jsonconfig.h"
#include "wifi.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path); // send the right file to the client (if it exists)
void webserversetup();
void webserverloop();
void setAliases();
void root_from_spiffs();
void settings_from_spiffs();
void help_from_spiffs();
void about_from_spiffs();
void favicon_from_spiffs();
void trigger_OTA();
void trigger_wifi_reset();
void trigger_wifi_reset2();
void http_json();
void settings_json();
void handleNotFound();
void android_chrome_192x192png_from_spiffs();
void android_chrome_512x512png_from_spiffs();
void apple_touch_iconpng_from_spiffs();
void apple_touch_icon_114x114png_from_spiffs();
void apple_touch_icon_120x120png_from_spiffs();
void apple_touch_icon_144x144png_from_spiffs();
void apple_touch_icon_152x152png_from_spiffs();
void apple_touch_icon_180x180png_from_spiffs();
void apple_touch_icon_57x57png_from_spiffs();
void apple_touch_icon_60x60png_from_spiffs();
void apple_touch_icon_72x72png_from_spiffs();
void apple_touch_icon_76x76png_from_spiffs();
void favicon_16x16png_from_spiffs();
void favicon_32x32png_from_spiffs();
void mstile_144x144png_from_spiffs();
void mstile_150x150png_from_spiffs();
void mstile_310x310png_from_spiffs();
void safari_pinned_tabsvg_from_spiffs();
void manifestjson_from_spiffs();
void configjson_from_spiffs();
void testbubblesjson_from_spiffs();
void manifest_json_from_spiffs();
void config_json_from_spiffs(); // TODO: This should be temp
void testbubbles_json_from_spiffs(); // TODO: This should be temp

#endif /* WEBSERVER_H */
