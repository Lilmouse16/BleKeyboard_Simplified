#include "human_simulator.h"

void HumanSimulator::init() {
    fatigueLevel = 0.0f;
    countClips();
    Serial.println("Human simulator initialized");
}

void HumanSimulator::typeText(const String& text) {
    Serial.println("Starting to type text...");
    currentWord = "";
    wordsInBurst = 0;
    
    for (char c : text) {
        // Check if hardware is paused before each character
        if (hardware.isPaused()) {
            Serial.println("Typing paused");
            return;  // Exit typing if paused
        }

        if (c == ' ' || c == '\n') {
            Serial.printf("Typing word: %s\n", currentWord.c_str());
            handleTypos(currentWord);
            currentWord = "";
            wordsInBurst++;
            
            if (random(100) < Constants::HumanBehavior::DOUBLE_SPACE_CHANCE * 100) {
                Serial.println("Adding double space");
                keyboard.type(" ");
            }
            
            keyboard.type(String(c));
            simulateThinking();
        } else {
            currentWord += c;
        }
    }
    
    if (currentWord.length() > 0 && !hardware.isPaused()) {
        Serial.printf("Typing final word: %s\n", currentWord.c_str());
        handleTypos(currentWord);
    }
    Serial.println("Finished typing text");
}

void HumanSimulator::handleTypos(const String& word) {
    // Check for pause before starting word
    if (hardware.isPaused()) return;

    if (random(100) < Constants::HumanBehavior::TYPO_CHANCE * 100) {
        int wordLen = word.length();
        int typoPos = random(wordLen);
        
        Serial.printf("Making typo in word at position %d\n", typoPos);
        
        // Type up to typo, checking for pause
        for (int i = 0; i < typoPos; i++) {
            if (hardware.isPaused()) return;
            keyboard.type(String(word[i]));
            simulateTypingDelay();
        }
        
        if (hardware.isPaused()) return;
        
        // Make typo
        char wrongChar = 'a' + random(26);
        keyboard.type(String(wrongChar));
        
        // Maybe correct it
        if (wordLen < Constants::HumanBehavior::UNCORRECTED_TYPO_THRESHOLD || 
            random(100) >= Constants::HumanBehavior::UNCORRECTED_TYPO_CHANCE * 100) {
            if (hardware.isPaused()) return;
            
            Serial.println("Correcting typo");
            delay(Constants::Typing::CORRECTION_DELAY);
            keyboard.pressKey(KEY_BACKSPACE);
            delay(Constants::Typing::CORRECTION_DELAY);
            keyboard.type(String(word[typoPos]));
            
            // Type rest of word
            for (int i = typoPos + 1; i < wordLen; i++) {
                if (hardware.isPaused()) return;
                keyboard.type(String(word[i]));
                simulateTypingDelay();
            }
        } else {
            Serial.println("Leaving typo uncorrected");
            keyboard.type(word.substring(typoPos + 1));
        }
    } else {
        // Type word normally
        for (char c : word) {
            if (hardware.isPaused()) return;
            keyboard.type(String(c));
            simulateTypingDelay();
        }
    }
}

void HumanSimulator::simulateTypingDelay() {
    if (hardware.isPaused()) return;
    
    applyFatigue();
    int adjustedDelay = Constants::Typing::BASE_CHAR_DELAY * (1.0f + fatigueLevel);
    adjustedDelay += random(-Constants::Typing::BASE_CHAR_DELAY/4, 
                           Constants::Typing::BASE_CHAR_DELAY/4);
    delay(max(adjustedDelay, Constants::Typing::BASE_CHAR_DELAY/2));
}

void HumanSimulator::applyFatigue() {
    fatigueLevel = min(fatigueLevel + Constants::HumanBehavior::FATIGUE_FACTOR,
                      Constants::HumanBehavior::MAX_FATIGUE_LEVEL);
    if (random(100) < 10) {
        fatigueLevel = max(0.0f, fatigueLevel - Constants::HumanBehavior::RECOVERY_RATE);
    }
}

void HumanSimulator::simulateThinking() {
    if (hardware.isPaused()) return;
    
    if (random(100) < Constants::HumanBehavior::THINKING_PAUSE_CHANCE) {
        Serial.println("Taking a thinking pause...");
        delay(random(Constants::HumanBehavior::MIN_THINKING_PAUSE,
                    Constants::HumanBehavior::MAX_THINKING_PAUSE));
    }
}

void HumanSimulator::processClip(int clipNumber) {
    Serial.printf("\n=== Processing Clip %d ===\n", clipNumber);
    
    File file = SPIFFS.open("/text.txt", "r");
    if (!file) {
        Serial.println("ERROR: Failed to open text.txt");
        return;
    }
    
    Serial.println("Navigating to clip position...");
    keyboard.navigate(clipNumber == 1 ? 
                     Constants::Navigation::FIRST_CLIP_TAB_COUNT :
                     Constants::Navigation::NEXT_CLIP_TAB_COUNT);
    
    if (hardware.isPaused()) {
        file.close();
        return;
    }
    
    String clipText = "";
    int clipCount = 0;
    bool isReadingClip = false;
    
    Serial.println("Reading clip content...");
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.indexOf("Clip #") != -1) {
            Serial.printf("Found clip marker: %s\n", line.c_str());
            if (isReadingClip) break;
            clipCount++;
            if (clipCount == clipNumber) {
                isReadingClip = true;
                continue;
            }
        } else if (isReadingClip && line.length() > 0) {
            clipText += line + "\n";
        }
    }
    
    file.close();
    
    if (clipText.length() > 0 && !hardware.isPaused()) {
        Serial.printf("Found clip content (%d characters). Starting typing...\n", clipText.length());
        typeText(clipText);
        if (!hardware.isPaused()) {
            Serial.println("Finished typing clip content");
            Serial.printf("Completed processing clip %d\n", clipNumber);
        }
    } else {
        Serial.println("WARNING: No content found for this clip");
    }
}

void HumanSimulator::countClips() {
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
            Serial.printf("Found clip: %s\n", line.c_str());
        }
    }
    
    file.close();
    Serial.printf("Found %d total clips\n", totalClips);
}