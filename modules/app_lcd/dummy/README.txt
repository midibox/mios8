$Id$

Driver dummy for custom LCDs (selected with LCD type #7)

In distance to other app_lcd drivers, this one only provides "empty hooks"
to ensure, that MIOS won't crash if LCD type #7 is selected in PIC ID Header

Accordingly, this dummy should be included into an application, whenever 
no special app_lcd module is used. The user will be able to change the
driver by selecting a different module in the Makefile


Files
~~~~~

  app_lcd.inc: can be included into "asm-only" programs (non-relocatable code)
  app_lcd.asm: relocatable wrapper for C based applications
  app_lcd.mk:  include this into Makefile to integrate the driver


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "app_lcd.o" to OBJS variable

  2) Makefile: include app_lcd.mk

---
# include application specific driver (select app_lcd/dummy if MIOS internal driver used)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk
---

  3) main.c: ensure that MIOS_LCD_TypeSet() not used in Init() hook, so that
     LCD is selected from PIC ID header



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): ensure, that the application includes 
     the global <app_lcd.inc> instead of "app_lcd.inc"

  2) Makefile: include app_lcd.mk

---
# include application specific driver (select app_lcd/dummy if MIOS internal driver used)
include $(MIOS_PATH)/modules/app_lcd/dummy/app_lcd.mk
---

  3) main.asm (or main.inc): ensure that MIOS_LCD_TypeSet not called from 
     USER_Init hook, so that LCD is selected from PIC ID header
