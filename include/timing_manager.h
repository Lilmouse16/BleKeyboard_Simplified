#pragma once
#include "aht_calculator.h"
#include "time_utils.h"
#include "constants.h"
#include <vector>

class TimingManager {
public:
    struct TypingMetrics {
        float targetWPM;
        float currentProgress;
        float estimatedCompletion;
        bool needsSpeedAdjustment;
        
        TypingMetrics() : targetWPM(0), currentProgress(0),
                         estimatedCompletion(0), needsSpeedAdjustment(false) {}
    };

    TimingManager(AHTCalculator& ahtCalc);
    
    void init();
    float getSpeedMultiplier();
    void updateProgress(float charactersTyped, float totalCharacters);
    TypingMetrics getCurrentMetrics() const;
    int calculateCharacterDelay() const;
    int calculateWordDelay() const;
    int calculateThinkingDelay() const;
    void resetSection();

private:
    AHTCalculator& ahtCalculator;
    unsigned long startTime;
    float targetDuration;
    float speedMultiplier;
    float progressRatio;
    float estimatedTimeRemaining;
    std::vector<float> progressHistory;
    
    void calculateSpeedMultiplier();
    float calculateTargetWPM() const;
    void updateProgressHistory(float progress);
    float calculateProgressRate() const;
};