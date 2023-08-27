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

#ifndef _WEBSERVER_H
#define _WEBSERVER_H
#pragma once

#include <Arduino.h>

const char FILE_NOT_FOUND[] PROGMEM = "File Not Found";
const char FS_INIT_ERROR[] PROGMEM = "FS Init Error";

void startWebServer();
void stopWebServer();
void replyOK();
void replyOKWithMsg(String msg);
void replyNotFound(String msg);
void replyBadRequest(String msg);
void replyServerError(String msg);
void handleNotFound();
bool handleFileRead(String path);
void setJsonHandlers();
void setSettingsAliases();

bool handleControllerPost();
bool handleTemperaturePost();
bool handleURLTargetPost();
bool handleBrewersFriendTargetPost();
bool handleBrewfatherTargetPost();
bool handleThingSpeakTargetPost();

#endif // _WEBSERVER_H
