#include <Arduino.h>
#include <vector>

class GuiWorker {

    typedef void (*valChangeFunc)(int attribute);   

public:
    // Constructor
    // GuiWorker();
    void init();
    void handleGui();
    String getHtml();
    String getName();
    void sendXY(int x, int y);
    void onMoseSpeedChange(valChangeFunc func) {onMoseSpeedChangeCallback = func;}
    void onScrollspeedChange(valChangeFunc func);
    void onBrightnessChange(valChangeFunc func);
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

    valChangeFunc onMoseSpeedChangeCallback = NULL;
    valChangeFunc onScrollspeedChangeCallback = NULL;
    valChangeFunc onBrightnessChangeCallback = NULL;
};


void GuiWorker::init() {
    
}

String GuiWorker::getHtml() {
    return R"rawliteral(
    
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Zeichenfeld mit Reset</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        #canvas {
            border: 2px solid black;
            cursor: crosshair;
            image-rendering: pixelated; /* Pixelige Darstellung */
        }
        #controls {
            margin-top: 20px;
        }
        #coordinates {
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <h1>Zeichnen mit der Maus</h1>
    <canvas id="canvas" width="170" height="320"></canvas>
    <div id="coordinates">X: 0, Y: 0</div>
    <div id="controls">
        <button id="resetButton">Zurücksetzen</button>
        <label for="brightness">Helligkeit:</label>
        <input type="range" id="brightness" min="0" max="255" value="128">

    </div>

    <script>
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

        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');
        const coordinatesDiv = document.getElementById('coordinates');
        const resetButton = document.getElementById('resetButton');
        const brightnessInput = document.getElementById('brightness');

        let webDrawing = false;

        var ws = new WebSocket('ws://' + window.location.hostname + ':81');
        ws.onopen = function() {
            console.log("Hello, Server");
        };
        ws.onmessage = function (evt) {
            console.log("msg: " + evt.data);
            if (evt.data == "clear") {
                ctx.clearRect(0, 0, canvas.width, canvas.height);
            }
            const command = extractCommand(evt.data);
            const { argNames, args } = extractArgs(evt.data);
            console.log("Command: " + command);
            console.log("ArgNames: " + argNames);
            console.log("Args: " + args);
            if (command === "/coord") {
                const x = 170-parseInt(args[0]);
                const y = 320-parseInt(args[1]);
                
                if(webDrawing) {
                    ctx.lineTo(x, y);
                    ctx.stroke();
                } else {
                    webDrawing = true;
                    ctx.beginPath();
                    ctx.moveTo(x, y);
                }
                
            } else if (command === "clear") {
                ctx.clearRect(0, 0, canvas.width, canvas.height);
            } else if (command === "release") {
                webDrawing = false;
                console.log("release");
                
            }

        };
        ws.onclose = function() {
            console.log("Connection is closed...");
        };

        let drawing = false;

        // Linienbreite für größere "Pixel"
        ctx.lineWidth = 5;  // Erhöht die Linienstärke
        ctx.linecolor = "red"; // Linienfarbe
        // Maus-Position verfolgen und anzeigen
        canvas.addEventListener('mousemove', (e) => {
            if (!drawing) {
                return;
            }
            const rect = canvas.getBoundingClientRect();
            const x = e.clientX - rect.left;
            const y = e.clientY - rect.top;
            coordinatesDiv.textContent = `X: ${Math.round(x)}, Y: ${Math.round(y)}`;
            console.log("/coord?X=" + Math.round(x) + "&Y=" + Math.round(y));
            ws.send("/coord?X=" + Math.round(x) + "&Y=" + Math.round(y));
            if (drawing) {

                ctx.lineTo(x, y);
                ctx.strokeStyle = '#ff0000';    // Linienfarbe rot
                ctx.stroke();
            }
        });

        // Zeichnen beginnen
        canvas.addEventListener('mousedown', (e) => {
            drawing = true;
            ctx.beginPath();
            const rect = canvas.getBoundingClientRect();
            const pointX = e.clientX - rect.left;
            const pointY = e.clientY - rect.top;
            console.log("/coord?X=" + Math.round(pointX) + "&Y=" + Math.round(pointY));
            ws.send("/coord?X=" + pointX + "&Y=" + pointY);
            ctx.moveTo(pointX, pointY);
        });

        // Zeichnen beenden
        canvas.addEventListener('mouseup', () => {
            drawing = false;

        });

        // Zeichnung zurücksetzen
        resetButton.addEventListener('click', () => {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            coordinatesDiv.textContent = `X: 0, Y: 0`;
            ws.send("clear");
        });

        brightnessInput.addEventListener("input", function() {
        setTimeout(() => {
            const value = brightnessInput.value;
            console.log("brightness?value=" + value);
            ws.send("brightness?value=" + value);
        }, 100
    );
    });
        function updateBrightness(value) {
            ws.send("brightness?value=" + value);
        }
    </script>
</body>
</html>




)rawliteral";

}