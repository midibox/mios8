# $Id$

# define the processor, linker file and project name
PROCESSOR = 18f452
LKR_FILE  = $(MIOS_PATH)/etc/lkr/p$(PROCESSOR).lkr
PROJECT   = project

# list of objects that should be created and linked
OBJS = mios_wrapper.o app_lcd.o main.o blm_scalar.o

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

# include Button/Duo-LED driver for BLM_SCALAR
# only DIN part is used here, and only 4 matrices
# DOUT matrices for LEDs disabled
# see $MIOS_PATH/modules/blm/blm.inc for default assignments
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_GREEN1=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_GREEN2=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_GREEN3=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_GREEN4=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_GREEN5=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_RED1=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_RED2=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_RED3=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_RED4=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_RED5=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_CATHODES1=1
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_CATHODES2=2
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_CATHODES3=3
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_CATHODES4=4
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DOUT_CATHODES5=0
BLM_SCALAR_DEFINES += -DBLM_SCALAR_CATHODES_INV_MASK=0x00
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DIN1=1
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DIN2=2
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DIN3=3
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DIN4=4
BLM_SCALAR_DEFINES += -DBLM_SCALAR_DIN5=5
BLM_SCALAR_DEFINES += -DBLM_SCALAR_NO_DEBOUNCING=1
include $(MIOS_PATH)/modules/blm_scalar/blm_scalar.mk
