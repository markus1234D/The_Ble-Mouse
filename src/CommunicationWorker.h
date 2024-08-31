#include <Arduino.h>
#include "WiFi.h"
#include <WebServer.h>
#include <WebSocketsServer.h>


class CommunicationWorker {
public:
    // Constructor
    CommunicationWorker();

    // Destructor
    ~CommunicationWorker();

    // Public member functions
    void initWiFi();
    // WebServer server(80);
    WebSocketsServer webSocketServer = WebSocketsServer(81);
    unsigned long millisLast = 0;
private:
    // Private member variables

    // Private member functions
};