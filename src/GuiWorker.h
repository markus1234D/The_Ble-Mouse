#include <Arduino.h>
#include <vector>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include "pin_config.h"
#include <ESPAsyncWebServer.h>
#include "TFT_eSPI.h"


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
    TFT_eSPI tft;

    valChangeFunc onMouseSpeedChangeCallback = NULL;
    valChangeFunc onScrollspeedChangeCallback = NULL;
    valChangeFunc onBrightnessChangeCallback = NULL;
    valChangeFunc onRotationChangeCallback = NULL;
    valChangeFunc onModeChangeCallback = NULL;
    funcChangeFunc onGestureFunctionChangeCallback = NULL;
    

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
        } else if (command == "gestureFunction") {
            if (onGestureFunctionChangeCallback != NULL && numArgs >= 3) {
                // item, action, function
                onGestureFunctionChangeCallback(args[0], args[1], args[2]);
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
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Config GUI</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<style>
		body { font-family: Arial, sans-serif; margin: 2em; }
		header { margin-bottom: 2em; text-align: center;}
		main { margin: auto; }
    .row-container {
      display: flex; /* Flexbox aktivieren */
      flex-wrap: wrap; /* Zeilenumbruch aktivieren */
      padding: 10px;
      column-gap: 20px; /* Abstand zwischen den Spalten */
      justify-content: center; /* Zentriert die Spalten */
      text-align: center;
	  	width: 100%;
    }

    .column-container {
      flex: 1; /* Gleichmäßige Breite für alle Spalten */
      display: flex;
      flex-direction: column; /* Spaltenanordnung */
      border-color: #4CAF50;
      border-style: solid;
      border-width: 2px;
      color: white;
      text-align: center;
      padding: 20px;
      border-radius: 8px;
      align-items: center;
      justify-content: center;
			min-width: 40%;
			row-gap: 10px; /* Abstand zwischen den Zeilen */
    }
	</style>
</head>
<body>
	<header>
		<h1>Configuration GUI</h1>
	</header>
	<main>
		<div class="row-container">
			<div class="column-container">
				<div class="row-container">
						<h2>Touch Control</h2>
				</div>
				<div class="row-container">
					<div class="column-container">
						<h2>Screen Rotation</h2>
						<label for="rotation" class="label">USB Position</label>
            <select id="rotation">
                <option value="1">Down</option>
                <option value="0">Up</option>
                <option value="2">Left</option>
                <option value="3">Right</option>
            </select>
					</div>
					<div class="column-container">
						<h2>Gesture functions</h2>
						<label for="swipe-up">Swipe-Up:</label>
            <select class="function-select" id="swipe-up">
							<!-- wird in js befüllt -->
            </select>
						<label for="swipe-down">Swipe-Down:</label>
						<select class="function-select" id="swipe-down">
							<!-- wird in js befüllt -->
						</select>
						<label for="swipe-left">Swipe-Left:</label>
						<select class="function-select" id="swipe-left">
							<!-- wird in js befüllt -->
						</select>
						<label for="swipe-right">Swipe-Right:</label>
						<select class="function-select" id="swipe-right">
							<!-- wird in js befüllt -->
						</select>
					</div>
				</div>
			</div>
			<div class="column-container">
				<h2>Button Control</h2>
					<div class="row-container">
						<label>Action:</label>
						<label>Function:</label>
					</div>
				Button 1:
				<div class="btn_functions" id="button1">
					<div class="row-container">
						<button id="add_funk_to_button1" class="add_funk_btn">+</button>
					</div>
				</div>
				Button 2:
				<div class="btn_functions" id="button2">
					<div class="row-container">
						<button id="add_funk_to_button2" class="add_funk_btn">+</button>
					</div>
				</div>
			</div>
		</div>

		<script>

			const gestureFunctions = [
				{ value: 'none', text: 'Keine Funktion' },
				{ value: 'open_menu', text: 'Menü öffnen' },
				{ value: 'scroll_up', text: 'Scrollen nach oben' },
				{ value: 'scroll_down', text: 'Scrollen nach unten' },
				{ value: 'scroll_left', text: 'Scrollen nach links' },
				{ value: 'scroll_right', text: 'Scrollen nach rechts' },
				{ value: 'volume_up', text: 'Lautstärke erhöhen' },
				{ value: 'volume_down', text: 'Lautstärke verringern' },
				{ value: 'copy', text: 'Kopieren' },
				{ value: 'paste', text: 'Einfügen' },
				{ value: 'undo', text: 'Rückgängig' },
				{ value: 'redo', text: 'Wiederholen' }
			];
			const buttonActions = [
				{ value: 'none', text: 'Keine Aktion' },
				{ value: 'click', text: 'Klick' },
				{ value: 'long_press', text: 'Langer Druck' },
				{ value: 'double_click', text: 'Doppelklick' },
				{ value: 'hold_swipe up', text: 'Halten und nach oben wischen' },
				{ value: 'hold_swipe_down', text: 'Halten und nach unten wischen' },
				{ value: 'hold_swipe_left', text: 'Halten und nach links wischen' },
				{ value: 'hold_swipe_right', text: 'Halten und nach rechts wischen' }
			];

			// Fülle die Dropdowns für Gestenfunktionen
			const swipe_function_selects = document.querySelectorAll('.function-select');
			swipe_function_selects.forEach(select => {
				gestureFunctions.forEach(func => {
					const option = document.createElement('option');
					option.value = func.value;
					option.textContent = func.text;
					select.appendChild(option);
				});
			});
			swipe_function_selects.forEach(select => {
				select.addEventListener('change', function() {
					const url = `funcChange?item=screen&action=${this.id}&function=${this.value}`;
					console.log(url);
					// TODO: Webserver send Message
				});
			});

			// Event Delegation für dynamisch hinzugefügte Remove-Buttons
			document.body.addEventListener('click', function(e) {
				if (e.target.classList.contains('remove_funk_btn')) {
					const btnFunctions = e.target.parentElement.parentElement;
					if (btnFunctions.children.length > 1) {
						btnFunctions.removeChild(e.target.parentElement);
					}
				}
			});
			// bei klick auf + Button ein neues Action-Function Paar über dem +-Button hinzufügen
			document.querySelectorAll('.add_funk_btn').forEach(button => {
				button.addEventListener('click', function() {
					const btnFunctions = this.parentElement.parentElement;
					const newFunctionRow = document.createElement('div');
					newFunctionRow.classList.add('row-container');
					newFunctionRow.innerHTML = `
						<select class="action-select">
							<!-- wird in js befüllt -->
						</select>
						<select class="function-select">
							<!-- wird in js befüllt -->
						</select>
						<button class="remove_funk_btn">-</button>
					`;
					btnFunctions.insertBefore(newFunctionRow, this.parentElement);

					const actionSelect = newFunctionRow.querySelector('.action-select');
					buttonActions.forEach(action => {
						const option = document.createElement('option');
						option.value = action.value;
						option.textContent = action.text;
						actionSelect.appendChild(option);
					});
					actionSelect.addEventListener('change', function() {
						const url = `funcChange?item=${btnFunctions.id}&action=${actionSelect.value}&function=${functionSelect.value}`;
						console.log(url);
						// TODO: Webserver send Message
					});

					const functionSelect = newFunctionRow.querySelector('.function-select');
					gestureFunctions.forEach(func => {
						const option = document.createElement('option');
						option.value = func.value;
						option.textContent = func.text;
						functionSelect.appendChild(option);
					});
					functionSelect.addEventListener('change', function() {
						const url = `funcChange?item=${btnFunctions.id}&action=${actionSelect.value}&function=${functionSelect.value}`;
						console.log(url);
						// TODO: Webserver send Message
					});
				});
			});

			const rotationSelect = document.getElementById('rotation');
			rotationSelect.addEventListener('change', function() {
				const url = `rotation?value=${this.value}`;
				console.log(url);
				// TODO: Webserver send Message
			});
		</script>
	</main>
</body>
</html>




)rawliteral";

}
