Custom Driver for DOG CLCD displays (selected with LCD type #7)

Documentation
~~~~~~~~~~~~~

This is an alternative driver for EA DOG Character displays. The main
difference between this driver and the standard dog driver is that this
driver uses SPI to interface with the display.

This driver is quicker than the standard parallel driver and can support
as many displays as there are available output pins.

By default this driver uses LATB (D0-D7 on J15) for display select:
D0=LCD1, D1=LCD2, D2=LCD3, D3=LCD4, D4=LCD5, D5=LCD6, D6=LCD7, D7=LCD8

USER_LCD_Select function is used to select current display like with clcd_multi. By default LATB
is 0x00 so that all displays will be initialized simultaneously. This means that until
USER_LCD_Select is called, text will also be displayed on all displays.

The SPI clock uses LATD Pin7 (E on J15 which should be connected to PIN29 on all displays)
The Data line is LATD Pin7 (RW on J15, connected to PIN28 SI on all displays)
The RS line is LATD Pin5 (RS on J15, connected to PIN39 RS on all displays)

All other connections (power/backlight etc) are as per the standard DOG wiring diagram)

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
include $(MIOS_PATH)/modules/app_lcd/clcd/app_lcd.mk
---

  3) main.c: add MIOS_LCD_TypeSet(0x07, 0x00, 0x00); to select Custom Drivers
     The second and third value are not used by the driver


"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1) main.asm (or main.inc): ensure, that the application includes 
     the global <app_lcd.inc> instead of "app_lcd.inc"

  2) Makefile: include app_lcd.mk

---
# include application specific driver
include $(MIOS_PATH)/modules/app_lcd/clcd/app_lcd.mk
---

  3) main.asm (or main.inc): call MIOS_LCD_TypeSet from USER_Init hook:
---
	;; select LCD type #7 (custom LCD driver, hooks in app_lcd module will be used)
	clrf	MIOS_PARAMETER1	; variable enable pin not provided
	clrf	MIOS_PARAMETER2	; variable enable pin not provided
	movlw	0x07
	call	MIOS_LCD_TypeSet
---


Integration Examples
~~~~~~~~~~~~~~~~~~~~

  -> see $MIOS_PATH/apps/examples/lcd7/clcd
