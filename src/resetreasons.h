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

#ifndef _RESETREASONS_H
#define _RESETREASONS_H

const char *resetReason[7] = {
    "REASON_DEFAULT_RST",       // = 0, /* normal startup by power on */
    "REASON_WDT_RST",           // = 1, /* hardware watch dog reset */
    "REASON_EXCEPTION_RST",     // = 2, /* exception reset, GPIO status won’t change */
    "REASON_SOFT_WDT_RST",      // = 3, /* software watch dog reset, GPIO status won’t change */
    "REASON_SOFT_RESTART",      // = 4, /* software restart ,system_restart , GPIO status won’t change */
    "REASON_DEEP_SLEEP_AWAKE",  // = 5, /* wake up from deep-sleep */
    "REASON_EXT_SYS_RST"        //  = 6 /* external system reset */
};

const char *resetDescription[7] = {
    "Normal startup by power on",
    "Hardware watch dog reset",
    "Exception reset, GPIO status won’t change",
    "Software watch dog reset, GPIO status won’t change",
    "Software restart, system_restart, GPIO status won’t change",
    "Wake up from deep-sleep",
    "External system reset"};

#endif // _RESETREASONS_H
