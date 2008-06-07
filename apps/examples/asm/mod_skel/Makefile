# $Id: Makefile 323 2008-05-03 19:06:15Z tk $

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
all: cleanall main.hex


# include the asm_only.mk file
include $(MIOS_PATH)/include/makefile/asm_only.mk

# include application specific LCD driver (dummy driver by default)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk

# include mod_skel driver
mod_skel_DEFINE += -Dmod_skel_def=1

include $(MIOS_PATH)/modules/mod_skel/mod_skel.mk
