# pv-storinator

This project is about powering a Single Board Computer (SBC) from a photovoltaic (PV) panel.
The hardware documented in this repository was not tested against any safety standards.

**Use-cases:**

* Weather logging station
* Minimalist game or web server
* ???

**Used applications and blueprints:**

* Schematics: [KiCad 9.0.1](https://www.kicad.org/)
* Diagrams: [draw.io](https://app.diagrams.net/)
* From the *examples* directory of this repository:
    * Hardware: Current Sense, Ideal Diode
    * Software: PWM ADC, USART RS232 (WIP)
* DC/DC converter: recycled boards

## Project progress

* &#10004; MCU state machine
* &#10004; MCU pin mapping
* &#10004; Schematic
* &#10004; Layout
* Firmware implementation
* Dry run testing
* Real-world testing

## Requirements

* PV/Input voltage: 0 V ... 25 V
* PV/Input current: 0 A ... 1.5 A
* Battery undervoltage protection
* Battery overvoltage protection
* Constant Current (CC) charging
* Maximum Power Point Tracking (MPPT) for the PV panel
