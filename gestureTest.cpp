#include <iostream>
#include <vector>

struct Point {
    int x;
    int y;
};

enum Gesture {
    NONE,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN,
    TAP,
    UNKNOWN
};

Gesture detectGesture(const std::vector<Point>& buffer, int currentIdx) {
    const int arraySize = buffer.size();
    const Point& currentPoint = buffer[currentIdx];
    const Point& prevPoint = buffer[(currentIdx - 1 + arraySize) % arraySize];

    // Prüfen, ob vorheriger Punkt gültig ist
    if (prevPoint.x == -1 && prevPoint.y == -1) {
        // Dies könnte ein einfacher Tap sein
        return TAP;
    }

    // Berechnung der Differenz zwischen den Punkten
    int deltaX = currentPoint.x - prevPoint.x;
    int deltaY = currentPoint.y - prevPoint.y;

    // Schwellenwerte für die Erkennung
    const int threshold = 5; // Anpassbar je nach Empfindlichkeit

    if (abs(deltaX) > abs(deltaY)) {
        // Horizontaler Swipe
        if (deltaX > threshold) {
            return SWIPE_RIGHT;
        } else if (deltaX < -threshold) {
            return SWIPE_LEFT;
        }
    } else {
        // Vertikaler Swipe
        if (deltaY > threshold) {
            return SWIPE_DOWN;
        } else if (deltaY < -threshold) {
            return SWIPE_UP;
        }
    }

    return NONE;
}

int main() {
    // Beispiel-Ringpuffer mit einigen x,y-Koordinaten
    std::vector<Point> ringBuffer = {
        {100, 100}, {120, 100}, {140, 100}, {160, 100}, {-1, -1}
    };

    int currentIdx = 3; // Der Finger ist aktuell auf Position 160, 100

    Gesture gesture = detectGesture(ringBuffer, currentIdx);

    switch (gesture) {
        case SWIPE_LEFT:
            std::cout << "Gesture: Swipe Left\n";
            break;
        case SWIPE_RIGHT:
            std::cout << "Gesture: Swipe Right\n";
            break;
        case SWIPE_UP:
            std::cout << "Gesture: Swipe Up\n";
            break;
        case SWIPE_DOWN:
            std::cout << "Gesture: Swipe Down\n";
            break;
        case TAP:
            std::cout << "Gesture: Tap\n";
            break;
        default:
            std::cout << "Gesture: None\n";
            break;
    }

    return 0;
}
