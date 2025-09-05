#include <Arduino.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();
}

void loop() {
    if (bleKeyboard.isConnected()) {
        Serial.println("BLE Keyboard connected");
        // Serial.println("Sending 'hello' over BLE Keyboard");
        // bleKeyboard.print("hello");
        Serial.println("Sending 'INSERT' key over BLE Keyboard");
        bleKeyboard.print("hello");
        delay(10000); // Wait 10 seconds before sending again
    } else {
        Serial.println("BLE Keyboard not connected");
    }
    delay(1000);
}