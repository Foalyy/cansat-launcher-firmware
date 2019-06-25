## libtungsten project compilation config
# Remember to do 'make clean' whenever you change anything in this Makefile

NAME=cansat_launcher

#BOOTLOADER=true
DEBUG=true

# Available modules : adc dac eic gloc i2c spi tc trng usart
# Some modules such as gpio and flash are already compiled by default
# and must not be added here.
MODULES=i2c tc adc spi

# Available utils modules : RingBuffer Servo
UTILS_MODULES=

# User-defined modules to compile with your project
# Example :
# USER_MODULES=\
#     leds
#     drivers/SDCard \
#     drivers/GPS
USER_MODULES=\
	drivers/leds \
	drivers/servo \
	drivers/altimeter

# Path to the toolchain. If the tools (such as arm-none-eabi-g++) are
# not in your default $PATH, you MUST define their location here.
# (don't forget the trailing slash)
TOOLCHAIN_PATH=/opt/arm-none-eabi/bin/

# Include the main lib makefile
include libtungsten/Makefile
