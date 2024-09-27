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

    void onSwipeLeft(functionPointer callback) { onSwipeLeftCallback = callback; }
    void onSwipeRight(functionPointer callback) { onSwipeRightCallback = callback; }
    void onSwipeUp(functionPointer callback) { onSwipeUpCallback = callback; }
    void onSwipeDown(functionPointer callback) { onSwipeDownCallback = callback; }
    void onSingleClick(functionPointer callback) { onSingleClickCallback = callback; }
    void onSingleClickRelease(functionPointer callback) { onSingleClickReleaseCallback = callback; }
    void onDoubleClick(functionPointer callback) { onDoubleClickCallback = callback; }
    void onDoubleClickRelease(functionPointer callback) { onDoubleClickReleaseCallback = callback; }
    void onLongPress(functionPointer callback) { onLongPressCallback = callback; }
    void onLongPressRelease(functionPointer callback) { onLongPressReleaseCallback = callback; }
    void onNoGesture(functionPointer callback) { noGestureCallback = callback; }
    void setMaxGestureTime(unsigned long time) { maxGestureTime = time; }

private:
    bool swipe_read = false;
    bool gesture_timeout = false;
    bool movemnet = true;
    bool clicked = false;
    bool start = false;
    

    uint16_t last_x = 0;
    uint16_t last_y = 0;
    unsigned long last_millis = 0;

    functionPointer onPressCallback = NULL;
    functionPointer onReleaseCallback = NULL;
    functionPointer onSingleClickCallback = NULL;
    functionPointer onSingleClickReleaseCallback = NULL;
    functionPointer onDoubleClickCallback = NULL;
    functionPointer onDoubleClickReleaseCallback = NULL;
    functionPointer onLongPressCallback = NULL;
    functionPointer onLongPressReleaseCallback = NULL;
    functionPointer onSwipeLeftCallback = NULL;
    functionPointer onSwipeRightCallback = NULL;
    functionPointer onSwipeUpCallback = NULL;
    functionPointer onSwipeDownCallback = NULL;
    functionPointer noGestureCallback = NULL;
    functionPointer gestureCallback = NULL;
    uint16_t gestureX = 0;
    uint16_t gestureY = 0;
    unsigned long maxGestureTime = 1000;
    // unsigned long maxLongPressTime = 250;

private:
    void checkGesture();

};

void CST816t_TouchWorker::init() {
    touchpad.begin(mode_touch);

}

void CST816t_TouchWorker::handleTouch() {

    if (start && millis() - last_millis > maxGestureTime) {
        Serial.println("Gesture timeout");
        // if(clicked){
        //     if(onSingleClickCallback != NULL){
        //         onSingleClickCallback(last_x, last_y);
        //     }
        // }
        gesture_timeout = true;
        start = false;
        clicked = false;
        if(gestureCallback != NULL){
            gestureCallback(gestureX, gestureY);
            gestureCallback = NULL;
        }
    }

    // if (gestureCallback != NULL){
    //     gestureCallback(gestureX, gestureY);
    //     gestureCallback = NULL;
    // }

    if(touchpad.available()){
        // Serial.println("touch detected");
        if (last_x == 0) {
            if (clicked) {
                if (onDoubleClickCallback != NULL){
                    onDoubleClickCallback(touchpad.x, touchpad.y);
                }
            } else {
                // Serial.println("first touch");
                last_millis = millis();
                start = true;
                gesture_timeout = false;
                swipe_read = false;
                movemnet = false;
                if (onSingleClickCallback != NULL){
                    onSingleClickCallback(touchpad.x, touchpad.y);
                }
            }
        } else {
            // not first touch/or move
            if (last_x != touchpad.x || last_y != touchpad.y) {
                movemnet = true;
            }
        }

        if (gesture_timeout) {
            // no gesture
            gestureCallback = NULL;
            if(noGestureCallback != NULL){
                noGestureCallback(touchpad.x, touchpad.y);
            }
            if (swipe_read) {
                // too late
            } else {
                if (movemnet) {
                    // movement detected
                    // nothing
                } else {
                    // no movement
                    if (clicked) {
                        // double click and hold
                    } else {
                        // click and hold
                    }
                }
            }
        } else {
            // not timeout
            if (swipe_read) {
                // wait until timeout
            } else {
                checkGesture();
                if (movemnet) {
                    // movement detected
                    // nothing
                } else {
                    // no movement
                    if (clicked) {
                        // could become double click and hold
                    } else {
                        // could become click and hold
                    }
                }
            }
        }

        last_x = touchpad.x;
        last_y = touchpad.y;
    } else {
        // Serial.println("no touch");
        if (last_x != 0) {
            // release

            if (gesture_timeout){
                // too late for gesture
            } else {              

                if (swipe_read) {
                    // swipe detected
                    if (gestureCallback != NULL){
                        gestureCallback(gestureX, gestureY);
                        gestureCallback = NULL;
                    }
                    start = false;
                } else {
                    // no gesture
                    if (movemnet) {
                        // movement detected
                    } else {
                        // no movement
                        if (clicked) {
                            // double click release
                            if (onDoubleClickReleaseCallback != NULL){
                                gestureCallback = onDoubleClickReleaseCallback;
                                gestureX = last_x;
                                gestureY = last_y;
                                // clicked = false;
                                start = true;
                            }
                        } else {
                            // click release
                            clicked = true;
                            if (onSingleClickReleaseCallback != NULL){
                                gestureCallback = onSingleClickReleaseCallback;
                                gestureX = last_x;
                                gestureY = last_y;
                                start = true;
                            }
                        }
                        if (gestureCallback != NULL){
                            gestureCallback(gestureX, gestureY);
                            gestureCallback = NULL;
                        }
                    }
                }
            }
        }
        // when there is no touch
        last_x = 0;
        last_y = 0;
    }
}

void CST816t_TouchWorker::checkGesture(){
    uint8_t gesture_id = touchpad.gesture_id;

    switch (gesture_id) {
    case GESTURE_NONE:
        // Serial.println("NONE");
        break;
    case GESTURE_SWIPE_DOWN:
        // Serial.println("SWIPE DOWN");
        swipe_read = true;
        if(onSwipeUpCallback != NULL){
            gestureCallback = onSwipeUpCallback;
            gestureX = touchpad.x;
            gestureY = touchpad.y;
        }
        break;
    case GESTURE_SWIPE_UP:
        // Serial.println("SWIPE UP");
        swipe_read = true;
        if(onSwipeDownCallback != NULL){
            gestureCallback = onSwipeDownCallback;
            gestureX = touchpad.x;
            gestureY = touchpad.y;
        }
        break;
    case GESTURE_SWIPE_LEFT:             
        // Serial.println("SWIPE LEFT");
        swipe_read = true;
        if(onSwipeLeftCallback != NULL){
            gestureCallback = onSwipeLeftCallback;
            gestureX = touchpad.x;
            gestureY = touchpad.y;
        }
        break;
    case GESTURE_SWIPE_RIGHT:
        // Serial.println("SWIPE RIGHT");
        swipe_read = true;
        if(onSwipeRightCallback != NULL){
            gestureCallback = onSwipeRightCallback;
            gestureX = touchpad.x;
            gestureY = touchpad.y;
        }
        break;
    case GESTURE_LONG_PRESS:
        // Serial.println("LONG PRESS");
        if(onLongPressCallback != NULL){
            gestureCallback = onLongPressCallback;
            gestureX = touchpad.x;
            gestureY = touchpad.y;
        }
        break;
    default:
        Serial.println("?");
        break;
    }
}