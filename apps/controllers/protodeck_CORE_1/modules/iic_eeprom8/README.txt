$Id: README.txt 415 2008-07-29 22:06:15Z tk $

Driver to access an external IIC EEPROM with 8bit addressing range

For accessing EEPROMs with 16bit address range, use the common
MIOS_BANKSTICK_* routines


Files
~~~~~

  iic_eeprom8.inc: can be included into "asm-only" programs (non-relocatable code)
  iic_eeprom8.asm: relocatable wrapper for C based applications
  iic_eeprom8.h:   C headers
  iic_eeprom8.mk:  include this into Makefile to integrate the module


Functions
~~~~~~~~~

  see function heads in iic_eeprom8.inc, and C declarations in iic_eeprom.h


Defines
~~~~~~~

  none


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "iic_eeprom8.o" to OBJS variable

  2) Makefile: include iic_eeprom8.mk
---
# include IIC_EEPROM8 driver
include $(MIOS_PATH)/modules/iic_eeprom8/iic_eeprom8.mk
---

  3) main.c: #include <iic_eeprom8.h>



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): #include <iic_eeprom8.inc>

  2) Makefile: include iic_eeprom8.mk

