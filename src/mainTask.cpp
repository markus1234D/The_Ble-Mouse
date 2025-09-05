#include <Arduino.h>
// #include "DisplayWorker.h"
#include "GuiWorker.h"
// #include "TouchWorker.h"
#include "CST816t_TouchWorker.h"
#include "MouseWorker.h"
// #include "ButtonWorker.h"
// #include "TFT_eSPI.h"
#include "pin_config.h"

#include <Wire.h>
// #include <BleMouse.h>
// #include <BleKeyboard.h>
// #include "BleCombo.h"
// #include "ButtonWorker.h"


#define DEBUG

// DisplayWorker displayWorker;
GuiWorker guiWorker;
// TouchWorker touchWorker;
// CST816TouchWorker cst816touchWorker;
// TwoWire Wire2(PIN_IIC_SDA, PIN_IIC_SCL);
// cst816t touchpad(Wire, PIN_TOUCH_RES, PIN_TOUCH_INT);
CST816t_TouchWorker cst816t_touchWorker;
MouseWorker mouseWorker;
// BleKeyboard bleKeyboard;
// ButtonWorker buttonWorker;


int posModulo(int num, int mod){
    int rest = (num) % mod;
    if (rest < 0) {
        rest = mod + rest;
    }
    return rest;
}

void debugPrint(String str) {
#ifdef DEBUG
    Serial.println(str);
#endif
}




void setup() {
    // Mouse.begin();
    Serial.begin(115200);
    delay(4000);
    debugPrint("Hello World");


    mouseWorker.init();
    mouseWorker.setScrollSpeed(15);
    mouseWorker.setMouseSpeed(2);
    mouseWorker.setMode(MouseWorker::Mode::MOUSE_MODE);

    cst816t_touchWorker.init();
    cst816t_touchWorker.setMaxGestureTime(500);
    cst816t_touchWorker.setRotation(CST816t_TouchWorker::USB_LEFT);
    cst816t_touchWorker.setActionFunction("noGesture", [](int x, int y) {
        mouseWorker.getFunction("move")(x, y);
    });
    // buttonWorker.init();
    guiWorker.init();
    guiWorker.onFunctionChange([](String item, String action, String function) {
        // debugPrint("Function changed: " + item + " " + action + " " + function);
        if (item == "screen" ){
            bool result = cst816t_touchWorker.setActionFunction(action, mouseWorker.getFunction(function));
            if (result) {
                debugPrint("Action function set successfully");
            } else {
                debugPrint("Failed to set action function");
            }
        }
        // else if (item.startsWith("button")) {
        //     buttonWorker.setBtnActionFunction(item, action, mouseWorker.getFunction(function));
        // }
    });

    Keyboard.begin();



    guiWorker.onMouseSpeedChange([](int speed) {
            mouseWorker.setMouseSpeed(speed);
    });
    guiWorker.onScrollspeedChange([](int speed) {
        mouseWorker.setScrollSpeed(speed);
    });
    // guiWorker.onBrightnessChange([](int brightness) {
    //     displayWorker.setBrightness(brightness);
    // });
    guiWorker.onRotationChange([](int rotation) {
        debugPrint("Rotation: " + String((CST816t_TouchWorker::Rotation)rotation));
        cst816t_touchWorker.setRotation((CST816t_TouchWorker::Rotation)rotation);
    });
    guiWorker.onModeChange([](int mode) {
        mouseWorker.setMode((MouseWorker::Mode)mode);
    });
}

void loop() {
    guiWorker.handleGui();
    cst816t_touchWorker.handleTouch();
    delay(20);
}