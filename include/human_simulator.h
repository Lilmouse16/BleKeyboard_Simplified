#pragma once
#include "keyboard.h"
#include "hardware.h"
#include "constants.h"
#include <SPIFFS.h>

class HumanSimulator {
public:
    HumanSimulator(Keyboard& kb, Hardware& hw) : keyboard(kb), hardware(hw) {}
    void init();
    void typeText(const String& text);
    void processClip(int clipNumber);
    int getTotalClips() const { return totalClips; }
    void countClips();

private:
    void applyFatigue();
    void handleTypos(const String& word);
    void simulateThinking();
    void simulateTypingDelay();

    Keyboard& keyboard;
    Hardware& hardware;  // Added hardware reference
    float fatigueLevel = 0.0f;
    int totalClips = 0;
    String currentWord;
    int wordsInBurst = 0;
};