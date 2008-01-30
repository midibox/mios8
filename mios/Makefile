# $Id$

# define the processor here
PROCESSOR = p18f452

# default flags to GPASM here
GPASM_FLAGS   = 

# include pathes (more will be added by asm_only.mk files)
GPASM_INCLUDE = 

# optional defines that should be passed to GPASM
GPASM_DEFINES = 

# default rule that defines the .hex files which should be created
# note: always require a "cleanall", since dependencies (e.g. on .h and .inc files) are not properly declared

all: cleanall mios_p18f452.hex mios_p18f4620.hex mios_p18f4520.hex mios_p18f4682.hex mios_p18f4685.hex

mios_p18f452.hex: main.asm
	gpasm $< $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -p 18f452 -o mios_p18f452.hex

mios_p18f4520.hex: main.asm
	gpasm $< $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -p 18f4520 -o mios_p18f4520.hex

mios_p18f4620.hex: main.asm
	gpasm $< $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -p 18f4620 -o mios_p18f4620.hex

mios_p18f4682.hex: main.asm
	gpasm $< $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -p 18f4682 -o mios_p18f4682.hex

mios_p18f4685.hex: main.asm
	gpasm $< $(GPASM_FLAGS) $(GPASM_INCLUDE) $(GPASM_DEFINES) -p 18f4685 -o mios_p18f4685.hex


# include the asm_only.mk file
include $(MIOS_PATH)/include/makefile/asm_only.mk
