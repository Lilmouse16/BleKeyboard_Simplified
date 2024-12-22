#pragma once
#include "keyboard.h"
#include "hardware.h"
#include "constants.h"
#include "aht_calculator.h"
#include "timing_manager.h"
#include "time_utils.h"
#include <SPIFFS.h>

class HumanSimulator {
public:
    HumanSimulator(Keyboard& kb, Hardware& hw) 
        : keyboard(kb), hardware(hw) {}
    
    bool init();
    void setDifficulty(float difficulty);
    void typeText(const String& text);
    void processClip(int clipNumber);
    int getTotalClips() const { return totalClips; }
    void countClips();
    float getCurrentProgress() const;
    float getEstimatedTimeRemaining() const;

private:
    void applyFatigue();
    void handleTypos(const String& word);
    void simulateThinking();
    void simulateTypingDelay();
    void updateTypingProgress(size_t charsTyped, size_t totalChars);
    
    Keyboard& keyboard;
    Hardware& hardware;
    AHTCalculator ahtCalculator;
    TimingManager* timingManager;
    float fatigueLevel = 0.0f;
    int totalClips = 0;
    String currentWord;
    int wordsInBurst = 0;
    size_t totalCharCount = 0;
    size_t typedCharCount = 0;
};