#include <Arduino.h>
#include <Wire.h>
#include <cst816t.h>
#include "pin_config.h"
#include <map>
#include <blecombo.h>

#define DEBUG

class CST816t_TouchWorker {
public:
    enum Rotation {
        USB_UP = 0,
        USB_DOWN = 1,
        USB_LEFT = 2,
        USB_RIGHT = 3
    };
    Rotation rotation = USB_DOWN;
public:
    void init();
    void handleTouch();
    cst816t touchpad = cst816t(Wire, PIN_TOUCH_RES, PIN_TOUCH_INT); //begin of Wire is in Libra

    void setMaxGestureTime(unsigned long time) { maxGestureTime = time; }
    void setRotation(Rotation rotation) { this->rotation = rotation; }
    uint16_t getRawX() { return rawX; }
    uint16_t getRawY() { return rawY; }

    bool setActionFunction(String action, std::function<void(int, int)> callback);

private:
    void debugPrint(String str);
    bool swipe_read = false;
    bool gesture_timeout = false;
    bool movemnet = true;
    bool clicked = false;
    bool start = false;
    bool longPress = false;
    bool doubleLongPress = false;
    
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t last_x = 0;
    uint16_t last_y = 0;
    u_int16_t rawX = 0;
    u_int16_t rawY = 0;
    unsigned long last_millis = 0;
    uint16_t xMin = 0;
    uint16_t xMax = 170;
    uint16_t yMin = 0;
    uint16_t yMax = 320;
    uint16_t real_xMin = 7;
    uint16_t real_xMax = 154;
    uint16_t real_yMin = 7;
    uint16_t real_yMax = 312;

    std::map<String, std::function<void(int, int)>> callbacks;


    std::function<void(int, int)> gestureCallbackBuffer = NULL;
    uint16_t gestureX = 0;
    uint16_t gestureY = 0;
    unsigned long maxGestureTime = 1000;

private:
    void checkGesture();
    void setXY(uint16_t x, uint16_t y);
};

void CST816t_TouchWorker::debugPrint(String str) {
#ifdef DEBUG
    Serial.print("[TouchWorker]: ");
    Serial.println(str);
#endif
}

bool CST816t_TouchWorker::setActionFunction(String action, std::function<void(int, int)> callback) {
    if (callbacks.find(action) != callbacks.end()) {
        callbacks[action] = callback;
        return true;
    }
    return false;
}

void CST816t_TouchWorker::setXY(uint16_t x, uint16_t y) {
    rawX = x;
    rawY = y;
    switch (rotation) {
    case USB_UP:
        this->x = xMax - x;
        this->y = yMax - y;
        break;
    case USB_DOWN:
        this->x = x;
        this->y = y;
        break;
    case USB_LEFT:
        this->x = yMax - y;
        this->y = x;
        break;
    case USB_RIGHT:
        this->x = y;
        this->y = xMax - x;
        break;
    default:
        Serial.println("Unknown rotation");
        break;
    }
}

void CST816t_TouchWorker::init() {
    touchpad.begin(mode_touch);

    this->callbacks["swipeLeft"] = NULL;
    this->callbacks["swipeRight"] = NULL;
    this->callbacks["swipeUp"] = NULL;
    this->callbacks["swipeDown"] = NULL;
    this->callbacks["singleClick"] = NULL;
    this->callbacks["singleClickRelease"] = NULL;
    this->callbacks["doubleClick"] = NULL;
    this->callbacks["doubleClickRelease"] = NULL;
    this->callbacks["longPress"] = NULL;
    this->callbacks["longPressRelease"] = NULL;
    this->callbacks["noGesture"] = NULL;
}

void CST816t_TouchWorker::handleTouch() {

    if (start && millis() - last_millis > maxGestureTime) {
        debugPrint("Gesture timeout");

        gesture_timeout = true;
        start = false;
        clicked = false;
    }

    if(touchpad.available()){
        // Serial.println("touch detected");
        setXY(touchpad.x, touchpad.y);
        if (last_x == 0) {
            if (clicked) {
                if (callbacks["doubleClick"] != NULL){
                    callbacks["doubleClick"](this->x, this->y);
                }
                debugPrint("Double click detected");
            } else {
                // Serial.println("first touch");
                last_millis = millis();
                start = true;
                gesture_timeout = false;
                swipe_read = false;
                movemnet = false;
                if (callbacks["singleClick"] != NULL){
                    callbacks["singleClick"](this->x, this->y);
                }
                debugPrint("Single click detected");
            }
        } else {
            // not first touch/or move
            if (last_x != this->x || last_y != this->y) {
                movemnet = true;
            }
        }

        if (gesture_timeout) {
            // no gesture but movement detected
            debugPrint("no X: " + String(this->x) + ", Y: " + String(this->y));
            gestureCallbackBuffer = NULL;
            if(callbacks["noGesture"] != NULL){
                callbacks["noGesture"](this->x, this->y);
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
                        if(callbacks["longPress"] != NULL){
                            callbacks["longPress"](this->x, this->y);
                        }
                        debugPrint("Long press detected");
                        longPress = true;
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

        last_x = this->x;
        last_y = this->y;
    } else {
        // Serial.println("no touch");
        if (last_x != 0) {
            // release

            if (gesture_timeout){
                // too late for gesture
                if (longPress) {
                    // drag and drop release
                    if(callbacks["longPressRelease"] != NULL){
                        callbacks["longPressRelease"](this->x, this->y);
                    }
                    debugPrint("Long press release detected");
                    longPress = false;
                }
            } else {              
                if (swipe_read) {
                    // swipe detected
                    if (gestureCallbackBuffer != NULL){
                        gestureCallbackBuffer(gestureX, gestureY);
                        gestureCallbackBuffer = NULL;
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
                            if (callbacks["doubleClickRelease"] != NULL){
                                gestureCallbackBuffer = callbacks["doubleClickRelease"];
                                gestureX = last_x;
                                gestureY = last_y;
                                // clicked = false;
                                start = true;
                            }
                            debugPrint("Double click release detected");
                        } else {
                            // click release
                            clicked = true;
                            if (callbacks["singleClickRelease"] != NULL){
                                gestureCallbackBuffer = callbacks["singleClickRelease"];
                                gestureX = last_x;
                                gestureY = last_y;
                                start = true;
                            }
                            debugPrint("Single click release detected");
                        }
                        if (gestureCallbackBuffer != NULL){
                            gestureCallbackBuffer(gestureX, gestureY);
                            gestureCallbackBuffer = NULL;
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

        switch (rotation)
        {
        case USB_DOWN :
            if(callbacks["swipeUp"] != NULL){
                gestureCallbackBuffer = callbacks["swipeUp"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Up detected");
            break;
        case USB_UP :
            if(callbacks["swipeDown"] != NULL){
                gestureCallbackBuffer = callbacks["swipeDown"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Down detected");
            break;
        case USB_LEFT:
            if(callbacks["swipeRight"] != NULL){
                gestureCallbackBuffer = callbacks["swipeRight"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Right detected");
            break;
        case USB_RIGHT:
            if(callbacks["swipeLeft"] != NULL){
                gestureCallbackBuffer = callbacks["swipeLeft"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Left detected");
            break;
        default:
            break;
        }
        break;
    case GESTURE_SWIPE_UP:
        // Serial.println("SWIPE UP");
        swipe_read = true;
        switch (rotation)
        {
        case USB_DOWN :
            if(callbacks["swipeUp"] != NULL){
                gestureCallbackBuffer = callbacks["swipeUp"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Up detected");
            break;
        case USB_UP :
            if(callbacks["swipeDown"] != NULL){
                gestureCallbackBuffer = callbacks["swipeDown"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Down detected");
            break;
        case USB_LEFT:
            if(callbacks["swipeLeft"] != NULL){
                gestureCallbackBuffer = callbacks["swipeLeft"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Left detected");
            break;
        case USB_RIGHT:
            if(callbacks["swipeRight"] != NULL){
                gestureCallbackBuffer = callbacks["swipeRight"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Right detected");
            break;
        default:
            break;
        }
        break;
    case GESTURE_SWIPE_LEFT:             
        // Serial.println("SWIPE LEFT");
        swipe_read = true;
        switch (rotation)
        {
        case USB_DOWN :
            if(callbacks["swipeLeft"] != NULL){
                gestureCallbackBuffer = callbacks["swipeLeft"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Left detected");
            break;
        case USB_UP :
            if(callbacks["swipeRight"] != NULL){
                gestureCallbackBuffer = callbacks["swipeRight"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Right detected");
            break;
        case USB_LEFT:
            if(callbacks["swipeUp"] != NULL){
                gestureCallbackBuffer = callbacks["swipeUp"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Up detected");
            break;
        case USB_RIGHT:
            if(callbacks["swipeDown"] != NULL){
                gestureCallbackBuffer = callbacks["swipeDown"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Down detected");
            break;
        default:
            break;
        }
        break;
    case GESTURE_SWIPE_RIGHT:
        // Serial.println("SWIPE RIGHT");
        swipe_read = true;
        switch (rotation)
        {
        case USB_DOWN :
            if(callbacks["swipeRight"] != NULL){
                gestureCallbackBuffer = callbacks["swipeRight"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Right detected");
            break;
        case USB_UP :
            if(callbacks["swipeLeft"] != NULL){
                gestureCallbackBuffer = callbacks["swipeLeft"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Left detected");
            break;
        case USB_LEFT:
            if(callbacks["swipeDown"] != NULL){
                gestureCallbackBuffer = callbacks["swipeDown"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Down detected");
            break;
        case USB_RIGHT:
            if(callbacks["swipeUp"] != NULL){
                gestureCallbackBuffer = callbacks["swipeUp"];
                gestureX = this->x;
                gestureY = this->y;
            }
            debugPrint("Swipe Up detected");
            break;
        default:
            break;
        }
        break;
    case GESTURE_LONG_PRESS:
        // Serial.println("LONG PRESS");
        if(callbacks["longPress"] != NULL){
            gestureCallbackBuffer = callbacks["longPress"];
            gestureX = this->x;
            gestureY = this->y;
        }
        break;
    default:
        debugPrint("?");
        break;
    }
}