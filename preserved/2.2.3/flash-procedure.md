# Brew Bubbles 2.2.3 Flash Procedure

Target: Wemos D1 Mini / ESP8266 with 4 MB flash.

## Verify artifacts

```bash
shasum -a 256 -c SHA256SUMS
```

## Erase and flash

Connect the D1 Mini by USB and identify its serial port.

```bash
esptool --chip esp8266 --port /dev/ttyUSB0 erase_flash
esptool --chip esp8266 --port /dev/ttyUSB0 --baud 460800 \
  write_flash 0x000000 firmware.bin 0x300000 littlefs.bin
```

Replace `/dev/ttyUSB0` with the actual serial port.

Firmware serial-monitor speed: 74880 baud.
