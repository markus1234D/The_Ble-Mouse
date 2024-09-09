#include <Arduino.h>
#include "TouchDrvCSTXXX.hpp"
#include "pin_config.h"
#include <Wire.h>

enum Gesture {
    NONE,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN,
    TAP,
    UNKNOWN
};

struct Data {
    int x;
    int y;
    std::vector<Gesture> gestureList;
};

class TouchWorker {
    typedef void (*functionPointer)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx);
    typedef void (*functionPointer_d)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx);


public:
    // Constructor
    // TouchWorker();

    // Destructor
    // ~TouchWorker();

    // Public member functions
    void init();
    void handleTouch();
    bool getTouch(int &x, int &y);
    int pasteIdx(int steps=0);
    void onPress(void (*callback)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx));
    void onMove(void (*callback)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx));
    void onRelease(void (*callback)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx));

private:
    // Private member variables
    Data touchDataArr[TOUCH_ARRAY_SIZE];
    TouchDrvCSTXXX touch;
    unsigned long lastMillis = 0;
    int v_pasteIdx = 0;
    void (*onPressCallback)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) = NULL;
    void (*onMoveCallback)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) = NULL;
    void (*onReleaseCallback)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx) = NULL;

    enum DataIdx {
    TOUCH_X,
    TOUCH_Y,
    HOME_BUTTON
    };

    const int swipeSteps = 5;
    const int longPressSteps = 5;
    const int swipeThreshold = 10;

    // Private member functions
    void checkGesture();
    void analyseGestureArray();
};

void TouchWorker::checkGesture() {
    int x = touchDataArr[pasteIdx()].x;
    int y = touchDataArr[pasteIdx()].y;
    int xPrev = touchDataArr[pasteIdx(-1)].x;
    int xPrev2 = touchDataArr[pasteIdx(-2)].x;
    int yPrev = touchDataArr[pasteIdx(-1)].y;
    int yPrev2 = touchDataArr[pasteIdx(-2)].y;
    if (xPrev < 0 || xPrev2 < 0 || yPrev < 0 || yPrev2 < 0) {
        return;
    }
    // swipe left
    if (x - xPrev < -swipeThreshold) {
        // Serial.println("Swipe left");
        if(std::find(touchDataArr[pasteIdx()].gestureList.begin(), touchDataArr[pasteIdx()].gestureList.end(), Gesture::SWIPE_LEFT) == touchDataArr[pasteIdx()].gestureList.end()) {
            touchDataArr[pasteIdx()].gestureList.push_back(Gesture::SWIPE_LEFT);
        }
    }
    // swipe right
    if (x - xPrev > swipeThreshold) {
        // Serial.println("Swipe right");
        if(std::find(touchDataArr[pasteIdx()].gestureList.begin(), touchDataArr[pasteIdx()].gestureList.end(), Gesture::SWIPE_RIGHT) == touchDataArr[pasteIdx()].gestureList.end()) {
            touchDataArr[pasteIdx()].gestureList.push_back(Gesture::SWIPE_RIGHT);
        }
    }
    // long press
    // int longPressThreshold = 5;
    // if(abs(x - xPrev) < longPressThreshold && abs(xPrev - xPrev2) < longPressThreshold && abs(y - yPrev) < longPressThreshold && abs(yPrev - yPrev2) < longPressThreshold) {
    //     Serial.println("Long press");
    // }
}

void TouchWorker::analyseGestureArray() {
    std::vector<Gesture> gestures = touchDataArr[pasteIdx()].gestureList;
    gestures.erase(std::remove(gestures.begin(), gestures.end(), Gesture::NONE), gestures.end());

    for(Gesture gesture : gestures){
        // Serial.println("gesture: " + String(gesture));
        for (int i=1; i<swipeSteps; i++) {
            if(std::find(touchDataArr[pasteIdx(-i)].gestureList.begin(), touchDataArr[pasteIdx(-i)].gestureList.end(), gesture) != gestures.end()) {
                // found
                // Serial.println("Found gesture: " + String(gesture) + " in past " + String(i) + " steps");
                // let it in gestures
            } else {
                // gesture not found in past i steps
                // Serial.println("Gesture: " + String(gesture) + " not found in past " + String(i) + " steps");
                gestures.erase(std::remove(gestures.begin(), gestures.end(), gesture), gestures.end());
                break;  // break for loop
            }
        }
    }
    Serial.print("Gestures: ");
    for (Gesture gesture : gestures) {
        Serial.print(String(gesture) + ", ");
        // if (gesture == Gesture::SWIPE_LEFT) {
        //     Serial.println("Swipe left");
        // }
        // if (gesture == Gesture::SWIPE_RIGHT) {
        //     Serial.println("Swipe right");
        // }
    }
    Serial.println();
}

void TouchWorker::onPress(void (*func)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx)) {
    onPressCallback = func;
}
void TouchWorker::onMove(void (*func)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx)) {
    onMoveCallback = func;
}
void TouchWorker::onRelease(void (*func)(Data (&array)[TOUCH_ARRAY_SIZE], int currentIdx)) {
    onReleaseCallback = func;
}

int TouchWorker::pasteIdx(int steps){
    int idx = (v_pasteIdx + steps) % TOUCH_ARRAY_SIZE;
    if (idx < 0) {
        idx = TOUCH_ARRAY_SIZE + idx;
    }
    return idx;
}

void TouchWorker::handleTouch() {

    if(millis() - lastMillis > 50) {
        lastMillis = millis();
        // uint8_t touchNum = touch.getSupportTouchPoint();
        // Serial.println("Touch number: " + String(touchNum));
        int16_t xArr, yArr;
        uint16_t touched = touch.getPoint(&xArr, &yArr, 1);
        int x = touched ? (int)xArr : -1;
        int y = touched ? (int)yArr : -1;
        touchDataArr[pasteIdx()].x = x;
        touchDataArr[pasteIdx()].y = y;
        touchDataArr[pasteIdx()].gestureList.push_back(Gesture::NONE);

        if(touched > 0){
            // Serial.println("Touch detected");

                
            if(touchDataArr[TouchWorker::pasteIdx(-1)].x == -1) {
                //press
                // touchDataArr[pasteIdx()].gestureList.push_back(Gesture::TAP);
                // Serial.println("First Touch");
                if(onPressCallback != NULL) {
                    (this->onPressCallback)(touchDataArr, pasteIdx());
                }
            } else {
                //move
                checkGesture();
                // Serial.println("Touch moved");
                if(onMoveCallback != NULL) {
                    (this->onMoveCallback)(touchDataArr, v_pasteIdx);
                } 
            }

            // Serial.println("touch x: " + String(xArr) + " y: " + String(yArr));
            // tft.fillCircle(/* 320- */yArr, 170-xArr, 5, TFT_WHITE);
            // webSocketServer.broadcastTXT("/coord?X=" + String(xArr) + "&Y=" + String(yTouch));
            
        } else {
            if (touchDataArr[TouchWorker::pasteIdx(-1)].x != -1){
                //release
                // Serial.println("Touch released");
                // webSocketServer.broadcastTXT("release");
                if(onReleaseCallback != NULL) {
                    (this->onReleaseCallback)(touchDataArr, pasteIdx());
                }
                analyseGestureArray();
                // if swipeLeft && x_y_homeTouch[pasteIdx(-1)].gesture == SWIPE_LEFT {
                //     gestureCounter++;
                //     Serial.println("Swipe left");
                // }
            }
        }
        // Serial.println("idx: " + String(pasteIdx()) + " x: " + String(x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_X]) + " y: " + String(x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_Y]));
        v_pasteIdx = pasteIdx(1);
    }
}

void TouchWorker::init() {
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);


#if SENSOR_RST != -1
    pinMode(PIN_TOUCH_RES, OUTPUT);
    digitalWrite(PIN_TOUCH_RES, LOW);
    delay(30);
    digitalWrite(PIN_TOUCH_RES, HIGH);
    delay(50);
    // delay(1000);
#endif

    // Search for known CSTxxx device addresses
    uint8_t address = 0x15;
    Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);

    touch.setPins(PIN_TOUCH_RES, PIN_TOUCH_INT);
    touch.begin(Wire, address, PIN_IIC_SDA, PIN_IIC_SCL);

    Serial.print("Model :"); Serial.println(touch.getModelName());

    // T-Display-S3 CST816 touch panel, touch button coordinates are is 85 , 160
    touch.setCenterButtonCoordinate(85, 360);

    // T-Display-AMOLED 1.91 Inch CST816T touch panel, touch button coordinates is 600, 120.
    // touch.setCenterButtonCoordinate(600, 120);  // Only suitable for AMOLED 1.91 inch


    // Depending on the touch panel, not all touch panels have touch buttons.
    touch.setHomeButtonCallback([](void *user_data) {
        Serial.println("Home key pressed!");
        // data[(int)DataIdx::HOME_BUTTON][pasteIdx] = 1;
    }, NULL);

    touch.enableAutoSleep();
    
    // Unable to obtain coordinates after turning on sleep
    // CST816T sleep current = 1.1 uA
    // CST226SE sleep current = 60 uA
    // touch.sleep();

    // Set touch max xy
    touch.setMaxCoordinates(EXAMPLE_LCD_V_RES, EXAMPLE_LCD_H_RES);

    // Set swap xy
    // touch.setSwapXY(true);

    // Set mirror xy
    touch.setMirrorXY(true, true);

}