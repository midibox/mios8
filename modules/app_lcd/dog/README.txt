$Id$

Custom Driver for DOG displays (selected with LCD type #7)

MIOS_PARAMETER1 and 2 select the enable pins like for common CLCD driver

This driver switches to CLCD (type #0) after initialisation to save code size,
because after the special commands have been written into configuration registers,
the display can be handled like a common character LCD.

Up to 2 DOG based LCDs are supported


Hardware hints
~~~~~~~~~~~~~~

8bit parallel mode is used by default, 4bit parallel mode can be optionally
selected.

Due to the weak output drivers of DOG technology, a 4.7k pull-down resistor 
has to be added to the "D7" pin of the display.

Higher or lower resistor values will result into hang-ups or a very slow 
display output!


Wiring diagram: http://www.ucapps.de/mbhp/mbhp_lcd7_dog.pdf


Software hints
~~~~~~~~~~~~~~

The original cursor addresses for upper/middle/lower line is mapped to "usual"
addresses to make the display output compatible to common MIOS applications.

           Original       Mapped
1st Line: 0x00..0x0f -> 0x00..0x0f
2nd Line: 0x10..0x1f -> 0x40..0x4f
3rd Line: 0x20..0x2f -> 0x50..0x5f

This is done via MIOS_LCD_YAddressSet in USER_LCD_Init (app_lcd.inc)
Note that an application could overwrite these settings if it uses MIOS_LCD_YAddressSet
as well! In such cases, the application has to be adapted manually.


Coding of Enable pins:
   RA0..RA7: 0x00..0x07    8bit mode
   RB0..RB7: 0x10..0x17    8bit mode
   RC0..RC7: 0x20..0x27    8bit mode
   RD0..RD7: 0x30..0x37    8bit mode
   RE0..RE7: 0x40..0x47    8bit mode

4bit mode (e.g. for PIC18F4685 based MIDIbox SID V2) can be selected
by setting bit 7:
   RA0..RA7: 0x80..0x87    4bit mode
   RB0..RB7: 0x90..0x97    4bit mode
   RC0..RC7: 0xa0..0xa7    4bit mode
   RD0..RD7: 0xb0..0xb7    4bit mode
   RE0..RE7: 0xc0..0xc7    4bit mode


DOG LCDs provide some nice special features like Icons and "big letters", which
are specified in the ST7036 datasheet:
   http://www.lcd-module.de/eng/pdf/zubehoer/st7036.pdf


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
include $(MIOS_PATH)/modules/app_lcd/clcd/app_lcd.mk
---

  3) main.c: add MIOS_LCD_TypeSet(0x07, 0x37, 0x24); to select Custom Drivers
     The second and third value select the enable pins like for a common CLCD


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
	;; select LCD type #7
	;; (custom LCD driver, hooks in $MIOS_PATH/modules/app_lcd/dog/app_lcd.asm module will be used)
	;; Parameter #1 and #2 select the enable pins like for common CLCD driver
	;; Note that the DOG driver switches to CLCD (type #0) after initialisation!
	;; Up to 2 DOG based LCDs are supported
	;; E input of first LCD at D.7, E of second LCD @C.4
	movlw	0x37		; E pin
	movwf	MIOS_PARAMETER1
	movlw	0x24		; E2 pin
	movwf	MIOS_PARAMETER2
	movlw	0x07
	call	MIOS_LCD_TypeSet
---


Integration Examples
~~~~~~~~~~~~~~~~~~~~

  -> see $MIOS_PATH/apps/examples/lcd7/dog
