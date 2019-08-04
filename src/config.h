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

#ifndef CONFIG_H
#define CONFIG_H

//////////////////////////////////////////////////////////////////////////
//
// Set verbosity of debug messages 0-6
//
//      * 0 - LOG_LEVEL_SILENT     no output 
//      * 1 - LOG_LEVEL_FATAL      fatal errors 
//      * 2 - LOG_LEVEL_ERROR      all errors  
//      * 3 - LOG_LEVEL_WARNING    errors, and warnings 
//      * 4 - LOG_LEVEL_NOTICE     errors, warnings and notices 
//      * 5 - LOG_LEVEL_TRACE      errors, warnings, notices & traces 
//      * 6 - LOG_LEVEL_VERBOSE    all 
//
// Uncomment #define DISABLE_LOGGING to remove all logging
//
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_VERBOSE
// #define DISABLE_LOGGING
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Define data source (identifies JSON type to target)
//
#ifndef API_KEY
#define API_KEY "brewbubbles"
#endif
//
//////////////////////////////////////////////////////////////////////////
 
//////////////////////////////////////////////////////////////////////////
//
// Define build version (Git tag from PIO Env)
//
#ifndef VERSION_STRING
#define VERSION_STRING PIO_SRC_TAG
#endif
//
//////////////////////////////////////////////////////////////////////////
 
//////////////////////////////////////////////////////////////////////////
//
// Define build commit (Git commit from PIO Env)
//
#ifndef BUILD_NAME
#define BUILD_NAME PIO_SRC_REV
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set serial baud rate
//
#ifndef BAUD
#define BAUD 115200
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Host name default
//
#ifndef HOSTNAME
#define HOSTNAME "brewbubbles"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// AP default password
//
#ifndef AP_PASSWD
#define AP_PASSWD "br3wbubbl3s"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Input pin for counter
//
#ifndef COUNTPIN
#define COUNTPIN D1
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Define whether to report temperatures
//
#ifndef READTEMP
#define READTEMP
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Define temperature format
//
#ifndef TEMPFORMAT
#define TEMPFORMAT F
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Define ambient temp sensor pin
//
#ifndef AMBSENSOR
#define AMBSENSOR D2 // Uncomment to use Ambient temp sensor
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Define vessel temp sensor pin
//
#ifndef VESSENSOR
#define VESSENSOR D3 // Uncomment to use vessel temp sensor
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Bubble loop time in milliseconds - Must be declared as an UL
//
#ifndef BUBLOOP
#define BUBLOOP 60000UL // One minute
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Resolution time in microseconds (debouncer) - Must be declared as an UL
//
#ifndef RESOLUTION
#define RESOLUTION 500UL
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Port for services
//
#ifndef PORT
#define PORT 80
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// HTTP endpoint address
//
#ifndef HTTPTARGET
#define HTTPTARGET "http://192.168.168.132/brew-bubbles.php"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// HTTP endpoint port
//
#ifndef HTTPTARGETPORT
#define HTTPTARGETPORT 80U
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// DO NTP
//
#ifndef DO_NTP
#define DO_NTP true
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// NTP Server
//
#ifndef NTP_SERVER
#define NTP_SERVER "pool.ntp.org"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Output pin for LED
//
#ifndef LEDPIN
#define LEDPIN LED_BUILTIN
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Serial delay time in millisecond
//
#ifndef SERDELAY
#define SERDELAY 500UL
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Load defaults for items not set here
//
#ifndef DEFAULTS_H
#include "defaults.h"
#define DEFAULTS_H
#endif // DEFAULTS_H
//
//////////////////////////////////////////////////////////////////////////

#endif // CONFIG_H
