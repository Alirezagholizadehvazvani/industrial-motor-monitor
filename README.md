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
| Three-phase current measurement | Implemented in the prototype. Calibration against a reference is not yet documented |
| Phase-voltage measurement | Implemented in the prototype. The scale factor is not yet calibrated; see Known Issues |
| Overcurrent detection | Implemented in code. Limit is 150% of a per-pump full-load current set in the source |
| Under/overvoltage detection | Implemented in code, not verified. Levels depend on the uncalibrated scale |
| OLED operating and trip indications | Implemented, including the trip cause |
| LED status indication | Implemented |
| Manual reset input | Implemented |
| Relay output for the contactor-coil control circuit | Prototype interface implemented. The relay stays open until start-up checks pass, and a watchdog is armed |
| Phase-loss detection | Implemented in code; thresholds not validated |
| Phase-imbalance detection | Implemented in code; threshold not validated |
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

See - [Requirements Specification (PDF)](docs/requirements-specification.pdf) for additional hardware information and [`docs/assembly-notes.md`](docs/assembly-notes.md) for the available assembly documentation.

## Firmware and Response Time

The firmware acquires sensor readings, evaluates the implemented protection conditions, updates status indications, and manages the prototype's trip and reset behavior.

The startup window is **5 seconds**. It opens when the firmware detects a motor start (a phase current above 0.5 A after the motor was off) and inhibits the overcurrent and imbalance checks only. It is a startup behavior, not a measured fault-response time.

The overcurrent limit is 150% of `PUMP_FLA`, the pump's full-load current, which is set in the source for each pump (the committed value is 10 A, so 15 A). The voltage window is 150 V to 280 V, phase loss is a phase below 80 V while another is present, and imbalance is a phase current more than 25% from the average. Voltage and current values pass through a median over three passes. An overcurrent condition must persist for 500 ms. The voltage scale factor is still marked "calibrate later" in the code, so these voltage levels have not been validated.

On every trip, the cause is shown on the OLED and written to the serial port with a timestamp. The relay is held open until the display and ADC initialise, and an 8-second watchdog is armed after that.

Each processing pass reads the ADS1115 current channels repeatedly. By inspection of the code, a trip needs several passes of at least 0.3 seconds each, so the response is of the order of a second or more. **End-to-end trip time has not yet been measured and documented**, so this README makes no claim that the system trips within a specific number of electrical cycles or within a verified time limit.

A timing result should be added only after a controlled test records the interval between fault injection and the corresponding trip output, including the test conditions and measurement method.

## Requirements Specification

The requirements specification documents the intended requirements and scope of the prototype.

- [Requirements Specification (PDF)](docs/requirements-specification.pdf)

The document should be read alongside the implementation and safety notes. A requirement's presence in the specification does not, by itself, demonstrate that the current prototype satisfies it.

## Repository Structure

```text
industrial-motor-monitor/
├── README.md
├── LICENSE
├── docs/
│   ├── hardware.md
│   ├── assembly-notes.md
│   ├── safety.md
│   ├── requirements-specification.pdf
│   ├── challenges/
│   ├── Pump_Protection_Assembly_Integration_Report.pdf
│   ├── Pump_Protection_Progress_Report.pdf
│   ├── Pump_Protection_Project_Status.pdf
│   ├── Pump_Protection_Shopping_List.pdf
│   └── schematics/
│       ├── monitor-circuit.svg
│       ├── monitor-circuit.png
│       └── panel-illustration.svg
├── firmware/
│   ├── main.prototype/
│   │   └── main.prototype.ino
│   └── platformio.ini
└── hardware/
    └── photos/
```

The repository includes firmware, hardware documentation, photographs, schematics and illustrations, a requirements specification, and early project reports. A bill of materials has not been written yet.

## Schematics

![Monitor circuit](docs/schematics/monitor-circuit.png)

`docs/schematics/monitor-circuit.svg` is the editable source. Both schematics in this repository are illustrations drawn for the project, not company documents. Assumptions that still need confirming against the build are marked `[A]` in the drawing. `panel-illustration.svg` shows a typical DOL starter panel with the monitor's connection points.

## Known Issues and Limitations

- The voltage-sensing scale factor has not yet been calibrated against a suitable reference.
- Current and voltage measurement accuracy has not yet been documented through reference-based testing.
- Protection thresholds have not all been validated experimentally.
- End-to-end fault detection and trip response time have not yet been measured and documented.
- The firmware and hardware have not been certified as a motor-protection system.
- The `platformio.ini` file is currently a placeholder; a complete PlatformIO build configuration and dependency list have not yet been established.

This repository documents a personal engineering prototype and its development status. It does not establish certified protective performance or suitability for deployment in an industrial installation.
