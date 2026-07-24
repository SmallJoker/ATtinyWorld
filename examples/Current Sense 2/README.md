# Current Sense circuit V2

## This DIY discrete circuit

This circuit was created and simulated in [Qucs-S](https://github.com/ra3xdh/qucs_s), using [ngspice](https://sourceforge.net/projects/ngspice/).

Schematic: [Current Sense 2.png](Current%20Sense%202.png)

### Features

* Improved linearity across input voltages
* Adjustable current threshold
* Discrete circuit for experimenting.

Use-cases:

* "Digital" current sense of constant voltage outputs.
* Battery charging using a DC/DC converter (Feedback pin)

### Testing Results / Component Choice

1. `R4` should prevent damaging the (often) lower voltage tolerant Feedback pin of
   the DC/DC converter in case of failure.
2. `Q1` and `Q2` are of different types to highlight that they are not a matched pair.
   However, a low difference in h<sub>fe</sub> is recommended, also to ensure similar V<sub>eb</sub>.
3. `R6` should be tuned to the desired threshold.
4. `Q3` and `Q4` improve linearity of the current amplifier. Can be of any type.

## Proper industrial solutions

If you are looking for a more reliable solution, here's several chips:

* https://www.ti.com/amplifier-circuit/current-sense/overview.html
* https://www.allegromicro.com/en/products/sense/current-sensor-ics
