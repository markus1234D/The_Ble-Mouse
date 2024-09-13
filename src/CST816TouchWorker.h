#include <Arduino.h>
#include <Wire.h>
#include <CST816_TouchLib.h>
#include "pin_config.h"

using namespace MDO;

class CST816TouchWorker {
public:
    void init();
    void handleTouch();
private:
    CST816Touch oTouch;
};

void CST816TouchWorker::init() {
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
	Wire.setClock(400000);	//For reliable communication, it is recommended to use a *maximum* communication rate of 400Kbps

	if (!oTouch.begin(Wire)) {
		Serial.println("Touch screen initialization failed..");
		while(true){
			delay(100);
		}
	}

	// if (!oTouch.setOperatingModeHardwareBased()) {
	// 	Serial.println("Set full hardware operational mode failed");
	// }
	if(!oTouch.setOperatingModeFast()) {
		Serial.println("Set fast operational mode failed");
	}

	if (oTouch.setNotifyOnMovement()) {
		oTouch.setMovementInterval(10);	//as example: limit to 10 per second (so 100 msec as interval)
	} else {
		//only available in 'fast'-mode
		Serial.println("Set notify on movement failed");
	}

	if (!oTouch.setNotificationsOnAllEvents()) {
		//only available in 'fast'-mode, provides events on touch and release of the screen
		Serial.println("Set notify on touch-and-release failed");
	}
			
	CST816Touch::device_type_t eDeviceType;
	if (oTouch.getDeviceType(eDeviceType)) {
		Serial.print("Device is of type: ");
		Serial.println(CST816Touch::deviceTypeToString(eDeviceType));
	}
	Serial.println("Touch screen initialization done");
}

int xprev = -1;
int yprev = -1;
int x = -1;
int y = -1;
void CST816TouchWorker::handleTouch() {
    oTouch.control();
	// oTouch.printBuff();

	// /* if (oTouch.hadTouch() || oTouch.hadGesture()) */ {
	// 	if (xprev == -1 && yprev == -1) {
	// 		Serial.println("first touch");
	// 	}
	// 	CST816Touch::gesture_t eGesture = CST816Touch::GESTURE_NONE;
	// 	oTouch.getLastGesture(eGesture, x, y);
	// 	Serial.print("Gesture (");
	// 	Serial.print(CST816Touch::gestureIdToString(eGesture));
	// 	Serial.print(") received at: (");
	// 	Serial.print(x);
	// 	Serial.print(",");
	// 	Serial.print(y);
	// 	Serial.println(")");
	// 	xprev = x;
	// 	yprev = y;
	// }
	// /* else */{
	// 	if (xprev != -1 && yprev != -1 && x == 0 && y == 0) {
	// 		Serial.print("release at: (");
	// 		Serial.print(xprev);
	// 		Serial.print(",");
	// 		Serial.print(yprev);
	// 		Serial.println(")");
	// 		xprev = -1;
	// 		yprev = -1;
	// 	}		
	// }
	
	  
		// Serial.println("(" + String(xprev) + "," + String(yprev) + ")");

		
	delay(100);
}