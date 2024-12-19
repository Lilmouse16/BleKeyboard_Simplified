#pragma once
#include "constants.h"

class Hardware {
public:
    enum class Pattern {
        ALL_OFF,
        ALL_ON,
        ALTERNATING,
        RED_ONLY,
        SYNC_FLASH,
        BLUE_ONLY
    };

    enum class SoundType {
        SECTION_COMPLETE,
        ERROR,
        SUCCESS
    };

    void init();
    void setLedPattern(Pattern pattern);
    void handleButton();
    bool isButtonPressed();
    void playSound(SoundType type);
    bool isPaused() const { return paused; }
    bool isSectionComplete() const { return sectionComplete; }
    void setSectionComplete(bool complete) { sectionComplete = complete; }

private:
    bool paused = true;
    bool sectionComplete = false;
    bool ledState = false;
    unsigned long lastLedToggle = 0;
    int lastButtonState = HIGH;
    unsigned long lastDebounceTime = 0;
};