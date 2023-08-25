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

#include "serialhandler.h"

#include "config.h"
#include "tools.h"
#include "ntp.h"
#include "version.h"
#include "thatVersion.h"
#include <ArduinoLog.h>
#include <TelnetSpy.h>
#include <sntp.h>   // sntp_get_current_timestamp()
#include <ArduinoJson.h>

#undef SERIAL
#if DOTELNET == true
TelnetSpy SerialAndTelnet;
#define SERIAL SerialAndTelnet // Use Telnet
#else
#define SERIAL Serial // Use hardware serial
#endif

#ifndef DISABLE_LOGGING

void setSerial()
{
#if DOTELNET == true
    char buffer[32];
    strcpy(buffer, (const char *)"\nConnected to ");
    strcat(buffer, (const char *)API_KEY);
    strcat(buffer, (const char *)"\n");
    SERIAL.setWelcomeMsg(buffer);
#endif
    _delay(3000); // Delay to allow a monitor to start
    SERIAL.begin(BAUD);
    SERIAL.println();
    SERIAL.flush();
#ifndef DISABLE_LOGGING
    SERIAL.setDebugOutput(false);
    // SERIAL.setDebugOutput(true);
    Log.begin(LOG_LEVEL, &SERIAL, true);
    Log.setPrefix(printPrefix);
    Log.notice(F("Serial logging started at %l." LF), BAUD);
#endif
}

void printPrefix(Print* _logOutput, int logLevel) {
    printTimestamp(_logOutput);
    // printLogLevel (_logOutput, logLevel);
}

void printLogLevel(Print* _logOutput, int logLevel) {
    /// Show log description based on log level
    switch (logLevel)
    {
        default:
        case 0:_logOutput->print("SILENT " ); break;
        case 1:_logOutput->print("FATAL "  ); break;
        case 2:_logOutput->print("ERROR "  ); break;
        case 3:_logOutput->print("WARNING "); break;
        case 4:_logOutput->print("INFO "   ); break;
        case 5:_logOutput->print("TRACE "  ); break;
        case 6:_logOutput->print("VERBOSE "); break;
    }   
}

void printTimestamp(Print *_logOutput)
{
    time_t now;
    time_t rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char locTime[22] = {'\0'};
    strftime(locTime, sizeof(locTime), "%FT%TZ ", &ts);
    _logOutput->print(locTime);
}

#else // DISABLE_LOGGING

void serial() {}

#endif // DISABLE_LOGGING

size_t printDot()
{
    return printDot(false);
}

size_t printDot(bool safe)
{
#ifndef DISABLE_LOGGING
    return SERIAL.print(F("."));
#else
    return 0;
#endif
}

size_t printChar(const char *chr)
{
    return printChar(false, chr);
}

size_t printChar(bool safe, const char *chr)
{
#ifndef DISABLE_LOGGING
    return SERIAL.println(chr);
#else
    return 0;
#endif
}

size_t printCR()
{
    return printCR(false);
}

size_t printCR(bool safe)
{
#ifndef DISABLE_LOGGING
    return SERIAL.println();
#else
    return 0;
#endif
}

void flush()
{
    flush(false);
}

void flush(bool safe)
{
    SERIAL.flush();
}

void serialLoop()
{
#if DOTELNET == true
    SerialAndTelnet.handle();
    if (SerialAndTelnet.available() > 0)
    {
#else
    if (Serial.available() > 0)
    {
#endif
        switch (SERIAL.read())
        {
        // Serial command menu
        case 'r': // Reset controller
            setDoReset();
            nullDoc("b");
            break;
        case 'p': // /ping/
            nullDoc("");
            break;
        case '?': // Help
            SERIAL.println(F("Available serial commands:"));
            SERIAL.println(F("\tp:\t'Ping, e.g. {}' (null json)"));
            SERIAL.println(F("\tr:\tRestart controller"));
            SERIAL.println(F("\t?:\tHelp (this menu)"));
            SERIAL.flush();
            break;
        default:
            break;
        }
    }
}

size_t myPrint(const __FlashStringHelper *ifsh)
{
    return SERIAL.print(ifsh);
}

size_t myPrint(const String &s)
{
    return SERIAL.print(s);
}

size_t myPrint(const char str[])
{
    return SERIAL.print(str);
}

size_t myPrint(char c)
{
    return SERIAL.print(c);
}

size_t myPrint(unsigned char b, int base)
{
    return SERIAL.print(b, base);
}

size_t myPrint(int n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(unsigned int n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(long n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(unsigned long n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(double n, int digits)
{
    return SERIAL.print(n, digits);
}

size_t myPrint(const Printable &x)
{
    return SERIAL.print(x);
}

// size_t myPrint(struct tm *timeinfo, const char *format)
// {
//     return SERIAL.print(timeinfo, format);
// }

size_t myPrintln(const __FlashStringHelper *ifsh)
{
    return SERIAL.println(ifsh);
}

size_t myPrintln(void)
{
    return SERIAL.println();
}

size_t myPrintln(const String &s)
{
    return SERIAL.println(s);
}

size_t myPrintln(const char c[])
{
    return SERIAL.println(c);
}

size_t myPrintln(char c)
{
    return SERIAL.println(c);
}

size_t myPrintln(unsigned char b, int base)
{
    return SERIAL.println(b, base);
}

size_t myPrintln(int num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(unsigned int num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(long num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(unsigned long num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(double num, int digits)
{
    return SERIAL.println(num, digits);
}

size_t myPrintln(const Printable &x)
{
    return SERIAL.println(x);
}

// size_t myPrintln(struct tm *timeinfo, const char *format)
// {
//     return SERIAL.println(timeinfo, format);
// }

void nullDoc(const char *wrapper)
{
    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity);
    doc[wrapper] = nullptr;
    serializeJson(doc, SERIAL);
    printCR();
}
