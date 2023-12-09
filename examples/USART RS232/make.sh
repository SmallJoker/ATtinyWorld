#!/usr/bin/env bash
set -eu -o pipefail

LIB="$PWD/../../lib"

DEFS=$(cat <<-_EOF
	-mmcu=attiny2313
	-DF_CPU=8000000

	-DUSART_SPEED=384
	-DUSART_RX_DEVICEID=0
_EOF
)

flags=$(cat <<-_EOF
	-c "$LIB/USART_RS232_RX_BUF.c"
	-c "$LIB/USART_RS232_TX_BUF.c"

	-c "$PWD/demo_rs232.c"
_EOF
)

rm -f *.o

eval avr-gcc -Os -Wall -fno-exceptions $DEFS \
	$flags \
	-I "\"$LIB\""

if [ $? -eq 0 ]; then
	avr-gcc $DEFS -o GENERATED.elf *.o
	avr-objcopy -O ihex GENERATED.elf GENERATED.hex
	#rm GENERATED.elf
	echo "### PROJECT WAS COMPILED ###"
	size=$(avr-objdump -h GENERATED.hex | grep ".sec1" | tr -s ' ' | cut -d ' ' -f 4)
	echo "Binary size: $((0x$size)) / 2048"
fi
