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

#include "version.h"

const char* build() {return stringify(PIO_SRC_REV);}
const char* branch() {return stringify(PIO_SRC_BRH);}
const char* version() {return stringify(PIO_SRC_TAG);}

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
int versionCompare(String v1, String v2) {
    // vnum stores each numeric part of the version
    unsigned int vnum1 = 0, vnum2 = 0;

    //  Loop until both string are processed
    for (unsigned int i = 0, j = 0; (i < v1.length() || j < v2.length());) {
        // Store numeric part of version 1 in vnum1
        while (i < v1.length() && v1[i] != '.') {
            vnum1 = vnum1 * 10 + (v1[i] - '0');
            i++;
        }

        // Store numeric part of version 2 in vnum2
        while (j < v2.length() && v2[j] != '.') {
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
