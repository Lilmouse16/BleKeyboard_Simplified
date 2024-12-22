#pragma once
#include <Arduino.h>
#include <algorithm>
#include <ctype.h>

class TimeUtils {
public:
    struct TimeStamp {
        int minutes;
        int seconds;
        int milliseconds;
        
        float toSeconds() const {
            return minutes * 60.0f + seconds + milliseconds / 1000.0f;
        }
    };

    // Static methods
    static bool validateTimeFormat(const char* timeStr);
    static bool parseTimeString(const char* timeStr, TimeStamp& timestamp);
    static bool parseTimeString(const char* timeStr, float& seconds);
    static String formatSeconds(float seconds);
    static String formatTimeStamp(const TimeStamp& timestamp);
    static float calculateTimeDifference(const TimeStamp& start, const TimeStamp& end);
    static float calculateTimeDifference(float startSeconds, float endSeconds);
    static float calculateProgress(float currentTime, float totalDuration);
    static int calculateProgressPercentage(float currentTime, float totalDuration);

private:
    static bool isDigit(char c) { return c >= '0' && c <= '9'; }
};