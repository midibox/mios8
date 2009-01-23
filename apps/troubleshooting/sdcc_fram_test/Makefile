# $Id: Makefile 538 2008-11-23 13:58:59Z this $

# define the processor, linker file and project name
PROCESSOR = 18f452
LKR_FILE  = $(MIOS_PATH)/etc/lkr/p$(PROCESSOR).lkr
PROJECT   = project

# list of objects that should be created and linked
OBJS = mios_wrapper.o app_lcd.o fram.o main.o

# include pathes (more will be added by .mk files)
GPASM_INCLUDE = 
SDCC_INCLUDE  = 

# optional defines that should be passed to GPASM/SDCC
GPASM_DEFINES = -DDEBUG_MODE=0
SDCC_DEFINES  = -DDEBUG_MODE=0

FRAM_DEFINES += -DFRAM_MIOS_IIC=0

FRAM_DEFINES += -DFRAM_IIC_SLOWDOWN=0

FRAM_DEFINES += -DFRAM_MULTIPLEX_ENABLE=0

#FRAM_DEFINES += -DFRAM_IIC_PORT_SCL=PORTD
#FRAM_DEFINES += -DFRAM_IIC_TRIS_SCL=TRISD
#FRAM_DEFINES += -DFRAM_IIC_PIN_SCL=5

#FRAM_DEFINES += -DFRAM_IIC_PORT_SDA=PORTA
#FRAM_DEFINES += -DFRAM_IIC_TRIS_SDA=TRISA
#FRAM_DEFINES += -DFRAM_IIC_PIN_SDA=4



#FRAM_DEFINES += -DFRAM_MULTIPLEX_PORT_MSB=PORTD
#FRAM_DEFINES += -DFRAM_MULTIPLEX_TRIS_MSB=TRISD
#FRAM_DEFINES += -DFRAM_MULTIPLEX_PIN_MSB=5

#FRAM_DEFINES += -DFRAM_MULTIPLEX_PORT_LSB=PORTD
#FRAM_DEFINES += -DFRAM_MULTIPLEX_TRIS_LSB=TRISD
#FRAM_DEFINES += -DFRAM_MULTIPLEX_PIN_LSB=6



# pass parameters to MIOS wrapper
MIOS_WRAPPER_DEFINES = -DSTACK_HEAD=0x37f -DSTACK_IRQ_HEAD=0x33f

# directories and files that should be part of the distribution (release) package
# more will be added by *.mk files
DIST = ./


# include the common.mk file
include $(MIOS_PATH)/include/makefile/common.mk

# include application specific driver (select app_lcd/dummy if MIOS internal driver used)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk
include $(MIOS_PATH)/modules/fram/fram.mk
