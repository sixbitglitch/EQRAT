# EQRAT-UNO
Simple Equatorial Mount RA Sidereal Tracker for Arduino UNO/328


## Overview

Minimal sidereal tracking implementation for equatorial mounts. Uses Timer1 CTC mode to generate precise step timing without blocking the main loop. Built for Skywatcher EQ3-2 with Asteron Drive Kit Light.

## Hardware

- Skywatcher EQ3-2 mount
- 0.9°/400 step NEMA 17 steppers
- DRV8825 drivers (32 microsteps)
- Arduino UNO
- CNC V3 Arduino Shield

## Implementation

Timer1 configured for CTC mode with prescaler 8. Step timing calculated from sidereal rate and gear ratios. ISR toggles step pin state every timer overflow.

Key insight: `delayMicroseconds()` overflows above 16383μs, making it unsuitable for sidereal tracking rates.

## Configuration

Modify these constants for your mount:

```cpp
const int Mount_Worm_Gear_Ratio_RA = 130;    // RA worm gear ratio
const float Motor_Gear_Ratio_RA = 2.5;       // Motor gear reduction
const int Steps_Per_Rev = 200;               // Stepper steps per revolution
const int Microstep_Setting = 32;            // Microstep setting
```

## Timing Calculations

Sidereal rate: 86164.09053 seconds per day

```
Earth_Seconds_Per_Degree = 86164.09053 / 360 = 239.34
MicroSteps_Per_Degree = (130 × 2.5 × 200 × 32) / 360 = 6933.33
Step_Delay_Microseconds = (239.34 / 6933.33) × 1000000 = 34520.8
Step_Delay_Timer_Half_Phase = 34520.8 / 2 = 17260.4
```

Timer1 OCR1A calculation (prescaler 8):
```
OCR1A = (16000000 / ((1/17260.4) × 1000000) × 8) - 1 = 34520
```

## References

- [Cloudy Nights DIY RA Drive Thread](https://www.cloudynights.com/topic/731261-yet-another-diy-ra-drive-for-eq5-exos2-eq3-etc-etc/)
- [Arduino Timer Calculator](http://www.arduinoslovakia.eu/application/timer-calculator)
