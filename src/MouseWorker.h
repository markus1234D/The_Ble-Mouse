#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include <Arduino.h>
#include "BleCombo.h"

#define DEBUG

// needed for noGestureCallback (x, y) in CST816t_TouchWorker.h
class MouseWorker {
public:
    void init(); // Method to initialize the mouse worker
    enum Mode{
        MOUSE_MODE,
        JOYSTICK_MODE,
        SCROLL_MODE
    };
public:
    void nextMode();
    void setMode(Mode mode) { this->mode = mode; }
    Mode getMode() { return this->mode; }
    void move(int x, int y);
    void press(int x, int y);
    void release();
    void setScrollSpeed(int speed) { scrollSpeed = speed; }
    void setMouseSpeed(int speed) { mouseSpeed = speed; }
    void setJoystickSpeed(int speed) { joystickSpeed = speed; }

private:
    int scrollSpeed = 6;
    int mouseSpeed = 1;
    int joystickSpeed = 1;
    int xCenter = -1;
    int yCenter = -1;
    int last_x = -1;
    int last_y = -1;
    int xDiff = 0;
    int yDiff = 0;
    unsigned long ticks = 0;
    Mode mode = MOUSE_MODE;
private:
    void debugPrint(String str) {
        #ifdef DEBUG
            Serial.println(str);
        #endif
    }
};

void MouseWorker::init() {
    Mouse.begin();
}

void MouseWorker::nextMode() {
    if (mode == MOUSE_MODE) {
        mode = JOYSTICK_MODE;
        Serial.println("Joystick mode");
    } else if (mode == JOYSTICK_MODE) {
        mode = SCROLL_MODE;
        Serial.println("Scroll mode");
    } else if (mode == SCROLL_MODE) {
        mode = MOUSE_MODE;
        Serial.println("Mouse mode");
    }
}

void MouseWorker::move(int x, int y) {
    ticks++;
    if (ticks % mouseSpeed == 0) {
        if (mode == MOUSE_MODE) {
            if (last_x != -1 && last_y != -1) {
                xDiff = x - last_x;
                yDiff = y - last_y;
                Mouse.move(xDiff, yDiff, 0);
            }
            last_x = x;
            last_y = y;
        }
    }
    if(ticks % joystickSpeed == 0) {
        if(mode == JOYSTICK_MODE) {
            if (xCenter != -1 && yCenter != -1) {
                xDiff = x - xCenter;
                yDiff = y - yCenter;
                if (abs(xDiff) < 5){ xDiff = 0; }
                if (abs(yDiff) < 5){ yDiff = 0; }
                Mouse.move(xDiff/5, yDiff/5, 0);
            }
        }
    }
    if (ticks % scrollSpeed == 0) {
        if(mode == SCROLL_MODE) {
            if (xCenter != -1 && yCenter != -1) {
                xDiff = x - xCenter;
                yDiff = y - yCenter;
                if (abs(xDiff) < 5){ xDiff = 0; }
                if (abs(yDiff) < 5){ yDiff = 0; }
                Mouse.move(0, 0, yDiff/10, xDiff/10);
                debugPrint("xDiff: " + String(xDiff/1) + " yDiff: " + String(yDiff/1));
            }
        }
        // Serial.println("xDiff: " + String(xDiff) + " yDiff: " + String(yDiff));
    }
}

void MouseWorker::press(int x, int y) {
    if (mode == JOYSTICK_MODE) {
        xCenter = x;
        yCenter = y;
    }
    else if(mode == MOUSE_MODE) {
        last_x = x;
        last_y = y;
    }
    else if(mode == SCROLL_MODE) {
        xCenter = x;
        yCenter = y;
        debugPrint("xCenter: " + String(xCenter) + " yCenter: " + String(yCenter));
    }
}

void MouseWorker::release() {
    last_x = -1;
    last_y = -1;
    xCenter = -1;
    yCenter = -1;
}

#endif // MOUSEWORKER_H

