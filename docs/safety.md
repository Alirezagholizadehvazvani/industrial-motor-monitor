# Safety

This is a prototype. It is not a certified protection device and must
not be relied on as the only protection for a pump or for people.

## Current status

- Built and tested on a breadboard on the bench.
- Not yet validated on the real pump panel at Jahan Pump Co.
- The protection thresholds (over-current 32 A, voltage window
  180-250 V, 4 s startup delay) have not been checked against the real
  inrush current or the pump nameplate data.
- Fail-safe behaviour (what the relay does if the ESP32 hangs, resets,
  or loses power) and watchdog behaviour have not been verified. This
  is listed as a risk in the requirements specification.

## Existing protection stays in place

The installation already has a Hyundai HGC 25 contactor and an
HMMS-32K manual motor starter. This system is intended to add
monitoring and a trip signal alongside them, not to replace them. Their
overload and short-circuit protection must remain connected and
functional.

## Mains voltage hazards

- The pump circuit is 3-phase mains and can be lethal.
- The ZMPT101B voltage sensors and the SCT-013 current sensors are
  connected to or around live conductors. The breadboard layout does
  not provide the insulation, clearances, or enclosure required for
  permanent installation.
- Input protection on the sensor channels is currently missing (listed
  as a risk in the requirements specification).
- The relay module is opto-isolated from the ESP32. The isolation
  rating of the relay contacts and wiring has not been documented here
  yet.

## Rules for working on this project

- Work on mains wiring only with the supply isolated and verified dead.
- Do not connect the prototype to the real panel until the open items
  below are resolved and the connection has been reviewed by a
  qualified electrician at the site.
- Do not run the prototype unattended on live mains from a breadboard.
- SCT-013 current-output variants must never be left open-circuit while
  current flows in the monitored conductor, because the secondary can
  develop a dangerous voltage. TODO: confirm which variant is in use
  (see hardware.md).

## Open items before connecting to the real panel

- [ ] Confirm thresholds against pump nameplate data and measured inrush
- [ ] Verify fail-safe relay behaviour and add a watchdog
- [ ] Add input protection on sensor channels
- [ ] Measure the ADS1115 sample rate and confirm it is adequate
- [ ] Move from breadboard to a properly insulated, enclosed build
- [ ] Review of the installation by a qualified person
