#include <Arduino.h>

const int buttonPins[] = {43, 44, 45, 46};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < numButtons; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

void loop() {
    for (int i = 0; i < numButtons; i++) {
        int state = digitalRead(buttonPins[i]);
        Serial.print("Button on GPIO ");
        Serial.print(buttonPins[i]);
        Serial.print(": ");
        Serial.println(state == LOW ? "PRESSED" : "RELEASED");
    }
    Serial.println("---");
    delay(500);
}