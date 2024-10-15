#include <Arduino.h>
// #include "DisplayWorker.h"
#include "GuiWorker.h"
// #include "TouchWorker.h"
#include "CST816t_TouchWorker.h"
#include "MouseWorker.h"
// #include "TFT_eSPI.h"
#include "pin_config.h"

#include <Wire.h>
// #include <BleMouse.h>
// #include <BleKeyboard.h>
#include "BleCombo.h"

#define DEBUG
// bool checkGesture(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx);

// BleMouse Mouse;

// DisplayWorker displayWorker;
GuiWorker guiWorker;
// TouchWorker touchWorker;
// CST816TouchWorker cst816touchWorker;
// TwoWire Wire2(PIN_IIC_SDA, PIN_IIC_SCL);
// cst816t touchpad(Wire, PIN_TOUCH_RES, PIN_TOUCH_INT);
CST816t_TouchWorker cst816t_touchWorker;
MouseWorker mouseWorker;
// BleKeyboard bleKeyboard;

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
    // displayWorker.init();
    guiWorker.init();
    // touchWorker.init();
    mouseWorker.init();
    mouseWorker.setScrollSpeed(15);
    mouseWorker.setMouseSpeed(2);
    cst816t_touchWorker.init();
    cst816t_touchWorker.setMaxGestureTime(300);
    cst816t_touchWorker.setRotation(CST816t_TouchWorker::USB_LEFT);

    Keyboard.begin();

    // touchWorker.onPress(&joystickMousePressFunc);
    // touchWorker.onRelease(&joystickMouseReleaseFunc);
    // touchWorker.onMove(&mouseMoveFunc);

    cst816t_touchWorker.onSwipeLeft([](uint16_t x, uint16_t y) {
        debugPrint("SWIPE LEFT");
        mouseWorker.nextMode();
    });
    cst816t_touchWorker.onSwipeRight([](uint16_t x, uint16_t y) {
        debugPrint("SWIPE RIGHT");
        Mouse.click(MOUSE_RIGHT);
    });
    cst816t_touchWorker.onSwipeUp([](uint16_t x, uint16_t y) {
        debugPrint("SWIPE UP");
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.print("c");
        delay(100);
        Keyboard.releaseAll();
    });
    cst816t_touchWorker.onSwipeDown([](uint16_t x, uint16_t y) {
        debugPrint("SWIPE DOWN");
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.print("v");
        delay(100);
        Keyboard.releaseAll();
    });
    cst816t_touchWorker.onSingleClick ([](uint16_t x, uint16_t y) {
        debugPrint("SINGLE CLICK");
        mouseWorker.press((int)x, (int)y);
    });
    cst816t_touchWorker.onSingleClickRelease([](uint16_t x, uint16_t y) {
        debugPrint("SINGLE CLICK RELEASE");
        mouseWorker.release();
        Mouse.click();
    });
    cst816t_touchWorker.onDoubleClick([](uint16_t x, uint16_t y) {
        debugPrint("DOUBLE CLICK");
        mouseWorker.press((int)x, (int)y);
        Mouse.click();
    });
    cst816t_touchWorker.onDoubleClickRelease([](uint16_t x, uint16_t y) {
        debugPrint("DOUBLE CLICK RELEASE");
        mouseWorker.release();
    });
    cst816t_touchWorker.onLongPress([](uint16_t x, uint16_t y) {
        // debugPrint("LONG PRESS");
        Mouse.press(MOUSE_LEFT);
    });
    cst816t_touchWorker.onLongPressRelease([](uint16_t x, uint16_t y) {
        debugPrint("LONG PRESS RELEASE; X:" + String(x) + " Y:" + String(y));
        Mouse.release(MOUSE_LEFT);
    });
    cst816t_touchWorker.onNoGesture([](uint16_t x, uint16_t y) {
        debugPrint("X:" + String(x) + " Y:" + String(y));
        mouseWorker.move((int)x, (int)y);

        guiWorker.sendXY((int)cst816t_touchWorker.getRawX(), (int)cst816t_touchWorker.getRawY());
    });


    guiWorker.onMoseSpeedChange([](int speed) {
        mouseWorker.setMouseSpeed(speed);
    });
    guiWorker.onScrollspeedChange([](int speed) {
        mouseWorker.setScrollSpeed(speed);
    });
    guiWorker.onBrightnessChange([](int brightness) {
        // displayWorker.setBrightness(brightness);
    });
    guiWorker.onRotationChange([](int rotation) {
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