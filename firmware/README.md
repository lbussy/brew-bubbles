[![Brew Bubbles Logo](https://i1.wp.com/www.brewbubbles.com/wp-content/uploads/2019/08/BB-full-logo.png "Brew Bubbles")](http://www.brewbubbles.com/)

# Loading Brew Bubbles Firmware

There are two important files in this project:

**File Name** | **Description** | **Address**
-----|-----|-----
`firmware.bin` | The Brew Bubbles application layer | 0x00000
`spiffs.bin` | The Brew Bubbles web layer | 0x300000

Optionally, for certain workflows I have included:

**File Name**|**Description**|**Address**
-----|-----|-----
`blank.bin` | A blank file used to reset parts of the controller to defaults | 0xfe000 & 0x3fe000

## Flashing Firmware - Initial

Flashing the firmware may be done from many platforms with a variety of tools.  Following are some methods which have been tested.  Other methods may work however the author has no experience with them.

### Unix-Based Platforms (or Windows with Python support)

Espressif, the makers of the ESP8266, have adopted a python-based tool named [*esptool*](https://github.com/espressif/esptool).  Assuming you have either Python 2.7 or 3.4+ on your system, you can install *esptool* with *pip*:

```
$ pip install esptool
```

**Note:** With some Python installations this may not work and you'll receive an error, try `python -m pip install esptool` or `pip2 install esptool`.

After installing, you will have esptool.py installed into the default Python executables directory.  For manual installation instructions, please visit the [GitHub repository](https://github.com/espressif/esptool).

Once *esptool* is installed, you may use the following command line to flash the firmware (assuming the firmware is in the local directory):

```
esptool write_flash -fm dio -fs 4MB-c1 0x00000 firmware.bin 0x300000 spiffs.bin
```

Please note that this takes advantage of *esptool*'s capability to auto-detect the controller attached via USB.  If you have other devices directly attached to your system this may fail and oyu will need to specify the port manually, for example: `-p /dev/ttyUSB0` (or `-p COM3` on Windows.)

If you desire to completely erase your controller, you may also use `esptool`'s erase flash option:

```
esptool erase_flash
```

If for some reason you are having issues with WiFi which is not addressed by any of the other methods presented, you may either use the erase flash option above, or use the `blank.bin` file in this repository:

```
esptool write_flash -fm dio -fs 4MB-c1 0xfe000 blank.bin 0x3fe000 blank.bin
```

### Mac Platforms

I do not have access to a Mac, however anecdotally I believe Mac users may follow the "*Unix-Based Platforms*" above.

### Windows-Based Platforms

Using Espressif's Flash Download tool is the recommended method for Windows platforms.  [Download the tool](https://www.espressif.com/en/support/download/other-tools) directly from Espressif's website.  Unzip the tool to a convenient folder and execute the application (named `flash_download_tools_v3.6.7.exe` at the time of writing.)

You will see a console window and a main screen:

![Splash Screen](Splash.PNG)

Select "ESP8266 DownloadTool."  Setup as follows:

- Add the firmware file
  - Check the first checkbox
  - Click the ellipsis (...) next to the text field
  - Navigate to the firmware directory, select `firmware.bin` and click "*Open*"
  - In the right-most text field after the "*@*" symbol, enter the address `0x00000` (zero, the lower-case letter "X", followed by five zeros)
- Add the SPIFFS file
  - Check the second checkbox
  - Click the ellipsis (...) next to the text field
  - Navigate to the firmware directory, select `spiffs.bin` and click "*Open*"
  - In the right-most text field after the "*@*"* symbol, enter the address `0x300000` (zero, the lower-case letter "X", followed by the number "3" and five zeros)
- CrystalFreq should be set to `26M`
- SPI Speed should be set to `40MHz`
- SPI MODE should be set to `QIO` (`DIO` may be used if you experience issues flashing the firmware)
- FLASH SIZE should be set to `32Mbit-C1` (32 Megabits = 4 Megabytes)
- Select the proper COM port
- Set BAUD to `460800` (a lower speed may be used if you experience issues flashing the firmware)

![Setup Screen](Setup.PNG)]

When setup is complete, click on the "*START*" button underneath the green box.  The darker green box will proceed across the bottom of the window, and when complete the bright green box will change to "*FINISH*".

![Finished Screen](Finish.PNG)]

At this point, you may close the tool as well as the selection screen, and proceed with setup.

## Firmware Updates

The web application provides Over The Air (OTA) update functionality.  Navigate to the *Settings* page and scroll down to the *Update Firmware* section.
