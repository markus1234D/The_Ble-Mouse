#include <Arduino.h>
// #include "CommunicationWorker.h"
// #include "DisplayWorker.h"
// #include "GuiWorker.h"
// #include "TouchWorker.h"
#include "CST816t_TouchWorker.h"
#include "MouseWorker.h"
// #include "TFT_eSPI.h"
#include "pin_config.h"

#include <Wire.h>
// #include <BleMouse.h>
// #include <BleKeyboard.h>
#include "BleCombo.h"


// bool checkGesture(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx);

// BleMouse Mouse;

// CommunicationWorker communicationWorker;
// DisplayWorker displayWorker;
// GuiWorker guiWorker;
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

// void mouseMoveFunc(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) {
//     // checkGesture(array, currentIdx);

//     if(array[currentIdx].x != -1) {
//         int prevIdx = posModulo(currentIdx-1, TOUCH_ARRAY_SIZE);

//         int x = array[currentIdx].x;
//         int y = array[currentIdx].y;
//         int xPrev = array[prevIdx].x;
//         int yPrev = array[prevIdx].y;
//         // Serial.println("prevIdx: " + String(prevIdx) + " xPrev: " + String(xPrev) + " yPrev: " + String(yPrev));
//         // Serial.println("currIdx: " + String(currentIdx) + " x: " + String(array[currentIdx].x) + " y: " + String(array[currentIdx].y));
//         int xDiff = array[currentIdx].x-array[prevIdx].x;
//         int yDiff = array[currentIdx].y-array[prevIdx].y;
//         Mouse.move(xDiff, yDiff, 0);
//         // Serial.println("xDiff: " + String(xDiff) + " yDiff: " + String(yDiff));
//     }
// }

// int xCenter = -1;
// int yCenter = -1;
// void joystickMousePressFunc(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) {
//         xCenter = array[currentIdx].x;
//         yCenter = array[currentIdx].y;
// }
// void joystickMouseReleaseFunc(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) {
//         xCenter = -1;
//         yCenter = -1;
// }
// void joystickMouseMoveFunc(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) {
//     int xDiff = array[currentIdx].x-xCenter;
//     int yDiff = array[currentIdx].y-yCenter;
//     if (xDiff < 5){ xDiff = 0; }
//     if (yDiff < 5){ yDiff = 0; }
//     Mouse.move(xDiff, yDiff, 0);
// }

// int swipeThreshold = 20;
// bool checkGesture(Data (&touchDataArr)[TOUCH_ARRAY_SIZE], int currentIdx) {
//     // swipe left
//     if (touchDataArr[currentIdx].x - touchDataArr[posModulo(currentIdx-1, TOUCH_ARRAY_SIZE)].x < -swipeThreshold) {
//         Serial.println("Swipe left");
//         return true;
//     }
//     // swipe right
//     if (touchDataArr[currentIdx].x - touchDataArr[posModulo(currentIdx-1, TOUCH_ARRAY_SIZE)].x > swipeThreshold) {
//         Serial.println("Swipe right");
//         return true;
//     }
//     // long press
//     int x = touchDataArr[currentIdx].x;
//     int y = touchDataArr[currentIdx].y;
//     int xPrev = touchDataArr[posModulo(currentIdx-1, TOUCH_ARRAY_SIZE)].x;
//     int xPrev2 = touchDataArr[posModulo(currentIdx-2, TOUCH_ARRAY_SIZE)].x;
//     int yPrev = touchDataArr[posModulo(currentIdx-1, TOUCH_ARRAY_SIZE)].y;
//     int yPrev2 = touchDataArr[posModulo(currentIdx-2, TOUCH_ARRAY_SIZE)].y;
//     if (xPrev < 0 || xPrev2 < 0 || yPrev < 0 || yPrev2 < 0) {
//         return false;
//     }
//     int longPressThreshold = 5;
//     if(abs(x - xPrev) < longPressThreshold && abs(xPrev - xPrev2) < longPressThreshold && abs(y - yPrev) < longPressThreshold && abs(yPrev - yPrev2) < longPressThreshold) {
//         Serial.println("Long press");
//         return true;
//     }
// }

void setup() {
    // Mouse.begin();
    Serial.begin(115200);
    delay(4000);
    Serial.println("Hello World");
    // bleKeyboard.begin();
    // communicationWorker.init();
    // displayWorker.init();
    // guiWorker.init();
    // touchWorker.init();
    mouseWorker.init();
    cst816t_touchWorker.init();

    Keyboard.begin();

    // touchWorker.onPress(&joystickMousePressFunc);
    // touchWorker.onRelease(&joystickMouseReleaseFunc);
    // touchWorker.onMove(&mouseMoveFunc);

    cst816t_touchWorker.onSwipeLeft([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE LEFT");
        mouseWorker.nextMode();
    });
    cst816t_touchWorker.onSwipeRight([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE RIGHT");
        Mouse.click(MOUSE_RIGHT);
    });
    cst816t_touchWorker.onSwipeUp([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE UP");
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.print("c");
        delay(100);
        Keyboard.releaseAll();
    });
    cst816t_touchWorker.onSwipeDown([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE DOWN");
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.print("v");
        delay(100);
        Keyboard.releaseAll();
    });
    cst816t_touchWorker.onSingleClick ([](uint16_t x, uint16_t y) {
        Serial.println("SINGLE CLICK");
        mouseWorker.press((int)x, (int)y);
    });
    cst816t_touchWorker.onSingleClickRelease([](uint16_t x, uint16_t y) {
        Serial.println("SINGLE CLICK RELEASE");
        mouseWorker.release();
        Mouse.click();
    });
    cst816t_touchWorker.onDoubleClick([](uint16_t x, uint16_t y) {
        Serial.println("DOUBLE CLICK");
        mouseWorker.press((int)x, (int)y);
        Mouse.click();
    });
    cst816t_touchWorker.onDoubleClickRelease([](uint16_t x, uint16_t y) {
        Serial.println("DOUBLE CLICK RELEASE");
        mouseWorker.release();
    });
    // cst816t_touchWorker.onLongPress([](uint16_t x, uint16_t y) {
    //     Serial.println("LONG PRESS");
    // });
    // cst816t_touchWorker.onLongPressRelease([](uint16_t x, uint16_t y) {
    //     Serial.println("LONG PRESS RELEASE");
    // });
    cst816t_touchWorker.onNoGesture([](uint16_t x, uint16_t y) {
        mouseWorker.move((int)x, (int)y);
    });
    cst816t_touchWorker.setMaxGestureTime(300);
}

void loop() {
    // communicationWorker.handleCommunication();
    // touchWorker.handleTouch();
    // delay(500);
    cst816t_touchWorker.handleTouch();
    delay(20);
}