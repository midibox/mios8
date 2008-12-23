# $Id: Makefile 378 2008-07-06 20:38:32Z tk $

# define the processor, linker file and project name
PROCESSOR = 18f452
LKR_FILE  = $(MIOS_PATH)/etc/lkr/p$(PROCESSOR).lkr
PROJECT   = project

# list of objects that should be created and linked
OBJS = mios_wrapper.o app_lcd.o main.o sdcard.o

# include pathes (more will be added by .mk files)
GPASM_INCLUDE = -I./
SDCC_INCLUDE  = -I./

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

# include SD Card Driver
# use following pin assignments:
SDCARD_DEFINES += -DSDCARD_LAT_CS=LATC -DSDCARD_TRIS_CS=TRISC -DSDCARD_PIN_CS=0
SDCARD_DEFINES += -DSDCARD_LAT_SCLK=LATC -DSDCARD_TRIS_SCLK=TRISC -DSDCARD_PIN_SCLK=1
SDCARD_DEFINES += -DSDCARD_LAT_DI=LATC -DSDCARD_TRIS_DI=TRISC -DSDCARD_PIN_DI=2
SDCARD_DEFINES += -DSDCARD_PORT_DO=PORTC -DSDCARD_TRIS_DO=TRISC -DSDCARD_PIN_DO=3
include $(MIOS_PATH)/modules/sdcard/sdcard.mk
