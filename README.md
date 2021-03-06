﻿### Bathroom Monkey

Bathroom monkey is a personalized toothbrush and shower timer
designed and built by Jim and Shaun Garlick.

![](https://github.com/garlick/brmonkey/blob/master/doc/P1010733_small.PNG)

The heart of bathroom monkey is the
[PIC 16F84](http://ww1.microchip.com/downloads/en/devicedoc/30430c.pdf).
OK, that's the part we had laying around.  It's actually obsolete now,
but any of the 18-pin 16F series PIC parts should be fine, provided you
review the `__CONFIG` setting in the source code, edit appropriately,
and recompile.

The PIC controls an LCD, buzzer, and has two switches as inputs.
We happened to have a 1.8432 MHz crystal oscillator on hand, but you
could just as easily use a ceramic resonator or even the internal RC clock,
again with a recompile.

![](https://github.com/garlick/brmonkey/blob/master/doc/schematic.png)

#### LCD

The LCD employed in this project uses a Hitachi HD44780 compatible]
controller chip.  It is configured in 4 byte data mode so only 7 PIC I/O
pins are consumed by the LCD.

A 1K resistor is attached between pin 3 (contrast) and pin 1 (GND) to set
the display contrast.  The value was determined experimentally by attaching
a potentiometer, adjusting the display, measuring the value and substituting
a fixed resistance.

The LCD is powered by a PIC I/O line so it can be shut down when the PIC
goes to sleep.  This is OK because the LCD only draws 3 mA max, while
the PIC can source 80 mA (on PORT A).

We didn't attempt to drive the LCD backlight as that would have required
external switching since it requires 675 mA max.

#### Buzzer

We used a Radio Shack no. 273-0059, 3.0-20VDC piezo buzzer which resonates
at 2700Hz +/-500Hz.  The black wire (GND) is connected to a parallel output
line and the red wire (+) is connected directly to the +5VDC supply, so
writing a 0 to the output line activates the buzzer.

#### Switches

Two normally open pushbuttons are connected to two inputs on the PIC.
These have 2.2K pullups in our design, although one could omit them and
use the internal pullups.  The inputs are grounded when the switches are
closed.

#### Power Supply

A recycled +5VDC, 700mA Samsung wall wart (fron an old cell phone) is
the power supply.

#### Chassis

We purchased the 1/2 inch thick, 4 inch square clear acrylic sheets for the
sides and a 1/8 inch wall, 2.5 inch diameter 5 inch long clear cast acrylic
tube for the body from TAP Plastics, Inc.  Holes were drilled in these to
mount the switches, to let the wall wart cable pass through, and (directly
behind the piezo buzzer) to let the sound out without too much attenuation.

The LCD and a perfboard were mounted on either side of a piece of 1/4 inch
thick colored plastic material we had laying around the shop.  We drilled
some holes in the plastic and used some extra wire wrap wire to loosely
attach the LCD and perfboard.  Then we slid the plastic into the cylinder
and glued on the sides on with TAP acrylic cement.
