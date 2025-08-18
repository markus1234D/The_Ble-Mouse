#ifndef BUTTONWORKER_H
#define BUTTONWORKER_H

#include <Arduino.h>
#include <Button.h>
#include <vector>

#define DEBUG

#define BUTTON_1 GPIO_NUM_2
#define BUTTON_2 GPIO_NUM_3
#define BUTTON_3 GPIO_NUM_4
#define BUTTON_4 GPIO_NUM_5
#define BUTTON_5 GPIO_NUM_6

class ButtonWorker {
public:
    ButtonWorker();
    void init();
    void setBtnActionFunction(String button, String action, std::function<void(int, int)> callback);
private:
    struct CallbackWrapper {
        std::function<void(int, int)> func;
        int btnId;
        int actionId;
    };
    std::map<String, Button*> buttons;
    std::map<String, std::map<String, CallbackWrapper*>> callbackWrappers;
    static void staticCallback(void *button_handle, void *usr_data);


    static void debugPrint(String str) {
        #ifdef DEBUG
            Serial.println(str);
        #endif
    }

    static void staticCallback(void *button_handle, void *usr_data) {
        CallbackWrapper* wrapper = static_cast<CallbackWrapper*>(usr_data);
        if (wrapper && wrapper->func) {
            wrapper->func(wrapper->btnId, wrapper->actionId);
        }
    }
    static void debugPrint(String str) {
        #ifdef DEBUG
            Serial.println(str);
        #endif
    }


};

void ButtonWorker::init() {

    buttons["btn_1"] = new Button(BUTTON_1, true);
    buttons["btn_2"] = new Button(BUTTON_2, true);
    buttons["btn_3"] = new Button(BUTTON_3, true);
    buttons["btn_4"] = new Button(BUTTON_4, true);
    buttons["btn_5"] = new Button(BUTTON_5, true);
}

void ButtonWorker::setBtnActionFunction(String button, String action, std::function<void(int, int)> callback) {
    if (buttons.find(button) == buttons.end()) {
        debugPrint("Button not found: " + button);
        return;
    }    
    if(action == "pressDown") {
        // Clean up previous wrapper if exists
        if (callbackWrappers[button][action]) {
            delete callbackWrappers[button][action];
        }
        CallbackWrapper* wrapper = new CallbackWrapper{callback, 0, 0}; // set btnId/actionId as needed
        callbackWrappers[button][action] = wrapper;
        buttons[button]->attachPressDownEventCb(staticCallback, wrapper);
    }
    else if(action == "pressUp") {
        if (callbackWrappers[button][action]) {
            delete callbackWrappers[button][action];
        }
        CallbackWrapper* wrapper = new CallbackWrapper{callback, 0, 1}; // set btnId/actionId as needed
        callbackWrappers[button][action] = wrapper;
        buttons[button]->attachPressUpEventCb(staticCallback, wrapper);
    }


}

#endif // BUTTONWORKER_H