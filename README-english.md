# ESP32 Human-Like Typing Automation with AHT Control

## Overview
This project implements an advanced human-like typing automation system using an ESP32 microcontroller. It simulates natural typing patterns with dynamic speed control based on Average Handling Time (AHT) targets derived from video duration. The system includes features like natural pauses, typos with corrections, fatigue simulation, and real-time progress tracking.

## Features

### Core Functionality
- Bluetooth keyboard emulation
- Natural typing patterns simulation
- Dynamic speed adjustment based on AHT targets
- Real-time progress tracking
- 7-Level difficulty system

### Human-Like Behaviors
- Random typos with automatic corrections
- Natural typing speed variations
- Fatigue simulation
- Thinking pauses
- Double space variations

### Hardware Feedback
- LED status indicators
- Button control interface
- Buzzer notifications
- Visual progress indication

## Project Structure

```plaintext
esp32-typing-automation/
├── include/
│   ├── aht_calculator.h     # AHT calculation system
│   ├── constants.h          # Project configuration constants
│   ├── hardware.h          # Hardware interface management
│   ├── human_simulator.h   # Human behavior simulation
│   ├── keyboard.h         # Bluetooth keyboard interface
│   ├── time_utils.h       # Time handling utilities
│   └── timing_manager.h   # Timing control system
├── src/
│   ├── aht_calculator.cpp  # AHT implementation
│   ├── hardware.cpp        # Hardware control implementation
│   ├── human_simulator.cpp # Behavior simulation implementation
│   ├── keyboard.cpp        # Keyboard interface implementation
│   ├── main.cpp           # Main program flow
│   ├── time_utils.cpp     # Time utilities implementation
│   └── timing_manager.cpp # Timing management implementation
├── data/
│   ├── text.txt           # Task descriptions
│   └── task_format.txt    # Task formatting guide
└── platformio.ini         # Project configuration
```

## Component Details

### AHT Calculator System
The AHT (Average Handling Time) calculator determines target typing speeds based on video duration and difficulty settings.

```cpp
class AHTCalculator {
    // Calculates required typing speed based on:
    // - Video duration
    // - Difficulty multiplier
    // - AHT graph data points
}
```

### Timing Manager
Manages typing speed and delays to meet AHT targets while maintaining natural variations.

```cpp
class TimingManager {
    // Controls:
    // - Character delays
    // - Word pauses
    // - Thinking delays
    // - Progress tracking
}
```

### Human Simulator
Implements human-like typing behaviors and manages the overall typing process.

```cpp
class HumanSimulator {
    // Handles:
    // - Typing simulation
    // - Error generation
    // - Fatigue effects
    // - Natural variations
}
```

## Difficulty System
The system features 7 difficulty levels that affect typing duration:
- 1.00x: Very Easy (Base time)
- 2.00x: Easy (2x Base time)
- 3.00x: Medium (3x Base time)
- 4.00x: Hard (4x Base time)
- 5.00x: Very Hard (5x Base time)
- 6.00x: Extreme (6x Base time)
- 7.00x: Impossible (7x Base time)

## Task Format Specification

### Basic Structure
```
Video [video_id]

Clip #[number] <start_time> - <end_time>
[Main description of the scene]

<start_time> - <end_time>
[Action description]

<start_time> - <end_time> [CM]
[Description of camera movement]

<start_time> - <end_time> [CT]
[Description of transition]
```

### Time Format
- Format: `<MM:SS.mmm>`
  - MM: minutes (00-99)
  - SS: seconds (00-59)
  - mmm: milliseconds (000-999)

### Important Rules
1. Each clip must start with "Clip #"
2. All times must include leading zeros
3. Main description follows clip header
4. [CM] denotes Camera Movement
5. [CT] denotes Camera Transition
6. Total duration = Last end time - First start time

### Validation Requirements
1. Time format must be exact
2. Clip numbers must be sequential
3. No time gaps or overlaps allowed
4. Only valid tags are [CM] and [CT]
5. All timeframes require descriptions

## AHT Calculation System

The system uses an AHT graph to determine target typing speeds:

### AHT Graph Data Points
```plaintext
Video Duration (sec) | Lower Bound | Target | Upper Bound
     5               |    30       |  47.5  |    65
    10               |    60       |   95   |   130
    15               |    90       |  142.5 |   195
    20               |   120       |  190   |   260
    25               |   150       |  237.5 |   325
    30               |   180       |  285   |   390
    35               |   210       |  332.5 |   455
    40               |   240       |  380   |   520
    45               |   270       |  427.5 |   585
```

## Hardware Setup

### Required Components
- ESP32-S3 DevKit (or ESP32-WROOM)
- LED indicators (Blue and Red)
- Buzzer (passive)
- USB Cable (data capable)
- Breadboard and jumper wires

### Pin Connections
```plaintext
Button: GPIO0 (Built-in BOOT button)
Buzzer: GPIO19
Blue LED: GPIO2 (External)
Red LED: GPIO13 (External)
```

## Usage

### Initial Setup
1. Clone repository
2. Install PlatformIO in VS Code
3. Configure `platformio.ini`
4. Connect hardware components
5. Upload `text.txt` to SPIFFS

### Serial Commands
- `d X.XX` - Set difficulty multiplier (1.00 to 7.00)
- `s` - Show current status
- `r` - Reset current clip (when paused)
- `h` - Show difficulty guide

### LED Patterns
- Both OFF: Standby
- Alternating: Active typing
- Both ON: Connected, ready
- Red ON: Paused
- Blue ON: Waiting for connection

### Button Controls
- Single press: Start/Pause/Resume
- Press when paused: Start next section

## Performance Monitoring

### Status Information
The system provides real-time status updates:
- Current clip progress
- Typing speed and difficulty level
- AHT compliance
- Estimated completion time

### Debug Output
Serial monitor provides detailed information:
- Connection status
- Typing events
- Error corrections
- Progress updates

## License
MIT License - See LICENSE file for details

## Contributing
1. Fork the repository
2. Create a feature branch
3. Submit pull request

## Troubleshooting
- Check USB connection
- Verify COM port settings
- Confirm LED connections
- Check SPIFFS file upload
- Ensure serial monitor is in the correct mode for command input