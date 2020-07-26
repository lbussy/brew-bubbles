Troubleshooting
===============

The easiest way to get help with the project is to join the `Brew Bubbles discussion on Homebrewtalk.com`_.  You may also open an issue on Github_.

.. _Brew Bubbles discussion on Homebrewtalk.com: https://support.brewbubbles.com
.. _Github: https://github.com/lbussy/brew-bubbles/issues

As a means to help you determine what is going wrong with your setup, this page includes various bits and pieces which do not fit elsewhere.

LED Flashing
------------

In typical operation, the blue LED on the controller indicates the photo-receptor status.  The LED lights when the photo-receptor is not blocked (or when a bubble passes), and is dark when the photo-receptor is blocked (no bubble).

During non-operating mode, the LED flashes at different frequencies as an indicator of the process underway.

0.5Hz:
    The LED blinks at 0.5Hz or 1 second on, one second off, when in Access Point mode

0.66Hz:
    The LED blinks at 1.5Hz (33 seconds on, .33 seconds off) while the controller is attempting to connect to your WiFi.  After trying for 30 seconds, the connection attempt fails, and the controller enters AP mode.  AP mode times out after 120 seconds and resets the controller.

10Hz:
    The controller needs to access an Internet Network Time Protocol (NTP) server to get the correct time.  While attempting to get the time, the LED flashes at 10Hz or .05 seconds on, .05 seconds off.  Occasionally, the controller cannot get network time, in which case it remains in that loop indefinitely since the controller cannot operate without the correct time.  If you see this, you can reset the controller with the reset button located within the cutout, and try again.

Double-Reset Detect
-------------------

If you need to access the AP configuration portal without resetting the WiFi settings via the web page, press the reset button twice within 2 seconds.  The controller resets and starts AP mode where the LED flashes at 0.5Hz.

Emergency AP mode
-----------------

If you are unable to access the AP by any other means, you may do so by jumpering D5 to GND on the controller and resetting it via the reset button or with a power cycle.  The controller starts in AP mode, where the LED flashes at 0.5Hz.

Serial Debug
------------

Serial debug has been left enabled in the firmware.  You may connect the device to your computer and use a terminal program set at a baud rate of 74880 to review the debug messages.

FAQ
---

No FAQ yet, I'll capture that as time goes on.
