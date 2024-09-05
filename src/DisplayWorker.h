#include <Arduino.h>
#include <TFT_eSPI.h>

class DisplayWorker {
public:
    // // Constructor
    // DisplayWorker();

    // // Destructor
    // ~DisplayWorker();

    // Public member functions
    void init();
    void drawPoint(int x, int y);

private:
    // Private member variables

    // Private member functions
};

void DisplayWorker::init() {
    TFT_eSPI tft = TFT_eSPI();
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
}