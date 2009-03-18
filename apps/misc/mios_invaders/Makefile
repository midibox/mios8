# $Id: Makefile 311 2008-05-01 17:56:23Z tk $

# define the processor, linker file and project name
PROCESSOR = 18f4685
LKR_FILE  = p$(PROCESSOR).lkr
PROJECT   = project

# list of objects that should be created and linked
OBJS = mios_wrapper.o app_lcd.o main.o space_font_splash.o space_font.o 

# add the fonts
OBJS += debug_msg.o

# include pathes (more will be added by .mk files)
GPASM_INCLUDE = 
SDCC_INCLUDE  = 

# optional defines that should be passed to GPASM/SDCC
GPASM_DEFINES = -DDEBUG_MODE=0
SDCC_DEFINES  = -DDEBUG_MODE=0

# pass parameters to MIOS wrapper
MIOS_WRAPPER_DEFINES = -DSTACK_HEAD=0x37f -DSTACK_IRQ_HEAD=0x33f

# directories and files that should be part of the distribution (release) package
# more will be added by *.mk files
DIST = ./


# include the common.mk file
include $(MIOS_PATH)/include/makefile/common.mk

# include application specific driver for KS0107/0108 based LCDs
include $(MIOS_PATH)/modules/app_lcd/dog_g/app_lcd.mk

# include make rules for GLCD fonts
#include $(MIOS_PATH)/modules/glcd_font/glcd_font.mk

# include make rules for Debug Messages
include $(MIOS_PATH)/modules/debug_msg/debug_msg.mk
