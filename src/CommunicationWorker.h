#include <Arduino.h>
#include "WiFi.h"
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <vector>


class CommunicationWorker {
public:
    // Constructor
    CommunicationWorker();

    // Destructor
    ~CommunicationWorker();
    
    // Public member variables
    const char* ssid = "FRITZ!Mox";
    const char* password = "BugolEiz42";
    // const char *ssid = "SM-Fritz";
    // const char *password = "47434951325606561069";
    // const char* ssid = "ZenFone7 Pro_6535";
    // const char* password = "e24500606";


    // Public member functions
    void initWiFi();
    IPAddress getIP();
    void sendMessageToWeb(String message);
    void fetchMessageFromWeb();
    void handleRoot();  //TODO: { guiWorker.getHtml(); }
    int extractArgs(const String& input, std::vector<String>& argNames, std::vector<String>& args);
    String extractCommand(const String& input);
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    void handleCommunication();

private:
    // Private member variables
    IPAddress ip;
    WebServer server;
    WebSocketsServer webSocketServer;

    // Private member functions
};