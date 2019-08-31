#!/usr/bin/python

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

import subprocess

# Get 0.0.0 version from latest Git tag
tagcmd = "git describe --tags --abbrev=0"
version = subprocess.check_output(tagcmd, shell=True).decode().strip()

# Get latest commit short from Git
revcmd = "git log --pretty=format:'%h' -n 1"
commit = subprocess.check_output(revcmd, shell=True).decode().strip()

# Get branch name from Git
branchcmd = "git rev-parse --abbrev-ref HEAD"
branch = subprocess.check_output(branchcmd, shell=True).decode().strip()

# Make all available for use in the macros
print("-DPIO_SRC_TAG={0}".format(version))
print("-DPIO_SRC_REV={0}".format(commit))
print("-DPIO_SRC_BRH={0}".format(branch))
