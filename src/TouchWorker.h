#include <Arduino.h>
#include "TouchDrvCSTXXX.hpp"
#include "pin_config.h"
#include <Wire.h>

#define TOUCH_ARRAY_SIZE 10


class TouchWorker {
    typedef void (TouchWorker::*functionPointer)(void);

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
    void onPress(TouchWorker::functionPointer callback);
    void onMove(TouchWorker::functionPointer callback);
    void onRelease(TouchWorker::functionPointer callback);

private:
    // Private member variables
    int x_y_homeTouch[TOUCH_ARRAY_SIZE][3];
    TouchDrvCSTXXX touch;
    unsigned long lastMillis = 0;
    int v_pasteIdx = 0;
    TouchWorker::functionPointer onPressCallback = NULL;
    TouchWorker::functionPointer onMoveCallback = NULL;
    TouchWorker::functionPointer onReleaseCallback = NULL;

    enum DataIdx {
    TOUCH_X,
    TOUCH_Y,
    HOME_BUTTON
};


    // Private member functions
};

int TouchWorker::pasteIdx(int steps){
    return (v_pasteIdx + steps) % TOUCH_ARRAY_SIZE;
}

void TouchWorker::handleTouch() {

    if(millis() - lastMillis > 50) {
        lastMillis = millis();
        // uint8_t touchNum = touch.getSupportTouchPoint();
        // Serial.println("Touch number: " + String(touchNum));
        int16_t xArr, yArr;
        uint16_t touched = touch.getPoint(&xArr, &yArr, 1);


        if(touched > 0){
            x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_X] = round((int)xArr);
            x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_Y] = round((int)yArr);
            // Serial.println("Touch detected");
            if(x_y_homeTouch[TouchWorker::pasteIdx(-1)][TouchWorker::DataIdx::TOUCH_X] == -1) {
                //press
                Serial.println("First Touch");
                if(onPressCallback != NULL) {
                    (this->*onPressCallback)();
                }
            }
            // Serial.println("touch x: " + String(xArr) + " y: " + String(yArr));
            // tft.fillCircle(/* 320- */yArr, 170-xArr, 5, TFT_WHITE);
            // webSocketServer.broadcastTXT("/coord?X=" + String(xArr) + "&Y=" + String(yTouch));
            
        } else {
            x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_X] = -1;
            x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_Y] = -1;
            // Serial.println("Touch not ");
            if (x_y_homeTouch[TouchWorker::pasteIdx(-2)][TouchWorker::DataIdx::TOUCH_X] != -1){
                //release
                Serial.println("Touch released");
                // webSocketServer.broadcastTXT("release");
            }
        }
        Serial.println("Touch x: " + String(x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_X]) + " y: " + String(x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_Y]));
        v_pasteIdx++;
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
    // touch.setMaxCoordinates(536, 240);

    // Set swap xy
    // touch.setSwapXY(true);

    // Set mirror xy
    // touch.setMirrorXY(true, false);

}