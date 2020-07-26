[![Brew Bubbles Logo](https://www.brewbubbles.com/wp-content/uploads/2019/08/BB-full-logo.png "Brew Bubbles")](http://www.brewbubbles.com/)

## Brew Bubbles Firmware

This directory contains the firmware files and scripts to prepare the controller. Instructions to program the controller are contained in [the documentaiton](https://docs.brewbubbles.com).

## Flashing Manually

If you have ``esptool.py`` on yor system, you may use the following command line:

```
esptool.py --chip esp8266 --before default_reset --after hard_reset write_flash 0x00000000 firmware.bin 0x00300000 spiffs.bin
```

## Files in this Directory

**File Name**|**Description**
-----|-----
`createInstaller.py` | Freezes the Python environment and packages a cross-platform installer.
`flashFirmware.py`| Uses `esptool.py` to flash firmware files to ESP-8266.
`firmware.bin`| Main firmware file containing the Brew Bubbles application.
`littlefs.bin`| Controller filesystem image containing the web pages used by Brew Bubbles.
`version.json` | File used for version control.
