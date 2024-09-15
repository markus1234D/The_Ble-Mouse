#include <Arduino.h>
#include <Wire.h>
#include <cst816t.h>
#include "pin_config.h"

class CST816t_TouchWorker {
    typedef void (*functionPointer)(uint16_t x, uint16_t y);
public:
    void init();
    void handleTouch();
    cst816t touchpad = cst816t(Wire, PIN_TOUCH_RES, PIN_TOUCH_INT); //begin of Wire is in Libra
    void onPress(functionPointer callback) { onSingleClickCallback = callback; }
    void onSwipeLeft(functionPointer callback) { onSwipeLeftCallback = callback; }
    void onSwipeRight(functionPointer callback) { onSwipeRightCallback = callback; }
    void onSwipeUp(functionPointer callback) { onSwipeUpCallback = callback; }
    void onSwipeDown(functionPointer callback) { onSwipeDownCallback = callback; }
    void onDoubleClick(functionPointer callback) { onDoubleClickCallback = callback; }
    void onLongPress(functionPointer callback) { onLongPressCallback = callback; }

private:
    bool gesture_read = false;
    bool ignore_gesture = false;
    uint16_t last_x = 0;
    uint16_t last_y = 0;
    unsigned long last_millis = 0;
    functionPointer onSingleClickCallback = NULL;
    functionPointer onSwipeLeftCallback = NULL;
    functionPointer onSwipeRightCallback = NULL;
    functionPointer onSwipeUpCallback = NULL;
    functionPointer onSwipeDownCallback = NULL;
    functionPointer onDoubleClickCallback = NULL;
    functionPointer onLongPressCallback = NULL;

    functionPointer gestureCallback = NULL;
    uint16_t gestureX = 0;
    uint16_t gestureY = 0;
    unsigned long maxGestureTime = 500;

};

void CST816t_TouchWorker::init() {
    touchpad.begin(mode_touch);
}

void CST816t_TouchWorker::handleTouch() {

    if(touchpad.available()) {
        if(last_x == 0){
            Serial.println("first touch");
            last_millis = millis();
        }else{
            if(!ignore_gesture && last_millis + maxGestureTime < millis()){
                Serial.println("Gesture timeout");
                ignore_gesture = true;
            }
        }
        
        uint16_t x = touchpad.x;
        uint16_t y = touchpad.y;
        
        if(!gesture_read && !ignore_gesture){
            uint8_t gesture_id = touchpad.gesture_id;

            switch (gesture_id) {
            case GESTURE_NONE:
                Serial.println("NONE");
                break;
            case GESTURE_SWIPE_DOWN:
                // Serial.println("SWIPE DOWN");
                gesture_read = true;
                if(onSwipeDownCallback != NULL){
                    gestureCallback = onSwipeDownCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            case GESTURE_SWIPE_UP:
                // Serial.println("SWIPE UP");
                gesture_read = true;
                if(onSwipeUpCallback != NULL){
                    gestureCallback = onSwipeUpCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            case GESTURE_SWIPE_LEFT:             
                // Serial.println("SWIPE LEFT");
                gesture_read = true;
                if(onSwipeLeftCallback != NULL){
                    gestureCallback = onSwipeLeftCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            case GESTURE_SWIPE_RIGHT:
                // Serial.println("SWIPE RIGHT");
                gesture_read = true;
                if(onSwipeRightCallback != NULL){
                    gestureCallback = onSwipeRightCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            case GESTURE_SINGLE_CLICK:
                Serial.println("SINGLE CLICK");
                gesture_read = true;
                if(onSingleClickCallback != NULL){
                    gestureCallback = onSingleClickCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            case GESTURE_DOUBLE_CLICK:
                Serial.println("DOUBLE CLICK");
                gesture_read = true;
                if(onDoubleClickCallback != NULL){
                    gestureCallback = onDoubleClickCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            case GESTURE_LONG_PRESS:
                Serial.println("LONG PRESS");
                gesture_read = true;
                if(onLongPressCallback != NULL){
                    gestureCallback = onLongPressCallback;
                    gestureX = x;
                    gestureY = y;
                }
                break;
            default:
                Serial.println("?");
                break;
            }
        } else {
            Serial.println("X: " + String(x) + " Y: " + String(y));
        }
        last_x = touchpad.x;
        last_y = touchpad.y;
    } else {
        if(last_x != 0 && last_y != 0){
            Serial.println("Touch released");
            last_x = 0;
            last_y = 0;
            if (gesture_read){ 
                if(!ignore_gesture){
                    if(gestureCallback != NULL){
                        gestureCallback(gestureX, gestureY);
                    }
                }
                gesture_read = false;
            }
            ignore_gesture = false;
        }
    }
}