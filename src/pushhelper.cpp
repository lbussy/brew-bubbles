/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#include "pushhelper.h"

bool pushToTarget(PushTarget *target, IPAddress targetIP, int port)
{
    LCBUrl lcburl;
    lcburl.setUrl(String(target->url) + String(target->key.name));

    Log.notice(F("Posting to: %s" CR), lcburl.getHost().c_str());

    const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
    StaticJsonDocument<capacity> doc;

    if (target->apiName.enabled)
        doc[target->apiName.name] = F(API_KEY);
    if (target->bubName.enabled)
        doc[target->bubName.name] = config.bubble.name;
    if (target->bpm.enabled)
        doc[target->bpm.name] = bubbles.getAvgBpm();
    if (target->ambientTemp.enabled)
        doc[target->ambientTemp.name] = bubbles.getAvgAmbient();
    if (target->vesselTemp.enabled)
        doc[target->vesselTemp.name] = bubbles.getAvgVessel();
    if (target->tempFormat.enabled)
    {
        if (config.bubble.tempinf == true)
            doc[target->tempFormat.name] = F("F");
        else
            doc[target->tempFormat.name] = F("C");
    }
    String json;
    serializeJson(doc, json);

    // Use the IP address we resolved (necessary for mDNS)
    Log.verbose(F("Connecting to: %s at %s on port %l" CR),
                lcburl.getHost().c_str(),
                targetIP.toString().c_str(),
                port);

    WiFiClient client;
    //  1 = SUCCESS
    //  0 = FAILED
    // -1 = TIMED_OUT
    // -2 = INVALID_SERVER
    // -3 = TRUNCATED
    // -4 = INVALID_RESPONSE
    client.setNoDelay(true);
    client.setTimeout(10000);
    if (client.connect(targetIP, port))
    {
        Log.notice(F("Connected to: %s." CR), target->target.name);

        // Open POST connection
        if (lcburl.getAfterPath().length() > 0)
        {
            Log.verbose(F("POST /%s%s HTTP/1.1" CR),
                        lcburl.getPath().c_str(),
                        lcburl.getAfterPath().c_str());
        }
        else
        {
            Log.verbose(F("POST /%s HTTP/1.1" CR), lcburl.getPath().c_str());
        }
        client.print(F("POST /"));
        client.print(lcburl.getPath().c_str());
        if (lcburl.getAfterPath().length() > 0)
        {
            client.print(lcburl.getAfterPath().c_str());
        }
        client.println(F(" HTTP/1.1"));

        // Begin headers
        //
        // Host
        Log.verbose(F("Host: %s" CR), lcburl.getHost().c_str());
        client.print(F("Host: "));
        client.println(lcburl.getHost().c_str());
        //
        Log.verbose(F("Connection: close" CR));
        client.println(F("Connection: close"));
        // Content
        Log.verbose(F("Content-Length: %l" CR), json.length());
        client.print(F("Content-Length: "));
        client.println(json.length());
        // Content Type
        Log.verbose(F("Content-Type: application/json" CR));
        client.println(F("Content-Type: application/json"));
        // Terminate headers with a blank line
        Log.verbose(F("End headers." CR));
        client.println();
        //
        // End Headers

        // Post JSON
        client.println(json);
        // Check the HTTP status (should be "HTTP/1.1 200 OK")
        char status[32] = {0};
        client.readBytesUntil('\r', status, sizeof(status));
        client.stop();
        Log.verbose(F("Status: %s" CR), status);
        if (strcmp(status + 9, "200 OK") == 0)
        {
            if (target->checkBody.enabled == true)
            {
                // Check body
                String response = String(status);
                if (response.indexOf(target->checkBody.name) >= 0)
                {
                    Log.verbose(F("Response body ok." CR));
                    return true;
                }
                else
                {
                    Log.error(F("Unexpected body content: %s" CR), response.c_str());
                    return false;
                }
            }
            else
            {
                return true;
            }
        }
        else
        {
            Log.error(F("Unexpected status: %s" CR), status);
            return false;
        }
    }
    else
    {
        Log.warning(F("Connection failed, Host: %s, Port: %l (Err: %d)" CR),
                    lcburl.getHost().c_str(), port, client.connected());
        return false;
    }
}

void setDoURLTarget()
{                       // Do URL Target
    doURLTarget = true; // Semaphore required for Ticker + radio event
}

void setDoBFTarget()
{                      // Do Brewfather target
    doBFTarget = true; // Semaphore required for Ticker + radio event
}

void setDoBrewfTarget()
{                         // Do Brewer's Friend target
    doBrewfTarget = true; // Semaphore required for Ticker + radio event
}

void setDoReset()
{                   // Do reset
    doReset = true; // Semaphore required for reset in callback
}

void setDoOTA()
{                 // Do OTA upgrade
    doOTA = true; // Semaphore required for OTA in callback
}

void tickerLoop()
{
    Target *target = Target::getInstance();
    TSTarget *tsTarget = TSTarget::getInstance();
    BFTarget *bfTarget = BFTarget::getInstance();
    BrewfTarget *brewfTarget = BrewfTarget::getInstance();

    // Trigger Bubble check
    //
    if (doBubble)
    {
        doBubble = false;
        if (bubbles.update())
            Log.verbose(F("Current BPM is %D. Averages (%l in sample): BPM = %D, Ambient = %D, Vessel = %D." CR),
                        bubbles.lastBpm,
                        bubbles.sampleSize,
                        bubbles.getAvgBpm(),
                        bubbles.getAvgAmbient(),
                        bubbles.getAvgVessel());
    }

    // Handle JSON posts
    //
    if (doURLTarget)
    { // Do URL Target post
        doURLTarget = false;
        target->push();
    }

    if (doBFTarget)
    { // Do Brewer's Friend Post
        doBFTarget = false;
        bfTarget->push();
    }

    if (doTSTarget)
    { // Do Thingspeak Post
        doTSTarget = false;
        tsTarget->push();
    }

    if (doBrewfTarget)
    { // Do Brewfather Post
        doBrewfTarget = false;
        brewfTarget->push();
    }

    // Other maintenance
    //
    if (doReset)
    { // Check for Reset Pending
        doReset = false;
        resetController();
    }

    if (doOTA)
    { // Handle OTA update
        doOTA = false;
        execfw();
    }
}
