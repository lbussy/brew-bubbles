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

#include "post.h"

bool httppost(String json) {
    HTTPClient http; // Declare object of class HTTPClient
    // Ports other than 80 need to be in the format of: http://192.168.168.199:8080/
    http.begin(HTTPTARGET); // Specify request destination
    http.addHeader("Content-Type", "application/json"); // Specify content-type header
    int httpCode = http.POST(json); // Post json
#if DEBUG > 0
    String payload = http.getString(); // Get the response payload
    Serial.print("HTTP Post return code: ");
    Serial.println(httpCode); // Print HTTP return code
    Serial.print("HTTP Post response payload: ");
    Serial.print(payload); // Print request response payload
    Serial.flush();
    Serial.print("JSON was: ");
    Serial.println(json);
    Serial.flush();
#else
    http.POST(json); // Post json
#endif
    http.end(); // Close connection
    if(httpCode == 200) { // 200 = ok
        return true;
    } else {
        return false;
    }
}
