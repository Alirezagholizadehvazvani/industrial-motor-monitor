# Build challenges and lessons

Problems met while building the prototype, what caused them, what was done, and what I would take from each. Measured observations are in [`../assembly-notes.md`](../assembly-notes.md). Details I did not record are not given here.

## 1. Limited parts and budget

**Problem.** The build had to use parts already in the company or parts that were cheap to buy.
**What was done.** Decommissioned company computers were supplied for spare parts. Only their power supplies proved useful. A metal enclosure was also supplied. I stripped, cleaned and reused it.
**Lesson.** A parts and cost limit is a design requirement. It decides what the first version can do.

## 2. Getting components

**Problem.** Modules and development boards were hard to get. Local availability and prices were poor, delivery took two to three weeks, and some parts arrived faulty and had to be replaced.
**Lesson.** The schedule needs contingency for both availability and part quality.

## 3. ATX power supply

**Problem.** The ATX supply shut down after one work session unless specific jumper connections were made, because it is designed for a desktop PC.
**What was done.** I identified the correct pins and wired the jumpers permanently.

## 4. Current-sensing circuit

**Problem.** This was the first sensing circuit I built fully on my own. The first wiring diagram for current measurement was wrong and gave noisy readings, described at the time as being in the 52 A range. The second attempt was missing the return wire to the ESP32, so no usable data could be collected.
**What was done.** The bias circuit (two 10 kohm resistors) was corrected on the third attempt. The exact bias voltage was not recorded.
**Lesson.** Independent practical work shows gaps that supervised lab exercises hide. Discard a faulty design early and record the measured values after each change.

## 5. Enclosure wiring

Cable management inside the enclosure was the main task in this phase. It took one focused day.

## 6. Staged testing and interference

**Plan.** Testing on the target pumps was judged too risky at first because of the current involved, so other machines were used.
**Bandsaw.** The first test machine was a bandsaw. Current readings are in `assembly-notes.md`: about 3.2 A running unloaded and 7.2 A cutting under load. The saw also read about 1.5 A on every phase when powered but not running, which is unexplained.
**Welder.** The next test machine was a welding machine. Its electrical noise corrupted the sensor readings when it was near the prototype. About five days went into troubleshooting. Shortening wiring, moving the prototype away and switching off noisy machinery reduced the interference, but the welder test was abandoned and did not give a successful result.
**Lesson.** In a workshop with high-power machines, the monitor must be physically separated from strong noise sources.

## 7. Motor start current

**Problem.** Many motors draw several times their running current at start. A protection function that ignores this trips on every start.
**What was done.** The firmware opens a 5 second startup window when it detects a motor start. During the window the over-current and imbalance checks are inhibited. The voltage checks stay active. An earlier version timed the window from boot, which did not follow the real start.
**Limit.** The start current of a real pump has not been recorded in this repository.

## Lessons

- Parts and cost limits are design requirements.
- Component availability and quality can set the schedule.
- Electromagnetic noise is a first-order problem in an industrial workshop.
- Protection logic has to allow for normal behaviour such as start current and idle operation, not only for faults.
- Record reference readings during every test, so errors can be calculated later.
