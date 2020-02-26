/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

#include "urltarget.h"

URLTarget* URLTarget::single = NULL;

URLTarget* URLTarget::getInstance() {
    if (!single) {
        single = new URLTarget();
        single->config = JsonConfig::getInstance();
        single->target = new PushTarget;
        single->target->ip = INADDR_NONE;

        // Enable target and target name
        single->target->target.enabled = (String(TARGETURL).length() > 3);
        strlcpy(single->target->target.name, TARGET_NAME, sizeof(TARGET_NAME));
        //
        // Check return body for success
        single->target->checkBody.enabled = CHECKBODY_ENABLED;
        strlcpy(single->target->checkBody.name, CHECKBODY_NAME, sizeof(CHECKBODY_NAME));
        //
        // Change JSON point enabled and name for target type
        single->target->apiName.enabled = APINAME_ENABLED;
        strlcpy(single->target->apiName.name, APINAME_NAME, sizeof(APINAME_NAME));
        //
        single->target->bubName.enabled = BUBNAME_ENABLED;
        strlcpy(single->target->bubName.name, BUBNAME_NAME, sizeof(BUBNAME_NAME));
        //
        single->target->bpm.enabled = BPM_ENABLED;
        strlcpy(single->target->bpm.name, BPM_NAME, sizeof(BPM_NAME));
        //
        single->target->ambientTemp.enabled = AMBIENTTEMP_ENABLED;
        strlcpy(single->target->ambientTemp.name, AMBIENTTEMP_NAME, sizeof(AMBIENTTEMP_NAME));
        //
        single->target->vesselTemp.enabled = VESSELTEMP_ENABLED;
        strlcpy(single->target->vesselTemp.name, VESSELTEMP_NAME, sizeof(VESSELTEMP_NAME));
        //
        single->target->tempFormat.enabled = TEMPFORMAT_ENABLED;
        strlcpy(single->target->tempFormat.name, TEMPFORMAT_NAME, sizeof(TEMPFORMAT_NAME));
        //
        // Grab correct URL for target type
        strlcpy(single->target->url, TARGETURL, sizeof(TARGETURL));
        //
    }
    return single;
}

bool URLTarget::push() {
    Log.verbose(F("Triggered %s push." CR), single->target->target.name);
    LCBUrl lcburl;
    if (single->target->apiName.enabled) {
        if (lcburl.setUrl(String(single->target->url))) {
            IPAddress resolvedIP = resolveHost(lcburl.getHost().c_str());
            if (resolvedIP == INADDR_NONE) {
                if (single->target->ip == INADDR_NONE) {
                    Log.error(F("Unable to resolve host %s to IP address." CR), lcburl.getHost().c_str());
                    Serial.println(); // DEBUG
                    return false;
                } else {
                    Log.verbose(F("Using cached information for host %s at IP %s." CR), lcburl.getHost().c_str(), single->target->ip.toString().c_str());
                }
            } else {
                Log.verbose(F("Resolved host %s to IP %s." CR), lcburl.getHost().c_str(), resolvedIP.toString().c_str());
                single->target->ip = resolvedIP;
            }
        } else {
            Log.error(F("Invalid URL in %s configuration: %s" CR), single->target->target.name, single->target->url);
            return false;
        }
    } else {
        Log.verbose(F("%s not enabled, skipping." CR), single->target->target.name);
        return true;
    }

    if (pushTarget(single->target, target->ip, lcburl.getPort())) {
        Log.notice(F("%s post ok." CR), single->target->target.name);
        Serial.println(); // DEBUG
        return true;
    } else {
        Log.error(F("%s post failed." CR), single->target->target.name);
        Serial.println(); // DEBUG
        return false;
    }
}
