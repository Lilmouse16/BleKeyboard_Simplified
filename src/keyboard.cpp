#include "keyboard.h"

void Keyboard::init() {
    bleKeyboard.begin();
}

bool Keyboard::isConnected() {
    return bleKeyboard.isConnected();
}

void Keyboard::type(const String& text) {
    bleKeyboard.print(text);
}

void Keyboard::pressKey(uint8_t key) {
    bleKeyboard.write(key);
}

void Keyboard::navigate(int tabCount) {
    for (int i = 0; i < tabCount; i++) {
        pressKey(KEY_TAB);
        simulateTabDelay();
    }
    delay(Constants::Navigation::CLIP_DELAY);
}

void Keyboard::simulateTabDelay() {
    delay(random(Constants::Navigation::MIN_TAB_DELAY, 
                 Constants::Navigation::MAX_TAB_DELAY));
}
