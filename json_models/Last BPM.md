# Last BPM JSON:

```
{
    "lastBpm": 99999.99999,
    "lastAmb": 999.9999,
    "lastVes": 999.9999,
    "dts": 1546300800
}
```

## Size:

```
const size_t capacity = JSON_OBJECT_SIZE(4) + 40;
64+28 = 92
```

## Parsing:

```
const size_t capacity = JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc(capacity);

const char* json = "{\"lastBpm\":99999.99999,\"lastAmb\":999.9999,\"lastVes\":999.9999,\"dts\":1546300800}";

deserializeJson(doc, json);

float lastBpm = doc["lastBpm"]; // 99999.99999
float lastAmb = doc["lastAmb"]; // 999.9999
float lastVes = doc["lastVes"]; // 999.9999
long dts = doc["dts"]; // 1546300800
```

## Serializing:

```
const size_t capacity = JSON_OBJECT_SIZE(4);
DynamicJsonDocument doc(capacity);

doc["lastBpm"] = 99999.99999;
doc["lastAmb"] = 999.9999;
doc["lastVes"] = 999.9999;
doc["dts"] = 1546300800;

serializeJson(doc, Serial);
```
