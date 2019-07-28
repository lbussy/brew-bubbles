#!/bin/bash

# Copyright (C) 2019 Lee C. Bussy (@LBussy)

# This file is part of Lee Bussy's Brew Bubbles (Brew-Bubbles).
#
# Brew Bubbles is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# Brew Bubbles is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with Brew Bubbles. If not, see <https://www.gnu.org/licenses/>.

exe="spiffsgen.py"      # ESP32 partition table generation tool
spiffs="spiffs.bin"     # Output file
base="data"             # Base directory to add to populate SPIFFS
out="bin"               # Output directory
kb=188                  # SPIFFS size in KB
#kb=196                 # SPIFFS size in KB
tools="tools"           # Tools directory (where these files live)

size=$(( kb * 1024 ))                   # Get SPIFFS size in bytes
root=".."
#root=$(git rev-parse --show-toplevel)  # Get root of project
data="$root/$base"                      # Concat output file
spiffs="$root/$out/$spiffs"

exe="$root/$tools/$exe"
csv="$root/$tools/$csv"
parts="$root/$out/$parts"

command -v "$exe" > /dev/null 2>&1
retval="$?"
if [[ "$retval" -gt 0 ]]; then
    echo -e "\nERROR: $exe not found."
    exit 1
else
    cmd="$exe $size $data $spiffs"
    echo -e "\nCreating SPIFFS file as $spiffs.\n"
    eval "$cmd"
fi
