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

#include "thingspeaktarget.h"

#include "config.h"
#include <ThingSpeak.h>
#include <LCBUrl.h>
#include <ArduinoLog.h>
#include <Arduino.h>

bool pushThingspeak()
{
    Log.verbose(F("Triggered %s push." LF), tsName);
    if (strlen(config.thingspeak.key) && config.thingspeak.channel > 0)
    {
        WiFiClient client;
        ThingSpeak.begin(client); // Initialize ThingSpeak

        // Channel fields:
        //
        
        ThingSpeak.setField(1, bubbles.getAvgBpm()); // bpm
        ThingSpeak.setField(2, bubbles.getAvgAmbient()); // temp
        ThingSpeak.setField(3, bubbles.getAvgVessel()); // temp
        
        // Write to the ThingSpeak channel
        int retVal = ThingSpeak.writeFields(config.thingspeak.channel, config.thingspeak.key);
        if (retVal == 200)
        {
            Log.notice(F("%s channel update successful." LF), tsName);
            return true;
        }
        else
        {
            Log.error(F("Problem updating %s channel %d. HTTP error code %d." LF), tsName, config.thingspeak.channel, retVal);
            return false;
        }
    }
    else
    {
        Log.verbose(F("ThingSpeak not enabled, skipping." LF));
        return false;
    }
}
