cmake_minimum_required(VERSION 3.12) # add_compile_definitions, list(TRANSFORM ...)

include(../../avr-target.cmake)

# --------------------------------

project(MCU)

set(TYPE_GCC     attiny2313)
set(TYPE_MACRO   __AVR_ATtiny2313__)
set(TYPE_AVRDUDE -c usbtiny -p t2313)

set(LIB_DIR "${CMAKE_SOURCE_DIR}/../../../lib/")

set(LIB_SRC_FILES
	USART_RS232_RX_BUF.c
	USART_RS232_TX_BUF.c
)
list(TRANSFORM LIB_SRC_FILES PREPEND ${LIB_DIR})

set(SRC_FILES
	adc.c
	main.c
)

add_executable("${PROJECT_NAME}.elf"
	${SRC_FILES}
	${LIB_SRC_FILES}
)

# /usr/lib/avr/include/
target_include_directories("${PROJECT_NAME}.elf" PUBLIC
	"${LIB_DIR}"
)

# Macros
add_compile_definitions(
	F_CPU=8000000
	USART_SPEED=384
	USART_RX_BUFFER_SIZE=0
	USART_TX_BUFFER_SIZE=0
)

setup_avr_target(${PROJECT_NAME})
