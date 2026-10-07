# Assembly notes

Breadboard prototype. These notes record the build order, the problem
areas, and how the result was checked. Detail marked TODO has not been
written down yet.

## Build order

Each stage was brought up before the next one was added.

1. Power: fused 5 V rail from an ATX supply
2. OLED display (SSD1306, I2C)
3. Relay (opto-isolated, GPIO25)
4. Voltage sensors (ZMPT101B x3)
5. Current sensors (SCT-013 x3, through burden resistors and bias
   dividers into the ADS1115)

## Problem areas

- **Noise.** TODO: which signals were affected (voltage channels,
  current channels, or both), what it looked like, and what reduced it.
- **Pin issues.** TODO: which pins, what went wrong, and how it was
  resolved.
- **Bias point on the current channels.** The SCT-013 output is AC, so
  it has to be centred within the ADS1115 input range with a bias
  divider. Getting this bias point right was one of the problems of the
  build. TODO: the target bias voltage, the divider values used, and
  the symptom before the fix.

## Verification

- Multimeter measurements on the bench.
- Live tests of the sensing and protection behaviour.

TODO: what each measurement was compared against (a multimeter reading,
a known load, a clamp meter), the values seen, and the observed
difference between the readings. Also state what the live tests
covered and what they did not. Per safety.md, the system has not yet
been validated on the real pump panel.

## What I would do differently

Build it again with better equipment and better components. TODO: name
the specific parts or instruments that limited the result, for example
the breadboard, the sensor modules, or the measuring tools.
