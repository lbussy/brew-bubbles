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
        single->target->target.enabled = (String(single->targeturl).length() > 3);
        strlcpy(single->target->target.name, single->target_name, sizeof(single->target_name));
        //
        // Check return body for success
        single->target->checkBody.enabled = single->checkbody_enabled;
        strlcpy(single->target->checkBody.name, single->checkbody_name, sizeof(single->checkbody_name));
        //
        // Change JSON point enabled and name for target type
        single->target->apiName.enabled = single->apiname_enabled;
        strlcpy(single->target->apiName.name, single->apiname_name, sizeof(single->apiname_name));
        //
        single->target->bubName.enabled = single->bubname_enabled;
        strlcpy(single->target->bubName.name, single->bubname_name, sizeof(single->bubname_name));
        //
        single->target->bpm.enabled = single->bpm_enabled;
        strlcpy(single->target->bpm.name, single->bpm_name, sizeof(single->bpm_name));
        //
        single->target->ambientTemp.enabled = single->ambienttemp_enabled;
        strlcpy(single->target->ambientTemp.name, single->ambienttemp_name, sizeof(single->ambienttemp_name));
        //
        single->target->vesselTemp.enabled = single->vesseltemp_enabled;
        strlcpy(single->target->vesselTemp.name, single->vesseltemp_name, sizeof(single->vesseltemp_name));
        //
        single->target->tempFormat.enabled = single->tempformat_enabled;
        strlcpy(single->target->tempFormat.name, single->tempformat_name, sizeof(single->tempformat_name));
        //
        // Grab correct URL for target type
        strlcpy(single->target->url, single->targeturl, sizeof(single->targeturl));
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

    if (pushToTarget(single->target, target->ip, lcburl.getPort())) {
        Log.notice(F("%s post ok." CR), single->target->target.name);
        return true;
    } else {
        Log.error(F("%s post failed." CR), single->target->target.name);
        return false;
    }
}
