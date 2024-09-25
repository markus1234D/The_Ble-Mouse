#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include <Arduino.h>
#include <BleMouse.h>

// needed for noGestureCallback (x, y) in CST816t_TouchWorker.h
class MouseWorker {
public:
    void init(); // Method to initialize the mouse worker
    enum Mode{
        MOUSE_MODE,
        JOYSTICK_MODE
    };
    void nextMode();
    void move(int x, int y){
        if (Mode == MOUSE_MODE) {
            bleMouse.move(x, y, 0);
        }

        else if (Mode == JOYSTICK_MODE)
        {
            /* code */
        }
        
    }

private:
    BleMouse bleMouse;  
    Mode Mode = MOUSE_MODE;

};

void MouseWorker::init() {
    bleMouse.begin();
}

void MouseWorker::nextMode() {
    if (Mode == MOUSE_MODE) {
        Mode = JOYSTICK_MODE;
    } else {
        Mode = MOUSE_MODE;
    }
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

