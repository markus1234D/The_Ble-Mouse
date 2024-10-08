/*
 demo of P168H002-CTP display.
 */
#include <Arduino.h>
// #include <SPI.h>
// #include <Adafruit_GFX.h>     // Core graphics library
// #include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789

#include <Wire.h>
#include "cst816t.h"          // capacitive touch

// display
#define TFT_X 240
#define TFT_Y 280

#define TFT_CS PB1
#define TFT_RST PA4
#define TFT_DC PB0
#define TFT_MOSI PB15
#define TFT_SCLK PB13
#define TFT_LED PA8

// touch
#define TP_SDA PB11
#define TP_SCL PB10
#define TP_RST PA15
#define TP_IRQ PB3

cst816t touchpad(Wire, 21, 16);

// SPIClass SPI_1(PB15, PB14, PB13);
// Adafruit_ST7789 tft = Adafruit_ST7789(&SPI_1, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  Serial.println("boot");

  // analogWriteResolution(8);
  // analogWrite(TFT_LED, 127);  // display backlight at 50%

  // tft.init(240, 280);  // Init ST7789 280x240
  // tft.fillScreen(ST77XX_BLACK);
  // tft.setTextColor(ST77XX_WHITE);
  // tft.setTextSize(0);
  // tft.setRotation(2);

  touchpad.begin(mode_touch);
  // tft.setCursor(TFT_X / 2, TFT_Y / 2);
  // tft.println(touchpad.version());
}

void loop() {
  if (touchpad.available()) {
    // tft.setCursor(touchpad.x, touchpad.y);
    // tft.fillScreen(ST77XX_BLACK);
    switch (touchpad.gesture_id) {
      case GESTURE_NONE:
        Serial.print("NONE");
        break;
      case GESTURE_SWIPE_DOWN:
        Serial.print("SWIPE DOWN");
        break;
      case GESTURE_SWIPE_UP:
        Serial.print("SWIPE UP");
        break;
      case GESTURE_SWIPE_LEFT:
        Serial.print("SWIPE LEFT");
        break;
      case GESTURE_SWIPE_RIGHT:
        Serial.print("SWIPE RIGHT");
        break;
      case GESTURE_SINGLE_CLICK:
        Serial.print("SINGLE CLICK");
        break;
      case GESTURE_DOUBLE_CLICK:
        Serial.print("DOUBLE CLICK");
        break;
      case GESTURE_LONG_PRESS:
        Serial.print("LONG PRESS");
        break;
      default:
        Serial.print("?");
        break;
    }
    
  Serial.println(" X: " + String(touchpad.x) + " Y: " + String(touchpad.y));
  }
  delay(100);
}
