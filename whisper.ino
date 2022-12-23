#include <ArduinoJson.h>

#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include "private.h"
#include "utility.h"

AsyncWebServer server(80);
AsyncWebSocket socket("/ws");

String json;
const size_t caps = 64;

void handleWebSocketMessage(void* arg, uint8_t* message, size_t len, const char* ip) {
    AwsFrameInfo* info = static_cast<AwsFrameInfo*>(arg);
    if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        message[len] = 0;
        StaticJsonDocument<caps> bmsg;
        Serial.printf("usr: %s\n", String(ip));
        Serial.printf("msg: %s\n", String(reinterpret_cast<char*>(message)));
        bmsg["username"] = ip;
        bmsg["message"] = String(reinterpret_cast<char*>(message));
        serializeJson(bmsg, json);
        Serial.printf("JSON: %s\n", json);
        socket.textAll(json);
        json = "";
    }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch(type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len, client->remoteIP().toString().c_str());
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void setup()
{
    Serial.begin(115200);

    if(!SPIFFS.begin()) {
        Serial.println("Error in mounting SPIFFS!");
        return;
    }

    Serial.print("Connecting to WiFi");
    WiFi.begin(auth::ssid, auth::pswd);
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("");
    Serial.printf("Connected to AP [%s] with address [%s].", auth::ssid, ip2cstr(WiFi.localIP()));

    Serial.println("Starting server...");

    socket.onEvent(onEvent);
    server.addHandler(&socket);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/", SPIFFS, "/");

    Serial.println("Server started");
    server.begin();
}

void loop()
{
	socket.cleanupClients();
}
