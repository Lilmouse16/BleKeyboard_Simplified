#pragma once
#include <BleKeyboard.h>
#include "constants.h"

class Keyboard {
public:
    void init();
    bool isConnected();
    void type(const String& text);
    void pressKey(uint8_t key);
    void releaseKey(uint8_t key);
    void navigate(int tabCount);

private:
    BleKeyboard bleKeyboard{"PRO X TSL", "Logitech", 100};
    void simulateTabDelay();
};