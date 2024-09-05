#include <Arduino.h>
#include "CommunicationWorker.h"
#include "DisplayWorker.h"
#include "GuiWorker.h"
#include "TouchWorker.h"

// #include "TFT_eSPI.h"
#include "pin_config.h"


CommunicationWorker communicationWorker;
DisplayWorker displayWorker;
GuiWorker guiWorker;
TouchWorker touchWorker;

void setup() {
    Serial.begin(115200);
    delay(4000);
    Serial.println("Hello World");
    
    communicationWorker.init();
    displayWorker.init();
    guiWorker.init();
    touchWorker.init();

}

void loop() {
    communicationWorker.handleCommunication();
    delay(1000);
}