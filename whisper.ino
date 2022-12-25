#include <ArduinoJson.h>

#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <BluetoothSerial.h>

#include "utility.h"

#define BTDBG

#ifndef BTDBG
#include "private.h"
#else
namespace auth {
    const char* ssid = "aninvalidssid";
    const char* pswd = "aninvalidpswd";
}
#endif

void wifiWatchDog(void* pvParameters) {
    while(true) {
        // check if wifi connected every  10s
        if(WiFi.status() == WL_CONNECTED) {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        // not connected, try again
        // Serial.printf("Retrying to connect to [%s]...\n", auth::ssid);
        Serial.print(F("retrying to connect to "));
        Serial.println(auth::ssid);
        WiFi.mode(WIFI_OFF);
        delay(1000);
        WiFi.mode(WIFI_STA);
        WiFi.begin(auth::ssid, auth::pswd);

        // wait for 20s, check every other second
        auto attempt_begin = millis();
        while(WiFi.status() != WL_CONNECTED && millis() - attempt_begin < 20000) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        // if still not connecting,...
        if(WiFi.status() != WL_CONNECTED) {
            WiFi.mode(WIFI_OFF); // RF exclusive
            BluetoothSerial SerialBT;
            //...get to manual mode.
            Serial.println(F("Failed. Entering manual mode..."));
            SerialBT.begin(F("whisper-BT-interface"));
            while(true) {
                if(Serial.available()) {
                    SerialBT.write(Serial.read());
                }
                if(SerialBT.available()) {
                    Serial.write(SerialBT.read());
                }
                delay(100);
            }
        }
    }
}

AsyncWebServer server(80);
AsyncWebSocket socket("/ws");

String json;
const size_t caps = 64;

void handleWebSocketMessage(void* arg, uint8_t* message, size_t len, const char* ip) {
    AwsFrameInfo* info = static_cast<AwsFrameInfo*>(arg);
    Serial.printf("len: %u\n", len);
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
//TODO: notify client connect/dis- to chat room
void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch(type) {
        case WS_EVT_CONNECT:
            // Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            Serial.print(F("#"));
            Serial.print(client->id());
            Serial.print(F(" from "));
            Serial.println(client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len, (String(client->id()) + "@" + client->remoteIP().toString()).c_str());
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
        Serial.println(F("Error in mounting SPIFFS!"));
        return;
    }

    WiFi.mode(WIFI_STA);

    xTaskCreate(
        wifiWatchDog,
        "WiFi Watchdog",
        5000,
        nullptr,
        1,
        nullptr
    );

    Serial.print(F("Connecting to WiFi..."));
    WiFi.begin(auth::ssid, auth::pswd);
    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    // Serial.printf("Connected to AP [%s] with address [%s].", auth::ssid, ip2cstr(WiFi.localIP()));
    Serial.print(F("gateway: "));
    Serial.print(auth::ssid);
    Serial.print(F(" ip: "));
    Serial.println(ip2cstr(WiFi.localIP()));

    Serial.println(F("Starting server..."));

    socket.onEvent(onEvent);
    server.addHandler(&socket);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/", SPIFFS, "/");

    Serial.println(F("Server started"));
    server.begin();
}

void loop()
{
	socket.cleanupClients();
}
