bool postJson(String targetUrl) {
    const int capacity = JSON_ARRAY_SIZE(2) + 4 * JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;

    // Add the "location" object
    JsonObject location = doc.createNestedObject("location");
    location["lat"] = 48.748010;
    location["lon"] = 2.293491;

    // Add the "feeds" array
    JsonArray feeds = doc.createNestedArray("feeds");

    // Add the first feed: "a1"
    JsonObject feed1 = feeds.createNestedObject();
    feed1["key"] = "a1";
    feed1["value"] = random(255);

    // Add the second feed: "a2"
    JsonObject feed2 = feeds.createNestedObject();
    feed2["key"] = "a2";
    feed2["value"] = random(255);

    // Connect to the HTTP server
    WiFiClient client;
    client.setTimeout(10000);
    if (!client.connect("brewpi.local", 80)) {
        Serial.println(F("Connection failed."));
        return false;
    } else {
        Serial.print("Connected to endpoint... ");
    }

    // Open connection
    client.println(F("POST /brewpi-api.php HTTP/1.1"));

    // Send the HTTP headers
    client.println(F("Host: brewpi.local"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(measureJson(doc));
    client.println(F("Content-Type: application/json"));
    client.println(F("X-AIO-Key: 1234567890ABCDEF"));

    // Terminate headers with a blank line
    client.println();

    // Send the JSON document in body
    serializeJson(doc, client);

    // Check the  HTTP status (should be "HTTP/1.1 200 OK")
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    client.stop();

    if (strcmp(status + 9, "200 OK") != 0) {
        Serial.println();
        Serial.print(F("Unexpected status: "));
        Serial.println(status);
        return false;
    }

    // Close the connection
    client.stop();
    Serial.println("JSON posted.");
}

void _delay(unsigned long ulDelay) {
    // Safe blocking delay() replacement with yield()
    unsigned long ulNow = millis();
    unsigned long ulThen = ulNow + ulDelay;
    while (ulThen > millis()) {
        yield();
    }
}
