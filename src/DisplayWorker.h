#include <Arduino.h>

class DisplayWorker {
public:
    // Constructor
    DisplayWorker();

    // Destructor
    ~DisplayWorker();

    // Public member functions
    void init();
    void drawPoint(int x, int y);

private:
    // Private member variables

    // Private member functions
};