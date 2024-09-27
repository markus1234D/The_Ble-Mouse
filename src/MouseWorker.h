#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include <Arduino.h>
#include "BleCombo.h"


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
    void move(int x, int y);
    void press(int x, int y);
    void release();
    void setScrollSpeed(int speed) { scrollSpeed = speed; }
    // BleMouse Mouse;  


private:
    int scrollSpeed = 6;
    int xCenter = -1;
    int yCenter = -1;
    int last_x = -1;
    int last_y = -1;
    int xDiff = 0;
    int yDiff = 0;
    unsigned long ticks = 0;
private:
    Mode Mode = MOUSE_MODE;
};

void MouseWorker::init() {
    Mouse.begin();
}

void MouseWorker::nextMode() {
    if (Mode == MOUSE_MODE) {
        Mode = JOYSTICK_MODE;
        Serial.println("Joystick mode");
    } else if (Mode == JOYSTICK_MODE) {
        Mode = SCROLL_MODE;
        Serial.println("Scroll mode");
    } else if (Mode == SCROLL_MODE) {
        Mode = MOUSE_MODE;
        Serial.println("Mouse mode");
    }
}

void MouseWorker::move(int x, int y) {
    ticks++;
    if (ticks % scrollSpeed == 0) {
        if (Mode == MOUSE_MODE) {
            if (last_x != -1 && last_y != -1) {
                xDiff = x - last_x;
                yDiff = y - last_y;
                Mouse.move(2*xDiff, 2*yDiff, 0);
            }
            last_x = x;
            last_y = y;
        }
        else if(Mode == JOYSTICK_MODE) {
            if (xCenter != -1 && yCenter != -1) {
                xDiff = x - xCenter;
                yDiff = y - yCenter;
                if (abs(xDiff) < 5){ xDiff = 0; }
                if (abs(yDiff) < 5){ yDiff = 0; }
                Mouse.move(xDiff/5, yDiff/5, 0);
            }
        }
        else if(Mode == SCROLL_MODE) {
            if (last_x != -1 && last_y != -1) {
                xDiff = x - xCenter;
                yDiff = y - yCenter;
                if (abs(xDiff) < 5){ xDiff = 0; }
                if (abs(yDiff) < 5){ yDiff = 0; }
                Mouse.move(0, 0, yDiff/10, xDiff/5);
            }
        }
        Serial.println("xDiff: " + String(xDiff) + " yDiff: " + String(yDiff));
    }
}

void MouseWorker::press(int x, int y) {
    if (Mode == JOYSTICK_MODE) {
        xCenter = x;
        yCenter = y;
    }
    else if(Mode == MOUSE_MODE) {
        last_x = x;
        last_y = y;
    }
    else if(Mode == SCROLL_MODE) {
        xCenter = x;
        yCenter = y;
    }
}

void MouseWorker::release() {
    last_x = -1;
    last_y = -1;
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

#endif // MOUSEWORKER_H

