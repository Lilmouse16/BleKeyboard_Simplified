#include "time_utils.h"

bool TimeUtils::validateTimeFormat(const char* timeStr) {
    if (strlen(timeStr) != 9) return false;
    if (timeStr[2] != ':' || timeStr[5] != '.') return false;
    
    // Check if all other characters are digits
    for (int i = 0; i < 9; i++) {
        if (i != 2 && i != 5) {
            if (!isDigit(timeStr[i])) return false;
        }
    }
    
    return true;
}

bool TimeUtils::parseTimeString(const char* timeStr, TimeStamp& timestamp) {
    if (!validateTimeFormat(timeStr)) {
        Serial.printf("Invalid time format: %s\n", timeStr);
        return false;
    }
    
    char buffer[3];
    // Parse minutes
    strncpy(buffer, timeStr, 2);
    buffer[2] = '\0';
    timestamp.minutes = atoi(buffer);
    
    // Parse seconds
    strncpy(buffer, timeStr + 3, 2);
    buffer[2] = '\0';
    timestamp.seconds = atoi(buffer);
    
    // Parse milliseconds
    char msBuffer[4];
    strncpy(msBuffer, timeStr + 6, 3);
    msBuffer[3] = '\0';
    timestamp.milliseconds = atoi(msBuffer);
    
    // Validate ranges
    if (timestamp.seconds >= 60 || timestamp.milliseconds >= 1000) {
        Serial.println("Invalid time values");
        return false;
    }
    
    return true;
}

bool TimeUtils::parseTimeString(const char* timeStr, float& seconds) {
    TimeStamp timestamp;
    if (!parseTimeString(timeStr, timestamp)) {
        return false;
    }
    
    seconds = timestamp.toSeconds();
    return true;
}

String TimeUtils::formatSeconds(float seconds) {
    int mins = (int)(seconds / 60);
    int secs = (int)(seconds) % 60;
    int ms = (int)((seconds - (int)seconds) * 1000);
    
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", mins, secs, ms);
    return String(buffer);
}

String TimeUtils::formatTimeStamp(const TimeStamp& timestamp) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", 
            timestamp.minutes, 
            timestamp.seconds, 
            timestamp.milliseconds);
    return String(buffer);
}

float TimeUtils::calculateTimeDifference(const TimeStamp& start, const TimeStamp& end) {
    return end.toSeconds() - start.toSeconds();
}

float TimeUtils::calculateTimeDifference(float startSeconds, float endSeconds) {
    return endSeconds - startSeconds;
}

float TimeUtils::calculateProgress(float currentTime, float totalDuration) {
    if (totalDuration <= 0) return 0.0f;
    return std::min(1.0f, std::max(0.0f, currentTime / totalDuration));
}

int TimeUtils::calculateProgressPercentage(float currentTime, float totalDuration) {
    return (int)(calculateProgress(currentTime, totalDuration) * 100.0f);
}