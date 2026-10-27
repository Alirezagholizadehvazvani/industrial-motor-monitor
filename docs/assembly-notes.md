# Assembly notes

These notes describe the breadboard prototype, its assembly challenges,
and the observations made during bench testing. They distinguish
reported test observations from formal calibration or safety validation.

## Build order

Each stage was brought up before the next one was added.

1. Power from a fused 5 V rail supplied by an ATX supply.
2. SSD1306 OLED display over I2C.
3. Opto-isolated relay module controlled by GPIO25.
4. Three ZMPT101B voltage-sensing modules.
5. Three SCT-013 current sensors, with burden resistors and bias-divider
   circuits feeding the ADS1115.

## Problem areas

- **Noise:** Noise affected current monitoring, particularly with
  electrically noisy equipment operating nearby. Shortening wiring,
  moving the prototype farther from the equipment under test, and
  switching off noisy heavy machinery such as welding machines reduced
  interference. The attempted welder test was abandoned because the
  noise was excessive; it did not establish successful operation with
  a welder.

- **Pin fault:** One ESP32-side pin was faulty. It was repaired
  by soldering. The exact pin and the original electrical fault were
  not recorded.

- **Current-channel bias point:** The current-sensing circuit used two
  10 kΩ resistors as a bias divider. The first assembly was incorrect
  and produced highly noisy current readings, described at the time as
  being in the 52 A range. The second attempt was still incorrect
  because the return wire to the ESP32 was missing, so no usable
  data could be collected. The bias circuit was corrected on the final
  attempt. The bias measured on 27 October 2026 is in `hardware.md`.
  The voltage before the repair was not recorded.

- **Current transformers:** The SCT-013 sensors were the most difficult
  part of the assembly. Getting their connections and signal conditioning
  correct was challenging, and the current channels were susceptible to
  noise.

## Verification and recorded observations

This section keeps three kinds of statement apart: measured (read from an
instrument or the display, with the date), reported (from the author's
notes, source named) and recalled (no record exists). Nothing recalled is
presented as measured.

### Retest, 27 October 2026

The committed sketch (`firmware/main.prototype/main.prototype.ino`) was
flashed on the prototype, and the OLED current readings were read in six
states. Values are L1, L2 and L3 in amperes as read from the display by the
author (reported). The saw is a BMSY 230 DG bandsaw. Its nameplate states
1.3 kW, 400 V and 50 Hz. Nameplate values are specifications, not measured
consumption.

| State | L1 | L2 | L3 |
|-------|---:|---:|---:|
| CT jack unplugged | 0 | 0 | 0 |
| Jack plugged in, CT closed around nothing, away from the saw | 0.1 | 0.1 | 0.1 |
| CT next to the saw's cable, saw unplugged | 0.1 | 0.1 | 0.1 |
| Saw plugged in, switch off | 1.5 | 1.5 | 1.6 |
| Saw running, not cutting | 3.6 | 3.5 | 3.6 |
| Saw cutting wood | 7.1 | 7.1 | 7.1 |

A clamp meter was read only in the saw-plugged-in, switched-off state and
showed 0 A (reported). No clamp-meter readings were recorded for the other
states, so no error figure can be calculated for them.

Observations:

- With the CT jack unplugged the display reads 0 A. With the jack plugged in
  and the CT around nothing it reads about 0.1 A, which is around the
  firmware's 0.15 A noise floor.
- With the saw plugged in but switched off, all three phases read about
  1.5 A on the monitor while a clamp meter read 0 A (reported). This is a
  measurement effect, not a real current. It does not appear with the saw
  unplugged. The cause is unresolved, and it is not known whether it adds
  to the readings while the saw runs.
- Running and cutting read about 3.6 A and 7.1 A. The committed sketch
  applies no offset.

### Earlier observations (earlier firmware build)

These come from an earlier firmware build that is not the committed sketch.
They are reported or recalled, and the numbers were not kept as a record.

- That build subtracted a fixed 1.5 A offset from the calculated current to
  bring the idle reading to 0. The original `CURRENT_CAL` was recalled as
  0.85. The final offset and calibration values were not retained.
- With that build the monitor and a clamp meter both showed about 3.5 A with
  the saw running without cutting, and about 7.5 A while cutting (reported).
- Voltage comparison: multimeter 235 V, monitor 235 V and occasionally
  233-234 V (reported). This is one point only, not a calibration across the
  range.
- The earlier build and the committed sketch cannot be compared directly,
  because their calibration differs.

### Test scope and limitations

- The 27 October retest used the display. A clamp meter was read only in the
  saw-plugged-in, switched-off state.
- Voltage was compared with a multimeter at one point, with the earlier build.
- The attempted welder test was abandoned because of excessive noise.
- No trip test, trip-time measurement or fail-safe test was run.
- These tests do not establish the protection system's trip accuracy,
  fail-safe behavior, long-term reliability, or suitability for a real
  pump panel.
- The prototype has not been validated on the real pump installation.

## What I would do differently

- Replace the breadboard arrangement with a properly designed,
  insulated, enclosed assembly before any site evaluation.
- Improve current-sensor signal conditioning and wiring layout to
  reduce noise and make assembly less error-prone.
- Document the current and voltage calibration procedures and verify them
  across the operating range.
- Retain reference-instrument readings during tests so that measurement
  differences can be calculated rather than recalled qualitatively.
- Use documented, suitable input-protection components and verify their
  suitability as part of an engineering review.
- Record the exact ESP32 pin fault and repair details during future
  troubleshooting.
