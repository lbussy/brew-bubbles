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

#ifndef _MAIN_H
#define _MAIN_H

#include "ota.h"
#include "localtime.h"
#include "config.h"
#include "jsonconfig.h"
#include "mdns.h"
#include "bubbles.h"
#include "webserver.h"
#include "wifi.h"
#include "serial.h"

void setup();
void loop();

#endif // _MAIN_H