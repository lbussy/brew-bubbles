# Configuration JSON:

```
{
    "apconfig": {
        "ssid": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "passphrase": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    },
    "hostname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "bubble": {
        "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "tempinf": false
    },
    "calibrate": {
        "room": -999.9999,
        "vessel": -999.9999
    },
    "urltarget": {
        "url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "freq": 999,
        "update": false
    },
    "brewersfriend": {
        "key": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "channel": 0,
        "freq": 9999,
        "update": false
    },
    "brewfather": {
        "key": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "channel": 0,
        "freq": 9999,
        "update": false
    },
    "thingspeak": {
        "key": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "channel": 999999999,
        "freq": 9999,
        "update": false
    },
    "dospiffs1": false,
    "dospiffs2": false,
    "didupdate": false,
    "nodrd": false
}
```

## Size:

```
const size_t capacity = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(11) + 810;
512+728 = 1240
```

## Deserializing:

```
// char* input;
// size_t inputLength; (optional)

StaticJsonDocument<768> doc;

DeserializationError error = deserializeJson(doc, input, inputLength);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* apconfig_ssid = doc["apconfig"]["ssid"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* apconfig_passphrase = doc["apconfig"]["passphrase"];

const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

const char* bubble_name = doc["bubble"]["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool bubble_tempinf = doc["bubble"]["tempinf"]; // false

float calibrate_room = doc["calibrate"]["room"]; // -999.9999
float calibrate_vessel = doc["calibrate"]["vessel"]; // -999.9999

JsonObject urltarget = doc["urltarget"];
const char* urltarget_url = urltarget["url"];
int urltarget_freq = urltarget["freq"]; // 999
bool urltarget_update = urltarget["update"]; // false

JsonObject brewersfriend = doc["brewersfriend"];
const char* brewersfriend_key = brewersfriend["key"];
int brewersfriend_channel = brewersfriend["channel"]; // 0
int brewersfriend_freq = brewersfriend["freq"]; // 9999
bool brewersfriend_update = brewersfriend["update"]; // false

JsonObject brewfather = doc["brewfather"];
const char* brewfather_key = brewfather["key"];
int brewfather_channel = brewfather["channel"]; // 0
int brewfather_freq = brewfather["freq"]; // 9999
bool brewfather_update = brewfather["update"]; // false

JsonObject thingspeak = doc["thingspeak"];
const char* thingspeak_key = thingspeak["key"];
long thingspeak_channel = thingspeak["channel"]; // 999999999
int thingspeak_freq = thingspeak["freq"]; // 9999
bool thingspeak_update = thingspeak["update"]; // false

bool dospiffs1 = doc["dospiffs1"]; // false
bool dospiffs2 = doc["dospiffs2"]; // false
bool didupdate = doc["didupdate"]; // false
bool nodrd = doc["nodrd"]; // false
```

## Serializing:

```
DynamicJsonDocument doc(1536);

JsonObject apconfig = doc.createNestedObject("apconfig");
apconfig["ssid"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
apconfig["passphrase"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject bubble = doc.createNestedObject("bubble");
bubble["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
bubble["tempinf"] = false;

JsonObject calibrate = doc.createNestedObject("calibrate");
calibrate["room"] = -999.9999;
calibrate["vessel"] = -999.9999;

JsonObject urltarget = doc.createNestedObject("urltarget");
urltarget["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
urltarget["freq"] = 999;
urltarget["update"] = false;

JsonObject brewersfriend = doc.createNestedObject("brewersfriend");
brewersfriend["key"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
brewersfriend["channel"] = 0;
brewersfriend["freq"] = 9999;
brewersfriend["update"] = false;

JsonObject brewfather = doc.createNestedObject("brewfather");
brewfather["key"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
brewfather["channel"] = 0;
brewfather["freq"] = 9999;
brewfather["update"] = false;

JsonObject thingspeak = doc.createNestedObject("thingspeak");
thingspeak["key"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
thingspeak["channel"] = 999999999;
thingspeak["freq"] = 9999;
thingspeak["update"] = false;
doc["dospiffs1"] = false;
doc["dospiffs2"] = false;
doc["didupdate"] = false;
doc["nodrd"] = false;

serializeJson(doc, output);
```
