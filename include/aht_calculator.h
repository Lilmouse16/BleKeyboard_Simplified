#pragma once
#include <Arduino.h>
#include <vector>
#include <SPIFFS.h>

class AHTCalculator {
public:
    struct TimeRange {
        float startTime;
        float endTime;
    };

    struct AHTPoint {
        float duration;
        float lowerBound;
        float target;
        float upperBound;
    };

static constexpr AHTPoint AHT_POINTS[] = {
    // duration, lower,  target,  upper  (all values in minutes)
    {5.0f,    30.0f,    47.5f,    65.0f},     // 0:30 - 1:05
    {6.0f,    36.0f,    57.0f,    78.0f},     // 0:36 - 1:18
    {7.0f,    42.0f,    66.5f,    91.0f},     // 0:42 - 1:31
    {8.0f,    48.0f,    76.0f,    104.0f},    // 0:48 - 1:44
    {9.0f,    54.0f,    85.5f,    117.0f},    // 0:54 - 1:57
    {10.0f,   60.0f,    95.0f,    130.0f},    // 1:00 - 2:10
    {11.0f,   66.0f,    104.5f,   143.0f},    // 1:06 - 2:23
    {12.0f,   72.0f,    114.0f,   156.0f},    // 1:12 - 2:36
    {13.0f,   78.0f,    123.5f,   169.0f},    // 1:18 - 2:49
    {14.0f,   84.0f,    133.0f,   182.0f},    // 1:24 - 3:02
    {15.0f,   90.0f,    142.5f,   195.0f},    // 1:30 - 3:15
    {16.0f,   96.0f,    152.0f,   208.0f},    // 1:36 - 3:28
    {17.0f,   102.0f,   161.5f,   221.0f},    // 1:42 - 3:41
    {18.0f,   108.0f,   171.0f,   234.0f},    // 1:48 - 3:54
    {19.0f,   114.0f,   180.5f,   247.0f},    // 1:54 - 4:07
    {20.0f,   120.0f,   190.0f,   260.0f},    // 2:00 - 4:20
    {21.0f,   126.0f,   199.5f,   273.0f},    // 2:06 - 4:33
    {22.0f,   132.0f,   209.0f,   286.0f},    // 2:12 - 4:46
    {23.0f,   138.0f,   218.5f,   299.0f},    // 2:18 - 4:59
    {24.0f,   144.0f,   228.0f,   312.0f},    // 2:24 - 5:12
    {25.0f,   150.0f,   237.5f,   325.0f},    // 2:30 - 5:25
    {26.0f,   156.0f,   247.0f,   338.0f},    // 2:36 - 5:38
    {27.0f,   162.0f,   256.5f,   351.0f},    // 2:42 - 5:51
    {28.0f,   168.0f,   266.0f,   364.0f},    // 2:48 - 6:04
    {29.0f,   174.0f,   275.5f,   377.0f},    // 2:54 - 6:17
    {30.0f,   180.0f,   285.0f,   390.0f},    // 3:00 - 6:30
    {31.0f,   186.0f,   294.5f,   403.0f},    // 3:06 - 6:43
    {32.0f,   192.0f,   304.0f,   416.0f},    // 3:12 - 6:56
    {33.0f,   198.0f,   313.5f,   429.0f},    // 3:18 - 7:09
    {34.0f,   204.0f,   323.0f,   442.0f},    // 3:24 - 7:22
    {35.0f,   210.0f,   332.5f,   455.0f},    // 3:30 - 7:35
    {36.0f,   216.0f,   342.0f,   468.0f},    // 3:36 - 7:48
    {37.0f,   222.0f,   351.5f,   481.0f},    // 3:42 - 8:01
    {38.0f,   228.0f,   361.0f,   494.0f},    // 3:48 - 8:14
    {39.0f,   234.0f,   370.5f,   507.0f},    // 3:54 - 8:27
    {40.0f,   240.0f,   380.0f,   520.0f},    // 4:00 - 8:40
    {41.0f,   246.0f,   389.5f,   533.0f},    // 4:06 - 8:53
    {42.0f,   252.0f,   399.0f,   546.0f},    // 4:12 - 9:06
    {43.0f,   258.0f,   408.5f,   559.0f},    // 4:18 - 9:19
    {44.0f,   264.0f,   418.0f,   572.0f},    // 4:24 - 9:32
    {45.0f,   270.0f,   427.5f,   585.0f}     // 4:30 - 9:45
};

    AHTCalculator();
    
    // Initialize the calculator with the task file
    bool init();
    
    // Set the difficulty multiplier (0.5 to 2.0)
    void setDifficultyMultiplier(float multiplier);
    
    // Calculate total video duration from text file
    float calculateTotalDuration();
    
    // Get the target AHT based on duration and difficulty
    float getTargetAHT();
    
    // Get upper and lower AHT bounds
    float getLowerBoundAHT();
    float getUpperBoundAHT();
    
    // Get individual clip durations
    std::vector<float> getClipDurations();

private:
    float difficultyMultiplier;
    float totalDuration;
    float targetAHT;
    float lowerBoundAHT;
    float upperBoundAHT;
    std::vector<TimeRange> clipTimeRanges;

    // Internal calculation methods
    void calculateAHTBounds();
    float interpolateAHT(float duration, bool isLower, bool isUpper);
    bool parseTimeString(const String& timeStr, float& seconds);
    void extractClipTimes();
};