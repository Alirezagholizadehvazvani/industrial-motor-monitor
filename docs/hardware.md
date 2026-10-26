# Hardware

Status: breadboard prototype, tested on the bench. It has not been
validated on the real pump panel. This is a prototype, not a certified
protection device. See [safety.md](safety.md).

## Target equipment

The system is intended to monitor and provide a trip signal for a
3-phase submersible drainage pump installation at Jahan Pump Co.

| Item | Detail |
|------|--------|
| Contactor | Hyundai HGC 25 (as recorded for the target installation) |
| Manual motor starter | HMMS-32K |
| Pumps | TYPE 4 submersible drainage pumps, 10–30 A |

The target equipment details do not establish that the prototype has
been validated with this installation.

## Bill of materials (as built)

| Qty | Part | Role |
|-----|------|------|
| 1 | ESP32-WROOM-32 | Controller |
| 3 | ZMPT101B | AC voltage sensing, one per phase |
| 3 | YHDO TM SCT-013, 100 A : 50 mA | Current sensing, one per phase |
| 1 | ADS1115, 16-bit ADC | Digitises the SCT-013 signals |
| 1 | SSD1306 0.96-inch OLED | Status display |
| 1 | Opto-isolated relay module | Controls a separate contactor-control circuit |
| 1 | Push button | Trip reset |
| - | Status LEDs | Indication |
| 1 | ATX supply, fused 5 V rail | Prototype power |

## Pin map and phase mapping

| Phase | Voltage sensor | ESP32 voltage input | Current sensor | ADS1115 input |
|-------|----------------|---------------------|----------------|---------------|
| L1 | ZMPT101B | GPIO35 | SCT-013 | AIN0 |
| L2 | ZMPT101B | GPIO32 | SCT-013 | AIN1 |
| L3 | ZMPT101B | GPIO33 | SCT-013 | AIN2 |

The physical sensor arrangement was reported to follow this phase order.
All three current channels and voltage sensing are enabled in the firmware.

| Function | Connection |
|----------|------------|
| ADS1115 | I2C address 0x48 |
| OLED SSD1306 | SDA = GPIO21, SCL = GPIO22 |
| Relay control signal | GPIO25 |
| Reset button | GPIO26 |
| Red LED | GPIO27 |
| Yellow LED | GPIO14 |
| Green LED | GPIO12 |

GPIO32, GPIO33, and GPIO35 are used for the ESP32 voltage-sensing inputs.
The ADS1115 uses I2C at 400 kHz in the firmware.

## Current sensing chain

Each SCT-013 is a current-output CT rated 100 A : 50 mA. Its signal
passes through the current-sensing circuit, including a burden resistor
and bias divider, before reaching an ADS1115 input.

- Burden resistor: 33 ohms.
- Bias divider: two 10 kΩ resistors.
- ADS1115 gain: `GAIN_ONE`.
- Configured ADS1115 data rate: `RATE_ADS1115_860SPS` (860 samples/s).
- Firmware current constants: `CT_RATIO = 2000.0`, `CT_TURNS = 1`,
  `CURRENT_CAL = 1.0`, and `NOISE_FLOOR_A = 0.15`.
- Firmware samples the enabled current channels in sequence. The
  configured ADC data rate is not an independent measurement of the
  complete system's per-channel update rate.
- The exact bias voltage was not recorded.
- The current calibration factor is a firmware setting, not evidence
  of formal calibration or verified measurement accuracy.

The CT is a current-output variant. Its secondary must not be left
open-circuit while primary current is flowing. See [safety.md](safety.md).

## Voltage sensing chain

Each ZMPT101B module feeds an ESP32 ADC input:

| Phase | ESP32 input |
|-------|-------------|
| L1 | GPIO35 |
| L2 | GPIO32 |
| L3 | GPIO33 |

The firmware uses `VOLTAGE_SCALE = 100.0`, with a code comment indicating
that calibration remains to be done. A bench comparison reportedly
showed 235 V on both the monitor and a multimeter, while the monitor
occasionally showed 233–234 V. This is an observation from that test,
not a complete calibration or accuracy specification.

No additional external input-protection components were present on the
ZMPT101B outputs in the documented prototype. Module-internal protection
has not been independently verified. Input protection remains an open
engineering requirement.

## Output stage

The opto-isolated relay module is controlled by GPIO25. It is connected
to a separate circuit that controls the contactor, rather than directly
to the contactor coil.

The firmware drives GPIO25 LOW during initialization and when a fault
is detected; it drives GPIO25 HIGH after initialization succeeds and
when a trip is reset. This describes the logic signal only. The resulting
relay-contact state and the external circuit's response must be verified.

- Relay module model and contact rating: not recorded.
- Contactor coil voltage: not recorded.
- Fail-safe behavior after loss of controller power, reset, or firmware
  failure: not verified.

Do not infer a safe contactor state from the GPIO logic alone.

## Power

The prototype is powered from an ATX supply's fused 5 V rail.
The recorded fuse rating is 1 A.

## Not yet documented

- Complete schematic and wiring details.
- Verified relay-module specifications and contactor-control interface.
- Verified input-protection design.
- Photos of the breadboard build under `hardware/photos/`.
