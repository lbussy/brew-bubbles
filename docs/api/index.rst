Brew Bubbles API
================

Brew Bubbles utilizes JSON to send and receive data.  The device likewise stores its configuration in JSON within SPIFFS:

::

    {
        "apconfig":{
            "ssid":"brewbubbles",
            "appwd":"brewbubbles"
        },
        "hostname":"brewbubbles",
        "bubbleconfig":{
            "name":"Fermenter 1",
            "tempinf":true
        },
        "calibrate":{
            "room":0,
            "vessel":0
        },
        "targetconfig":{
            "targeturl":"",
            "freq":2
        },
        "bfconfig":{
            "bfkey":"c6e88f70f575c4ecdca3dcb686381185",
            "freq":15
        },
        "dospiffs1":false,
        "dospiffs2":false,
        "didupdate":false
    }

These keys represent the following settings:

apconfig:
    ssid: The AP name (SSID) which broadcasts when the controller is in AP mode

    appwd: The AP password required to connect to the AP

hostname:
    The mDNS hostname used by the controller

bubbleconfig:
    name: The fermenter name

    tempinf: Controls whether the device reports in Fahrenheit (true) or Celcius (false)

calibrate:
    room: The offset applied to the room sensor

    vessel: The offset applied to the vessel sensor

targetconfig:
    targeturl: The HTML endpoint which receives a JSON POST on schedule

    freq: The frequency (in minutes) at which the application sends a status POST

bfconfig:
    bfkey: The hexadecimal key provided by Brewer's Friend to allow POSTing data to your brew

    freq: The frequency (in minutes) at which the application sends a status POST to Brewer's Friend

dospiffs1:
    Sets a semaphore indicating that the controller has reset one time after the firmware update

dospiffs2:
    Sets a semaphore indicating that the controller has reset two times after firmware update and that SPIFFS update may begin

didupdate:
    An indication that both firmware and SPIFFS OTA has completed

The following represents the different methods and messages used.

Outbound API
------------

/configuration/:
    Triggers the controller to send all of its configuration items

/bubble/:
    Triggers the controller to send the last status payload

::

    {
        "api_key":"Brew Bubbles",
        "device_source":"Brew Bubbles",
        "name":"Fermenter 1",
        "bpm":3.2,
        "ambient":65.3,
        "temp":65.525,
        "temp_unit":"F",
        "datetime":"2019-12-15T21:48:07Z"
    }


/thisVersion/:
    Returns the current controller firmware and SPIFFS version in JSON format:

::

    {
        "version": "0.1.1"
    }


/thatVersion/:
    Returns the currently available controller firmware and SPIFFS version from the Brew Bubbles website in the same format as the local version

Inbound API
-----------

The controller uses inbound web page access to configure and control Brew Bubbles.

Configuration
`````````````

The controller uses inbound endpoints to configure Brew Bubbles:

/settings/update/:
    Processes a JSON POST to single configuration item only in the format shown above

/clearupdate/:
    Clears all update related semaphores

/config/apply/:
    Allows the application of all configuration items in the format above in a single JSON POST

Control
```````

The following pages take action upon access:

/wifi2/:
    Accessing this page resets all WiFi configuration items and reset the controller

/otastart/:
    Accessing this page begins the OTA update process

