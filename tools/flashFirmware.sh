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

exe="esptool.py"        # esptool.py v2.7 - ESP8266 ROM Bootloader Utility
chip="esp8266"          # {auto,esp8266,esp32}
before="default_reset"  # {hard_reset,soft_reset,no_reset}
after="hard_reset"      # {hard_reset,soft_reset,no_reset}
arg="write_flash"       # Positional arguments:
                        #   {load_ram,dump_mem,read_mem,write_mem,write_flash,
                        #   run,image_info,make_image,elf2image,read_mac,
                        #   chip_id,flash_id,read_flash_status,
                        #   write_flash_status,read_flash,verify_flash,
                        #   erase_flash,erase_region,version}
                        # Run esptool {command} -h for additional help    
img1="firmware.bin"
img2="spiffs.bin"
loc1="0x00000000"
loc2="0x00300000"

root=".."
buildloc=".pio/build/d1_mini"
#root=$(git rev-parse --show-toplevel)
img1="$root/$buildloc/$img1"
img2="$root/$buildloc/$img2"

command -v "$exe" > /dev/null 2>&1
retval="$?"
if [[ "$retval" -gt 0 ]]; then
    echo -e "\nERROR: $exe not found."
    exit 1
else
    cmd="$exe --chip $chip --before $before --after $after $arg $loc1 $img1 $loc2 $img2"
    echo -e "\nFlashing firmware to $chip.\n"
    eval "$cmd"
fi
