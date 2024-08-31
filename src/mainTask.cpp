#include <Arduino.h>
#include "CommunicationWorker.h"
#include "DisplayWorker.h"
#include "GuiWorker.h"
#include "TouchWorker.h"

// #include "TFT_eSPI.h"
#include "pin_config.h"


CommunicationWorker communicationWorker();
DisplayWorker displayWorker();
GuiWorker guiWorker();
TouchWorker touchWorker();

void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("Hello World");
}

void loop() {
    delay(1000);
}