## USART RS232 demo

Information based on `make.sh` definitions:

 * Baud Rate: 38400
 * Format: 8 data, 1 stop, even parity, no flow control

#### Requirements

* 8 MHz clock, according to the root README.md
* USBtinyISP programmer
* USART <--> Serial port converter
    * e.g. MAX232 or custom PCB


#### Execution

1. Run `make.sh && flash.sh`
2. Connect with a serial terminal
3. Send two bytes, receive three.

