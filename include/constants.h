#pragma once
#include <Arduino.h>

namespace Constants {
    namespace Hardware {
        const int BUTTON_PIN = 0;      // G22 for external button OR 0 for built-in BOOT button
        const int BUZZER_PIN = 19;     // G19 for external buzzer 
        const int BLUE_LED = 2;        // External blue LED (GPIO 2)
        const int RED_LED = 13;        // External red LED 
        const int DATA_FLICKER_SPEED = 100;
        const int COMPLETE_FLICKER_SPEED = 500;
        const int SECTION_COMPLETE_BEEP = 200;
        const int DEBOUNCE_DELAY = 200;
        const bool BUZZER_ENABLED = true;
    }

    namespace Navigation {
        const int FIRST_CLIP_TAB_COUNT = 16;
        const int NEXT_CLIP_TAB_COUNT = 5;
        const int CLIP_DELAY = 1000;
        const int MIN_TAB_DELAY = 140;
        const int MAX_TAB_DELAY = 400;
    }

    namespace Typing {
        const int BASE_WPM = 65;
        const int BASE_CHAR_DELAY = (60 * 1000) / (BASE_WPM * 5);
        const int WORD_PAUSE = BASE_CHAR_DELAY * 2.5;
        const int SENTENCE_PAUSE = BASE_CHAR_DELAY * 3;
        const int CORRECTION_DELAY = BASE_CHAR_DELAY / 2;
    }

    namespace HumanBehavior {
        const float TYPO_CHANCE = 0.15f;
        const float DOUBLE_SPACE_CHANCE = 0.02f;
        const float UNCORRECTED_TYPO_CHANCE = 0.066f;
        const int UNCORRECTED_TYPO_THRESHOLD = 6;
        const float FATIGUE_FACTOR = 0.05f;
        const float MAX_FATIGUE_LEVEL = 0.3f;
        const float RECOVERY_RATE = 0.05f;
        const int THINKING_PAUSE_CHANCE = 15;
        const int MIN_THINKING_PAUSE = 800;
        const int MAX_THINKING_PAUSE = 2000;
    }
}