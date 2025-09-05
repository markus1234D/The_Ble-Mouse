#include <Arduino.h>
#include <vector>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include "pin_config.h"
#include <ESPAsyncWebServer.h>
#include "TFT_eSPI.h"
#include "DataWorker.h"

#define DEBUG

class GuiWorker {

    typedef void (*valChangeFunc)(int attribute);   
    typedef void (*funcChangeFunc)(String item, String action, String function);

public:
    // Constructor
    GuiWorker();
    void init();
    void handleGui();
    String getHtml();
    String getName();
    void sendXY(int x, int y);
    void onMouseSpeedChange(valChangeFunc func) {onMouseSpeedChangeCallback = func;}
    void onScrollspeedChange(valChangeFunc func) {onScrollspeedChangeCallback = func;}
    void onBrightnessChange(valChangeFunc func) {onBrightnessChangeCallback = func;}
    void onRotationChange(valChangeFunc func) {onRotationChangeCallback = func;}
    void onModeChange(valChangeFunc func) {onModeChangeCallback = func;}

    void notify(String message);
    void onFunctionChange(funcChangeFunc func) { onFunctionChangeCallback = func; }

private:
    // Private member variables
    DataWorker dataWorker;
    String html;
    String name;
    void (*callback)(void);
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    WebSocketsServer webSocketServer;
    AsyncWebServer server;
    TFT_eSPI tft;

    valChangeFunc onMouseSpeedChangeCallback = NULL;
    valChangeFunc onScrollspeedChangeCallback = NULL;
    valChangeFunc onBrightnessChangeCallback = NULL;
    valChangeFunc onRotationChangeCallback = NULL;
    valChangeFunc onModeChangeCallback = NULL;
    funcChangeFunc onFunctionChangeCallback = NULL;

private:
    // Private member functions
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    String extractCommand(const String& input);
    int extractArgs(const String& input, std::vector<String>& argNames, std::vector<String>& args);
    void debugPrint(String str);
};

GuiWorker::GuiWorker() : webSocketServer(81), server(80) {
    // Constructor
    // Serial.println("GuiWorker constructor called");
}

void GuiWorker::notify(String message) {
    webSocketServer.broadcastTXT("notify?msg=" + message);
    debugPrint("Notified: " + message);
}

void GuiWorker::debugPrint(String str) {
#ifdef DEBUG
    Serial.print("[GuiWorker]: ");
    Serial.println(str);
#endif
}

void GuiWorker::init() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(PIN_POWER_ON, OUTPUT);
    pinMode(PIN_LCD_BL, OUTPUT);

    digitalWrite(PIN_POWER_ON, HIGH);
    analogWrite(PIN_LCD_BL, 100);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, 0);
    tft.println(WiFi.localIP());


    // Start WebSocket-Server
    webSocketServer.begin();
    webSocketServer.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        this->webSocketEvent(num, type, payload, length);
    });
    // webSocketServer.onEvent(webSocketEvent);
    Serial.println("WebSocket server started.");

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", this->getHtml());
    });
    server.begin();

}

String GuiWorker::extractCommand(const String& input) {
    int pos = input.indexOf('?');
    if (pos != -1) {
        return input.substring(0, pos);
    } else {
        return input; // Wenn kein '?' gefunden wird, ist der ganze String der Command
    }
}

// Funktion, um die Argumentnamen und -werte zu extrahieren
// fails if input is not in the form "command?arg1=val1&arg2=val2&..."
int GuiWorker::extractArgs(const String& input, std::vector<String>& argNames, std::vector<String>& args) {
    // debugPrint("Extracting arguments");
    int pos = input.indexOf('?');
    if (pos == -1) {
        debugPrint("No arguments found");
        return 0; // Falls kein '?' vorhanden ist, keine Argumente
    }
    String query = input.substring(pos + 1);
    int start = 0;
    int end;
    int len = 0;

    while ((end = query.indexOf('&', start)) != -1) {
        String pair = query.substring(start, end);
        int equalPos = pair.indexOf('=');

        if (equalPos != -1) {
            len++;
            argNames.push_back(pair.substring(0, equalPos));
            args.push_back(pair.substring(equalPos + 1));
        }
        start = end + 1;
    }

    // Letztes Paar verarbeiten (nach dem letzten '&')
    String pair = query.substring(start);
    int equalPos = pair.indexOf('=');

    if (equalPos != -1) {
        len++;
        argNames.push_back(pair.substring(0, equalPos));
        args.push_back(pair.substring(equalPos + 1));
    }
    return len;
}

void GuiWorker::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
        Serial.printf("[%u] Connected!\n", num);
        webSocketServer.sendTXT(num, "Hello from ESP32!");

        break;
    case WStype_TEXT:
        // Serial.printf("[%u] Received text: %s\n", num, payload);
        String receivedMessage = String((char*)payload);
        // String response = "ESP32 received: " + receivedMessage;
        // webSocketServer.sendTXT(num, receivedMessage.c_str());

        std::vector<String> argNames;
        std::vector<String> args;

        String command = extractCommand(receivedMessage);
        // debugPrint("Command: " + command);

        int numArgs = extractArgs(receivedMessage, argNames, args);
        // debugPrint("Number of arguments: " + String(numArgs));
        // for (int i = 0; i < numArgs; i++) {
        //     debugPrint("Arg " + String(argNames[i]) + ": " + String(args[i]));
        // }
        if (command == "setMouseSpeed") {
            debugPrint("setMouseSpeed: " + String(argNames[0]) + " = " + String(args[0]));
            if (onMouseSpeedChangeCallback != NULL) {
                onMouseSpeedChangeCallback(args[0].toInt());
            }
        } else if (command == "setScrollSpeed") {
            debugPrint("setScrollSpeed: " + String(argNames[0]) + " = " + String(args[0]));
            if (onScrollspeedChangeCallback != NULL) {
                onScrollspeedChangeCallback(args[0].toInt());
            }
        } else if (command == "setBrightness") {
            if (onBrightnessChangeCallback != NULL) {
                onBrightnessChangeCallback(args[0].toInt());
            }
        } else if (command == "setRotation") {
            // debugPrint("setRotation: " + String(argNames[0]) + " = " + String(args[0]));
            if (onRotationChangeCallback != NULL) {
                onRotationChangeCallback(args[0].toInt());
            }
        } else if (command == "setMode") {
            if (onModeChangeCallback != NULL) {
                onModeChangeCallback(args[0].toInt());
            }
        } else if (command == "funcChange") {
            if (onFunctionChangeCallback != NULL && numArgs >= 3) {
                if(argNames[0] != "item" || argNames[1] != "action" || argNames[2] != "function") {
                    debugPrint("Invalid funcChange command format");
                    return;
                }
                debugPrint("Function change: " + String(args[0]) + " " + String(args[1]) + " " + String(args[2]));
                onFunctionChangeCallback(args[0], args[1], args[2]);
            }
        }
        break;
    }
}

void GuiWorker::handleGui() {
    webSocketServer.loop();
}

void GuiWorker::sendXY(int x, int y) {
    webSocketServer.broadcastTXT("coord?X=" + String(x) + "&Y=" + String(y));
}

String GuiWorker::getHtml() {

String str = dataWorker.getFileContent("/config_gui2.html");
    if (str != "failed") {
        return str;
    } else {
        return R"rawliteral(
            <html>
                <head>
                    <title>Configuration Error</title>
                </head>
                <body>
                    <h1>Failed to load configuration</h1>
                    <p>Please check the file system.</p>
                </body>
            </html>
        )rawliteral";
    }
}
