#!/bin/bash

avrdude -V -c usbtiny -p t2313 -B 8 -U flash:w:GENERATED.hex
