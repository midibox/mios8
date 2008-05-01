# $Id$

# define the processor, linker file and project name
PROCESSOR = 18f452
LKR_FILE  = $(MIOS_PATH)/etc/lkr/p$(PROCESSOR).lkr
PROJECT   = project

# list of objects that should be created and linked
OBJS = mios_wrapper.o app_lcd.o main.o blm.o

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

# include application specific driver (select app_lcd/dummy if MIOS internal driver used)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk

# include Button/Duo-LED driver
# commented out defines are shift register assignments of MBSEQ V3 hardware
# see $MIOS_PATH/modules/blm/blm.inc for default assignments
#BLM_DEFINES += -DBLM_DOUT_L1=6 -DBLM_DOUT_R1=9
#BLM_DEFINES += -DBLM_DOUT_CATHODES1=5 -DBLM_DOUT_CATHODES2=8
#BLM_DEFINES += -DBLM_CATHODES_INV_MASK=0x00
#BLM_DEFINES += -DBLM_DOUT_L2=7 -DBLM_DOUT_R2=10
#BLM_DEFINES += -DBLM_DIN_L=11 -DBLM_DIN_R=12
#BLM_DEFINES += -DBLM_NO_DEBOUNCING=0
include $(MIOS_PATH)/modules/blm/blm.mk
