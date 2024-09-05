class functionObject {

public:
    functionObject(String msgName, void (*callback)(void), int xMin, int xMax, int yMin, int yMax);
    void runFunction();
    String getName();
    int getXMin();
    int getXMax();
    int getYMin();
    int getYMax();
private:
    String name;
    void (*callback)(void);
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

class GuiWorker {
    #include <Arduino.h>
    #include <vector>
    
public:
    // Constructor
    // GuiWorker();
    // Destructor
    // ~GuiWorker();

    typedef void (*functionPointer)(void);   // Define a function pointer type

    // std::vector<String> serverCallbacksNames;
    // std::vector<functionPointer> serverCallbacks;
    // std::vector<int> xMin_xMax_yMin_yMax;
    std::vector<functionObject> functionObjects;

    // Public member functions
    void addCallback(String msgName, functionPointer callback, int xMin, int xMax, int yMin, int yMax);
    void runFunctions();
    void init();

private:
    // Private member variables
    const char* html;

    // Private member functions
};


void GuiWorker::init() {
    
}