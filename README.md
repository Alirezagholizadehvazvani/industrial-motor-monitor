# Industrial Motor Monitor

**ESP32-Based Prototype for Three-Phase Pump Motor Monitoring and Protection**

A personal engineering prototype exploring low-cost monitoring and protective control for three-phase industrial pump motors. The design uses an ESP32, current and voltage sensing modules, a display, and a relay interface to detect selected electrical faults and signal a protective trip.

The project is intended to demonstrate embedded systems, sensor acquisition, fault-detection logic, and integration concepts relevant to industrial pump protection. It is **not a certified motor-protection device** and must not be relied upon as the sole means of protecting equipment or people.

## Project Overview

Three-phase pump motors can be affected by abnormal current and voltage conditions. Monitoring these conditions can help identify potentially damaging operation and provide an additional layer of protection.

This project explores a prototype that:

- Monitors current on three phases using SCT-013 current transformers.
- Measures phase voltages using ZMPT101B voltage-sensing modules.
- Uses an ESP32 to process sensor readings and evaluate implemented protection conditions.
- Displays operating and trip status on an OLED screen.
- Provides LED status indication and a manual reset input.
- Uses a relay interface intended to interrupt the motor starter's contactor-coil control circuit when a configured fault is detected.

The prototype is designed to complement conventional motor-protection equipment, not replace it.

## Scope and Permission

This is a personal engineering project developed alongside my experience at Jahan Pump Co. Permission was granted to publish the prototype's hardware and assembly material, monitoring and control concepts, wiring illustrations, photographs, and engineering documentation.

The company's polished commercial design and other proprietary material are outside the scope of this repository. This repository describes a personal prototype and should not be interpreted as an official or certified Jahan Pump product.

## Implemented and Planned Protection

The distinction between implemented features and future work is important: listing a fault as a possible motor hazard does not mean the current firmware detects it.

| Condition or feature | Status |
|---|---|
| Three-phase current measurement | Implemented in the prototype |
| Phase-voltage measurement | Implemented in the prototype |
| Overcurrent detection | Implemented protection logic |
| Under/overvoltage detection | Implemented protection logic |
| OLED operating and trip indications | Implemented |
| LED status indication | Implemented |
| Manual reset input | Implemented |
| Relay output for the contactor-coil control circuit | Prototype interface implemented |
| Phase-loss detection | Not implemented as a validated protection function |
| Phase-imbalance detection | Not implemented as a validated protection function |
| Measured end-to-end fault trip time | Not yet established by a documented timing test |
| Certified protection performance | Outside the project's scope |

Implementation status describes the current prototype, not a guarantee of correct operation under all electrical or environmental conditions. Thresholds, sensor calibration, measurement accuracy, and response time require appropriate verification before any operational use.

## Hardware

The prototype uses the following main components:

| Component | Purpose |
|---|---|
| ESP32-WROOM-32 | Main controller |
| ADS1115 16-bit ADC | External analog-to-digital conversion for current-sensing channels |
| 3 × SCT-013 current transformers | Three-phase current measurement |
| 3 × ZMPT101B voltage-sensing modules | Phase-voltage sensing |
| 0.96-inch SSD1306 OLED, I²C | Status display |
| 5 V opto-isolated relay module | Control-interface output |
| Momentary push button | Manual reset input |
| Status LEDs | Visual indication |

The exact measurement accuracy and suitability of each module depend on its configuration, calibration, electrical isolation, and operating conditions. Component specifications alone do not establish the safety or accuracy of the complete system.

See [`docs/hardware.md`](docs/hardware.md) for additional hardware information and [`docs/assembly-notes.md`](docs/assembly-notes.md) for the available assembly documentation.

## Firmware and Response Time

The firmware acquires sensor readings, evaluates the implemented protection conditions, updates status indications, and manages the prototype's trip and reset behavior.

The configured startup delay is **5 seconds**. This is a startup behavior, not a measured fault-response time.

The firmware performs multiple ADC reads and uses a median-based sampling process, followed by a delay between processing passes. Consequently, actual fault-response time depends on the complete execution path and fault conditions. **End-to-end trip time has not yet been measured and documented**, so this README makes no claim that the system trips within a specific number of electrical cycles or within a verified time limit.

A timing result should be added only after a controlled test records the interval between fault injection and the corresponding trip output, including the test conditions and measurement method.

## Repository Structure

```text
industrial-motor-monitor/
├── README.md
├── LICENSE
├── docs/
│   ├── hardware.md
│   ├── assembly-notes.md
│   ├── safety.md
│   ├── challenges/
│   ├── Pump_Protection_Assembly_Integration_Report.pdf
│   ├── Pump_Protection_Progress_Report.pdf
│   ├── Pump_Protection_Project_Status.pdf
│   ├── Pump_Protection_Shopping_List.pdf
│   └── schematics/
│       ├── ESP32 Mains Monitoring Circuit Schematic.png
│       └── panel-illustration.svg
├── firmware/
│   ├── esp32jp/
│   │   └── esp32jp.ino
│   ├── src/
│   │   └── main.cpp
│   └── platformio.ini
└── hardware/
    ├── photos/
    └── bom/
```

The repository includes firmware, hardware documentation, photographs, bill
