# SolderingRT1

Soldering station compatible with Weller RT tips (12V 45W), custom developed, perfect for SMD work :).
- Two independent channels
- Fully Programmable (temperatures 150-450C, preset temperatures, power limitation to use various power supplies), also serial interface and source code available.
- Runs on standard external 12V power adapter (voltage range 7-24V)
- Can run also on batteries (7-24V)

# Main Functions
- Two independednt channels
- Two preset temperatures (programmable and loaded by default at startup)
- Input voltage monitoring (future implementation UVLO for batteries)
- Duty cycle limitation (allows using small power adapters, but slow heatup time)
- USB micro port for firmware update (Arduino pro micro 5V 16MHz) and for serial monitoring /debug

# Absolute maximum ratings and other specs:

- Input voltage: 		12V (7-24V working voltage)
- Power adapter:        12V min 2A (5-6A recomended for fast heatup time)
- Output current:		10A max (no current limit)
- Barrel:		        5.5*2.1mm, center positive
- Temperatures range:	150-450 degC
- WARNING: The output current for each channel is inly limited by the tip resistance (about 2 ohm) and by the wire resistance.

