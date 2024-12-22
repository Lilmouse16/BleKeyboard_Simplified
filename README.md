# ESP32 Human-Like Typing Automation with AHT Control

## Overview
This project implements an advanced human-like typing automation system using an ESP32 microcontroller. It simulates natural typing patterns with dynamic speed control based on Average Handling Time (AHT) targets derived from video duration. The system includes features like natural pauses, typos, fatigue simulation, and real-time progress tracking.

## Features

### Core Functionality
- Bluetooth keyboard emulation
- Natural typing patterns simulation
- Dynamic speed adjustment based on AHT targets
- Real-time progress tracking
- Configurable difficulty levels

### Human-Like Behaviors
- Random typos with corrections
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

## Task Format
Tasks are defined in text.txt following this format:

```plaintext
Video [video_id]

Clip #[number] <start_time> - <end_time>
[Main description of the scene]

<start_time> - <end_time>
[Action description]
```

Time Format: `<MM:SS.mmm>`
- MM: minutes (00-99)
- SS: seconds (00-59)
- mmm: milliseconds (000-999)

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

## Configuration

### Constants (`constants.h`)
```cpp
namespace Constants {
    namespace Typing {
        const int BASE_WPM = 65;        // Base typing speed
        const float MIN_SPEED_MULTIPLIER = 0.5f;
        const float MAX_SPEED_MULTIPLIER = 2.0f;
    }

    namespace HumanBehavior {
        const float TYPO_CHANCE = 0.15f;
        const float FATIGUE_FACTOR = 0.05f;
        // ... other behavior settings
    }
}
```

## Usage

### Initial Setup
1. Clone repository
2. Install PlatformIO in VS Code
3. Configure `platformio.ini`
4. Connect hardware components
5. Upload `text.txt` to SPIFFS

### Serial Commands
- `d X.XX` - Set difficulty multiplier (0.50 to 2.00)
- `s` - Show current status
- `r` - Reset current clip

### LED Patterns
- Both OFF: Standby
- Alternating: Active typing
- Both ON: Connected, ready
- Red ON: Paused
- Blue ON: Waiting for connection

### Button Controls
- Single press: Start/Pause
- Long press: Reset current section

## Performance Monitoring

### Status Information
The system provides real-time status updates:
- Current clip progress
- Typing speed
- AHT compliance
- Estimated completion time

### Debug Output
Serial monitor provides detailed information:
- Connection status
- Typing events
- Error corrections
- Progress updates

## Future Enhancements
- Multiple typing personality profiles
- Network configuration interface
- Advanced progress analytics
- Remote control capabilities
- Extended status reporting

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
