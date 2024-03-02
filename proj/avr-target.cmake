# Avoid -rdynamic error upon compiler test
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(AVR_GCC     avr-gcc     REQUIRED)
find_program(AVR_GPP     avr-g++     REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_SIZE    avr-size    REQUIRED)
find_program(AVRDUDE     avrdude     REQUIRED)

set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER   "${AVR_GCC}")
set(CMAKE_CXX_COMPILER "${AVR_GPP}")
set(CMAKE_BUILD_TYPE Release) # nothing else is supported so far

function(setup_avr_target TARGET_NAME)
	# Compiler: https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
	#  -> Optimizations: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
	# Linker: https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
	set_target_properties(
		"${TARGET_NAME}.elf"
		PROPERTIES
			COMPILE_FlAGS "<--UNUSED-->"
			# Maybe: -Wl,-Map=${TARGET_NAME}.map
			LINK_FLAGS "-mmcu=${TYPE_GCC} -Wall -Os -Wl,--gc-sections -mrelax -mtiny-stack"
	)

	add_compile_definitions(${TYPE_MACRO})

	add_custom_target("${TARGET_NAME}.hex"
		COMMAND
			${AVR_OBJCOPY} -O ihex "${TARGET_NAME}.elf" "${TARGET_NAME}.hex"
		COMMAND # Show the program usage (code + RAM)
			${AVR_SIZE} -C --mcu=${TYPE_GCC} "${TARGET_NAME}.elf" | grep "% "
		DEPENDS "${TARGET_NAME}.elf"
	)

	# make TARGET_NAME_flash: .elf -> .hex -> avrdude
	add_custom_target("${TARGET_NAME}_flash"
		COMMAND
			${AVRDUDE} -V ${TYPE_AVRDUDE} -B 8 -U flash:w:"${TARGET_NAME}.hex"
		DEPENDS "${TARGET_NAME}.hex"
	)
endfunction()
