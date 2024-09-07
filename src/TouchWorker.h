#include <Arduino.h>
#include "TouchDrvCSTXXX.hpp"
#include "pin_config.h"
#include <Wire.h>



class TouchWorker {
    typedef void (*functionPointer)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx);


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
    void onPress(void (*callback)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx));
    void onMove(void (*callback)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx));
    void onRelease(void (*callback)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx));

private:
    // Private member variables
    int x_y_homeTouch[TOUCH_ARRAY_SIZE][3];
    TouchDrvCSTXXX touch;
    unsigned long lastMillis = 0;
    int v_pasteIdx = 0;
    void (*onPressCallback)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx) = NULL;
    void (*onMoveCallback)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx) = NULL;
    void (*onReleaseCallback)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx) = NULL;

    enum DataIdx {
    TOUCH_X,
    TOUCH_Y,
    HOME_BUTTON
    };
    // Private member functions
};

void TouchWorker::onPress(void (*func)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx)) {
    onPressCallback = func;
}
void TouchWorker::onMove(void (*func)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx)) {
    onMoveCallback = func;
}
void TouchWorker::onRelease(void (*func)(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx)) {
    onReleaseCallback = func;
}

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
        int x = touched ? (int)xArr : -1;
        int y = touched ? (int)yArr : -1;
        x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_X] = x;
        x_y_homeTouch[pasteIdx()][TouchWorker::DataIdx::TOUCH_Y] = y;

        if(touched > 0){
            // Serial.println("Touch detected");
            if(x_y_homeTouch[TouchWorker::pasteIdx(-1)][TouchWorker::DataIdx::TOUCH_X] == -1) {
                //press
                // Serial.println("First Touch");
                if(onPressCallback != NULL) {
                    (this->onPressCallback)(x_y_homeTouch, pasteIdx());
                }
            } else {
                //move
                // Serial.println("Touch moved");
                if(onMoveCallback != NULL) {
                    (this->onMoveCallback)(x_y_homeTouch, v_pasteIdx);
                } 
            }

            // Serial.println("touch x: " + String(xArr) + " y: " + String(yArr));
            // tft.fillCircle(/* 320- */yArr, 170-xArr, 5, TFT_WHITE);
            // webSocketServer.broadcastTXT("/coord?X=" + String(xArr) + "&Y=" + String(yTouch));
            
        } else {
            if (x_y_homeTouch[TouchWorker::pasteIdx(-1)][TouchWorker::DataIdx::TOUCH_X] != -1){
                //release
                // Serial.println("Touch released");
                // webSocketServer.broadcastTXT("release");
                if(onReleaseCallback != NULL) {
                    (this->onReleaseCallback)(x_y_homeTouch, pasteIdx());
                }
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