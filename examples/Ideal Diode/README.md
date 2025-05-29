# Almost Ideal Diode

## This DIY discrete circuit

This circuit was created and simulated in [Qucs-S](https://github.com/ra3xdh/qucs_s), using [ngspice](https://sourceforge.net/projects/ngspice/). Real-world results can be found below.

Schematic: [Almost ideal diode.png](Almost ideal diode.png)

### Features

* Lower active power consumption compared to a regular Si diode.
* "Active" indicator, 50 µA output that does not influence the
  diode characteristic if short-circuited.
* Discrete circuit for experimenting.

### Testing Conditions

The characteristics were evaluated using the following components:

* Q3: TJ15S06M3L (footprint: TO-252/DPAK)
* 100 mil perfboard: 7 * 9 (18 mm * 25 mm)
* Laboratory PSU
* Various power resistors for load simulation

### Static Characteristics

Assuming V<sub>in</sub> = 10 V unless otherwise stated.

| Characteristic                         | Min.| Typ.| Max.| Unit|
|----------------------------------------|-----|-----|-----|-----|
| Input or output voltage                | 5   | -   | 30 ¹| V   |
| Idle current consumption               |     | 200 |     | µA  |
| Forward voltage drop (5 V to 20 V)     | 25  | -   | 90  | mV  |
| Forward voltage drop (5 mA, -, 700 mA) | 40  | -   | 70  | mV  |

**Notes:**

1. The maximum voltage depends on V<sub>DSS</sub> of the P-MOSFET `Q3` and
   V<sub>CEO</sub> of the NPN `Q1` of your choice.

### Testing Results / Component Choice

1. The breadboard that I used has shown remarkable line resistances of approx.
   500 mΩ over 40 mm and 6 contact surfaces. If in doubt: Use dedicated wiring
   for currents above 100 mA.
2. `Q3`: Ensure the P-MOSFET can switch to relatively low R<sub>DSon</sub>
   values when V<sub>in</sub> ≃ 5 V, thus V<sub>SG</sub> ≃ 4.2 V.
3. `Rc`: The circuit will drift depending on temperature. This resistor adds a
   safety margin, at the cost of higher forward voltage drops. The additional
   voltage drop is roughly proportional to V<sub>in</sub>.
4. The idle forward voltage drop should be >= 20 mV to prevent reverse leakage 
   current.
5. `Q1`: Testing has shown that a BJT results in a better linearity as
   compared to the combination of a diode (e.g. 1N4148) + 1.8 kΩ resistor.
6. `Q1`: Reverse operation is intended. Do not flip. The simulation would
   work incorrectly because the BJT model used in SPICE simulators
   [does not simulate breakdown characteristics](https://github.com/ra3xdh/qucs_s/issues/1376#issuecomment-2895031027).


## Proper industrial solutions

If you are looking for a more reliable solution, here's a chip:

* https://www.ti.com/lit/ds/symlink/lm74700-q1.pdf

All-in-one Ideal Diode boards can be found on AliExpress at a very low price.
