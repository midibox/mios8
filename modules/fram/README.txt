FRAM Module

This module enables you to drive the Ramtron Ferroelectrical RAM modules 
(FM24C512,FM24C256, FM24C64). The FM24C512 is treated like two FM24C256,
it's memory space is splitted into two sectors. Therefore the 512 has
just two device address pins, while FM24C256/FM24C64 have three.

So the chip select address is 3 bits for all device type, the address range
is 0x00 - 0x8000 for FM24C512 and FM24C256, 0x00 - 0x2000 for the FM24C64.

The smaller types are not compatible to this driver.

The chips can be driven with IIC@1MHZ. You can choose this option, but this
disables the sharing of a port with other IIC compatible devices (400kHZ).

Files
~~~~~

  fram.inc: can be included into "asm-only" programs (non-relocatable code)
  fram.asm: relocatable wrapper for C based applications
  fram.h:	C headers
  fram.mk:  include this into Makefile to integrate the driver
  fram_iic.inc: 1MHZ IIC - driver. will be included by fram.inc if needed


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "fram.o" to OBJS variable
  
---
# list of objects that should be created and linked
OBJS = mios_wrapper.o app_lcd.o fram.o main.o
---

  2) Makefile: include fram.mk, and enhance FRAM_DEFINES if required:

---
# include fram driver
FRAM_DEFINES += -DFRAM_MIOS_IIC=1

include $(MIOS_PATH)/modules/fram/fram.mk
---

  3) main.c: #include <fram.h>

---
  
  // do the fram thing
  fram_function();


---

  6) main.c: etc etc etc


  
  
  
"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  1a) add application specific #defines to the assembly code (e.g. in
      your main.asm file), e.g.:
---	  
#define FRAM_MIOS_IIC=1
---
	  
  1b) main.asm (or main.inc) include the module source: 

---  
#include <fram.inc>
---

  2) Makefile: include fram.mk, and enhance fram_DEFINE if required:

---
# include fram driver
FRAM_DEFINES += -DFRAM_MIOS_IIC=1

include $(MIOS_PATH)/modules/fram/fram.mk
---

  3) app_defines.inc: add the variables you can find declared in fram.asm
     Possible Range: 0x10..0x7f
	 Make sure the EQU address is unique in your app!
	 
---
;; for fram module
_fram_var	EQU	0x067
fram_int	EQU	0x068; ..0x69
---
