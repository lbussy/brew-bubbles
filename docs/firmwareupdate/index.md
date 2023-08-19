# Firmware Update

After you initially flash the controller, you may update Brew Bubbles via the web page when new versions are released.  You may do this without erasing any of the Brew Bubbles' settings.  If you instead choose to flash new firmware and LittleFS manually, your application configuration is lost.  This page displays your controller's current version, as well as the latest version available.

```{image} 2_update_firmware.jpg
:align: center
:alt: Version comparison
:scale: 100 %
```

If you wish to proceed, clicking "Update Firmware" button begins the process.

```{image} 3_update_firmware.jpg
:align: center
:alt: Update processing
:scale: 45 %
```

A page displays while the upgrade is in progress.

Warning:

: Do not close this page.  If you close the page before the update is complete, you may lose your application settings.

During this process, both the firmware and the LittleFS updates apply.  When the update is complete, the application settings are re-applied, and a completion message is displayed.

The controller's onboard LED may flicker during the upgrade.  Once complete, the LED remains steady on or off, depending on whether the sensor is blocked.

```{image} 4_update_firmware.jpg
:align: center
:alt: Update complete
:scale: 45 %
```

The page redirects after this message and opens the main page, or you may navigate there manually.
