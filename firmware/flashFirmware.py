#!/usr/bin/python

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
            app.disableButton("Continue")
            app.setLabel("title", "Please wait ...")
            app.setButton("Continue", "Working")
            t = threading.Thread(target=handleFlash)
            t.start()
        else:
            pass


def createGui(path):
    global app
    global logoname
    global appName
    if getattr(sys, 'frozen', False):
        logo = os.path.join(path, logoname, logoname)
    else:
        logo = os.path.join(path, logoname)
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
    global app
    global running
    global path
    global exe
    global chip
    global before
    global after
    global arg
    global img1
    global loc1
    global img2
    global loc2
    running = True

    # Use unbuffered
    u = Unbuffered
    sys.stderr = u(sys.__stderr__)
    sys.stdout = u(sys.__stdout__)

    if getattr(sys, 'frozen', False):
        print("DEBUG: Frozen.")
        firmware = os.path.join(path, img1, img1)
        spiffs = os.path.join(path, img2, img2)
    else:
        print("DEBUG: Not Frozen.")
        firmware = os.path.join(path, img1)
        spiffs = os.path.join(path, img2)

    # esptool.py --chip esp8266 --before default_reset --after hard_reset write_flash 0x00000000 firmware.bin 0x00300000 spiffs.bin
    try:
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

        while True:  # Loop while running
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

    except:
        print("Failed miserably, probably the fault of the user. Even so here's some information:")
        print("ESPTool: {}".format(shutil.which('esptool.py')))
        print("Chip: {}".format(chip))
        print("Before: ".format(before))
        print("After: ".format(after))
        print("Argument: ".format(arg))
        print("Location 1: ".format(loc1))
        print("Firmware 1: ".format(firmware))
        print("Location 2: ".format(loc2))
        print("SPIFFS 1: ".format(spiffs))

    app.setLabel("title", "Flash complete.")
    app.enableButton("Continue")
    app.setButton("Continue", "Exit")
    app.setButton("Continue", exit)


def exit():
    global app
    app.stop()


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
