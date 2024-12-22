#include "human_simulator.h"

bool HumanSimulator::init() {
    fatigueLevel = 0.0f;
    if (!ahtCalculator.init()) {
        Serial.println("Failed to initialize AHT calculator");
        return false;
    }
    
    timingManager = new TimingManager(ahtCalculator);
    timingManager->init();
    
    countClips();
    Serial.println("Human simulator initialized");
    return true;
}

void HumanSimulator::setDifficulty(float difficulty) {
    ahtCalculator.setDifficultyMultiplier(difficulty);
    timingManager->init(); // Reinitialize with new difficulty
}

void HumanSimulator::typeText(const String& text) {
    Serial.println("Starting to type text...");
    currentWord = "";
    wordsInBurst = 0;
    
    totalCharCount = text.length();
    typedCharCount = 0;
    
    for (char c : text) {
        if (hardware.isPaused()) {
            Serial.println("Typing paused");
            return;
        }

        if (c == ' ' || c == '\n') {
            if (!currentWord.isEmpty()) {
                Serial.printf("Typing word: %s\n", currentWord.c_str());
                handleTypos(currentWord);
                currentWord = "";
                wordsInBurst++;
                
                updateTypingProgress(++typedCharCount, totalCharCount);
                
                if (random(100) < Constants::HumanBehavior::DOUBLE_SPACE_CHANCE * 100) {
                    Serial.println("Adding double space");
                    keyboard.type(" ");
                    updateTypingProgress(++typedCharCount, totalCharCount);
                }
            }
            keyboard.type(String(c));
            updateTypingProgress(++typedCharCount, totalCharCount);
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
    if (hardware.isPaused()) return;

    if (random(100) < Constants::HumanBehavior::TYPO_CHANCE * 100) {
        int wordLen = word.length();
        int typoPos = random(wordLen);
        
        Serial.printf("Making typo in word at position %d\n", typoPos);
        
        // Type until the typo position
        for (int i = 0; i < typoPos; i++) {
            if (hardware.isPaused()) return;
            keyboard.type(String(word[i]));
            updateTypingProgress(++typedCharCount, totalCharCount);
            simulateTypingDelay();
        }
        
        if (hardware.isPaused()) return;
        
        // Type wrong character
        char wrongChar = 'a' + random(26);
        keyboard.type(String(wrongChar));
        updateTypingProgress(++typedCharCount, totalCharCount);
        
        // Always correct the typo
        Serial.println("Correcting typo");
        delay(timingManager->calculateWordDelay());
        keyboard.pressKey(KEY_BACKSPACE);
        updateTypingProgress(--typedCharCount, totalCharCount);
        delay(timingManager->calculateWordDelay());
        keyboard.type(String(word[typoPos]));
        updateTypingProgress(++typedCharCount, totalCharCount);
        
        // Continue with rest of word
        for (int i = typoPos + 1; i < wordLen; i++) {
            if (hardware.isPaused()) return;
            keyboard.type(String(word[i]));
            updateTypingProgress(++typedCharCount, totalCharCount);
            simulateTypingDelay();
        }
    } else {
        // Type word normally without typo
        for (char c : word) {
            if (hardware.isPaused()) return;
            keyboard.type(String(c));
            updateTypingProgress(++typedCharCount, totalCharCount);
            simulateTypingDelay();
        }
    }
}

void HumanSimulator::simulateTypingDelay() {
    if (hardware.isPaused()) return;
    
    applyFatigue();
    int adjustedDelay = timingManager->calculateCharacterDelay() * (1.0f + fatigueLevel);
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
        delay(timingManager->calculateThinkingDelay());
    }
}

void HumanSimulator::updateTypingProgress(size_t charsTyped, size_t totalChars) {
    typedCharCount = charsTyped;
    totalCharCount = totalChars;
    timingManager->updateProgress(typedCharCount, totalCharCount);
}

void HumanSimulator::processClip(int clipNumber) {
    Serial.printf("\n=== Processing Clip %d ===\n", clipNumber);
    
    File file = SPIFFS.open("/text.txt", "r");
    if (!file) {
        Serial.println("ERROR: Failed to open text.txt");
        return;
    }
    
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
    
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.indexOf("Clip #") != -1) {
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
        timingManager->resetSection();
        typeText(clipText);
    } else {
        Serial.println("WARNING: No content found for this clip");
    }
}

float HumanSimulator::getCurrentProgress() const {
    return typedCharCount / (float)totalCharCount;
}

float HumanSimulator::getEstimatedTimeRemaining() const {
    TimingManager::TypingMetrics metrics = timingManager->getCurrentMetrics();
    return metrics.estimatedCompletion;
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
        }
    }
    
    file.close();
    Serial.printf("Found %d total clips\n", totalClips);
}