# Brew Bubbles 2.2.3 Build Preservation

This directory preserves a controlled reconstruction of the Brew Bubbles 2.2.3
PlatformIO build environment.

## Canonical release artifacts

The authoritative flash artifacts are stored one directory above:

- `firmware.bin`
- `littlefs.bin`
- `SHA256SUMS`
- `flash-procedure.md`
- `build-manifest.txt`
- `source-2.2.3.tar.gz`

Always use those canonical binaries when restoring a device.

## Source provenance limitation

The released firmware identifies its source as:

- commit `6362a76`
- branch `master`
- version `2.2.3`

Commit `6362a76` is not present in the surviving repository history, tags,
remote references, or recoverable local Git objects. An exact source rebuild of
the released firmware is therefore not currently possible.

This reconstruction uses the closest surviving public pre-release source:

- commit `3beff558e54862332a419fd5431d5ed59dd25a05`
- branch identity `devel`

Its firmware is deterministic but is not byte-identical to the canonical
release firmware.

Expected reconstructed firmware SHA-256:

`cfd41b647268a70c71090f621a44ad50d968a7cfba6dc5370d9849f59bf85f01`

## Preserved environment

- Host container architecture: `linux/amd64`
- Python: 3.8.18
- PlatformIO Core: 5.1.0
- Espressif8266 platform: 2.6.3
- Arduino ESP8266 core: 2.7.4
- Xtensa GCC: 4.8.2
- ArduinoJson: 6.17.3

All Git-based libraries are pinned to full commit IDs in `platformio.ini`.

## External assets

The complete environment requires two release assets:

- `brew-bubbles-pio-historical-5.1.0-linux-amd64.tar.gz`
- `brew-bubbles-2.2.3-offline-cache-linux-amd64.tar.gz`

Their hashes are recorded in `EXTERNAL-ASSETS.sha256`.

The container archive preserves Python, Git, PlatformIO Core, and the Debian
userspace. The offline-cache archive preserves the compiler, framework,
PlatformIO platform, filesystem tools, SCons reconstruction, and all library
checkouts.

## Offline reconstruction

Docker is required. Run from any location:

```sh
preserved/2.2.3/build/build-offline.sh \
  /path/to/downloaded/release-assets \
  /path/to/output-directory
```

The script:

1. verifies both external assets;
2. loads the preserved container image when needed;
3. checks out the historical reconstruction source;
4. restores the complete offline package set;
5. disables container networking;
6. builds firmware and LittleFS;
7. verifies the deterministic firmware checksum;
8. unpacks LittleFS and compares every contained file with the canonical image.

## LittleFS reproducibility

Repeated `mklittlefs` runs produce different container-image bytes even when
all contained files are identical. Therefore, the generated `littlefs.bin`
checksum is not used as the reconstruction invariant.

The build instead unpacks both images and compares the path and SHA-256 of every
contained file. The canonical `littlefs.bin` remains the image to use for device
restoration.

## Container reconstruction

`Dockerfile`, `requirements.txt`, and `container-manifest.txt` document how the
container was assembled. The saved container archive is authoritative because
future availability of the historical Debian and Python dependencies cannot be
assumed.
