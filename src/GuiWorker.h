#include <Arduino.h>
#include <vector>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include "pin_config.h"
#include <ESPAsyncWebServer.h>

#define DEBUG

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
    void onMouseSpeedChange(valChangeFunc func) {onMouseSpeedChangeCallback = func;}
    void onScrollspeedChange(valChangeFunc func) {onScrollspeedChangeCallback = func;}
    void onBrightnessChange(valChangeFunc func) {onBrightnessChangeCallback = func;}
    void onRotationChange(valChangeFunc func) {onRotationChangeCallback = func;}
    void onModeChange(valChangeFunc func) {onModeChangeCallback = func;}
    int getXMin();
    int getXMax();
    int getYMin();
    int getYMax();
    void notify(String message);


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

    valChangeFunc onMouseSpeedChangeCallback = NULL;
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
    // Serial.println("GuiWorker constructor called");
}

void GuiWorker::notify(String message) {
    webSocketServer.broadcastTXT("notify?msg=" + message);
    debugPrint("Notified: " + message);
}

void GuiWorker::debugPrint(String str) {
#ifdef DEBUG
    Serial.print("[debug]: ");
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
        Serial.printf("[%u] Received text: %s\n", num, payload);
        String receivedMessage = String((char*)payload);
        // String response = "ESP32 received: " + receivedMessage;
        // webSocketServer.sendTXT(num, receivedMessage.c_str());

        std::vector<String> argNames;
        std::vector<String> args;

        String command = extractCommand(receivedMessage);
        // debugPrint("Command: " + command);

        int numArgs = extractArgs(receivedMessage, argNames, args);
        // debugPrint("Number of arguments: " + String(numArgs));
        for (int i = 0; i < numArgs; i++) {
            // debugPrint("Arg " + String(argNames[i]) + ": " + String(args[i]));
        }
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
        // } else if (command == "setBrightness") {
        //     if (onBrightnessChangeCallback != NULL) {
        //         onBrightnessChangeCallback(args[0].toInt());
        //     }
        } else if (command == "setRotation") {
            // debugPrint("setRotation: " + String(argNames[0]) + " = " + String(args[0]));
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
    <link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
    <title>Control Panel</title>
    <style>
        body {
            background-color: #1a1a1a;
            color: white;
            font-family: Arial, sans-serif;
        }

        .container {
            border: 20px solid #d2e2f2;
            padding: 20px;
            width: 80%;
            margin: 0 auto;
            background-color: #282828;
            display: flex;
            justify-content: space-between;
        }

        .output {
            width: 100%;
            height: 80px;
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
            width: 80%;
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
        <div class="w3-third w3-container w3-blue">
            <h2> Funktinen zuordnen </h2>
            <div class="w3-third w3-container w3-grey">
                <div class="function-dropdown">
                    <label class="label">Swipe-Up</label>
                    <select class="drop-class"></select>
                </div>
            </div>
            
            <div class="function-dropdown">
                <label class="label">Swipe-Down</label>
                <select class="drop-class"></select>
            </div>
            <div class="function-dropdown">
                <label class="label">Swipe-Left</label>
                <select class="drop-class"></select>
            </div>
            <div class="function-dropdown">
                <label class="label">Swipe-Right</label>
                <select class="drop-class"></select>
            </div>
            <div class="function-dropdown">
                <label class="label">Tap</label>
                <select class="drop-class"></select>
            </div>
        </div>


        <div class="w3-twothird w3-container w3-black">
            <div class="screen-rotation">
                <div class="screen-diagram down" id="mouse-screen">
                    <div class="circle"></div>
                </div>
            </div>

            <label for="rotation" class="label">Rotation</label>
            <select id="rotation">
                <option value="1">Down</option>
                <option value="0">Up</option>
                <option value="2">Left</option>
                <option value="3">Right</option>
            </select>
            <button id="paintBtn">paint</button>
        </div>

        <div class="w3-third w3-container w3-grey">
            <textarea class="output" readonly>Text\nText\n...</textarea>

            <label for="mode" class="label">Mode</label>
            <select id="mode">
                <option value="0">Mouse_Mode</option>
                <option value="1">Joystick_Mode</option>
                <option value="2">Scroll_Mode</option>
            </select>

            <div class="slider-container">
                <label for="mousespeed" class="label">Mousespeed</label>
                <input type="range" class="slider" id="mousespeed" min="1" max="10" value="1">
                <output for="mousespeed">1</output>
            </div>

            <div class="slider-container">
                <label for="scrollspeed" class="label">Scrollspeed</label>
                <input type="range" class="slider" id="scrollspeed" min="1" max="10" value="1">
                <output for="scrollspeed">1</output>
            </div>

            <div class="slider-container">
                <label for="brightness" class="label">Brightness</label>
                <input type="range" class="slider" id="brightness" min="1" max="10" value="1">
                <output for="brightness">1</output>
            </div>
        </div>


    <script>
        const rotationSelect = document.getElementById('rotation');
        const modeSelect = document.getElementById('mode');
        const mousespeedSlider = document.getElementById('mousespeed');
        const mousespeedSliderOutput = document.querySelector('output[for="mousespeed"]');
        const scrollspeedSlider = document.getElementById('scrollspeed');
        const scrollspeedSliderOutput = document.querySelector('output[for="scrollspeed"]');
        const brightnessSlider = document.getElementById('brightness');
        const brightnessSliderOutput = document.querySelector('output[for="brightness"]');
        const screenDiagram = document.getElementById('mouse-screen');
        const paintBtn = document.getElementById('paintBtn');
        const pointsArray = []; // Array zur Speicherung der Punkte
        const maxX = screenDiagram.clientWidth;
        const maxY = screenDiagram.clientHeight;

        const dropDowns = document.querySelectorAll('.drop-class');
        const options = ['Option 1', 'Option 2', 'Option 3'];

        dropDowns.forEach(dropDown => {
            options.forEach(optionText => {
                const option = document.createElement('option');
                option.value = optionText.toLowerCase().replace(' ', '_');
                option.text = optionText;
                dropDown.appendChild(option);
            });
        });

        var ws = new WebSocket('ws://' + window.location.hostname + ':81');
        
        function notify(text) {
            if (Notification.permission === 'granted') {
                new Notification(text);
            } else if (Notification.permission !== 'denied') {
                Notification.requestPermission().then(permission => {
                    if (permission === 'granted') {
                        new Notification(text);
                    }
                });
            }
        }

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
            if (command === "notify") {
                notify(args[0]);
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
            var rotation;
            switch (e.target.value) {
                case '0':
                    rotation = 'up';
                    break;
                case '1':
                    rotation = 'down';
                    break;
                case '2':
                    rotation = 'left';
                    break;
                case '3':
                    rotation = 'right';
                    break;
                default:
                    rotation = 'down';
                    break;
            }
            screenDiagram.className = 'screen-diagram ' + rotation;
            console.log('Rotation: ', rotation);
            ws.send("setRotation?rotation=" + e.target.value);
        });

        // Mode change event
        modeSelect.addEventListener('change', (e) => {
            console.log('Mode:', e.target.value);
            ws.send("setMode?mode=" + e.target.value);
        });

        // Mousespeed change event
        mousespeedSlider.addEventListener('input', (e) => {
            console.log('Mousespeed:', e.target.value);
            mousespeedSliderOutput.value = e.target.value;
            ws.send("setMouseSpeed?speed=" + e.target.value);
        });

        // Scrollspeed change event
        scrollspeedSlider.addEventListener('input', (e) => {
            console.log('Scrollspeed:', e.target.value);
            scrollspeedSliderOutput.value = e.target.value;
            ws.send("setScrollSpeed?speed=" + e.target.value);
        });

        // Brightness change event
        brightnessSlider.addEventListener('input', (e) => {
            console.log('Brightness:', e.target.value);
            brightnessSliderOutput.value = e.target.value;
            ws.send("setBrightness?value=" + e.target.value);
        });
    </script>

</body>

</html>




)rawliteral";

}
