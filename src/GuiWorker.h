#include <Arduino.h>
#include <vector>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include "pin_config.h"
#include <ESPAsyncWebServer.h>



class GuiWorker {

    typedef void (*valChangeFunc)(int attribute);   

public:
    // Constructor
    GuiWorker();
    void init();
    void handleGui();
    String getHtml();
    String getName();
    void sendXY(int x, int y);
    void onMoseSpeedChange(valChangeFunc func) {onMoseSpeedChangeCallback = func;}
    void onScrollspeedChange(valChangeFunc func) {onScrollspeedChangeCallback = func;}
    void onBrightnessChange(valChangeFunc func) {onBrightnessChangeCallback = func;}
    void onRotationChange(valChangeFunc func) {onRotationChangeCallback = func;}
    void onModeChange(valChangeFunc func) {onModeChangeCallback = func;}
    int getXMin();
    int getXMax();
    int getYMin();
    int getYMax();


private:
    // Private member variables
    String html;
    String name;
    void (*callback)(void);
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    WebSocketsServer webSocketServer;
    AsyncWebServer server;

    valChangeFunc onMoseSpeedChangeCallback = NULL;
    valChangeFunc onScrollspeedChangeCallback = NULL;
    valChangeFunc onBrightnessChangeCallback = NULL;
    valChangeFunc onRotationChangeCallback = NULL;
    valChangeFunc onModeChangeCallback = NULL;

    // Private member functions
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    String extractCommand(const String& input);
    int extractArgs(const String& input, std::vector<String>& argNames, std::vector<String>& args);
    void debugPrint(String str);
};

GuiWorker::GuiWorker() : webSocketServer(81), server(80) {
    // Constructor
    Serial.println("GuiWorker constructor called");
}

void GuiWorker::debugPrint(String str) {
#ifdef DEBUG
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
int GuiWorker::extractArgs(const String& input, std::vector<String>& argNames, std::vector<String>& args) {
    int pos = input.indexOf('?');
    if (pos == -1) return 0; // Falls kein '?' vorhanden ist, keine Argumente

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
        Serial.printf("[%u] Received text: %s\n", num, payload);
        String receivedMessage = String((char*)payload);
        // String response = "ESP32 received: " + receivedMessage;
        // webSocketServer.sendTXT(num, receivedMessage.c_str());

        std::vector<String> argNames;
        std::vector<String> args;
        String command = extractCommand(receivedMessage);
        Serial.println("Command: " + command);
        int numArgs = extractArgs(receivedMessage, argNames, args);
        for (int i = 0; i < numArgs; i++) {
            Serial.println("Arg " + argNames[i] + ": " + args[i]);
        }
        if (command == "setMouseSpeed") {
            if (onMoseSpeedChangeCallback != NULL) {
                onMoseSpeedChangeCallback(args[0].toInt());
            }
        } else if (command == "setScrollSpeed") {
            if (onScrollspeedChangeCallback != NULL) {
                onScrollspeedChangeCallback(args[0].toInt());
            }
        } else if (command == "setBrightness") {
            if (onBrightnessChangeCallback != NULL) {
                onBrightnessChangeCallback(args[0].toInt());
            }
        } else if (command == "setRotation") {
            if (onRotationChangeCallback != NULL) {
                onRotationChangeCallback(args[0].toInt());
            }
        } else if (command == "setMode") {
            if (onModeChangeCallback != NULL) {
                onModeChangeCallback(args[0].toInt());
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
    return R"rawliteral(
    
<!DOCTYPE html>
<html lang="de">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Control Panel</title>
    <style>
        body {
            background-color: #1a1a1a;
            color: white;
            font-family: Arial, sans-serif;
        }

        .container {
            border: 2px solid #d2e2f2;
            padding: 20px;
            width: 80%;
            margin: 0 auto;
            background-color: #282828;
            display: flex;
            justify-content: space-between;
        }

        .left-column, .right-column {
            width: 40%;
        }

        .output {
            width: 100%;
            height: 100px;
            color: rgb(255, 255, 255);
            background-color: #1a1a1a;
            border: 2px solid #7cb8ff;
            padding: 10px;
            resize: none;
        }

        .label {
            margin-bottom: 5px;
            display: block;
            color: white;
        }

        .slider-container {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
        }

        .slider {
            -webkit-appearance: none;
            width: 100%;
            height: 5px;
            background: #7cb8ff;
            outline: none;
            opacity: 0.8;
            transition: opacity 0.2s;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 15px;
            height: 15px;
            background: #7cb8ff;
            cursor: pointer;
        }

        .slider-container span {
            width: 120px;
            color: white;
        }

        .screen-rotation {
            display: flex;
            justify-content: center;
            align-items: center;
            margin-top: 20px;
            position: relative;
        }

        .screen-diagram {
            width: 170px;
            height: 360px;
            border: 2px solid #7cb8ff;
            position: relative;
            transition: transform 0.5s;
        }

        .screen-diagram.up {
            transform: rotate(0deg);
        }

        .screen-diagram.down {
            transform: rotate(180deg);
        }

        .screen-diagram.left {
            transform: rotate(270deg);
        }

        .screen-diagram.right {
            transform: rotate(90deg);
        }

        .circle {
            width: 15px;
            height: 15px;
            background-color: #7cb8ff;
            border-radius: 50%;
            position: absolute;
            top: 10px;
            left: calc(50% - 7.5px);
        }
        
        .point {
            width: 10px;
            height: 10px;
            background-color: magenta;
            border-radius: 50%;
            position: absolute;
        }

    </style>
</head>

<body>

    <div class="container">
        <div class="left-column">
            <textarea class="output" readonly>Text\nText\n...</textarea>

            <label for="mode" class="label">Mode</label>
            <select id="mode">
                <option value="Mouse_Mode">Mouse_Mode</option>
                <option value="Joystick_Mode">Joystick_Mode</option>
                <option value="Scroll_Mode">Scroll_Mode</option>
            </select>

            <div class="slider-container">
                <span>Mousespeed</span>
                <input type="range" class="slider" id="mousespeed" min="1" max="100">
            </div>

            <div class="slider-container">
                <span>Scrollspeed</span>
                <input type="range" class="slider" id="scrollspeed" min="1" max="100">
            </div>

            <div class="slider-container">
                <span>Brightness</span>
                <input type="range" class="slider" id="brightness" min="1" max="100">
            </div>
        </div>

        <div class="right-column">
            <div class="screen-rotation">
                <div class="screen-diagram down" id="mouse-screen">
                    <div class="circle"></div>
                </div>
            </div>

            <label for="rotation" class="label">Rotation</label>
            <select id="rotation">
                <option value="down">Down</option>
                <option value="up">Up</option>
                <option value="left">Left</option>
                <option value="right">Right</option>
            </select>
            <button id="paintBtn">paint</button>
        </div>
    </div>

    <script>
        const rotationSelect = document.getElementById('rotation');
        const modeSelect = document.getElementById('mode');
        const mousespeedSlider = document.getElementById('mousespeed');
        const scrollspeedSlider = document.getElementById('scrollspeed');
        const brightnessSlider = document.getElementById('brightness');
        const screenDiagram = document.getElementById('mouse-screen');
        const paintBtn = document.getElementById('paintBtn');
        const pointsArray = []; // Array zur Speicherung der Punkte
        const maxX = screenDiagram.clientWidth;
        const maxY = screenDiagram.clientHeight;

        var ws = new WebSocket('ws://' + window.location.hostname + ':81');

        ws.onopen = function() {
            console.log("Hello, Server");
        };
        ws.onmessage = function (evt) {
            const command = extractCommand(evt.data);
            const { argNames, args } = extractArgs(evt.data);
            console.log("Command: " + command);
            console.log("ArgNames: " + argNames);
            console.log("Args: " + args);
            if (command === "coord") {
                const x = 170-parseInt(args[0]);
                const y = 320-parseInt(args[1]);
                paintPoint(x, y);
            }
        }

        function extractCommand(input) {
            const pos = input.indexOf('?');
            if (pos !== -1) {
                return input.substring(0, pos);
            } else {
                return input; // Wenn kein '?' gefunden wird, ist der ganze String der Command
            }
        }

        function extractArgs(input) {
            const argNames = [];
            const args = [];
            
            const pos = input.indexOf('?');
            if (pos === -1) return { argNames, args }; // Falls kein '?' vorhanden ist, keine Argumente

            const query = input.substring(pos + 1);
            const pairs = query.split('&');

            pairs.forEach(pair => {
                const [name, value] = pair.split('=');
                if (name && value) {
                    argNames.push(name);
                    args.push(value);
                }
            });

            return { argNames, args };
        }

        function paintPoint(x, y) {
            if (x < 0 || x > maxX || y < 0 || y > 320) {
                console.error('Point out of bounds!');
                return;
            }
            const point = document.createElement('div');
            point.classList.add('point');

            // Punkt-Position setzen
            point.style.left = `${x}px`;
            point.style.top = `${y}px`;

            // Punkt zum Rechteck hinzufügen
            screenDiagram.appendChild(point);
            pointsArray.push(point); // Punkt zum Array hinzufügen

            // Überprüfen, ob mehr als 5 Punkte vorhanden sind
            if (pointsArray.length > 10) {
                const oldPoint = pointsArray.shift(); // Ältesten Punkt aus dem Array entfernen
                screenDiagram.removeChild(oldPoint); // Ältesten Punkt aus dem DOM entfernen
            }
        }

        paintBtn.addEventListener('click', () => {
            // punkt an zufälliger stelle malen
            console.log('Painting point...');
            
            const x = Math.floor(Math.random() * maxX);
            const y = Math.floor(Math.random() * maxY);
            paintPoint(x, y);
        });
        // Rotation change event
        rotationSelect.addEventListener('change', (e) => {
            screenDiagram.className = 'screen-diagram ' + e.target.value;
            console.log('Rotation:', e.target.value);
            ws.send("setRotation?" + e.target.value);
        });

        // Mode change event
        modeSelect.addEventListener('change', (e) => {
            console.log('Mode:', e.target.value);
            ws.send("setMode?" + e.target.value);
        });

        // Mousespeed change event
        mousespeedSlider.addEventListener('input', (e) => {
            console.log('Mousespeed:', e.target.value);
            ws.send("setMouseSpeed?" + e.target.value);
        });

        // Scrollspeed change event
        scrollspeedSlider.addEventListener('input', (e) => {
            console.log('Scrollspeed:', e.target.value);
            ws.send("setScrollSpeed?" + e.target.value);
        });

        // Brightness change event
        brightnessSlider.addEventListener('input', (e) => {
            console.log('Brightness:', e.target.value);
            ws.send("setBrightness?" + e.target.value);
        });
    </script>

</body>

</html>

)rawliteral";

}
