# Industrial Motor Monitor

Bench-verified prototype. Firmware pending import. Not yet validated on the real panel.

ESP32-based protection monitor for a 3-phase submersible drainage pump at Jahan Pump Co. It measures three phase voltages and three phase currents and opens the contactor coil circuit on a fault. A trip latches until the manual reset button is pressed.

Not a certified protection device. See [docs/safety.md](docs/safety.md).

## Target equipment

| Item | Detail |
|------|--------|
| Pumps | TYPE 4 submersible drainage pumps, 10-30 A |
| Contactor | Hyundai HGC 25 |
| Manual motor starter | HMMS-32K |

The monitor is intended to work alongside the existing HGC 25 and HMMS-32K protection, not replace it.

## Hardware

| Function | Part | Connection |
|----------|------|-----------|
| Controller | ESP32-WROOM-32 | |
| Voltage sensing | 3x ZMPT101B | GPIO35, GPIO32, GPIO33 |
| Current sensing | 3x SCT-013 | ADS1115 AIN0-2, I2C 0x48 |
| Display | SSD1306 0.96 in OLED | SDA GPIO21, SCL GPIO22 |
| Trip output | Opto-isolated relay | GPIO25 |
| Reset | Push button | GPIO26 |

Details and open items: [docs/hardware.md](docs/hardware.md).

## Protection parameters

From the existing firmware. To be re-confirmed against the source once it is imported.

| Parameter | Value |
|-----------|-------|
| Over-current threshold | 32 A |
| Voltage window | 180 V to 250 V |
| Startup inrush delay | 4 s |
| Reset | Manual only, no auto-restart |

These values have not been checked against the pump nameplate or measured inrush.

## Status

| Item | State |
|------|-------|
| Breadboard hardware | Built, checked with a multimeter and live tests on the bench |
| Firmware | Written, not yet in this repository (`firmware/src/main.cpp` is a placeholder) |
| Real panel | Not connected, not validated |
| Requirements specification | Rev A draft in `docs/` |
| Open risks | Listed in the specification and in `docs/safety.md` |

## Repository

| Path | Content |
|------|---------|
| `docs/requirements-specification.pdf` | Requirements and risk register, Rev A |
| `docs/hardware.md` | Parts, pin map, sensing chains |
| `docs/assembly-notes.md` | Build order, problems, verification |
| `docs/safety.md` | Hazards, current limits, open items |
| `firmware/` | PlatformIO project (placeholder) |
| `hardware/photos/` | Build photos (to be added) |

## License

MIT
