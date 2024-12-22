#include "timing_manager.h"

TimingManager::TimingManager(AHTCalculator& ahtCalc) 
    : ahtCalculator(ahtCalc),
      startTime(0),
      targetDuration(0),
      speedMultiplier(1.0f),
      progressRatio(0.0f),
      estimatedTimeRemaining(0.0f) {
}

void TimingManager::init() {
    startTime = millis();
    targetDuration = ahtCalculator.getTargetAHT() * 60 * 1000; // Convert minutes to milliseconds
    speedMultiplier = 1.0f;
    progressHistory.clear();
    Serial.printf("Timing Manager initialized. Target Duration: %.2f minutes (%.2f hours)\n", 
                 targetDuration/60000.0f, targetDuration/3600000.0f);
}

float TimingManager::getSpeedMultiplier() {
    calculateSpeedMultiplier();
    return speedMultiplier;
}

void TimingManager::updateProgress(float charactersTyped, float totalCharacters) {
    if (totalCharacters <= 0) return;
    
    float currentProgress = charactersTyped / totalCharacters;
    updateProgressHistory(currentProgress);
    
    unsigned long elapsedTime = millis() - startTime;
    progressRatio = (float)elapsedTime / targetDuration;
    
    // Calculate estimated time remaining in minutes
    float progressRate = calculateProgressRate();
    if (progressRate > 0) {
        estimatedTimeRemaining = ((1.0f - currentProgress) / progressRate) * 60.0f; // Convert to minutes
    }
    
    calculateSpeedMultiplier();
    
    // Debug output every 10 seconds
    if (elapsedTime % Constants::AHT::STATUS_UPDATE_INTERVAL == 0) {
        Serial.printf("Progress: %.1f%%, Time: %.1f/%.1f min, Speed: %.2fx, Est.Remaining: %.1f min\n",
                     currentProgress * 100.0f,
                     elapsedTime / 60000.0f,
                     targetDuration / 60000.0f,
                     speedMultiplier,
                     estimatedTimeRemaining);
    }
}

TimingManager::TypingMetrics TimingManager::getCurrentMetrics() const {
    TypingMetrics metrics;
    metrics.targetWPM = calculateTargetWPM();
    metrics.currentProgress = progressRatio;
    metrics.estimatedCompletion = estimatedTimeRemaining;
    metrics.needsSpeedAdjustment = abs(1.0f - progressRatio) > Constants::AHT::PROGRESS_THRESHOLD;
    return metrics;
}

int TimingManager::calculateCharacterDelay() const {
    int baseDelay = Constants::Typing::BASE_CHAR_DELAY;
    return (int)(baseDelay / speedMultiplier);
}

int TimingManager::calculateWordDelay() const {
    int baseDelay = Constants::Typing::WORD_PAUSE;
    return (int)(baseDelay / speedMultiplier);
}

int TimingManager::calculateThinkingDelay() const {
    float multiplier = (float)(random(80, 120)) / 100.0f; // Add 20% random variation
    return (int)(Constants::HumanBehavior::MIN_THINKING_PAUSE * multiplier / speedMultiplier);
}

void TimingManager::resetSection() {
    init();
}

void TimingManager::calculateSpeedMultiplier() {
    // Base multiplier on progress vs time ratio
    float targetProgress = progressRatio;
    float currentProgress = progressHistory.empty() ? 0 : progressHistory.back();
    
    // Calculate desired adjustment
    float adjustment = (targetProgress - currentProgress) * Constants::AHT::ADJUSTMENT_SENSITIVITY;
    
    // Smooth the adjustment and apply limits
    speedMultiplier = std::min(Constants::Typing::MAX_SPEED_MULTIPLIER, 
                              std::max(Constants::Typing::MIN_SPEED_MULTIPLIER, 
                                     1.0f + adjustment));
}

float TimingManager::calculateTargetWPM() const {
    return Constants::Typing::BASE_WPM * speedMultiplier;
}

void TimingManager::updateProgressHistory(float progress) {
    progressHistory.push_back(progress);
    if (progressHistory.size() > Constants::AHT::PROGRESS_HISTORY_SIZE) {
        progressHistory.erase(progressHistory.begin());
    }
}

float TimingManager::calculateProgressRate() const {
    if (progressHistory.size() < Constants::AHT::MIN_SAMPLES_FOR_ESTIMATION) return 0.0f;
    
    // Calculate rate based on progress history
    float progressDelta = progressHistory.back() - progressHistory.front();
    float timeSpan = (progressHistory.size() - 1) * (Constants::AHT::STATUS_UPDATE_INTERVAL / 60000.0f); // Convert to minutes
    
    return timeSpan > 0 ? progressDelta / timeSpan : 0.0f;
}