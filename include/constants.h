#pragma once
#include <Arduino.h>

namespace Constants {
    namespace Hardware {
        const int BUTTON_PIN = 23;      // 0 for Built-in BOOT button and 23 for external button
        const int BUZZER_PIN = 19;     // External buzzer 
        const int BLUE_LED = 2;        // External blue LED
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
        const int BASE_WPM = 55;         // Base words per minute
        const int BASE_CHAR_DELAY = (60 * 1000) / (BASE_WPM * 5);  // Milliseconds per character
        const int WORD_PAUSE = BASE_CHAR_DELAY * 3;        // Pause between words
        const int SENTENCE_PAUSE = BASE_CHAR_DELAY * 5;    // Pause between sentences
        const int CORRECTION_DELAY = BASE_CHAR_DELAY * 2;  // Delay for corrections
        const float MIN_SPEED_MULTIPLIER = 0.5f;
        const float MAX_SPEED_MULTIPLIER = 2.0f;
        const float SPEED_ADJUSTMENT_RATE = 0.05f;         // Speed change rate
        const int PROGRESS_UPDATE_INTERVAL = 5000;         // Update interval
    }

    namespace HumanBehavior {
        const float TYPO_CHANCE = 0.15f;
        const float DOUBLE_SPACE_CHANCE = 0.02f;
        const float FATIGUE_FACTOR = 0.02f;               // Fatigue increase rate
        const float MAX_FATIGUE_LEVEL = 0.4f;             // Maximum fatigue
        const float RECOVERY_RATE = 0.01f;                // Recovery rate
        const int THINKING_PAUSE_CHANCE = 20;             // Pause probability
        const int MIN_THINKING_PAUSE = 2000;              // Min thinking pause
        const int MAX_THINKING_PAUSE = 5000;              // Max thinking pause
        const float MIN_DIFFICULTY_MULTIPLIER = 1.0f;     // Changed from 0.5f to 1.0f
        const float MAX_DIFFICULTY_MULTIPLIER = 7.0f;     // Changed from 2.0f to 7.0f
        const float DEFAULT_DIFFICULTY = 2.0f;            // Starting difficulty
        const int SPEED_VARIATION_RANGE = 15;             // Speed variation
    }
    
    namespace AHT {
        const int PROGRESS_HISTORY_SIZE = 20;             // History size
        const float PROGRESS_THRESHOLD = 0.05f;           // Progress threshold
        const float ADJUSTMENT_SENSITIVITY = 0.2f;        // Adjustment rate
        const int MIN_SAMPLES_FOR_ESTIMATION = 5;         // Min samples needed
        const unsigned long STATUS_UPDATE_INTERVAL = 10000; // Status update interval
    }
}