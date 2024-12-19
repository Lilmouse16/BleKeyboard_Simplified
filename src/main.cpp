#include <BleKeyboard.h>
#include <SPIFFS.h>
#include <vector>
#include <map>

// Pin Configuration
const int BUTTON_PIN = 0;     // Changed to G22 for external button OR 0 for built-in BOOT button
const int BUZZER_PIN = 19;     // G19 for external buzzer 
const int BLUE_LED = 2;        // Built-in blue LED (GPIO 2)
const int RED_LED = 13;        // Red LED (change if not connected or absent)

// User-adjustable controls
const bool AUTO_PAUSE_AFTER_SECTION = true;    // Pause after each section
const bool BUZZER_ENABLED = true;              // Enable/disable buzzer
const int DATA_FLICKER_SPEED = 100;            // Alternating speed during typing
const int COMPLETE_FLICKER_SPEED = 500;        // Synchronized speed after section
const int SECTION_COMPLETE_BEEP = 200;         // Buzzer beep duration

// Navigation Constants
const int FIRST_CLIP_TAB_COUNT = 16;
const int NEXT_CLIP_TAB_COUNT = 5;
const int CLIP_DELAY = 1000;

// Typing Behavior Constants
const int BASE_WPM = 56;
const float TYPO_CHANCE = 0.15f;
const float DOUBLE_SPACE_CHANCE = 0.02f;
const float UNCORRECTED_TYPO_CHANCE = 0.066f;
const int UNCORRECTED_TYPO_THRESHOLD = 6;
const int MIN_BURST_WORDS = 6;
const int MAX_BURST_WORDS = 14;
const int PAUSE_AFTER_BURST = 500;

// Enhanced Randomness Settings
const int THINKING_PAUSE_CHANCE = 5;
const int MIN_THINKING_PAUSE = 500;
const int MAX_THINKING_PAUSE = 1500;
const float FATIGUE_FACTOR = 0.05f;
const float MAX_FATIGUE_LEVEL = 0.3f;
const float RECOVERY_RATE = 0.05f;

// Timing Constants
const int BASE_CHAR_DELAY = (60 * 1000) / (BASE_WPM * 5);
const int WORD_PAUSE = BASE_CHAR_DELAY * 1.5;
const int SENTENCE_PAUSE = BASE_CHAR_DELAY * 3;
const int CORRECTION_DELAY = BASE_CHAR_DELAY / 2;
const int DEBOUNCE_DELAY = 200;

// Global Variables
BleKeyboard bleKeyboard("PRO X TSL", "Logitech", 100);
bool isPaused = true;
bool lastButtonState = HIGH;
int currentClip = 1;
int totalClips = 0;
bool connectionAnnounced = false;
bool sectionComplete = false;
unsigned long lastLedToggle = 0;
bool ledState = false;

// Function Declarations
void setLedPattern(int pattern);
void playCompletionBeep();
void handleButton();
void navigateToClip(int clipNumber);
void countTotalClips();
void simulateTypingDelay();
void simulateThinkingPause();
void handleTypo(const String& word);
void humanTypeString(const String& text);
void processCurrentClip();

// LED Control Functions
void setLedPattern(int pattern) {
    static unsigned long lastToggle = 0;
    unsigned long currentMillis = millis();
    
    switch(pattern) {
        case 0: // Both OFF
            digitalWrite(BLUE_LED, HIGH);  // Active LOW
            digitalWrite(RED_LED, HIGH);   // Active LOW
            break;
            
        case 1: // Both ON (ready state)
            digitalWrite(BLUE_LED, LOW);   // Active LOW
            digitalWrite(RED_LED, LOW);    // Active LOW
            break;
            
        case 2: // Alternating pattern (typing)
            if (currentMillis - lastToggle >= DATA_FLICKER_SPEED) {
                ledState = !ledState;
                digitalWrite(BLUE_LED, ledState ? HIGH : LOW);   // Active LOW
                digitalWrite(RED_LED, ledState ? LOW : HIGH);    // Active LOW
                lastToggle = currentMillis;
            }
            break;
            
        case 3: // Only RED ON (paused)
            digitalWrite(BLUE_LED, HIGH);  // OFF (Active LOW)
            digitalWrite(RED_LED, LOW);    // ON (Active LOW)
            break;
            
        case 4: // Synchronized flashing (section complete)
            if (currentMillis - lastToggle >= COMPLETE_FLICKER_SPEED) {
                ledState = !ledState;
                digitalWrite(BLUE_LED, ledState ? LOW : HIGH);   // Active LOW
                digitalWrite(RED_LED, ledState ? LOW : HIGH);    // Active LOW
                lastToggle = currentMillis;
            }
            break;
            
        case 5: // Only BLUE ON (connecting)
            digitalWrite(BLUE_LED, LOW);   // ON (Active LOW)
            digitalWrite(RED_LED, HIGH);   // OFF (Active LOW)
            break;
    }
}

void playCompletionBeep() {
    if (!BUZZER_ENABLED) return;
    
    digitalWrite(BUZZER_PIN, HIGH);
    delay(SECTION_COMPLETE_BEEP);
    digitalWrite(BUZZER_PIN, LOW);
}

void handleButton() {
    static unsigned long lastDebounceTime = 0;
    static int lastStableState = HIGH;
    static int lastReading = HIGH;
    
    int reading = digitalRead(BUTTON_PIN);

    // If the reading has changed, reset debounce timer
    if (reading != lastReading) {
        lastDebounceTime = millis();
    }
    
    // Check if enough time has passed since the last change
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        // If the reading is stable and different from last stable state
        if (reading != lastStableState) {
            lastStableState = reading;
            
            // Button press detected (transition to LOW)
            if (reading == LOW) {
                if (sectionComplete) {
                    sectionComplete = false;
                    isPaused = false;
                    currentClip++;
                    Serial.println("\n=== Starting Next Section ===");
                } else {
                    isPaused = !isPaused;
                    if (isPaused) {
                        Serial.println("\n=== PAUSED ===");
                        Serial.println("Press button to resume");
                    } else {
                        Serial.println("\n=== RESUMING ===");
                    }
                }
            }
        }
    }
    
    lastReading = reading;
}

void navigateToClip(int clipNumber) {
    if (clipNumber == 1) {
        // First clip needs more tabs to reach
        for (int i = 0; i < FIRST_CLIP_TAB_COUNT; i++) {
            bleKeyboard.write(KEY_TAB);
            delay(50);  // Small delay between tabs
        }
    } else {
        // Subsequent clips need fewer tabs
        for (int i = 0; i < NEXT_CLIP_TAB_COUNT; i++) {
            bleKeyboard.write(KEY_TAB);
            delay(50);
        }
    }
}

void countTotalClips() {
    File file = SPIFFS.open("/text.txt", "r");
    if (!file) {
        Serial.println("ERROR: Failed to open text.txt for counting clips");
        return;
    }

    totalClips = 0;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.indexOf("Clip #") != -1) {
            totalClips++;
        }
    }
    
    file.close();
    Serial.printf("Found %d total clips\n", totalClips);
}

void simulateTypingDelay() {
    static float fatigueLevel = 0.0f;
    
    // Apply fatigue effect
    fatigueLevel = min(fatigueLevel + FATIGUE_FACTOR, MAX_FATIGUE_LEVEL);
    int adjustedDelay = BASE_CHAR_DELAY * (1.0f + fatigueLevel);
    
    // Random variation in typing speed
    adjustedDelay += random(-BASE_CHAR_DELAY/4, BASE_CHAR_DELAY/4);
    delay(max(adjustedDelay, BASE_CHAR_DELAY/2));
    
    // Occasional recovery from fatigue
    if (random(100) < 10) {
        fatigueLevel = max(0.0f, fatigueLevel - RECOVERY_RATE);
    }
}

void simulateThinkingPause() {
    if (random(100) < THINKING_PAUSE_CHANCE) {
        delay(random(MIN_THINKING_PAUSE, MAX_THINKING_PAUSE));
    }
}

void handleTypo(const String& word) {
    if (random(100) < TYPO_CHANCE * 100) {
        int wordLen = word.length();
        int typoPos = random(wordLen);
        
        // Type the word up to the typo
        for (int i = 0; i < typoPos; i++) {
            bleKeyboard.write(word[i]);
            simulateTypingDelay();
        }
        
        // Make a typo
        char wrongChar = 'a' + random(26);
        bleKeyboard.write(wrongChar);
        simulateTypingDelay();
        
        // Decide whether to correct the typo
        if (wordLen < UNCORRECTED_TYPO_THRESHOLD || random(100) >= UNCORRECTED_TYPO_CHANCE * 100) {
            // Correct the typo
            delay(CORRECTION_DELAY);
            bleKeyboard.write(KEY_BACKSPACE);
            delay(CORRECTION_DELAY);
            bleKeyboard.write(word[typoPos]);
            simulateTypingDelay();
            
            // Complete the word
            for (int i = typoPos + 1; i < wordLen; i++) {
                bleKeyboard.write(word[i]);
                simulateTypingDelay();
            }
        }
    } else {
        // Type the word normally
        for (char c : word) {
            bleKeyboard.write(c);
            simulateTypingDelay();
        }
    }
}

void humanTypeString(const String& text) {
    int wordsInBurst = 0;
    int currentBurstLimit = random(MIN_BURST_WORDS, MAX_BURST_WORDS);
    String currentWord = "";
    
    for (int i = 0; i < text.length(); i++) {
        if (isPaused) return;  // Check for pause state
        
        char c = text[i];
        
        // Handle word completion
        if (c == ' ' || c == '\n') {
            handleTypo(currentWord);
            currentWord = "";
            wordsInBurst++;
            
            // Add double space occasionally
            if (c == ' ' && random(100) < DOUBLE_SPACE_CHANCE * 100) {
                bleKeyboard.write(' ');
                simulateTypingDelay();
            }
            
            bleKeyboard.write(c);
            
            // Handle burst limits and pauses
            if (wordsInBurst >= currentBurstLimit) {
                delay(PAUSE_AFTER_BURST);
                wordsInBurst = 0;
                currentBurstLimit = random(MIN_BURST_WORDS, MAX_BURST_WORDS);
                simulateThinkingPause();
            }
            
            // Longer pause after sentences
            if (i > 0 && (text[i-1] == '.' || text[i-1] == '?' || text[i-1] == '!')) {
                delay(SENTENCE_PAUSE);
                simulateThinkingPause();
            } else {
                delay(WORD_PAUSE);
            }
        } else {
            currentWord += c;
        }
    }
    
    // Handle last word if any
    if (currentWord.length() > 0) {
        handleTypo(currentWord);
    }
}

void processCurrentClip() {
    Serial.printf("\n=== Processing Clip %d ===\n", currentClip);
    File file = SPIFFS.open("/text.txt", "r");
    if (!file) {
        Serial.println("ERROR: Failed to open text.txt");
        return;
    }
    
    navigateToClip(currentClip);
    if (isPaused) {
        file.close();
        return;
    }
    
    delay(CLIP_DELAY);
    
    String clipText = "";
    int clipCount = 0;
    bool isReadingClip = false;
    
    while (file.available() && !isPaused) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.indexOf("Clip #") != -1) {
            if (isReadingClip) break;
            clipCount++;
            if (clipCount == currentClip) {
                isReadingClip = true;
                continue;
            }
        } else if (isReadingClip && line.length() > 0) {
            clipText += line + "\n";
        }
    }
    
    file.close();
    
    if (clipText.length() > 0 && !isPaused) {
        Serial.printf("Typing clip content (%d characters)\n", clipText.length());
        humanTypeString(clipText);
        
        // Section completion actions
        if (!isPaused) {
            playCompletionBeep();
            sectionComplete = true;
            if (AUTO_PAUSE_AFTER_SECTION) {
                isPaused = true;
                Serial.println("\n=== Section Complete ===");
                Serial.println("Press button to continue");
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== ESP32 Human-like Typer Starting ===");
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    
    // Initialize outputs
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(BLUE_LED, HIGH);  // Turn OFF (ESP32 LEDs are active LOW)
    digitalWrite(RED_LED, HIGH);   // Turn OFF (ESP32 LEDs are active LOW)
    setLedPattern(0);  // Both OFF initially
    
    if (!SPIFFS.begin(true)) {
        Serial.println("ERROR: SPIFFS Mount Failed");
        return;
    }
    
    bleKeyboard.begin();
    countTotalClips();
    Serial.println("Ready! Press button to start/pause/resume");
}

void loop() {
    if (bleKeyboard.isConnected()) {
        if (!connectionAnnounced) {
            Serial.println("\n=== Bluetooth Connected ===");
            connectionAnnounced = true;
            setLedPattern(1);  // Both ON when ready
        }
        
        handleButton();
        
        if (!isPaused && !sectionComplete) {
            setLedPattern(2);  // Alternating pattern while typing
            processCurrentClip();
        } else if (isPaused && !sectionComplete) {
            setLedPattern(3);  // RED ON when paused
        } else if (sectionComplete) {
            setLedPattern(4);  // Synchronized flashing when section complete
        }
        
        if (currentClip > totalClips) {
            Serial.println("\n=== All Clips Completed ===");
            setLedPattern(1);  // Both ON when complete
            while(1) {
                delay(1000);  // Stop processing
            }
        }
    } else {
        connectionAnnounced = false;
        sectionComplete = false;
        setLedPattern(5);  // Only BLUE ON when connecting
        Serial.println("Waiting for Bluetooth connection...");
        delay(1000);
    }
}