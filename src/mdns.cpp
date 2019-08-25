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

#include "mdns.h"

void mdnssetup() {
    JsonConfig *config = JsonConfig::getInstance();
    if (!MDNS.begin(config->hostname)) { // Start the mDNS responder
        Log.error(F("Error setting up mDNS responder." CR));
    } else {
        Log.notice(F("mDNS responder started for %s.local." CR), config->hostname);
        if (!MDNS.addService("http", "tcp", PORT)) {
            Log.error(F("Failed to register mDNS service." CR));
        } else {
            Log.notice(F("HTTP registered via mDNS on port %i." CR), PORT);
        }
    }
}
