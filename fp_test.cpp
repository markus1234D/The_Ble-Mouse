#include <Arduino.h>

void (*functionPointer)(int (&array)[3][3], int currentIdx);

void testFunc(int (&array)[3][3], int currentIdx) {
    Serial.println("Test");
}

void setFunc(void (*func)(int (&array)[3][3], int currentIdx)) {
    functionPointer = func;
}

void setup() {
    Serial.begin(115200);
    delay(4000);
    Serial.println("Hello World");
    setFunc(testFunc);

    int array[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    functionPointer(array, 1);
}

void loop() {
}