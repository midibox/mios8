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
all: cleanall setup_pic18f452_mbfm_tk.hex setup_pic18f452_mbfm_v1.hex setup_pic18f4685_mbfm_tk.hex setup_pic18f4685_mbfm_v1.hex setup_pic18f4685_sammich_fm.hex

# special rules for compiling PIC18F4685 variants
setup_pic18f4685_mbfm_v1.hex:
	gpasm  -p p18f4685  -I./src -I $(MIOS_PATH)/include/asm -I $(MIOS_PATH)/include/share -I $(MIOS_PATH)/modules/app_lcd/dummy -I $(MIOS_PATH)/modules/aout   setup_pic18f4685_mbfm_v1.asm
setup_pic18f4685_mbfm_tk.hex:
	gpasm  -p p18f4685  -I./src -I $(MIOS_PATH)/include/asm -I $(MIOS_PATH)/include/share -I $(MIOS_PATH)/modules/app_lcd/dummy -I $(MIOS_PATH)/modules/aout   setup_pic18f4685_mbfm_tk.asm
setup_pic18f4685_sammich_fm.hex:
	gpasm  -p p18f4685  -I./src -I $(MIOS_PATH)/include/asm -I $(MIOS_PATH)/include/share -I $(MIOS_PATH)/modules/app_lcd/dummy -I $(MIOS_PATH)/modules/aout   setup_pic18f4685_sammich_fm.asm


# include the asm_only.mk file
include $(MIOS_PATH)/include/makefile/asm_only.mk

# include application specific LCD driver (dummy driver by default)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk

# include AOUT driver
include $(MIOS_PATH)/modules/aout/aout.mk
