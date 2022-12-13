#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

#include "private.h"
#include "utility.h"

AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);

    if(!SPIFFS.begin()) {
        Serial.println("Error in mounting SPIFFS!");
        return;
    }

    Serial.println("Connection to WiFi");
    WiFi.begin(auth::ssid, auth::pswd);
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("");
    Serial.printf("Connected to AP %s (%s) with address %s.", auth::ssid, ip2cstr(WiFi.gatewayIP()), ip2cstr(WiFi.localIP()));

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", String(), false, nullptr);
    });

    server.on("/global.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/global.css", String(), false, nullptr);
    });

    server.on("/bundle.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/bundle.css", String(), false, nullptr);
    });

    server.on("/bundle.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/bundle.js", String(), false, nullptr);
    });

    server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/favicon.png", String(), false, nullptr);
    });

    Serial.println("Starting server");
    server.begin();
}

void loop()
{
	
}
