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

#include "serial.h"

#ifndef DISABLE_LOGGING

void serial() { // Start serial with auto-detected rate (default to BAUD)
    _delay(3000); // Delay to allow monitor to start
    Serial.begin(BAUD);
    Serial.setDebugOutput(true);
    Serial.flush();
    Log.begin(LOG_LEVEL, &Serial, true);
    Log.setPrefix(printTimestamp);
    Log.notice(F("Serial logging started at %l." CR), BAUD);
}

void printTimestamp(Print* _logOutput) {
  char c[12];
  //int m = 
  sprintf(c, "%10lu ", millis());
  _logOutput->print(c);
}

#else // DISABLE_LOGGING

void serial(){}

#endif // Debug
