#!/usr/bin/python

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

import subprocess
import os
import sys
import time

exe = 'esptool.py'          # esptool.py v2.7 - ESP8266 ROM Bootloader Utility
chip = 'auto'               # {auto,esp8266,esp32}
before = 'default_reset'    # {hard_reset,soft_reset,no_reset}
after = 'hard_reset'        # {hard_reset,soft_reset,no_reset}
arg = 'write_flash'         # Execute

img1 = 'firmware.bin'
img2 = 'spiffs.bin'
loc1 = '0x00000000'
loc2 = '0x00300000'

cmd = exe + ' --chip ' + chip + ' --before ' + before + ' --after ' + \
    after + ' ' + arg + ' ' + loc1 + ' ' + img1 + ' ' + loc2 + ' ' + img2

try:
    FNULL = open(os.devnull, 'w')
    x = subprocess.check_call(exe + ' -h', shell=True, stdout=FNULL, stderr=FNULL)
except subprocess.CalledProcessError:
    print('\nError:  ' + exe + ' not found.')
    sys.exit(-1)
except:
    print('Unknown error detected.')
    sys.exit(-1)
else:
    try:
        print('\nFlashing controller.  This may take a minute.')
        env = os.environ
        subprocess.check_output(cmd, shell=True) # ), stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    except subprocess.CalledProcessError:
        print('\nError:  ' + exe + ' returned a process error.')
        sys.exit(-1)        
    except:
        print('\nUnknown error detected while flashing controller.')
        print('Waiting to exit at except.')
        sys.exit(-1)
    else:
        print('\nController flash completed successfully.')

print('Waiting to exit.')
time.sleep(10)

sys.exit(0)
