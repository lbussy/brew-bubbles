# Brew Bubbles API

Brew Bubbles stores and communicates with external agents via JSON and POST methods.  This document details those mechanics.

```{contents}
:depth: 3
```

## System Configuration

The device stores its configuration in JSON within LittleFS.  Web pages retrieve the configuration data at the following endpoint:

### /config/

Triggers the controller to send all of its configuration items

```{code-block} json
:linenos: true

{
"apconfig": {
    "ssid": "brewbubbles",
    "passphrase": "brewbubbles"
},
"hostname": "brewbubbles",
"bubble": {
    "name": "Fermenter 1",
    "tempinf": true
},
"calibrate": {
    "room": 0,
    "vessel": 0
},
"urltarget": {
    "url": "",
    "freq": 2,
    "update": false
},
"brewersfriend": {
    "channel": 0,
    "key": "c6e88f70f575c4ecdca3dcb686381185",
    "freq": 15,
    "update": false
},
"brewfather": {
    "channel": 0,
    "key": "q4F3wPfooBa3X3",
    "freq": 15,
    "update": false
},
"thingspeak": {
    "channel": 1244893,
    "key": "AB6C1ME2NWS1MSDS",
    "freq": 15,
    "update": false
},
"dospiffs1": false,
"dospiffs2": false,
"didupdate": false
}
```

These keys represent the following settings:

apconfig:
: - ssid: The AP name (SSID) which broadcasts when the controller is in AP mode
  - appwd: The AP password required to connect to the AP

hostname:
: - The mDNS hostname used by the controller

bubble:
: - name: The fermenter name
  - tempinf: Controls whether the device reports in Fahrenheit (true) or Celcius (false)

calibrate:
: - room: The offset applied to the room sensor
  - vessel: The offset applied to the vessel sensor

urltarget:
: - url: The HTML endpoint to which we send a JSON POST
  - freq: The frequency (in minutes) at which the application sends a status POST
  - update: Internal semaphore to trigger a reconfiguration

brewersfriend:
: - channel: Not used
  - key: The hexadecimal key provided by Brewer's Friend to allow POSTing data to your brew
  - freq: The frequency (in minutes) at which the application sends a status POST to Brewer's Friend
  - update: Internal semaphore to trigger a reconfiguration

brewfather:
: - channel: Not used
  - key: The hexadecimal key provided by Brewfather to allow POSTing data
  - freq: The frequency (in minutes) at which the application sends a status POST to Brewfather
  - update: Internal semaphore to trigger a reconfiguration

thingspeak:
: - channel: A numeric channel ID
  - key: The hexadecimal key provided by ThingSpeak to allow writing data to your channel
  - freq: The frequency (in minutes) at which the application sends a POST to ThingSpeak
  - update: Internal semaphore to trigger a reconfiguration

dospiffs1:
: - Sets a semaphore indicating that the controller has reset one time after the firmware update

dospiffs2:
: - Sets a semaphore indicating that the controller has reset two times after firmware update and that LittleFS update may begin

didupdate:
: - An indication that both firmware and LittleFS OTA has completed

## Outbound API

### /bubble/

Triggers the controller to send the last status payload:

```{code-block} json
:linenos: true

{
    "api_key": "Brew Bubbles",
    "device_source": "Brew Bubbles",
    "name": "Fermenter 1",
    "bpm": 123.456,
    "ambient": 72.5,
    "temp": 68.1,
    "temp_unit": "F",
    "datetime": "2020-12-03T20:44:40Z"
}
```

### /thisVersion/

This endpoint returns the current controller firmware and LittleFS version in JSON format:

```{code-block} json
:linenos: true

{
    "version": "2.2.0rc1",
    "branch": "devel",
    "build": "8ec3d68"
}
```

### /thatVersion/

This endpoint returns the currently available controller firmware and LittleFS version from the Brew Bubbles website in the same format as the local version.

## Inbound API

The controller uses inbound web page access to configure and control Brew Bubbles.

### Configuration Ingestion

The controller uses inbound POST endpoints to configure Brew Bubbles:

/settings/controller/
: - mdnsid: Sets the hostname of the controller
  - bubname: A display name for the controller

/settings/temperature/
: - calroom: A floating-point number by which the room/ambient sensor is offset
  - calvessel:  A floating-point number by which the room/ambient sensor is offset
  - tempformat: String, celsius or fahrenheit to configure temperature reporting

/settings/urltarget/
: - urltargeturl: The URL to which reports are posted
  - urlfreq: Frequency for reports in minutes

/settings/brewersfriendtarget/
: - brewersfriendkey: Brewer's Friend key
  - brewersfriendfreq: Frequency for reports in minutes

/settings/brewfathertarget/
: - brewfatherkey: Brewfather key
  - brewfatherfreq: Frequency for reports in minutes

/settings/thingspeaktarget/
: - thingspeakchannel: Channel to which the data will be posted
  - thingspeakkey: Write key for the channel
  - thingspeakfreq: Frequency for reports in minutes

/clearupdate/
: - Clears all update related semaphores.

### Control Points

The following pages take action upon access:

/wifi2/:

: Accessing this page resets all WiFi configuration items and resets the controller.

/otastart/:

: Accessing this page begins the OTA update process independent of web page processes.

## Downstream Targets

Downstream targets are systems to which Brew Bubbles sends data on a schedule.  Sending data to various targets is done in similar yet specific formats.

Note that since the temperature probes are optional, they report as -100 in either temperature format when not connected.  A sensor failure also results in this reading.

### General HTTP Targets

General targets are targets that take an unqualified HTTP post.  Currently, systems that are known to support Brew Bubbles are BrewPi Remix and Fermentrack.

Brew Bubbles makes the post with no authentication nor key, and in the following format:

```{code-block} json
:linenos: true

{
    "api_key":"Brew Bubbles",
    "device_source":"Brew Bubbles",
    "name":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "bpm":99.999,
    "ambient":70.3625,
    "temp":-196.6,
    "temp_unit":"F",
    "datetime":"2019-11-16T23:59:01.123Z"
}
```

### Brewer's Friend

Brew Bubbles natively and specifically supports posting data to Brewer's Friend. The payload sent to Brewer's Friend is according to the following format:

```{code-block} json
:linenos: true

{
    "api_key":"Brew Bubbles",
    "device_source":"Brew Bubbles",
    "name":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "bpm":99.999,
    "ambient":70.3625,
    "temp":-196.6,
    "temp_unit":"F",
    "datetime":"2019-11-16T23:59:01.123Z"
}
```

### Brewfather

Brew Bubbles natively and specifically supports posting data to Brewfather. The payload sent to Brewer's Friend is according to the following format:

```{code-block} json
:linenos: true

{
    "api_key":"Brew Bubbles",
    "device_source":"Brew Bubbles",
    "name":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "bpm":99.999,
    "aux_temp":70.3625,
    "temp":-196.6,
    "temp_unit":"F",
    "datetime":"2019-11-16T23:59:01.123Z"
}
```

### ThingSpeak

ThingSpeak receives POST reports according to their provided library.
