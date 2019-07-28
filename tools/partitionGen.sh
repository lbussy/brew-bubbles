#!/bin/bash

# Copyright (C) 2019 Lee C. Bussy (@LBussy)

# This file is part of Lee Bussy's Brew Bubbles (Brew-Bubbles).

# Brew Bubbles is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.

# Brew Bubbles is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with Brew Bubbles. If not, see <https://www.gnu.org/licenses/>.

exe="gen_esp32part.py"  # ESP32 partition table generation tool
csv="min_spiffs.csv"    # Partition table input
bin="partitions.bin"    # Partition bin file
out="bin"               # Output directory
tools="tools"           # Tools directory (where these files live)

root=".."
#root=$(git rev-parse --show-toplevel)

exe="$root/tools/$exe"
csv="$root/tools/$csv"
bin="$root/$out/$bin"

command -v "$exe" > /dev/null 2>&1
retval="$?"
if [[ "$retval" -gt 0 ]]; then
    echo -e "\nERROR: $exe not found."
    exit 1
else
    cmd="$exe $csv $bin"
    echo -e "\nCreating partition table as $bin.\n"
    eval "$cmd"
fi
