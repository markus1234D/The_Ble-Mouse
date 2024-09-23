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
    void onSingleClick(functionPointer callback) { onSingleClickCallback = callback; }
    void onDoubleClick(functionPointer callback) { onDoubleClickCallback = callback; }
    void onLongPress(functionPointer callback) { onLongPressCallback = callback; }
    void onNoGesture(functionPointer callback) { noGestureCallback = callback; }
    void setMaxGestureTime(unsigned long time) { maxGestureTime = time; }
private:
    bool gesture_read = false;
    bool gesture_timeout = false;
    bool movemnet = true;
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
    functionPointer noGestureCallback = NULL;
    functionPointer gestureCallback = NULL;
    uint16_t gestureX = 0;
    uint16_t gestureY = 0;
    unsigned long maxGestureTime = 400;
    unsigned long maxLongPressTime = 250;
    bool clicked = false;

};

void CST816t_TouchWorker::init() {
    touchpad.begin(mode_touch);
}

void CST816t_TouchWorker::handleTouch() {

    if(touchpad.available()) {
        if(last_x == 0){
            // Serial.println("first touch");
            if(!clicked) {
                last_millis = millis();
            }
        }else{
            // not first touch/or move
            if(!gesture_timeout && last_millis + maxGestureTime < millis()){
                Serial.println("Gesture timeout");
                gesture_timeout = true;
            }
            if(!movemnet && (last_x != touchpad.x || last_y != touchpad.y)){
                // Serial.println("movement detected");
                movemnet = true;
            }
        }
        
        // uint16_t x = touchpad.x;
        // uint16_t y = touchpad.y;
        // if(!gesture_timeout){
        //     if(!gesture_read){
        //         uint8_t gesture_id = touchpad.gesture_id;

        //         switch (gesture_id) {
        //         case GESTURE_NONE:
        //             // Serial.println("NONE");
        //             break;
        //         case GESTURE_SWIPE_DOWN:
        //             // Serial.println("SWIPE DOWN");
        //             gesture_read = true;
        //             if(onSwipeDownCallback != NULL){
        //                 gestureCallback = onSwipeDownCallback;
        //                 gestureX = x;
        //                 gestureY = y;
        //             }
        //             break;
        //         case GESTURE_SWIPE_UP:
        //             // Serial.println("SWIPE UP");
        //             gesture_read = true;
        //             if(onSwipeUpCallback != NULL){
        //                 gestureCallback = onSwipeUpCallback;
        //                 gestureX = x;
        //                 gestureY = y;
        //             }
        //             break;
        //         case GESTURE_SWIPE_LEFT:             
        //             // Serial.println("SWIPE LEFT");
        //             gesture_read = true;
        //             if(onSwipeLeftCallback != NULL){
        //                 gestureCallback = onSwipeLeftCallback;
        //                 gestureX = x;
        //                 gestureY = y;
        //             }
        //             break;
        //         case GESTURE_SWIPE_RIGHT:
        //             // Serial.println("SWIPE RIGHT");
        //             gesture_read = true;
        //             if(onSwipeRightCallback != NULL){
        //                 gestureCallback = onSwipeRightCallback;
        //                 gestureX = x;
        //                 gestureY = y;
        //             }
        //             break;

        //         default:
        //             Serial.println("?");
        //             break;
        //         }
                
        //     }    // end if(!gesture_read)
        // } // end if(!gesture_timeout)   
        // else{
        //     // after timeoout
        //     if (!gesture_read){
        //         if(!movemnet){
        //             // check if longpress or double_click_and_hold
        //             if(clicked){
        //                 // double click and hold
        //             } else {
        //                 // long press and hold
        //                 if(onLongPressCallback != NULL){
        //                     gestureCallback = onLongPressCallback;
        //                     gestureX = x;
        //                     gestureY = y;
        //                     gesture_read = true;
        //                 }
        //             }
        //         } else {
        //             // movement and no gesture
        //             if(noGestureCallback != NULL){
        //                 noGestureCallback(x, y);
        //             }
        //         }
        //     } else {
        //         // gesture read but no release
        //         if(noGestureCallback != NULL){
        //             noGestureCallback(x, y);
        //         }
        //     }
        // }
        last_x = touchpad.x;
        last_y = touchpad.y;


        
    } else { // end if(touchpad.available())
        // no touch
        if(last_x != 0 && last_y != 0){
            // Serial.println("Touch released");
            if(last_millis + maxGestureTime > millis()){
                // before timeout
                if (!gesture_read){
                    // should be click or double click or long press
                    if(last_millis + maxLongPressTime < millis()){
                        // long press and release
                    } else {
                        // click or double click
                        if(clicked){
                            clicked = false;
                            gesture_read = true;
                            if(onDoubleClickCallback != NULL){
                                gestureCallback = onDoubleClickCallback;
                                gestureX = last_x;
                                gestureY = last_y;
                            }
                        }          
                    }

                }

                if(clicked){
                    clicked = false;
                    gesture_read = true;
                    if(onDoubleClickCallback != NULL){
                        gestureCallback = onDoubleClickCallback;
                        gestureX = last_x;
                        gestureY = last_y;
                    }
                } else {
                    clicked = true;
                    gesture_read = true;
                    if(onSingleClickCallback != NULL){
                        gestureCallback = onSingleClickCallback;
                        gestureX = last_x;
                        gestureY = last_y;
                    }
                }            
            } else {
                // after timeout
                if (gesture_read && !gesture_timeout){ 
                    if(gestureCallback != NULL){
                        gestureCallback(gestureX, gestureY);
                    }
                }
            }
            last_x = 0;
            last_y = 0;
            movemnet = false;
            gesture_timeout = false;
            gesture_read = false;
        }
    }
}