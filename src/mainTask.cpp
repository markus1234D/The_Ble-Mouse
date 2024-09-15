#include <Arduino.h>
// #include "CommunicationWorker.h"
// #include "DisplayWorker.h"
// #include "GuiWorker.h"
// #include "TouchWorker.h"
// #include "CST816TouchWorker.h"
#include "CST816t_TouchWorker.h"
// #include "TFT_eSPI.h"
#include "pin_config.h"
// #include <BleMouse.h>
#include <Wire.h>

// bool checkGesture(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx);

// BleMouse bleMouse;

// CommunicationWorker communicationWorker;
// DisplayWorker displayWorker;
// GuiWorker guiWorker;
// TouchWorker touchWorker;
// CST816TouchWorker cst816touchWorker;
// TwoWire Wire2(PIN_IIC_SDA, PIN_IIC_SCL);
// cst816t touchpad(Wire, PIN_TOUCH_RES, PIN_TOUCH_INT);
CST816t_TouchWorker cst816t_touchWorker;


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
//         bleMouse.move(xDiff, yDiff, 0);
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
//     bleMouse.move(xDiff, yDiff, 0);
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
    // bleMouse.begin();
    Serial.begin(115200);
    delay(4000);
    Serial.println("Hello World");
    
    // communicationWorker.init();
    // displayWorker.init();
    // guiWorker.init();
    // touchWorker.init();

    // touchWorker.onPress(&joystickMousePressFunc);
    // touchWorker.onRelease(&joystickMouseReleaseFunc);
    // touchWorker.onMove(&mouseMoveFunc);
    cst816t_touchWorker.init();
    cst816t_touchWorker.onSwipeLeft([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE LEFT");
    });
    cst816t_touchWorker.onSwipeRight([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE RIGHT");
    });
    cst816t_touchWorker.onSwipeUp([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE UP");
    });
    cst816t_touchWorker.onSwipeDown([](uint16_t x, uint16_t y) {
        Serial.println("SWIPE DOWN");
    });
    
}

void loop() {
    // communicationWorker.handleCommunication();
    // touchWorker.handleTouch();
    // delay(500);
    cst816t_touchWorker.handleTouch();
    delay(20);
}