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
    // Serial.println("x: " + String(array[currentIdx][0]) + " y: " + String(array[currentIdx][1]) + " idx: " + String(currentIdx));
    if(array[currentIdx][0] == -1) {
        return;
    }
    Serial.println("previousIdx: " + String((currentIdx-1)<0? TOUCH_ARRAY_SIZE-currentIdx-1: currentIdx-1));
    Serial.println("currentIdx: " + String(currentIdx));
    int xDiff = array[currentIdx][0]-array[(currentIdx-1)%TOUCH_ARRAY_SIZE][0];
    int yDiff = array[currentIdx][1]-array[(currentIdx-1)%TOUCH_ARRAY_SIZE][1];
    bleMouse.move(xDiff, yDiff, 0);
    Serial.println("xDiff: " + String(xDiff) + " yDiff: " + String(yDiff));

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
    delay(50);
}