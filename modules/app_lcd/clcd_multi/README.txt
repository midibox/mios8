$Id$

Custom Driver for multiple CLCD displays (selected with LCD type #7)


Documentation
~~~~~~~~~~~~~

  -> see app_lcd.inc


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
# include application specific driver
include $(MIOS_PATH)/modules/app_lcd/clcd_multi/app_lcd.mk
---

  3) main.c: add MIOS_LCD_TypeSet(0x07, 0x00, 0x00); to select Custom Drivers
     The second and third value are not used by the driver

  4) CLCDs can be selected with USER_LCD_Select(<number>);  // number 0..7
     see also the example application under $MIOS_PATH/apps/examples/lcd7/clcd_multi


"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): ensure, that the application includes 
     the global <app_lcd.inc> instead of "app_lcd.inc"

  2) Makefile: include app_lcd.mk

---
# include application specific driver
include $(MIOS_PATH)/modules/app_lcd/clcd_multi/app_lcd.mk
---

  3) main.asm (or main.inc): call MIOS_LCD_TypeSet from USER_Init hook:
---
	;; select LCD type #7 (custom LCD driver, hooks in app_lcd module will be used)
	clrf	MIOS_PARAMETER1	; variable enable pin not provided
	clrf	MIOS_PARAMETER2	; variable enable pin not provided
	movlw	0x07
	call	MIOS_LCD_TypeSet
---

  4) CLCDs can be selected by calling USER_LCD_Select (expecting number 0..7 in WREG)
     see also the example application under $MIOS_PATH/apps/examples/lcd7/clcd_multi


Integration Examples
~~~~~~~~~~~~~~~~~~~~

  -> see $MIOS_PATH/apps/examples/lcd7/clcd_multi
