// industrial-motor-monitor: firmware placeholder
//
// STATUS: The working firmware (protection logic, trip/reset state machine,
// OLED status display) exists on a separate device and has not yet been
// transferred to this repository. This file intentionally contains no code
// so the repo does not claim logic that has not been re-verified here.
//
// TODO: import the verified firmware from the other device and commit it
//       as a separate, clearly labelled commit.
//
// Hardware map (verified on breadboard, see docs/hardware.md):
//   Voltage sensors (ZMPT101B x3) : ESP32 ADC GPIO35 / GPIO32 / GPIO33
//   Current sensors (SCT-013 x3)  : ADS1115 @ I2C 0x48, AIN0 / AIN1 / AIN2
//   OLED SSD1306 0.96"            : SDA = GPIO21, SCL = GPIO22
//   Relay (opto-isolated)         : GPIO25
//   Reset button                  : GPIO26
//
// Protection parameters of the existing firmware (to be re-confirmed
// against the transferred source, not yet validated on the real panel):
//   Over-current threshold : 32 A
//   Voltage window         : 180 V to 250 V
//   Startup inrush delay   : 4 s
