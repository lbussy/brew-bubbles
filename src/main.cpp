#include <ESP8266WiFi.h>        // ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>          // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>   // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>        // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>        // ESP8266 Multicast DNS
#include <Ticker.h>             // ESP8266 library which calls functions periodically

#define SERVICE_PORT 80 // HTTP port

Ticker ticker; // For LED status
ESP8266WebServer server(SERVICE_PORT); // Start web server
String hostname = "ESP" + String(ESP.getChipId());

void flash()
{
    // Toggle state
    int state = digitalRead(LED_BUILTIN);  // Get the current state of GPIO pin
    digitalWrite(LED_BUILTIN, !state);     // Set pin to the opposite state
}

// Gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
    Serial.println("Entered config mode.");
    Serial.println(WiFi.softAPIP());
    // If you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
    // Entered config mode, make LED toggle faster
    ticker.attach(0.5, flash);
}

void handleHTTPRequest() {
    server.send(200, "text/plain", "Host name resolved by mDNS.");
}

void setup() {
    // Put your setup code here, to run once:

    // Set serial baud rate
    Serial.begin(115200);

    // Set LED pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    // Start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(1.0, flash);

    // WiFiManager local intialization. Once its business is done, there is
    // no need to keep it around
    WiFiManager wifiManager;
    // Reset settings - for testing
    //wifiManager.resetSettings();

    // Set callback that gets called when connecting to previous WiFi fails,
    // and device enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    // Fetches SSID and PWD and tries to connect. If it does not connect it
    // starts an access point with the specified name here and goes into a
    // blocking loop awaiting configuration
    if (!wifiManager.autoConnect()) {
        Serial.println("Failed to connect, hit timeout.");
        Serial.println("Resetting.");
        // Reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(1000);
    }

    // Connected to the Wifi
    Serial.println("Connected to wifi.");
    Serial.println("Name: " + hostname);
    Serial.println("IP: " + WiFi.localIP().toString());

    // Turn off LED
    ticker.detach();
    digitalWrite(LED_BUILTIN, HIGH);

    if (!MDNS.begin(hostname)) {
        Serial.println("Error setting up MDNS responder.");
    } else {
        Serial.println("mDNS responder started, listening as " + hostname + ".local.");
        if (!MDNS.addService("http", "tcp", SERVICE_PORT)) { // Add service to MDNS-SD
            Serial.println("mDNS unable to add service.");
        } else {
            Serial.println("mDNS HTTP service added.");
        }
    }

    server.on("/", handleHTTPRequest); //Associate handler function to path
    server.begin(); //Start server
    Serial.println("HTTP server started.");
}

void loop() {
    // Loop code goes here:
    MDNS.update();
    server.handleClient();  
}
