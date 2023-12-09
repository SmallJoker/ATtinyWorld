## ATtiny World

A collection of libraries to make use of an ATtiny2313 MCU.

* [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2543-AVR-ATtiny2313_Datasheet.pdf)


### Setup

This repository assumes that a Debian-based distro is used. Package names may vary.

#### 1. Used programmers

* USBtinyISP
    * [Introduction](https://learn.adafruit.com/usbtinyisp/use-it)
    * [Schematic](https://cdn-learn.adafruit.com/assets/assets/000/010/326/original/usbtinyisp2sch.png)
    * [Tutorial including firmware](https://cdn-learn.adafruit.com/downloads/pdf/usbtinyisp.pdf)

#### 2. Required packages

* `gcc-avr`
* `avr-libc`
* `avrdude`

#### 3. Initial MCU fuse setup

Note: the fuse bits are inverted! 0 = selected

* Documentation chapter "Fuse Bits"
* Configuration tool: https://www.engbedded.com/fusecalc/

Example configuration:

    # 0xE4 : 8 MHz clock, slow start
    avrdude -c usbtiny -p t2313 -i 400 -U lfuse:w:0xE4:m
    # 0xDF : SPI flash enable
    avrdude -c usbtiny -p t2313 -i 400 -U hfuse:w:0xDF:m


#### Serial terminal apps (RS232)

* `cutecom`
* [hterm](http://der-hammer.info/pages/terminal.html)


### Licenses

 * Code: MIT
 * Other: CC-BY-SA 4.0


