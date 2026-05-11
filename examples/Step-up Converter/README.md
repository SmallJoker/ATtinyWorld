# Step-up converter circuit

## This DIY discrete circuit

This circuit was created and simulated in [Qucs-S](https://github.com/ra3xdh/qucs_s), using [ngspice](https://sourceforge.net/projects/ngspice/).

It is based on a few [Joule Thief](https://en.wikipedia.org/wiki/Joule_thief) circuit designs:

* https://www.mikrocontroller.net/topic/180242
* https://www.b-kainka.de/bastel36.htm
* https://www.elektronik-labor.de/Notizen/LEDwandler1W.html

Schematic: [Step-up% Converter.png](Step-up%20Converter.png)

### Measurement results

"Power on": [Measurement result](MEAS%20Start-up%204k7Ohm%20load.png) (Input: 5 V, Load: 4.7 kOhm)

 * Channel 1: Base of switching NPN
 * Channel 2: Output voltage

Under load: [Measurement result](MEAS%20195Ohm%20load.png) (Input: 4.5 V, Load: 195 Ohm)

 * Channel 1: Base of switching NPN
 * Channel 2: Collector of the switching NPN

### Features

* Good linearity across input voltages (2 V ... 8 V)
* Custom power-on voltage given by `1.3 V / Rru * (Rru + Ri)`
* Possible LED driving at input voltage down to 0.9 V without `Rru`. (Just- how???)
* Fixed output voltage given by `D2`
* Approx. 80% efficiency at 6 V, converted to 9 V / 46 mA (414 mW).
* Quiescent current appox. 2 mA (no load, at 5 V input)

### Testing Results / Component Choice

1. Two parallel switching transistors were used to achieve a better gain.
2. The two switching transistors should have a high `hFE`, low `Vce(ON)` and
   should be of the same type.
3. The forward diode must have a very low capacity to avoid counterproductive oscillations.
   A good non-Schottky alternative is 1N4148.
4. `R2` increases efficiency slightly
5. `Cs` makes the oscillation more stable and efficient.
6. `Rfb * Cfb` determine the power throughput and hence efficiency of the circuit due
   to `Vce(ON)` losses. Higher = more power, Lower = less power.
   Note that you might need to adjust `Rs` to get a matching NPN base current.
7. Lower `L1` inductance reduces the circuit efficiency due to switching losses.
8. The overvoltage PNP circuit is more efficient (lower currents) than an NPN which
   would pull the switching transistors' bases to 0 V.

Node: The circuit may damage itself if the overvoltage protection fails or is disconnected.

## Proper industrial solutions

Many, many.
