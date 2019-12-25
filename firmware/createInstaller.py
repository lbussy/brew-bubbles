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

from shutil import copyfile, copymode
import sys
from pathlib import Path
import os, platform
import time
import PyInstaller.__main__
import subprocess
import shutil
import json

# Get Environment
env = "d1_mini"
# Get Firmware names
firmware = "firmware.bin"
spiffs = "spiffs.bin"
installer = "flashFirmware"
icon = "favicon.ico"
logo = "logo.gif"
# Concatenate paths
currentPath = os.path.dirname(os.path.realpath(__file__))
path = Path(currentPath)
parentPath = path.parent

# Get 0.0.0 version from latest Git tag
tagcmd = "git describe --tags --abbrev=0"
version = subprocess.check_output(tagcmd, shell=True).decode().strip()
jsonVersion = {
    'version': version
}
with open('version.json', 'w') as json_file:
    json.dump(jsonVersion, json_file)

# Paths used for copying over new builds to the relevant (this) folder
# firmwarePath = r"{0}\.pio\build\{1}".format(parentPath, env)
firmwarePath = os.path.join(parentPath, ".pio", "build", env)
# graphicsPath = r"{0}\graphics".format(parentPath)
graphicsPath = os.path.join(parentPath, "graphics")
# iconPath = r"{0}\icons".format(graphicsPath)
iconPath = os.path.join(graphicsPath, "icons")


executable_suffix = ""  # Used when finding the generated executable (only a thing on Windows)

if platform.system() == "Darwin":
    system_suffix = "_mac"
elif platform.system() == "Windows":
    executable_suffix = ".exe"
    system_suffix = "_win.exe"


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


# Use unbuffered
u = Unbuffered
sys.stderr = u(sys.__stderr__)
sys.stdout = u(sys.__stdout__)


def handleCp(source, dest):
    try:
        copyfile(source, dest)
        copymode(source, dest)
    except FileNotFoundError:
        print("ERROR: File not found: {0} does not exist.".format(source))
    except IOError:
        print("ERROR: Unable to write file {0}.".format(dest))
    except:
        print("ERROR: Unhandled error copying {0}".format(firmware))


def copyFiles():
    firmwareSrc = os.path.join(firmwarePath, firmware)
    firmwareDest = os.path.join(currentPath, firmware)
    handleCp(firmwareSrc, firmwareDest)
    spiffsSrc = os.path.join(firmwarePath, spiffs)
    spiffsDest = os.path.join(currentPath, spiffs)
    graphicsSrc = os.path.join(graphicsPath, logo)
    graphicsDest = os.path.join(currentPath, logo)
    iconSrc = os.path.join(iconPath, icon)
    iconDest = os.path.join(currentPath, icon)
    handleCp(spiffsSrc, spiffsDest)
    handleCp(graphicsSrc, graphicsDest)
    handleCp(iconSrc, iconDest)


def freezeFlasher():
    # sourceInstaller = "{0}\dist\{1}.exe".format(currentPath, installer)
    sourceInstaller = os.path.join(currentPath, "dist", installer) + executable_suffix
    # destInstaller = "{0}\{1}.exe".format(currentPath, installer)
    destInstaller = os.path.join(currentPath, installer) + system_suffix

    # TODO:  Use version
    # TODO:  Fix icon

    process = subprocess.Popen([
            shutil.which('pyinstaller'),
            '--onefile',
            '--icon',
            './favicon.ico',
            '--add-data',
            './logo.gif' + os.pathsep + './logo.gif',
            '--add-data',
            './firmware.bin' + os.pathsep + './firmware.bin',
            '--add-data',
            './spiffs.bin' + os.pathsep + './spiffs.bin',
            '--noupx',
            '-y',
            '--debug=imports',
            'flashFirmware.py'
        ],
        stdout=subprocess.PIPE,
        universal_newlines=True)

    while True:
        output = process.stdout.readline()
        print(output.strip())
        # Do something else
        return_code = process.poll()
        if return_code is not None:
            print('Return code: {0}'.format(return_code))
            # Process has finished, read rest of the output 
            for output in process.stdout.readlines():
                print(output.strip())
            break

    handleCp(sourceInstaller, destInstaller) # Copy exe back to ./


def main():
    copyFiles()
    freezeFlasher()


if __name__ == '__main__':
    main()
    sys.exit(0)
