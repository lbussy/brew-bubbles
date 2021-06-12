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

#include "version.h"

static const char *filename = VERSIONJSON;
static char fs_ver[32];

const char *project() { return stringify(PIO_SRC_NAM); }
const char *fw_version() { return stringify(PIO_SRC_TAG); }
const char *fs_version()
{
    fsver();
    return (fs_ver);
}
const char *branch() { return stringify(PIO_SRC_BRH); }
const char *build() { return stringify(PIO_SRC_REV); }
const char *board() { return stringify(PIO_BOARD); }

void fsver()
{
    StaticJsonDocument<96> doc;
    // Filesystem Version
    if (LittleFS.begin())
    {
        // Loads the configuration from a file on LittleFS
        File file = LittleFS.open(filename, "r");
        if (LittleFS.exists(filename) || !file)
        {
            // Parse the JSON object in the file
            DeserializationError err = deserializeJson(doc, file);

            if (!err)
            {
                if (!doc["fs_version"].isNull())
                {
                    const char *fs = doc["fs_version"];
                    strlcpy(fs_ver, fs, sizeof(fs_ver));
                    file.close();
                    return;
                }
            }
        }
        else
        {
            Log.warning(F("Filesystem version not available." CR));
            file.close();
            File file = LittleFS.open(filename, "w");
            strlcpy(fs_ver, "0.0.0", sizeof(fs_ver));
            doc["fs_version"] = fs_ver;
            serializeJsonPretty(doc, file);
            file.close();
        }
    }
    else
    {
        Log.error(F("Filesystem not available." CR));
        strlcpy(fs_ver, "No FS", sizeof(fs_ver));
    }
    return;
}

/*
 * versionCompare: Compares two strings representing a semantic version
 *
 * Arguments:
 *      String v1:  String in nn.nn.nn format containing version to be
 *                  compared against
 *      String v2:  String in nn.nn.nn formatcontaining version to compare
 *
 * Returns:
 *      -1: String v1 < String v2
 *       0: String v1 == String v2
 *       1: String v1 > String v2
 */
int versionCompare(String v1, String v2)
{
    // vnum stores each numeric part of the version
    unsigned int vnum1 = 0, vnum2 = 0;

    //  Loop until both string are processed
    for (unsigned int i = 0, j = 0; (i < v1.length() || j < v2.length());)
    {
        // Store numeric part of version 1 in vnum1
        while (i < v1.length() && v1[i] != '.')
        {
            vnum1 = vnum1 * 10 + (v1[i] - '0');
            i++;
        }

        // Store numeric part of version 2 in vnum2
        while (j < v2.length() && v2[j] != '.')
        {
            vnum2 = vnum2 * 10 + (v2[j] - '0');
            j++;
        }

        if (vnum1 > vnum2)
            return 1;
        if (vnum2 > vnum1)
            return -1;

        // If equal, reset variables and go for next numeric part
        vnum1 = vnum2 = 0;
        i++;
        j++;
    }
    return 0;
}
