# Safety

This is a prototype, not a certified protection device. It must not be
relied on as the only protection for a pump or for people.

## Current status

- Built and tested on a breadboard on the bench.
- Not validated on the real pump panel at Jahan Pump Co.
- Firmware settings have not been demonstrated to be appropriate for
  the intended pump's nameplate current, operating conditions, or
  starting behavior.
- The firmware currently sets `PUMP_FLA = 10.0 A`, an overcurrent
  threshold of 15 A (150% of that setting), a 500 ms overcurrent delay,
  a 150–280 V voltage window, an 80 V phase-loss threshold, a 25%
  current-imbalance threshold, and a 5 s startup period. These are
  configuration values, not validated protection settings.
- Earlier documentation recorded different voltage-window and startup
  values. The source code is the current record of firmware settings;
  the appropriate settings for the target installation remain to be
  determined and reviewed.
- The firmware configures the ADS1115 for `GAIN_ONE` and 860 SPS.
  The effective end-to-end sampling and protection response have not
  been independently validated.
- The firmware initializes an 8 s watchdog. This does not by itself
  establish that the relay and external contactor-control circuit reach
  a safe state following a hang, reset, or loss of power. Fail-safe
  behavior remains unverified.

## Existing protection stays in place

The target installation is recorded as having a Hyundai HGC 25 contactor
and an HMMS-32K manual motor starter. The prototype is intended to add
monitoring and a trip signal, not replace the installation's existing
protective devices.

Existing overload and short-circuit protection must remain connected
and functional. Any proposed integration must be reviewed by a qualified
person familiar with the actual installation.

## Mains-voltage hazards and hardware limitations

- Three-phase mains voltage can cause fatal injury.
- The ZMPT101B modules and SCT-013 sensors are associated with circuits
  connected to or placed around live conductors. The breadboard layout
  does not provide the insulation, clearances, enclosure, or verified
  protective measures required for a permanent installation.
- No additional external input-protection components were present on
  the ZMPT101B outputs in the documented prototype. Module-internal
  protection has not been independently verified.
- The relay module is a TONGLING JQC-3FF-S-Z, described as opto-isolated,
  with contacts marked 10 A 250 V AC and 15 A 125 V AC (printed markings,
  reported). Its isolation specifications and its suitability for the
  external control circuit have not been verified.
- The contactor coil voltage is recorded as 220-240 V AC for the HGC 25. It has
  not been re-verified.
- The relay controls a separate contactor-control circuit; it is not
  documented as being directly connected to the contactor coil.
- GPIO25 is driven LOW during initialization and on a detected fault,
  and HIGH when the firmware arms protection or resets a trip. The
  physical relay-contact state and the external circuit's response must
  not be inferred from these logic levels alone.
- The SCT-013 installed in the prototype is a current-output variant
  rated 100 A : 50 mA. Its secondary must not be left open-circuit while
  current is flowing in the monitored conductor.

## Rules for this project

- Do not perform live mains wiring or testing as an unsupervised
  activity. Any work involving the real installation must be handled
  and reviewed by qualified personnel using appropriate isolation,
  verification, and protective procedures.
- Do not connect the prototype to the real panel until the design,
  insulation, sensor input protection, relay interface, and fail-safe
  behavior have been reviewed and approved by a qualified electrician
  or appropriately qualified engineer.
- Do not run the prototype unattended on live mains from a breadboard.
- Do not bypass, remove, or rely on the prototype instead of the
  installation's existing protective devices.

## Open items before any site integration

- [ ] Confirm the actual pump and motor nameplate data.
- [ ] Review all protection thresholds and startup behavior against the
      actual installation and its operating requirements.
- [ ] Validate current and voltage measurement accuracy with recorded
      reference-instrument measurements.
- [ ] Confirm the ADS1115's effective sampling behavior and the system's
      protection response time.
- [ ] Design and review suitable sensor input protection.
- [ ] Verify relay module specifications, the external control interface,
      and the intended safe state after loss of power or controller fault.
- [ ] Review watchdog and fail-safe behavior as a complete system.
- [ ] Replace the breadboard with a properly insulated and enclosed
      assembly designed for the intended environment.
- [ ] Obtain review and approval from qualified personnel before site
      integration.
