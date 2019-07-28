# Brew Bubbles Tools

[![Brew Bubbles Logo](http://www.brewbubbles.com/static/img/brewbubbles_logo.png "Brew Bubbles")](http://www.brewbubbles.com/)

[![Documentation Status](https://readthedocs.org/projects/brew-bubles/badge/?version=master)](http://brewbubbles.readthedocs.io/en/master/?badge=master)
                
## Tools in this Directory

**File Name**|**Description**
-----|-----
`partitionGen.sh`|Creates partition table .bin file for ESP-32 based on {git base}/tools/min\_spiffs.csv using `gen_esp32part.py`.  Moves .bin file to {git base}/bin directory.
`spiffsgen.sh`|Generates SPIFFS volume for ESP-32 storage.  Takes {git base}/data as input and moves .bin file to {git base}/bin directory.
`flashFirmware.sh`|Uses `esptool.py` to flash firmware files to ESP-32.  Uses files in {git base}/bin directory.
[min_spiffs.csv](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/min_spiffs.csv)|Input file to generate partitions.  Used by `partitionGen.sh`.
[gen\_esp32part.py](https://github.com/espressif/esp-idf/blob/master/components/partition_table/gen_esp32part.py)|Does the work of creating the partition table .bin file.
[spiffsgen.py](https://github.com/espressif/esp-idf/blob/master/components/spiffs/spiffsgen.py)|Does the work of creating the SPIFFS volume .bin file.