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
        "freq": 9999,
        "update": false
    },
    "brewfather": {
        "key": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "freq": 9999,
        "update": false
    },
    "thingspeak": {
        "key": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "freq": 9999,
        "update": false
    },
    "dospiffs1": false,
    "dospiffs2": false,
    "didupdate": false
}
```

## Size:

const size_t capacity = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(11) + 790;
464 + 704 = 1168

## Parsing:

const size_t capacity = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(11) + 790;
DynamicJsonDocument doc(capacity);

const char* json = "{\"apconfig\":{\"ssid\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"passphrase\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"},\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"bubble\":{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"tempinf\":false},\"calibrate\":{\"room\":-999.9999,\"vessel\":-999.9999},\"urltarget\":{\"url\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":999,\"update\":false},\"brewersfriend\":{\"key\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":9999,\"update\":false},\"brewfather\":{\"key\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":9999,\"update\":false},\"thingspeak\":{\"key\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":9999,\"update\":false},\"dospiffs1\":false,\"dospiffs2\":false,\"didupdate\":false}";

deserializeJson(doc, json);

const char* apconfig_ssid = doc["apconfig"]["ssid"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* apconfig_passphrase = doc["apconfig"]["passphrase"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

const char* bubble_name = doc["bubble"]["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool bubble_tempinf = doc["bubble"]["tempinf"]; // false

float calibrate_room = doc["calibrate"]["room"]; // -999.9999
float calibrate_vessel = doc["calibrate"]["vessel"]; // -999.9999

JsonObject urltarget = doc["urltarget"];
const char* urltarget_url = urltarget["url"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
int urltarget_freq = urltarget["freq"]; // 999
bool urltarget_update = urltarget["update"]; // false

JsonObject brewersfriend = doc["brewersfriend"];
const char* brewersfriend_key = brewersfriend["key"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
int brewersfriend_freq = brewersfriend["freq"]; // 9999
bool brewersfriend_update = brewersfriend["update"]; // false

JsonObject brewfather = doc["brewfather"];
const char* brewfather_key = brewfather["key"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
int brewfather_freq = brewfather["freq"]; // 9999
bool brewfather_update = brewfather["update"]; // false

JsonObject thingspeak = doc["thingspeak"];
const char* thingspeak_key = thingspeak["key"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
int thingspeak_freq = thingspeak["freq"]; // 9999
bool thingspeak_update = thingspeak["update"]; // false

bool dospiffs1 = doc["dospiffs1"]; // false
bool dospiffs2 = doc["dospiffs2"]; // false
bool didupdate = doc["didupdate"]; // false

## Serializing:

const size_t capacity = 3*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(11);
DynamicJsonDocument doc(capacity);

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
brewersfriend["freq"] = 9999;
brewersfriend["update"] = false;

JsonObject brewfather = doc.createNestedObject("brewfather");
brewfather["key"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
brewfather["freq"] = 9999;
brewfather["update"] = false;

JsonObject thingspeak = doc.createNestedObject("thingspeak");
thingspeak["key"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
thingspeak["freq"] = 9999;
thingspeak["update"] = false;
doc["dospiffs1"] = false;
doc["dospiffs2"] = false;
doc["didupdate"] = false;

serializeJson(doc, Serial);
