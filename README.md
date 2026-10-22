# Industrial Motor Monitor

**Prototype 3-phase pump motor protection system**

Low-cost ESP32-based monitoring and protection unit designed for industrial submersible drainage pumps (Jahan Pump TYPE 4 series, 10–30 A range). The system measures voltage and current on all three phases and opens the contactor coil circuit when a dangerous condition is detected. A manual reset is required after every trip.

> **Scope & Permission**  
> This repository documents the personal prototype I developed while working at Jahan Pump Co.  
> I have explicit permission to publish:
> - The prototype hardware and assembly process  
> - Power circuit, control circuit, and analyzing circuit  
> - Wiring diagrams and photographs of the prototype  
> - The full engineering process, including challenges and iterations  
>
> A more polished version was later delivered to the company. That version remains proprietary and is **not** included here.  
> This public repository represents only the open prototype phase.

---

## Problem

In industrial pump installations, motors are frequently damaged by:

- Over-current / locked rotor
- Under-voltage and over-voltage
- Phase loss or severe phase imbalance
- Prolonged abnormal operating conditions

Traditional thermal overload relays react relatively slowly. This prototype adds a fast electronic protection layer that can detect problems within a few cycles and cut power before serious damage occurs.

---

## Key Features

- 3-phase current monitoring (SCT-013 current transformers)
- 3-phase voltage monitoring (ZMPT101B modules)
- Over-current protection
- Over-voltage / under-voltage protection
- 4-second startup delay to ignore motor inrush current
- Status on OLED display (OK / STARTING / TRIPPED)
- Status LEDs
- Manual reset button
- Dry-contact relay output that interrupts the contactor coil
- Designed to work **alongside** existing DOL starters (not as a replacement)

---

## Hardware Overview

| Function              | Component                          | Notes                                      |
|-----------------------|------------------------------------|--------------------------------------------|
| Controller            | ESP32-WROOM-32                     | Dual-core, Wi-Fi, I2C                      |
| Precision ADC         | ADS1115                            | 16-bit, used for current channels          |
| Current sensors       | 3 × SCT-013 (100 A / 50 mA)        | Split-core + burden + bias circuit         |
| Voltage sensors       | 3 × ZMPT101B                       | Connected to ESP32 ADC                     |
| Display               | 0.96" SSD1306 OLED                 | I2C                                        |
| Actuation             | 5 V opto-isolated relay module     | Interrupts contactor coil                  |
| User input            | Momentary push button              | Manual trip reset                          |

---

## Current Status

| Item                        | State                                      |
|-----------------------------|--------------------------------------------|
| Breadboard hardware         | Built and bench-tested                     |
| Firmware                    | Written (import in progress)               |
| Real panel validation       | Not yet performed                          |
| Documentation               | In progress                                |
| Challenges & lessons learned| Being documented                           |

---

## Repository Structure

```text
industrial-motor-monitor/
├── README.md
├── docs/
│   ├── hardware.md
│   ├── assembly-notes.md
│   ├── safety.md
│   ├── challenges/              ← detailed build challenges and solutions
│   └── schematics/              ← permitted circuits only
├── firmware/
├── hardware/
│   ├── photos/                  ← chronological build photos
│   └── bom/
└── LICENSE
```

---

## Safety Notes

- This system is an **additional** protection layer. It does **not** replace the existing thermal overload relay or manual motor starter.
- The relay must only interrupt the **contactor coil** circuit (low current control circuit), never the main power path.
- All high-voltage work must be performed with proper lock-out / tag-out procedures.
- This is a prototype, not a certified protection device.

---

## License

MIT
