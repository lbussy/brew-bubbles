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

        // /////////////////////////////////////////////////////////////////////
        // //  Configure Target
        // /////////////////////////////////////////////////////////////////////

        String _tempVal;
        // Check return body for success
        single->target->checkBody.enabled = false;
        strlcpy(single->target->checkBody.name, "", sizeof(""));
        //
        // Grab correct URL for target type
        strlcpy(single->target->url, single->config->targeturl, sizeof(single->config->targeturl));
        //
        // Enable target and target name
        single->target->target.enabled = true;
        strlcpy(single->target->target.name, URLTARGET, sizeof(URLTARGET));
        //
        // Change JSON point enabled and name for target type
        single->target->apiName.enabled = (String(single->target->url).length() > 3);
        strlcpy(single->target->apiName.name, "api_key", sizeof("api_key"));
        //
        single->target->bubName.enabled = true;
        strlcpy(single->target->bubName.name, "name", sizeof("name"));
        //
        single->target->bpm.enabled = true;
        strlcpy(single->target->bpm.name, "bpm", sizeof("bpm"));
        //
        single->target->ambientTemp.enabled = true;
        strlcpy(single->target->ambientTemp.name, "ambient", sizeof("ambient"));
        //
        single->target->vesselTemp.enabled = true;
        strlcpy(single->target->vesselTemp.name, "temp", sizeof("temp"));
        //
        single->target->tempFormat.enabled = true;
        strlcpy(single->target->tempFormat.name, "temp_unit", sizeof("temp_unit"));
    }
    return single;
}

bool URLTarget::push() {
    Log.verbose(F("Triggered %s push." CR), single->target->target.name);
    if (single->target->apiName.enabled) {
        LCBUrl lcburl;
        if (lcburl.setUrl(String(single->target->url))) {
            single->target->ip = resolveHost(single->target);
            if (single->target->ip != INADDR_NONE) {
                Log.verbose(F("Resolved host %s to IP %s." CR), lcburl.getHost().c_str(), single->target->ip.toString().c_str());
                if (pushTarget(single->target)) {
                    Log.notice(F("%s post ok." CR), single->target->target.name);
                    return true;
                } else {
                    Log.error(F("%s post failed." CR), single->target->target.name);
                    return false;
                }
            } else {
                Log.error(F("Unable to resolve host %s to IP address." CR), lcburl.getHost().c_str());
                return false;
            }
        } else {
            Log.error(F("Invalid URL in %s configuration: %s" CR), single->target->target.name, single->target->url);
            return false;
        }
    } else {
        Log.verbose(F("%s not enabled, skipping." CR), single->target->target.name);
        return false;
    }
}
