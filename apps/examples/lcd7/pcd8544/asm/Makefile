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
all: cleanall main.hex


# include the asm_only.mk file
include $(MIOS_PATH)/include/makefile/asm_only.mk

# include application specific driver for PCD8544 based LCDs
include $(MIOS_PATH)/modules/app_lcd/pcd8544/app_lcd.mk

# include make rules for GLCD fonts
include $(MIOS_PATH)/modules/glcd_font/glcd_font.mk
