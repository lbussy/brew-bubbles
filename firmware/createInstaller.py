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

from shutil import copyfile
import sys
from pathlib import Path
import os
import time
import PyInstaller.__main__
import subprocess
import shutil

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
firmwarePath = r"{0}\.pio\build\{1}".format(parentPath, env)
graphicsPath = r"{0}\graphics".format(parentPath)
iconPath = r"{0}\icons".format(graphicsPath)


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
    except FileNotFoundError:
        print("ERROR: File not found: {0} does not exist.".format(source))
    except IOError:
        print("ERROR: Unable to write file {0}.".format(dest))
    except:
        print("ERROR: Unhandled error copying {0}".format(firmware))


def copyFiles():
    firmwareSrc = "{0}\{1}".format(firmwarePath, firmware)
    firmwareDest = "{0}\{1}".format(currentPath, firmware)
    handleCp(firmwareSrc, firmwareDest)
    spiffsSrc = "{0}\{1}".format(firmwarePath, spiffs)
    spiffsDest = "{0}\{1}".format(currentPath, spiffs)
    graphicsSrc = "{0}\{1}".format(graphicsPath, logo)
    graphicsDest = "{0}\{1}".format(currentPath, logo)
    iconSrc = "{0}\{1}".format(iconPath, icon)
    iconDest = "{0}\{1}".format(currentPath, icon)
    handleCp(spiffsSrc, spiffsDest)
    handleCp(graphicsSrc, graphicsDest)
    handleCp(iconSrc, iconDest)


def freezeFlasher():
    sourceInstaller = "{0}\dist\{1}.exe".format(currentPath, installer)
    destInstaller = "{0}\{1}.exe".format(currentPath, installer)

    #subprocess.run("pyinstaller --icon favicon.ico --add-binary logo.gif;logo.gif --add-binary firmware.bin;firmware.bin --add-binary spiffs.bin;spiffs.bin --noupx -y flashFirmware.py")
    # TODO:  Use version
    # TODO:  Fix logo
    # TODO:  Fix icon
    # TODO:  Use --onefile

    process = subprocess.Popen([
            shutil.which('pyinstaller'),
            '--onefile',
            '--icon',
            './favicon.ico',
            '--add-binary',
            './logo.gif;./logo.gif',
            '--add-binary',
            './firmware.bin;./firmware.bin',
            '--add-binary',
            './spiffs.bin;./spiffs.bin',
            '--noupx',
            '-y',
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
