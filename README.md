# Industrial Motor Monitor

This is the next phase of the [Motor Health Monitor](https://github.com/Alirezagholizadehvazvani/motor-health-monitor) project.

In Stage A, the detection logic and architecture were developed and validated in pure software simulation.  
This repository focuses on moving that same logic onto real hardware for industrial pump protection.

## Goal

Build a practical monitoring system that can detect:

- Phase imbalance / single-phasing
- Dry-running conditions

on real 3-phase motors, using ESP32 and current sensors.

## Relation to Stage A

The previous repository established:

- A clean Hardware Abstraction Layer
- Fault detection logic
- Simulation environment

This phase keeps the same software architecture and replaces the simulated sensor with real current measurements.

## Planned Components

- ESP32
- SCT-013 current sensors
- Signal conditioning
- MQTT communication
- Simple monitoring dashboard

## Current Status

This repository is in the early stage of hardware integration and system design.

More details, code, and documentation will be added as the work progresses.

## License

MIT
