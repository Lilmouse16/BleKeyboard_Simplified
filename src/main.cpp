#include "hardware.h"
#include "keyboard.h"
#include "human_simulator.h"
#include "aht_calculator.h"
#include "timing_manager.h"

Hardware hardware;
Keyboard keyboard;
HumanSimulator simulator(keyboard, hardware);

int currentClip = 1;
bool connectionAnnounced = false;
float taskDifficulty = Constants::HumanBehavior::DEFAULT_DIFFICULTY;

// Function prototypes
void handleSerialCommands();
void displayStatus();
void setDifficulty(float difficulty);

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== ESP32 Human-like Typer Starting ===");
    
    hardware.init();
    keyboard.init();
    
    if (!SPIFFS.begin(true)) {
        Serial.println("ERROR: SPIFFS Mount Failed");
        return;
    }
    
    if (!simulator.init()) {
        Serial.println("ERROR: Simulator initialization failed");
        return;
    }
    
    Serial.println("\nAvailable commands:");
    Serial.println("d X.XX - Set difficulty multiplier (0.50 to 2.00)");
    Serial.println("s - Show status");
    Serial.println("r - Reset current clip");
    Serial.println("\nReady! Press button to start/pause/resume");
}

void loop() {
    if (keyboard.isConnected()) {
        if (!connectionAnnounced) {
            Serial.println("\n=== Bluetooth Connected ===");
            connectionAnnounced = true;
            hardware.setLedPattern(Hardware::Pattern::ALL_ON);
        }
        
        handleSerialCommands();
        hardware.handleButton();
        
        // Status update at defined interval
        static unsigned long lastStatusTime = 0;
        if (millis() - lastStatusTime > Constants::AHT::STATUS_UPDATE_INTERVAL) {
            displayStatus();
            lastStatusTime = millis();
        }
        
        if (!hardware.isPaused() && !hardware.isSectionComplete()) {
            Serial.println("Starting to process clip...");
            hardware.setLedPattern(Hardware::Pattern::ALTERNATING);
            simulator.processClip(currentClip);
            
            // Only set section complete if we haven't been paused
            if (!hardware.isPaused()) {
                hardware.setSectionComplete(true);
                hardware.playSound(Hardware::SoundType::SECTION_COMPLETE);
                Serial.printf("Completed processing clip %d\n", currentClip);
                currentClip++;
                
                // Display completion status
                displayStatus();
            }
        } else if (hardware.isPaused() && !hardware.isSectionComplete()) {
            hardware.setLedPattern(Hardware::Pattern::RED_ONLY);
        } else if (hardware.isSectionComplete()) {
            hardware.setLedPattern(Hardware::Pattern::SYNC_FLASH);
        }
        
        // Check if all clips are completed
        if (currentClip > simulator.getTotalClips()) {
            Serial.println("\n=== All Clips Completed ===");
            displayStatus();
            hardware.setLedPattern(Hardware::Pattern::ALL_ON);
            while(1) delay(1000);  // Stop processing
        }

    } else {
        // Not connected to Bluetooth
        connectionAnnounced = false;
        hardware.setSectionComplete(false);
        hardware.setLedPattern(Hardware::Pattern::BLUE_ONLY);
        Serial.println("Waiting for Bluetooth connection...");
        delay(1000);
    }
}

void handleSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("d ")) {
            // Set difficulty command
            float newDifficulty = command.substring(2).toFloat();
            setDifficulty(newDifficulty);
        }
        else if (command == "s") {
            // Status command
            displayStatus();
        }
        else if (command == "r") {
            // Reset command
            if (hardware.isPaused()) {
                Serial.println("Resetting current clip...");
                hardware.setSectionComplete(false);
                displayStatus();
            } else {
                Serial.println("Pause typing first to reset");
            }
        }
        else {
            Serial.println("Unknown command. Available commands:");
            Serial.println("d X.XX - Set difficulty (0.50 to 2.00)");
            Serial.println("s - Show status");
            Serial.println("r - Reset current clip");
        }
    }
}

void displayStatus() {
    float progress = simulator.getCurrentProgress() * 100.0f;
    float timeRemaining = simulator.getEstimatedTimeRemaining();
    
    Serial.println("\n=== Status ===");
    Serial.printf("Current Clip: %d/%d\n", currentClip, simulator.getTotalClips());
    Serial.printf("Progress: %.1f%%\n", progress);
    Serial.printf("Difficulty: %.2fx\n", taskDifficulty);
    
    // Convert and display time in hours and minutes
    int hours = (int)(timeRemaining / 60.0f);
    int minutes = (int)timeRemaining % 60;
    Serial.printf("Est. Remaining: %dh %dm\n", hours, minutes);
    
    Serial.printf("State: %s\n", 
                 hardware.isPaused() ? "PAUSED" : 
                 (hardware.isSectionComplete() ? "COMPLETE" : "RUNNING"));
                 
    // Add completion estimate if running
    if (!hardware.isPaused() && !hardware.isSectionComplete()) {
        unsigned long currentTime = millis();
        time_t estimatedCompletion = time(nullptr) + (time_t)(timeRemaining * 60);
        struct tm* timeinfo = localtime(&estimatedCompletion);
        Serial.printf("Est. Completion: %02d:%02d:%02d\n", 
                     timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
}

void setDifficulty(float difficulty) {
    // Clamp difficulty between valid ranges
    taskDifficulty = constrain(difficulty, 
                             Constants::HumanBehavior::MIN_DIFFICULTY_MULTIPLIER,
                             Constants::HumanBehavior::MAX_DIFFICULTY_MULTIPLIER);
    
    simulator.setDifficulty(taskDifficulty);
    Serial.printf("Difficulty set to: %.2fx\n", taskDifficulty);
    displayStatus();
}