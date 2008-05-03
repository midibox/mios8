# $Id$

# define the processor here
PROCESSOR = 18f452

# default flags to GPASM here
GPASM_FLAGS   = 

# include pathes (more will be added by asm_only.mk files)
GPASM_INCLUDE = 

# optional defines that should be passed to GPASM
GPASM_DEFINES = 

# default rule that defines the .hex files which should be created
# note: always require a "cleanall", since dependencies (e.g. on .h and .inc files) are not properly declared
all: cleanall main.hex device_id_00.hex device_id_01.hex device_id_02.hex device_id_03.hex iic_midi_10.hex iic_midi_12.hex iic_midi_14.hex iic_midi_16.hex

# include the asm_only.mk file
include $(MIOS_PATH)/include/makefile/asm_only.mk

# include application specific LCD driver (dummy driver by default)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk


# additional make rules to assemble variants with predefined ID
device_id_00.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DMIOS_DEVICE_ID=0x00 -o device_id_00.hex main.asm
device_id_01.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DMIOS_DEVICE_ID=0x01 -o device_id_01.hex main.asm
device_id_02.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DMIOS_DEVICE_ID=0x02 -o device_id_02.hex main.asm
device_id_03.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DMIOS_DEVICE_ID=0x03 -o device_id_03.hex main.asm

iic_midi_10.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DIIC_MIDI_ADDR=0x10 -o iic_midi_10.hex main.asm
iic_midi_12.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DIIC_MIDI_ADDR=0x12 -o iic_midi_12.hex main.asm
iic_midi_14.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DIIC_MIDI_ADDR=0x14 -o iic_midi_14.hex main.asm
iic_midi_16.hex: main.asm
	gpasm $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -DIIC_MIDI_ADDR=0x16 -o iic_midi_16.hex main.asm

clean:
	rm -f *.cod *.lst *.err

cleanall: clean
	rm -f *.hex
