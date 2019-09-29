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
// Define API Key (identifies application to target)
//
#ifndef API_KEY
#define API_KEY "Brew Bubbles"
#endif
//
//////////////////////////////////////////////////////////////////////////
 
//////////////////////////////////////////////////////////////////////////
//
// Define data source (identifies sensor source to target)
//
#ifndef SOURCE
#define SOURCE "Brew Bubbles"
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
// Set Config json size
//
#ifndef CONFIGJSON
#define CONFIGJSON 768
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set Bubble JSON size
//
#ifndef BUBBLEJSON
#define BUBBLEJSON 277
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set Version JSON size
//
#ifndef VERSIONJSON
#define VERSIONJSON 30
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set Target JSON size
//
#ifndef TJSON
#define TJSON 300
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set BF JSON size
//
#ifndef BFJSON
#define BFJSON 300
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set serial baud rate
//
#ifndef BAUD
#define BAUD 74880
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Access Point name
//
#ifndef APNAME
#define APNAME "brewbubbles"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Access point password
//
#ifndef AP_PASSWD
#define AP_PASSWD "brewbubbles"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Set Version JSON location
//
#ifndef VERSIONJSONLOC
#define VERSIONJSONLOC "http://www.brewbubbles.com/firmware/version.json"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Firmware URL
//
#ifndef FIRMWAREURL
#define FIRMWAREURL "http://www.brewbubbles.com/firmware/firmware.bin"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// SPIFFS URL
//
#ifndef SPIFFSURL
#define SPIFFSURL "http://www.brewbubbles.com/firmware/spiffs.bin"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Default host name
//
#ifndef HOSTNAME
#define HOSTNAME "brewbubbles"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Default Bubble name
//
#ifndef BUBNAME
#define BUBNAME "Fermenter 1"
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
// Define temperature format (true = Farenheit, false = Celcius)
//
#ifndef TEMPFORMAT
#define TEMPFORMAT true
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
// Define reset wifi on boot pin - reset if pulled low on boot
//
#ifndef RESETWIFI
#define RESETWIFI D5
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Bubble loop time in milliseconds - Must be declared as an UL
//
#ifndef BUBLOOP
#define BUBLOOP 60 // One minute
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Number of Ppm readings in a sliding window for average
//
#ifndef BUBAVG
#define BUBAVG 15
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Number of temp readings in a sliding window for averaging
//
#ifndef TEMPAVG
#define TEMPAVG 60
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
// Frequency with which to blink LED when in Access Point blocking loop
//
#ifndef APBLINK
#define APBLINK 1000UL // 1000ms each state = 2000ms per cycle = 0.5Hz
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Frequency with which to blink LED when in Station Connect blocking loop
//
#ifndef STABLINK
#define STABLINK 333UL // 333ms each state = 666ms per cycle = ~1.5Hz
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Frequency with which to blink LED when in NTP blocking loop
//
#ifndef NTPBLINK
#define NTPBLINK 50UL // 50ms each state = 100ms per cycle = 10Hz
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Port for local web services
//
#ifndef PORT
#define PORT 80
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Target Frequency (default 60 seconds)
//
#ifndef TARGETFREQ
#define TARGETFREQ 60
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Brewer's Friend Frequency (default 15 mins)
//
#ifndef BFFREQ
#define BFFREQ 15
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Brewer's Friend URL
//
#ifndef BFURL
#define BFURL "log.brewersfriend.com/stream/"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// NTP Server
//
#ifndef TIMESERVER
#define TIMESERVER "pool.ntp.org"
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Output pin for LED
//
#ifndef LED
#define LED LED_BUILTIN
#endif
//
//////////////////////////////////////////////////////////////////////////

#endif // CONFIG_H
