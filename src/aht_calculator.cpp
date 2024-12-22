#include "aht_calculator.h"

// Define the static constexpr array
constexpr AHTCalculator::AHTPoint AHTCalculator::AHT_POINTS[];

AHTCalculator::AHTCalculator() : 
    difficultyMultiplier(1.0f),  // Start with easiest difficulty
    totalDuration(0.0f),
    targetAHT(0.0f),
    lowerBoundAHT(0.0f),
    upperBoundAHT(0.0f) {
}

bool AHTCalculator::init() {
    if (!SPIFFS.begin(true)) {
        Serial.println("ERROR: SPIFFS Mount Failed");
        return false;
    }
    
    extractClipTimes();
    totalDuration = calculateTotalDuration();
    calculateAHTBounds();
    
    Serial.printf("AHTCalculator initialized. Total duration: %.3f seconds\n", totalDuration);
    return true;
}

void AHTCalculator::setDifficultyMultiplier(float multiplier) {
    // Clamp multiplier between 1.0 and 7.0
    difficultyMultiplier = std::max(1.0f, std::min(7.0f, multiplier));
    calculateAHTBounds();
    Serial.printf("Difficulty multiplier set to: %.2f\n", difficultyMultiplier);
    
    // Print expected completion times for feedback
    float hours = (targetAHT * difficultyMultiplier) / 60.0f;
    Serial.printf("Expected completion time at difficulty %.2f: %.1f hours\n", 
                 difficultyMultiplier, hours);
}

float AHTCalculator::calculateTotalDuration() {
    if (clipTimeRanges.empty()) {
        return 0.0f;
    }
    
    // Calculate total duration from first start to last end
    float firstStart = clipTimeRanges.front().startTime;
    float lastEnd = clipTimeRanges.back().endTime;
    
    return lastEnd - firstStart;
}

float AHTCalculator::getTargetAHT() {
    return targetAHT * difficultyMultiplier;
}

float AHTCalculator::getLowerBoundAHT() {
    return lowerBoundAHT * difficultyMultiplier;
}

float AHTCalculator::getUpperBoundAHT() {
    return upperBoundAHT * difficultyMultiplier;
}

std::vector<float> AHTCalculator::getClipDurations() {
    std::vector<float> durations;
    for (const auto& range : clipTimeRanges) {
        durations.push_back(range.endTime - range.startTime);
    }
    return durations;
}

void AHTCalculator::calculateAHTBounds() {
    if (totalDuration <= 0) return;
    
    targetAHT = interpolateAHT(totalDuration, false, false);
    lowerBoundAHT = interpolateAHT(totalDuration, true, false);
    upperBoundAHT = interpolateAHT(totalDuration, false, true);
    
    Serial.printf("AHT Bounds calculated - Lower: %.2f minutes (%.2f hours), Target: %.2f minutes (%.2f hours), Upper: %.2f minutes (%.2f hours)\n",
                 getLowerBoundAHT(), getLowerBoundAHT()/60.0f,
                 getTargetAHT(), getTargetAHT()/60.0f,
                 getUpperBoundAHT(), getUpperBoundAHT()/60.0f);
}

float AHTCalculator::interpolateAHT(float duration, bool isLower, bool isUpper) {
    // Find the two points to interpolate between
    size_t numPoints = sizeof(AHT_POINTS) / sizeof(AHTPoint);
    size_t i = 0;
    
    while (i < numPoints - 1 && AHT_POINTS[i + 1].duration <= duration) {
        i++;
    }
    
    if (i >= numPoints - 1) {
        i = numPoints - 2;
    }
    
    const AHTPoint& p1 = AHT_POINTS[i];
    const AHTPoint& p2 = AHT_POINTS[i + 1];
    
    float v1 = isLower ? p1.lowerBound : (isUpper ? p1.upperBound : p1.target);
    float v2 = isLower ? p2.lowerBound : (isUpper ? p2.upperBound : p2.target);
    
    // Linear interpolation
    float t = (duration - p1.duration) / (p2.duration - p1.duration);
    return v1 + t * (v2 - v1);
}

bool AHTCalculator::parseTimeString(const String& timeStr, float& seconds) {
    // Remove any spaces from the time string
    String cleanTimeStr = "";
    for (char c : timeStr) {
        if (c != ' ') {
            cleanTimeStr += c;
        }
    }
    
    char buf[10];
    cleanTimeStr.toCharArray(buf, sizeof(buf));
    
    // Expected format: "MM:SS.mmm"
    int minutes = atoi(buf);
    int secs = atoi(buf + 3);
    int ms = atoi(buf + 6);
    
    seconds = minutes * 60.0f + secs + ms / 1000.0f;
    return true;
}

void AHTCalculator::extractClipTimes() {
    File file = SPIFFS.open("/text.txt", "r");
    if (!file) {
        Serial.println("Failed to open text.txt");
        return;
    }
    
    clipTimeRanges.clear();
    String line;
    bool foundClipStart = false;
    float clipStartTime = 0.0f;
    float clipEndTime = 0.0f;
    
    while (file.available()) {
        line = file.readStringUntil('\n');
        line.trim();
        
        // Look for clip headers
        if (line.startsWith("Clip #")) {
            // If we already found a clip start, save the previous clip
            if (foundClipStart) {
                TimeRange range = {clipStartTime, clipEndTime};
                clipTimeRanges.push_back(range);
            }
            
            // Extract times from clip header
            int startPos = line.indexOf("<");
            int endPos = line.lastIndexOf(">");
            if (startPos > 0 && endPos > startPos) {
                // Find the hyphen that separates start and end times
                int hyphenPos = line.indexOf("-", startPos);
                if (hyphenPos > 0 && hyphenPos < endPos) {
                    String startTimeStr = line.substring(startPos + 1, line.indexOf(">", startPos));
                    String endTimeStr = line.substring(line.lastIndexOf("<") + 1, endPos);
                    
                    if (parseTimeString(startTimeStr, clipStartTime) && 
                        parseTimeString(endTimeStr, clipEndTime)) {
                        foundClipStart = true;
                    }
                }
            }
        }
    }
    
    // Don't forget to add the last clip
    if (foundClipStart) {
        TimeRange range = {clipStartTime, clipEndTime};
        clipTimeRanges.push_back(range);
    }
    
    file.close();
    Serial.printf("Extracted %d clip time ranges from text.txt\n", clipTimeRanges.size());
}