#include <Arduino.h>
#include "CommunicationWorker.h"
#include "DisplayWorker.h"
#include "GuiWorker.h"
#include "TouchWorker.h"

// #include "TFT_eSPI.h"
#include "pin_config.h"
#include <BleMouse.h>

BleMouse bleMouse;

CommunicationWorker communicationWorker;
DisplayWorker displayWorker;
GuiWorker guiWorker;
TouchWorker touchWorker;



void mouseMoveFunc(int (&array)[TOUCH_ARRAY_SIZE][3], int currentIdx) {

    if(array[currentIdx][0] != -1) {
        int prevIdx = (currentIdx-1)%TOUCH_ARRAY_SIZE;
        if (prevIdx < 0) {
            prevIdx = TOUCH_ARRAY_SIZE + prevIdx;
            // why is this necessary?   -1 % 10 = -1
        }
        int x = array[currentIdx][0];
        int y = array[currentIdx][1];
        int xPrev = array[prevIdx][0];
        int yPrev = array[prevIdx][1];
        // Serial.println("prevIdx: " + String(prevIdx) + " xPrev: " + String(xPrev) + " yPrev: " + String(yPrev));
        // Serial.println("currIdx: " + String(currentIdx) + " x: " + String(array[currentIdx][0]) + " y: " + String(array[currentIdx][1]));
        int xDiff = array[currentIdx][0]-array[prevIdx][0];
        int yDiff = array[currentIdx][1]-array[prevIdx][1];
        bleMouse.move(xDiff, yDiff, 0);
        // Serial.println("xDiff: " + String(xDiff) + " yDiff: " + String(yDiff));
    }
}
    

void setup() {
    bleMouse.begin();
    Serial.begin(115200);
    delay(4000);
    Serial.println("Hello World");
    
    // communicationWorker.init();
    displayWorker.init();
    guiWorker.init();
    touchWorker.init();

    touchWorker.onMove(&mouseMoveFunc);
}

void loop() {
    // communicationWorker.handleCommunication();
    touchWorker.handleTouch();
    // delay(500);
}