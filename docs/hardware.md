# Hardware

Status: breadboard prototype, verified working on the bench. Not yet
validated on the real pump panel. See [safety.md](safety.md).

## Target equipment

The system is intended to protect a 3-phase submersible drainage pump
installation at Jahan Pump Co.:

| Item | Detail |
|------|--------|
| Contactor | Hyundai HGC 25 |
| Manual motor starter | HMMS-32K |
| Pumps | TYPE 4 submersible drainage pumps, 10-30 A |

## Bill of materials (as built)

| Qty | Part | Role |
|-----|------|------|
| 1 | ESP32-WROOM-32 | Controller |
| 3 | ZMPT101B | AC voltage sensing, one per phase |
| 3 | SCT-013 | Current sensing, one per phase |
| 1 | ADS1115 (16-bit ADC) | Digitises the SCT-013 signals |
| 1 | SSD1306 0.96" OLED | Status display |
| 1 | Opto-isolated relay module | Drives the contactor coil circuit |
| 1 | Push button | Trip reset |
| - | Status LEDs | Indication |
| 1 | ATX supply, fused 5 V rail | Power |

## Pin map

| Function | Connection |
|----------|-----------|
| ZMPT101B voltage sensors | ESP32 GPIO35, GPIO32, GPIO33 |
| SCT-013 current sensors | ADS1115 AIN0, AIN1, AIN2 |
| ADS1115 | I2C address 0x48 |
| OLED SSD1306 | SDA = GPIO21, SCL = GPIO22 |
| Relay | GPIO25 |
| Reset button | GPIO26 |

TODO: record which sensor channel corresponds to which phase (L1/L2/L3)
for both the voltage and current channels.

Note: GPIO32, 33 and 35 are ADC1 pins on the ESP32, which is the
usual choice when ADC2 may be contended by other peripherals.

## Current sensing chain

SCT-013 -> burden resistor -> bias divider -> ADS1115 input.

- TODO: SCT-013 variant (current-output or voltage-output) and rating
- TODO: burden resistor value
- TODO: bias divider values
- TODO: ADS1115 gain setting and sample rate (the sample rate has not
  been measured yet; it is listed as a risk in the requirements
  specification)

## Voltage sensing chain

ZMPT101B module -> ESP32 ADC input.

- TODO: calibration method and constants
- TODO: input protection (missing; listed as a risk in the
  requirements specification)

## Output stage

An opto-isolated relay on GPIO25 is wired toward the contactor coil.

- TODO: state whether the relay is connected to the real contactor
  yet or only to a bench load
- TODO: coil voltage and relay contact rating in use

## Power

Powered from a fused 5 V rail of an ATX supply.

- TODO: fuse rating

## Not yet documented

- Schematic (to be added under hardware/ once drawn)
- Photos of the breadboard build (hardware/photos/)
