[![Brew Bubbles Logo](https://www.brewbubbles.com/wp-content/uploads/2019/08/BB-full-logo.png "Brew Bubbles")](http://www.brewbubbles.com/)

## Brew Bubbles Firmware

This directory contains the firmware files to prepare the controller. Instructions to program the controller are contained in [the documentation](https://docs.brewbubbles.com).

## Flashing Manually

If you have ``esptool.py`` on yor system, you may use the following command line:

```
esptool.py --chip esp8266 --before default_reset --after hard_reset write_flash 0x00000000 firmware.bin 0x00300000 littlefs.bin
```

## Files in this Directory

### Versions <= 2.2.3

These files exist for legacy (version <= 2.2.3) Brew Bubbles users.

**File Name**|**Description**
-----|-----
`firmware.bin`| Main firmware file containing the Brew Bubbles application.
`littlefs.bin`| Controller filesystem image containing the web pages used by Brew Bubbles.
`version.json` | File used for version control.

### Versions > 2.2.3

These files exist for updated (version > 2.2.3) Brew Bubbles users.

**File Name**|**Description**
-----|-----
`{board_type}_firmware.bin`| Main firmware file containing the Brew Bubbles application.
`{board_type}_littlefs.bin`| Controller filesystem image containing the web pages used by Brew Bubbles.
`version.json` | File used for version control.
