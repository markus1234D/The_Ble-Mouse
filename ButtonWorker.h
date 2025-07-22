#ifndef BUTTONWORKER_H
#define BUTTONWORKER_H

#include <Arduino.h>
#include <Button.h>
#include <vector>

#define DEBUG

#define BUTTON_2F GPIO_NUM_2
#define BUTTON_2B GPIO_NUM_3
#define BUTTON_3 GPIO_NUM_4
#define BUTTON_4 GPIO_NUM_5
#define BUTTON_5 GPIO_NUM_6

class ButtonWorker {
public:
    ButtonWorker();
    void init();
    // vector of buttons
    std::vector<Button*> buttons;


private:
    static void debugPrint(String str) {
        #ifdef DEBUG
            Serial.println(str);
        #endif
    }

    Button *btn_2f = new Button(BUTTON_2F, true);
    Button *btn_2b = new Button(BUTTON_2B, true);
    Button *btn_3 = new Button(BUTTON_3, true);
    Button *btn_4 = new Button(BUTTON_4, true);
    Button *btn_5 = new Button(BUTTON_5, true);
    // 2f
    static void btn2f_pressDownCallback(void *button_handle, void *usr_data);
    static void btn2f_pressUpCallback(void *button_handle, void *usr_data);
    static void btn2f_pressRepeatCallback(void *button_handle, void *usr_data);
    static void btn2f_singleClickCallback(void *button_handle, void *usr_data);
    static void btn2f_doubleClickCallback(void *button_handle, void *usr_data);
    static void btn2f_longPressStartCallback(void *button_handle, void *usr_data);
    static void btn2f_longPressHoldCallback(void *button_handle, void *usr_data);
    static void btn2f_pressRepeatDoneCallback(void *button_handle, void *usr_data);
    static void btn2f_longPressUpCallback(void *button_handle, void *usr_data);

    // 2b
    static void btn2b_pressDownCallback(void *button_handle, void *usr_data);
    static void btn2b_pressUpCallback(void *button_handle, void *usr_data);
    static void btn2b_pressRepeatCallback(void *button_handle, void *usr_data);
    static void btn2b_singleClickCallback(void *button_handle, void *usr_data);
    static void btn2b_doubleClickCallback(void *button_handle, void *usr_data);
    static void btn2b_longPressStartCallback(void *button_handle, void *usr_data);
    static void btn2b_longPressHoldCallback(void *button_handle, void *usr_data);
    static void btn2b_pressRepeatDoneCallback(void *button_handle, void *usr_data);
    static void btn2b_longPressUpCallback(void *button_handle, void *usr_data);

    // 3
    static void btn3_pressDownCallback(void *button_handle, void *usr_data);
    static void btn3_pressUpCallback(void *button_handle, void *usr_data);
    static void btn3_pressRepeatCallback(void *button_handle, void *usr_data);
    static void btn3_singleClickCallback(void *button_handle, void *usr_data);
    static void btn3_doubleClickCallback(void *button_handle, void *usr_data);
    static void btn3_longPressStartCallback(void *button_handle, void *usr_data);
    static void btn3_longPressHoldCallback(void *button_handle, void *usr_data);
    static void btn3_pressRepeatDoneCallback(void *button_handle, void *usr_data);
    static void btn3_longPressUpCallback(void *button_handle, void *usr_data);

    // 4
    static void btn4_pressDownCallback(void *button_handle, void *usr_data);
    static void btn4_pressUpCallback(void *button_handle, void *usr_data);
    static void btn4_pressRepeatCallback(void *button_handle, void *usr_data);
    static void btn4_singleClickCallback(void *button_handle, void *usr_data);
    static void btn4_doubleClickCallback(void *button_handle, void *usr_data);
    static void btn4_longPressStartCallback(void *button_handle, void *usr_data);
    static void btn4_longPressHoldCallback(void *button_handle, void *usr_data);
    static void btn4_pressRepeatDoneCallback(void *button_handle, void *usr_data);
    static void btn4_longPressUpCallback(void *button_handle, void *usr_data);

    // 5
    static void btn5_pressDownCallback(void *button_handle, void *usr_data);
    static void btn5_pressUpCallback(void *button_handle, void *usr_data);
    static void btn5_pressRepeatCallback(void *button_handle, void *usr_data);
    static void btn5_singleClickCallback(void *button_handle, void *usr_data);
    static void btn5_doubleClickCallback(void *button_handle, void *usr_data);
    static void btn5_longPressStartCallback(void *button_handle, void *usr_data);
    static void btn5_longPressHoldCallback(void *button_handle, void *usr_data);
    static void btn5_pressRepeatDoneCallback(void *button_handle, void *usr_data);
    static void btn5_longPressUpCallback(void *button_handle, void *usr_data);

};

void ButtonWorker::init() {

    // fill the buttons vector
    buttons.push_back(btn_2f);
    buttons.push_back(btn_2b);
    buttons.push_back(btn_3);
    buttons.push_back(btn_4);
    buttons.push_back(btn_5);

    // 2f
    btn_2f->attachPressDownEventCb(btn2f_pressDownCallback, NULL);
    btn_2f->attachPressUpEventCb(btn2f_pressUpCallback, NULL);
    btn_2f->attachPressRepeatEventCb(btn2f_pressRepeatCallback, NULL);
    btn_2f->attachSingleClickEventCb(btn2f_singleClickCallback, NULL);
    btn_2f->attachDoubleClickEventCb(btn2f_doubleClickCallback, NULL);
    btn_2f->attachLongPressStartEventCb(btn2f_longPressStartCallback, NULL);
    btn_2f->attachLongPressHoldEventCb(btn2f_longPressHoldCallback, NULL);
    btn_2f->attachPressRepeatDoneEventCb(btn2f_pressRepeatDoneCallback, NULL);
    btn_2f->attachLongPressUpEventCb(btn2f_longPressUpCallback, NULL);

    // 2b
    btn_2b->attachPressDownEventCb(btn2b_pressDownCallback, NULL);
    btn_2b->attachPressUpEventCb(btn2b_pressUpCallback, NULL);
    btn_2b->attachPressRepeatEventCb(btn2b_pressRepeatCallback, NULL);
    btn_2b->attachSingleClickEventCb(btn2b_singleClickCallback, NULL);
    btn_2b->attachDoubleClickEventCb(btn2b_doubleClickCallback, NULL);
    btn_2b->attachLongPressStartEventCb(btn2b_longPressStartCallback, NULL);
    btn_2b->attachLongPressHoldEventCb(btn2b_longPressHoldCallback, NULL);
    btn_2b->attachPressRepeatDoneEventCb(btn2b_pressRepeatDoneCallback, NULL);
    btn_2b->attachLongPressUpEventCb(btn2b_longPressUpCallback, NULL);

    // 3
    btn_3->attachPressDownEventCb(btn3_pressDownCallback, NULL);
    btn_3->attachPressUpEventCb(btn3_pressUpCallback, NULL);
    btn_3->attachPressRepeatEventCb(btn3_pressRepeatCallback, NULL);
    btn_3->attachSingleClickEventCb(btn3_singleClickCallback, NULL);
    btn_3->attachDoubleClickEventCb(btn3_doubleClickCallback, NULL);
    btn_3->attachLongPressStartEventCb(btn3_longPressStartCallback, NULL);
    btn_3->attachLongPressHoldEventCb(btn3_longPressHoldCallback, NULL);
    btn_3->attachPressRepeatDoneEventCb(btn3_pressRepeatDoneCallback, NULL);
    btn_3->attachLongPressUpEventCb(btn3_longPressUpCallback, NULL);

    // 4
    btn_4->attachPressDownEventCb(btn4_pressDownCallback, NULL);
    btn_4->attachPressUpEventCb(btn4_pressUpCallback, NULL);
    btn_4->attachPressRepeatEventCb(btn4_pressRepeatCallback, NULL);
    btn_4->attachSingleClickEventCb(btn4_singleClickCallback, NULL);
    btn_4->attachDoubleClickEventCb(btn4_doubleClickCallback, NULL);
    btn_4->attachLongPressStartEventCb(btn4_longPressStartCallback, NULL);
    btn_4->attachLongPressHoldEventCb(btn4_longPressHoldCallback, NULL);
    btn_4->attachPressRepeatDoneEventCb(btn4_pressRepeatDoneCallback, NULL);
    btn_4->attachLongPressUpEventCb(btn4_longPressUpCallback, NULL);

    // 5
    btn_5->attachPressDownEventCb(btn5_pressDownCallback, NULL);
    btn_5->attachPressUpEventCb(btn5_pressUpCallback, NULL);
    btn_5->attachPressRepeatEventCb(btn5_pressRepeatCallback, NULL);
    btn_5->attachSingleClickEventCb(btn5_singleClickCallback, NULL);
    btn_5->attachDoubleClickEventCb(btn5_doubleClickCallback, NULL);
    btn_5->attachLongPressStartEventCb(btn5_longPressStartCallback, NULL);
    btn_5->attachLongPressHoldEventCb(btn5_longPressHoldCallback, NULL);
    btn_5->attachPressRepeatDoneEventCb(btn5_pressRepeatDoneCallback, NULL);
    btn_5->attachLongPressUpEventCb(btn5_longPressUpCallback, NULL);
}

void ButtonWorker::btn2f_pressDownCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Press Down");
}
void ButtonWorker::btn2f_pressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Press Up");
}

void ButtonWorker::btn2f_pressRepeatCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Press Repeat");
}

void ButtonWorker::btn2f_singleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Single Click");
}

void ButtonWorker::btn2f_doubleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Double Click");
}

void ButtonWorker::btn2f_longPressStartCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Long Press Start");
}

void ButtonWorker::btn2f_longPressHoldCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Long Press Hold");
}

void ButtonWorker::btn2f_pressRepeatDoneCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Press Repeat Done");
}

void ButtonWorker::btn2f_longPressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("2F Long Press Up");
}

void ButtonWorker::btn2b_pressDownCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Press Down");
}

void ButtonWorker::btn2b_pressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Press Up");
}

void ButtonWorker::btn2b_pressRepeatCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Press Repeat");
}

void ButtonWorker::btn2b_singleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Single Click");
}

void ButtonWorker::btn2b_doubleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Double Click");
}

void ButtonWorker::btn2b_longPressStartCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Long Press Start");
}

void ButtonWorker::btn2b_longPressHoldCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Long Press Hold");
}

void ButtonWorker::btn2b_pressRepeatDoneCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Press Repeat Done");
}

void ButtonWorker::btn2b_longPressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("2B Long Press Up");
}

void ButtonWorker::btn3_pressDownCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Press Down");
}

void ButtonWorker::btn3_pressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Press Up");
}

void ButtonWorker::btn3_pressRepeatCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Press Repeat");
}

void ButtonWorker::btn3_singleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Single Click");
}

void ButtonWorker::btn3_doubleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Double Click");
}

void ButtonWorker::btn3_longPressStartCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Long Press Start");
}

void ButtonWorker::btn3_longPressHoldCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Long Press Hold");
}

void ButtonWorker::btn3_pressRepeatDoneCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Press Repeat Done");
}

void ButtonWorker::btn3_longPressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("3 Long Press Up");
}

void ButtonWorker::btn4_pressDownCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Press Down");
}

void ButtonWorker::btn4_pressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Press Up");
}

void ButtonWorker::btn4_pressRepeatCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Press Repeat");
}

void ButtonWorker::btn4_singleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Single Click");
}

void ButtonWorker::btn4_doubleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Double Click");
}

void ButtonWorker::btn4_longPressStartCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Long Press Start");
}

void ButtonWorker::btn4_longPressHoldCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Long Press Hold");
}

void ButtonWorker::btn4_pressRepeatDoneCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Press Repeat Done");
}

void ButtonWorker::btn4_longPressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("4 Long Press Up");
}

void ButtonWorker::btn5_pressDownCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Press Down");
}

void ButtonWorker::btn5_pressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Press Up");
}

void ButtonWorker::btn5_pressRepeatCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Press Repeat");
}

void ButtonWorker::btn5_singleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Single Click");
}

void ButtonWorker::btn5_doubleClickCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Double Click");
}

void ButtonWorker::btn5_longPressStartCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Long Press Start");
}

void ButtonWorker::btn5_longPressHoldCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Long Press Hold");
}

void ButtonWorker::btn5_pressRepeatDoneCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Press Repeat Done");
}

void ButtonWorker::btn5_longPressUpCallback(void *button_handle, void *usr_data) {
    debugPrint("5 Long Press Up");
}

#endif // BUTTONWORKER_H