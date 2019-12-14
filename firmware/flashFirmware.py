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

exe = 'esptool.py'          # esptool.py v2.7
chip = 'esp8266'            # {auto,esp8266,esp32}
before = 'default_reset'    # {hard_reset,soft_reset,no_reset}
after = 'hard_reset'        # {hard_reset,soft_reset,no_reset}
arg = 'write_flash'         # Execute

img1 = 'firmware.bin'       # Firmware name
loc1 = '0x00000000'         # Address for Firmware
img2 = 'spiffs.bin'         # SPIFFS name
loc2 = '0x00300000'         # Address for SPIFFs

logoname = 'logo.gif'               # Logo for GUI
appName = 'Controller Flash Tool'   # Used for title bar

import subprocess
import os
import sys
import time
from appJar import gui
import shlex
from subprocess import Popen, PIPE
import shutil
import threading

path = None
app = None
running = False

class Unbuffered(object):
   def __init__(self, stream):
       self.stream = stream
   def write(self, data):
       self.stream.write(data)
       self.stream.flush()
   def writelines(self, datas):
       self.stream.writelines(datas)
       self.stream.flush()
   def __getattr__(self, attr):
       return getattr(self.stream, attr)


def getPath():
    ospath = os.path.dirname(os.path.realpath(__file__))
    if getattr(sys, 'frozen', False):
        path = sys._MEIPASS
    else:
        path = ospath
    return path

def press(button):
    global running
    if not running:
        if button == "Continue":
            t = threading.Thread(target=handleFlash)
            t.start()
        else:
            pass

def createGui(path):
    global app
    if getattr(sys, 'frozen', False):
        logo = "{0}\{1}\{2}".format(path, logoname, logoname)
    else:
        logo = "{0}\{1}".format(path, logoname)
    # Overall attributes
    app.setBg("white")
    app.setFont(18)
    # Add Image
    app.startLabelFrame("", 0, 0)
    app.addImage("", logo)
    app.stopLabelFrame()
    # Add & configure widgets
    app.addLabel("title", "Press 'Continue' to flash firmware")
    app.setLabelBg("title", "orange")
    # Link the buttons to the function called press
    app.addNamedButton("Continue", "Continue", press)
    # Start the GUI
    app.go()

def handleFlash():
    global running
    global path
    running = True

    # Use unbuffered
    u = Unbuffered
    sys.stderr = u(sys.__stderr__)
    sys.stdout = u(sys.__stdout__)

    if getattr(sys, 'frozen', False):
        firmware = "{0}\{1}\{2}".format(path, img1, img1)
        spiffs = "{0}\{1}\{2}".format(path, img2, img2)
    else:
        firmware = "{0}\{1}".format(path, img1)
        spiffs = "{0}\{1}".format(path, img2)

    # esptool.py --chip esp8266 --before default_reset --after hard_reset write_flash 0x00000000 firmware.bin 0x00300000 spiffs.bin
    process = subprocess.Popen([
            shutil.which('esptool.py'),
            '--chip', chip,
            '--before', before,
            '--after', after,
            arg,
            loc1, firmware,
            loc2, spiffs
        ],
        stdout=subprocess.PIPE,
        universal_newlines=True)

    while True: # Loop while running
        output = process.stdout.readline()
        print(output.strip())
        # TODO:  Maybe status updates according to output?
        return_code = process.poll()
        if return_code is not None:
            print('Return code = {0}'.format(return_code))
            # Process has finished, read rest of the output 
            for output in process.stdout.readlines():
                print(output.strip())
            break

def main():
    global app
    global path
    global appName
    path = getPath()
    app = gui(appName)
    createGui(path)

if __name__ == "__main__":
    # execute only if run as a script
    main()
    sys.exit(0)
