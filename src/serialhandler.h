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

#ifndef _SERIALHANDLER_H
#define _SERIALHANDLER_H

#include "config.h"
#include "tools.h"
#include "ntp.h"
#include "version.h"
#include "thatVersion.h"
#include <ArduinoLog.h>
#include <esptelnet.h>
#include <sntp.h>   // sntp_get_current_timestamp()

void setSerial();
void printTimestamp(Print *_logOutput);

void printTimestamp(Print *_logOutput);
void serialLoop();

extern struct ThatVersion thatVersion;
extern const char *resetReason[7];
extern const char *resetDescription[7];

// Print outputs
size_t printChar(bool, const char *);
size_t printChar(const char *);
size_t printDot();
size_t printDot(bool);
size_t printCR();
size_t printCR(bool);
void flush();
void flush(bool);

// Redefine Serial.print*() functions
size_t myPrint(const __FlashStringHelper *ifsh);
size_t myPrint(const String &s);
size_t myPrint(const char str[]);
size_t myPrint(char c);
size_t myPrint(unsigned char b, int base);
size_t myPrint(int n, int base);
size_t myPrint(unsigned int n, int base);
size_t myPrint(long n, int base);
size_t myPrint(unsigned long n, int base);
size_t myPrint(double n, int digits);
size_t myPrint(const Printable &x);
// size_t myPrint(struct tm *timeinfo, const char *format);
// size_t myPrintf(const char *format, ...);
size_t myPrintln(void);
size_t myPrintln(const __FlashStringHelper *ifsh);
size_t myPrintln(const String &s);
size_t myPrintln(const char c[]);
size_t myPrintln(char c);
size_t myPrintln(unsigned char b, int base);
size_t myPrintln(int num, int base);
size_t myPrintln(unsigned int num, int base);
size_t myPrintln(long num, int base);
size_t myPrintln(unsigned long num, int base);
size_t myPrintln(double num, int digits);
size_t myPrintln(const Printable &x);
// size_t myPrintln(struct tm *timeinfo, const char *format);
void nullDoc(const char *);

#define prefLen 22

#endif //_SERIALHANDLER_H
