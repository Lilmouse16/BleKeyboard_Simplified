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
        const int BASE_WPM = 55;         // Reduced base WPM for longer duration
        const int BASE_CHAR_DELAY = (60 * 1000) / (BASE_WPM * 5);  // Milliseconds per character
        const int WORD_PAUSE = BASE_CHAR_DELAY * 3;        // Increased pause between words
        const int SENTENCE_PAUSE = BASE_CHAR_DELAY * 5;    // Increased pause between sentences
        const int CORRECTION_DELAY = BASE_CHAR_DELAY * 2;  // Increased delay for corrections
        const float MIN_SPEED_MULTIPLIER = 0.5f;
        const float MAX_SPEED_MULTIPLIER = 2.0f;
        const float SPEED_ADJUSTMENT_RATE = 0.05f;         // Reduced for smoother changes
        const int PROGRESS_UPDATE_INTERVAL = 5000;         // Increased update interval
    }

    namespace HumanBehavior {
        const float TYPO_CHANCE = 0.15f;
        const float DOUBLE_SPACE_CHANCE = 0.02f;
        const float UNCORRECTED_TYPO_CHANCE = 0.066f;
        const int UNCORRECTED_TYPO_THRESHOLD = 6;
        const float FATIGUE_FACTOR = 0.02f;               // Reduced for longer duration
        const float MAX_FATIGUE_LEVEL = 0.4f;             // Increased max fatigue
        const float RECOVERY_RATE = 0.01f;                // Slower recovery
        const int THINKING_PAUSE_CHANCE = 20;             // Increased pause chance
        const int MIN_THINKING_PAUSE = 2000;              // Longer thinking pauses
        const int MAX_THINKING_PAUSE = 5000;
        const float MIN_DIFFICULTY_MULTIPLIER = 0.5f;
        const float MAX_DIFFICULTY_MULTIPLIER = 2.0f;
        const float DEFAULT_DIFFICULTY = 1.0f;
        const int SPEED_VARIATION_RANGE = 15;             // Reduced variation for stability
    }
    
    namespace AHT {
        const int PROGRESS_HISTORY_SIZE = 20;             // Increased history size
        const float PROGRESS_THRESHOLD = 0.05f;           // Reduced threshold for smoother adjustments
        const float ADJUSTMENT_SENSITIVITY = 0.2f;        // Reduced sensitivity
        const int MIN_SAMPLES_FOR_ESTIMATION = 5;         // Increased minimum samples
        const unsigned long STATUS_UPDATE_INTERVAL = 10000; // Status update every 10 seconds
    }
}