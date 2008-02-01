$Id: $

Custom Driver for Matrix Orbital IIC display (selected with LCD type #7)


Files
~~~~~

  app_lcd.inc: can be included into "asm-only" programs (non-relocatable code)
  app_lcd.asm: relocatable wrapper for C based applications
  app_lcd.mk:  include this into Makefile to integrate the driver


Defines
~~~~~~~

  used in app_lcd.inc, can be overruled with -D<define>=<value> from command line

  o APP_LCD_IIC_DEVICE_ADDRESS
    (IIC address of display, default: 0x5a)

  o APP_LCD_CONTRAST_VALUE
    (contrast value, 0xff = max, default: 0x90)

  o APP_LCD_BACKLIGHT_VALUE
    (backlight delay, 0x00 = backlight will remain on indefinitely)


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "app_lcd.o" to OBJS variable

  2) Makefile: include app_lcd.mk

---
# include application specific driver
include $(MIOS_PATH)/modules/app_lcd/iic/app_lcd.mk
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
include $(MIOS_PATH)/modules/app_lcd/iic/app_lcd.mk
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

  -> see $MIOS_PATH/apps/examples/lcd7/iic
