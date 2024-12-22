#include "hardware.h"

void Hardware::init() {
    pinMode(Constants::Hardware::BUTTON_PIN, INPUT_PULLUP);
    pinMode(Constants::Hardware::BUZZER_PIN, OUTPUT);
    pinMode(Constants::Hardware::BLUE_LED, OUTPUT);
    pinMode(Constants::Hardware::RED_LED, OUTPUT);
    
    digitalWrite(Constants::Hardware::BUZZER_PIN, LOW);
    digitalWrite(Constants::Hardware::BLUE_LED, HIGH);
    digitalWrite(Constants::Hardware::RED_LED, HIGH);
    Serial.println("Hardware initialized, Button pin: " + String(Constants::Hardware::BUTTON_PIN));
}

void Hardware::handleButton() {
    static unsigned long lastDebugTime = 0;
    int reading = digitalRead(Constants::Hardware::BUTTON_PIN);

    // Print button state every 3000ms to avoid flooding serial
    if (millis() - lastDebugTime > 3000) {
        Serial.printf("Button Reading: %d, LastState: %d, Paused: %d, DebounceTime: %lu\n", 
                     reading, lastButtonState, paused, millis() - lastDebounceTime);
        lastDebugTime = millis();
    }

    // If the button state changed, reset debounce timer
    if (reading != lastButtonState) {
        Serial.println("Button state changed!");
        lastDebounceTime = millis();
        lastButtonState = reading;  // Update last state immediately
    }

    // Check if enough time has passed for debounce
    if ((millis() - lastDebounceTime) > Constants::Hardware::DEBOUNCE_DELAY) {
        // Button is pressed (LOW in pull-up configuration)
        if (reading == LOW) {
            Serial.println("Button press confirmed after debounce!");
            
            if (sectionComplete) {
                Serial.println("Section was complete, starting next section");
                sectionComplete = false;
                paused = false;
                Serial.println("\n=== Starting Next Section ===");
            } else {
                paused = !paused;
                Serial.printf("Toggling pause state to: %d\n", paused);
                if (paused) {
                    Serial.println("\n=== PAUSED ===");
                    Serial.println("Press button to resume");
                } else {
                    Serial.println("\n=== RESUMING ===");
                }
            }
            
            // Reset debounce timer after handling the press
            lastDebounceTime = millis();
        }
    }
}

void Hardware::setLedPattern(Pattern pattern) {
    unsigned long currentMillis = millis();
    
    switch(pattern) {
        case Pattern::ALL_OFF:
            digitalWrite(Constants::Hardware::BLUE_LED, HIGH);
            digitalWrite(Constants::Hardware::RED_LED, HIGH);
            break;
            
        case Pattern::ALL_ON:
            digitalWrite(Constants::Hardware::BLUE_LED, LOW);
            digitalWrite(Constants::Hardware::RED_LED, LOW);
            break;
            
        case Pattern::ALTERNATING:
            if (currentMillis - lastLedToggle >= Constants::Hardware::DATA_FLICKER_SPEED) {
                ledState = !ledState;
                digitalWrite(Constants::Hardware::BLUE_LED, ledState ? HIGH : LOW);
                digitalWrite(Constants::Hardware::RED_LED, ledState ? LOW : HIGH);
                lastLedToggle = currentMillis;
            }
            break;
            
        case Pattern::RED_ONLY:
            digitalWrite(Constants::Hardware::BLUE_LED, HIGH);
            digitalWrite(Constants::Hardware::RED_LED, LOW);
            break;
            
        case Pattern::SYNC_FLASH:
            if (currentMillis - lastLedToggle >= Constants::Hardware::COMPLETE_FLICKER_SPEED) {
                ledState = !ledState;
                digitalWrite(Constants::Hardware::BLUE_LED, ledState ? LOW : HIGH);
                digitalWrite(Constants::Hardware::RED_LED, ledState ? LOW : HIGH);
                lastLedToggle = currentMillis;
            }
            break;
            
        case Pattern::BLUE_ONLY:
            digitalWrite(Constants::Hardware::BLUE_LED, LOW);
            digitalWrite(Constants::Hardware::RED_LED, HIGH);
            break;
    }
}

void Hardware::playSound(SoundType type) {
    if (!Constants::Hardware::BUZZER_ENABLED) return;
    
    int duration = 0;
    switch(type) {
        case SoundType::SECTION_COMPLETE:
            duration = Constants::Hardware::SECTION_COMPLETE_BEEP;
            break;
        case SoundType::ERROR:
            duration = 100;
            break;
        case SoundType::SUCCESS:
            duration = 50;
            break;
    }

    Serial.println("Playing sound...");
    digitalWrite(Constants::Hardware::BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(Constants::Hardware::BUZZER_PIN, LOW);
}

bool Hardware::isButtonPressed() {
    return digitalRead(Constants::Hardware::BUTTON_PIN) == LOW;
}