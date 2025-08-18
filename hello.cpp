#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    Serial.println("setup Hello, World!");
}

void loop() {
        Serial.println("Hello, World!");
        delay(1000);
}