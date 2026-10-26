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
  attempt. The exact target bias voltage and detailed measurements
  before and after the repair were not recorded.

- **Current transformers:** The SCT-013 sensors were the most difficult
  part of the assembly. Getting their connections and signal conditioning
  correct was challenging, and the current channels were susceptible to
  noise.

## Verification and recorded observations

The prototype was compared with separate measuring instruments during
bench testing. The records below are the monitor readings reported from
the bandsaw test; the bandsaw nameplate specifies 1.3 kW, 400 V, and
50 Hz. Nameplate values are specifications, not measured electrical
consumption.

### Voltage comparison

| Instrument | Observed reading |
|------------|------------------|
| Multimeter | 235 V |
| Monitor | 235 V, occasionally 233–234 V |

This was a reported bench comparison. It was not a full calibration
across the operating range or a formal uncertainty measurement.

### Bandsaw current observations

The monitored machine was a BMSY 230 DG bandsaw.

| Operating condition | L1 | L2 | L3 |
|----------------------|---:|---:|---:|
| Powered, saw not running | 1.6 A | 1.6 A | 1.5 A |
| Running unloaded | 3.2 A | 3.3 A | 3.3 A |
| Cutting under load | 7.2 A | 7.1 A | 7.2 A |

The powered-but-not-running readings reportedly fluctuated around
1.4–1.6 A. During unloaded operation, fluctuations were around 0.2 A.

The monitor was compared with a separate clamp meter, and the readings
were remembered as being very close. Exact clamp-meter readings were
not retained, so a numerical error or accuracy percentage cannot be
calculated. The current calibration factor was adjusted during testing;
the current firmware value is `CURRENT_CAL = 1.0`. The earlier value
was remembered as 0.85, but this is historical information rather than
a verified calibration result.

### Test scope and limitations

- Bench observations were made with the bandsaw in the operating
  conditions listed above.
- A voltage reading was compared with a multimeter.
- Current readings were compared qualitatively with a clamp meter, but
  exact reference readings were not retained.
- The attempted welder test was abandoned because of excessive noise.
- With the saw powered but not running, all three phases read about
  1.5-1.6 A. That is above the firmware noise floor of 0.15 A. The
  cause (noise, offset or a real load) has not been established.
- These tests do not establish the protection system's trip accuracy,
  fail-safe behavior, long-term reliability, or suitability for a real
  pump panel.
- The prototype has not been validated on the real pump installation.

## What I would do differently

- Replace the breadboard arrangement with a properly designed,
  insulated, enclosed assembly before any site evaluation.
- Improve current-sensor signal conditioning and wiring layout to
  reduce noise and make assembly less error-prone.
- Document and verify the exact bias voltage and the current and
  voltage calibration procedures.
- Retain reference-instrument readings during tests so that measurement
  differences can be calculated rather than recalled qualitatively.
- Use documented, suitable input-protection components and verify their
  suitability as part of an engineering review.
- Record the exact ESP32 pin fault and repair details during future
  troubleshooting.
