#!/bin/bash

# Copyright (C) 2019 Lee C. Bussy (@LBussy)

# This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

exe="esptool.py"        # esptool.py v2.7 - ESP8266 ROM Bootloader Utility
chip="esp8266"          # {auto,esp8266,esp32}
before="default_reset"  # {hard_reset,soft_reset,no_reset}
after="hard_reset"      # {hard_reset,soft_reset,no_reset}
arg="write_flash"       # Positional arguments:
                        #   {load_ram,dump_mem,read_mem,write_mem,write_flash,py
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

# "esptool.py --chip esp8266 --before default_reset --after $after $arg $loc1 $img1 $loc2 $img2"