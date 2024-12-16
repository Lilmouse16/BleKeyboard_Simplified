#include <BleKeyboard.h>
#include <SPIFFS.h>
#include <vector>
#include <map>

// Pin Configuration
const int BOOT_BUTTON = 0;     // Built-in BOOT button
const int BLUE_LED = 2;        // Built-in blue LED
const int RED_LED = 13;        // Built-in red LED

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

// QWERTY Keyboard Layout
const std::map<char, std::vector<char>> ADJACENT_KEYS = {
    {'q', {'w', 'a', '1'}}, {'w', {'q', 'e', 's', 'a', '2'}},
    {'e', {'w', 'r', 'd', 's', '3'}}, {'r', {'e', 't', 'f', 'd', '4'}},
    {'t', {'r', 'y', 'g', 'f', '5'}}, {'y', {'t', 'u', 'h', 'g', '6'}},
    {'u', {'y', 'i', 'j', 'h', '7'}}, {'i', {'u', 'o', 'k', 'j', '8'}},
    {'o', {'i', 'p', 'l', 'k', '9'}}, {'p', {'o', '[', ';', 'l', '0'}},
    {'a', {'q', 'w', 's', 'z'}}, {'s', {'w', 'e', 'd', 'x', 'a'}},
    {'d', {'e', 'r', 'f', 'c', 's'}}, {'f', {'r', 't', 'g', 'v', 'd'}},
    {'g', {'t', 'y', 'h', 'b', 'f'}}, {'h', {'y', 'u', 'j', 'n', 'g'}},
    {'j', {'u', 'i', 'k', 'm', 'h'}}, {'k', {'i', 'o', 'l', ',', 'j'}},
    {'l', {'o', 'p', ';', '.', 'k'}}, {'z', {'a', 's', 'x'}},
    {'x', {'s', 'd', 'c', 'z'}}, {'c', {'d', 'f', 'v', 'x'}},
    {'v', {'f', 'g', 'b', 'c'}}, {'b', {'g', 'h', 'n', 'v'}},
    {'n', {'h', 'j', 'm', 'b'}}, {'m', {'j', 'k', ',', 'n'}}
};

// Global Variables
BleKeyboard bleKeyboard("PRO X TSL", "Logitech", 100);
bool isPaused = true;
bool lastButtonState = HIGH;
int currentClip = 1;
int totalClips = 0;
bool connectionAnnounced = false;
int lastTabCount = 0;
float currentFatigue = 0.0f;
unsigned long lastRestTime = 0;
bool isComplete = false;
String currentWord = "";
size_t currentPos = 0;
bool processingPaused = false;

// Helper Functions
float getRandomSpeedMultiplier() {
    float base = 0.8f + (random(0, 400) / 1000.0f);
    float timeEffect = sin(millis() / 3600000.0f * PI) * 0.1f;
    float multiplier = base * (1.0f - currentFatigue) * (1.0f + timeEffect);
    return multiplier < 0.6f ? 0.6f : (multiplier > 1.4f ? 1.4f : multiplier);
}

char getTypoChar(char originalChar) {
    char lowerChar = tolower(originalChar);
    if (ADJACENT_KEYS.find(lowerChar) != ADJACENT_KEYS.end()) {
        const std::vector<char>& adjacent = ADJACENT_KEYS.at(lowerChar);
        char typoChar = adjacent[random(0, adjacent.size())];
        return isUpperCase(originalChar) ? toupper(typoChar) : typoChar;
    }
    return originalChar;
}

void makeTypo(const String& word) {
    if (isPaused) return;
    
    int typoPos = random(0, word.length());
    String typoWord = word;
    typoWord.setCharAt(typoPos, getTypoChar(word[typoPos]));
    
    // Type wrong word
    for (char c : typoWord) {
        if (isPaused) return;
        bleKeyboard.write(c);
        delay(BASE_CHAR_DELAY * getRandomSpeedMultiplier());
    }
    
    // Notice error pause
    if (!isPaused) delay(random(300, 800));
    
    // Backspace to error
    for (int i = typoWord.length() - 1; i >= typoPos; i--) {
        if (isPaused) return;
        bleKeyboard.write(KEY_BACKSPACE);
        delay(CORRECTION_DELAY * getRandomSpeedMultiplier());
    }
    
    // Type correction
    for (int i = typoPos; i < word.length(); i++) {
        if (isPaused) return;
        bleKeyboard.write(word[i]);
        delay(BASE_CHAR_DELAY * getRandomSpeedMultiplier());
    }
}

void handleButton() {
    int reading = digitalRead(BOOT_BUTTON);
    static unsigned long lastDebounceTime = 0;
    
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading == LOW) {
            isPaused = !isPaused;
            processingPaused = isPaused;
            if (isPaused) {
                Serial.println("\n=== PAUSED ===");
                Serial.println("Press BOOT to resume");
                digitalWrite(RED_LED, LOW);
            } else {
                Serial.println("\n=== RESUMING ===");
                if (!isComplete) digitalWrite(RED_LED, HIGH);
            }
            delay(DEBOUNCE_DELAY);
        }
    }
    
    lastButtonState = reading;
}

void humanTypeString(const String& text) {
    static char lastChar = 0;
    int wordCount = 0;
    int burstSize = random(MIN_BURST_WORDS, MAX_BURST_WORDS + 1);
    
    for (size_t i = 0; i < text.length() && !isPaused; i++) {
        handleButton();  // Check for pause button frequently
        if (isPaused) {
            processingPaused = true;
            return;
        }

        char currentChar = text[i];
        
        // Update fatigue
        float newFatigue = currentFatigue + FATIGUE_FACTOR / 1000.0f;
        currentFatigue = min(newFatigue, MAX_FATIGUE_LEVEL);

        // Random thinking pauses
        if (!isPaused && random(0, 100) < THINKING_PAUSE_CHANCE) {
            delay(random(MIN_THINKING_PAUSE, MAX_THINKING_PAUSE));
            currentFatigue = max(0.0f, currentFatigue - RECOVERY_RATE);
        }

        if (isAlphaNumeric(currentChar)) {
            currentWord += currentChar;
        } else if (currentWord.length() > 0) {
            // Handle word completion
            if (!isPaused && currentWord.length() > UNCORRECTED_TYPO_THRESHOLD && 
                random(0, 100) < UNCORRECTED_TYPO_CHANCE * 100) {
                int typoPos = random(0, currentWord.length());
                currentWord[typoPos] = getTypoChar(currentWord[typoPos]);
                for (char c : currentWord) {
                    if (isPaused) return;
                    bleKeyboard.write(c);
                    delay(BASE_CHAR_DELAY * getRandomSpeedMultiplier());
                }
            } else if (!isPaused && random(0, 100) < TYPO_CHANCE * 100) {
                makeTypo(currentWord);
            } else {
                for (char c : currentWord) {
                    if (isPaused) return;
                    bleKeyboard.write(c);
                    delay(BASE_CHAR_DELAY * getRandomSpeedMultiplier());
                }
            }
            currentWord = "";
            wordCount++;

            // Handle burst pauses
            if (!isPaused && wordCount >= burstSize) {
                delay(PAUSE_AFTER_BURST * getRandomSpeedMultiplier());
                wordCount = 0;
                burstSize = random(MIN_BURST_WORDS, MAX_BURST_WORDS);
                currentFatigue = max(0.0f, currentFatigue - 0.1f);
            }
        }

        // Type non-word characters
        if (!isPaused && !isAlphaNumeric(currentChar)) {
            bleKeyboard.write(currentChar);
            delay(BASE_CHAR_DELAY * getRandomSpeedMultiplier());
            lastChar = currentChar;
        }

        // Periodic rest
        if (!isPaused && millis() - lastRestTime > 60000 && random(0, 100) < 10) {
            delay(random(2000, 5000));
            currentFatigue = max(0.0f, currentFatigue - 0.15f);
            lastRestTime = millis();
        }
        
        // Check button state periodically
        if (i % 10 == 0) handleButton();
    }
}

void navigateToClip(int clipNumber) {
    int tabsNeeded;
    
    if (clipNumber == 1) {
        tabsNeeded = FIRST_CLIP_TAB_COUNT;
        lastTabCount = tabsNeeded;
    } else {
        tabsNeeded = NEXT_CLIP_TAB_COUNT;
    }
    
    Serial.printf("Navigating to clip %d (sending %d tabs)\n", clipNumber, tabsNeeded);
    
    for (int i = 0; i < tabsNeeded && !isPaused; i++) {
        handleButton();  // Check for pause button
        if (isPaused) return;
        
        bleKeyboard.write(KEY_TAB);
        delay(BASE_CHAR_DELAY * getRandomSpeedMultiplier());
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
    
    delay(CLIP_DELAY * getRandomSpeedMultiplier());
    
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
    }
}

void countTotalClips() {
    File file = SPIFFS.open("/text.txt", "r");
    if (!file) {
        Serial.println("ERROR: Failed to open text.txt");
        return;
    }
    
    totalClips = 0;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.indexOf("Clip #") != -1) totalClips++;
    }
    
    file.close();
    Serial.printf("Total clips found: %d\n", totalClips);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== ESP32 Human-like Typer Starting ===");
    
    pinMode(BOOT_BUTTON, INPUT_PULLUP);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    
    // Initialize LEDs
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED, LOW);
    
    if (!SPIFFS.begin(true)) {
        Serial.println("ERROR: SPIFFS Mount Failed");
        return;
    }
    
    bleKeyboard.begin();
    countTotalClips();
    isComplete = false;
    
    Serial.println("Ready! Press BOOT button to start/pause/resume");
    Serial.println("Press RST button to reset completely");
}

void loop() {
    digitalWrite(BLUE_LED, bleKeyboard.isConnected());
    
    if (bleKeyboard.isConnected()) {
        if (!connectionAnnounced) {
            Serial.println("\n=== Bluetooth Connected ===");
            connectionAnnounced = true;
        }
        
        handleButton();
        
        if (!isPaused && currentClip <= totalClips) {
            digitalWrite(RED_LED, HIGH);
            processCurrentClip();
            if (!isPaused) {  // Only increment if not paused
                currentClip++;
                delay(CLIP_DELAY);
                
                // Add completion check and messaging
                         if (currentClip > totalClips) {
                    isComplete = true;
                    Serial.println("\n=== All Clips Completed ===");
                    Serial.println("Press RST to start over");
                    digitalWrite(RED_LED, HIGH);  // Keep red LED on at completion
                }
            }
        }
    } else {
        connectionAnnounced = false;
        isComplete = false;  // Reset completion state on disconnect
        digitalWrite(RED_LED, LOW);  // Turn off red LED when disconnected
        Serial.println("Waiting for Bluetooth connection...");
        delay(1000);
    }
}
