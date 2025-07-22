#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

// Serve config_gui2.html from SPIFFS at root URL "/"
void setupHtmlFsTest(WebServer &server) {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    server.on("/", HTTP_GET, []() {
        File file = SPIFFS.open("/config_gui2.html", "r");
        if (!file) {
            server.send(404, "text/plain", "File Not Found");
            return;
        }
        server.streamFile(file, "text/html");
        file.close();
    });
}