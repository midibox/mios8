$Id: $

Custom Driver for for T6963C --- with horizontal screen (-> 240x64)

; IMPORTANT: this driver allocates 8 registers of the common user space between 0x010 and 0x37f
; Assembler-Only version: addresses 0x378..0x37f are used by default!
; Relocatable version: linker searches automatically for free addresses :)


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
include $(MIOS_PATH)/modules/app_lcd/t6963c_h/app_lcd.mk
---

  3) main.c: add MIOS_LCD_TypeSet(0x07, 0x00, 0x00); to select Custom Driver
     The second and third value is not used by the driver


"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): ensure, that the application includes 
     the global <app_lcd.inc> instead of "app_lcd.inc"

  2) Makefile: include app_lcd.mk

---
# include application specific driver
include $(MIOS_PATH)/modules/app_lcd/t6963c_h/app_lcd.mk
---

  3) main.asm (or main.inc): call MIOS_LCD_TypeSet from USER_Init hook:
---
	;; select LCD type #7 (custom LCD driver, hooks in app_lcd module will be used)
	movlw	0x00			; here you could forward an additional parameter
	movwf	MIOS_PARAMETER1
	movlw	0x00			; here you could forward an additional parameter
	movwf	MIOS_PARAMETER2
	movlw	0x07
	call	MIOS_LCD_TypeSet
---


Integration Examples
~~~~~~~~~~~~~~~~~~~~

  -> see $MIOS_PATH/apps/examples/lcd7/t6963c_h
