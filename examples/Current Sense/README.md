# Current Sense circuit

## This DIY discrete circuit

This circuit was created and simulated in [Qucs-S](https://github.com/ra3xdh/qucs_s), using [ngspice](https://sourceforge.net/projects/ngspice/).

Schematic: [Current Sense.png](Current Sense.png)

### Features

* Adjustable current threshold
* Discrete circuit for experimenting.

Use-cases:

* "Digital" current sense of constant voltage outputs.
* Battery charging using a DC/DC converter.
   * The current setpoint depends on the input/output voltage, thus making this
     circuit not functioning well in other situations.

### Testing Results / Component Choice

1. `Rz` should prevent damaging the (often) lower voltage tolerant Feedback pin of
   the DC/DC converter in case of failure.
2. `Q1` and `Q2` are of different types to highlight that they are not a matched pair.
   However, a low difference in h<sub>fe</sub> is recommended, also to ensure similar V<sub>eb</sub>.
3. Depending on the application, `Re` may have to be as low as 0 Ω. This also depends on `Rs`.
4. `R2` should be fine-tuned to the desired voltage.

## Proper industrial solutions

If you are looking for a more reliable solution, here's several chips:

* https://www.ti.com/amplifier-circuit/current-sense/overview.html
* https://www.allegromicro.com/en/products/sense/current-sensor-ics
