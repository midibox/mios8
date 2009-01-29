$Id: README.txt 69 2008-02-01 00:20:18Z tk $

Custom Driver for up to 8 * EA DOGM/L Series GLCD (every display provides a resolution of 128x64)


**** PLEASE NOTE ****
Although this has been designed for 8x GLCD's It has only been tested on a single display 
so I would welcome feedback. I have also written an SPI based driver for the character DOG 
displays, please ask me if you are interested in it but I think the Graphic ones are much better!

The performance beats anything else handsdown! Using lcd_benchmark:

4481 *8 *100 nS = 3.58mS!

Phil Taylor (phil@taylor.org.uk)


; This driver allows to drive up to 8 of them, every display is connected
; to a dedicated chip select line at port B. They can be addressed with
; following (graphical) cursor positions:
; 
; I have swapped these round to make it easier for people with PIC18f4685
; so they are limited to 4x displays. Therefore the first display is on PortB.7
; 
; CS at PortB.7     CS at PortB.6     
; +--------------+  +--------------+  
; |              |  |              |  
; | X =   0..127 |  | X = 128..255 |    
; | Y =   0..  7 |  | Y =   0..  7 | 
; |              |  |              |  
; +--------------+  +--------------+  
;
; CS at PortB.5     CS at PortB.4     
; +--------------+  +--------------+ 
; |              |  |              | 
; | X =   0..127 |  | X = 128..255 | 
; | Y =   8.. 15 |  | Y =   8.. 15 | 
; |              |  |              | 
; +--------------+  +--------------+ 
;
; CS at PortB.3     CS at PortB.2   
; +--------------+  +--------------+
; |              |  |              |
; | X =   0..127 |  | X = 128..255 |
; | Y =  16.. 23 |  | Y =  16.. 23 |
; |              |  |              |
; +--------------+  +--------------+
;
; CS at PortB.1     CS at PortB.0   
; +--------------+  +--------------+
; |              |  |              |
; | X =   0..127 |  | X = 128..255 |
; | Y =  24.. 31 |  | Y =  24.. 31 |
; |              |  |              |
; +--------------+  +--------------+
; The arrangement can be modified below the USER_LCD_Data_CS and USER_LCD_GCursorSet label


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
include $(MIOS_PATH)/modules/app_lcd/pcd8544/app_lcd.mk
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
include $(MIOS_PATH)/modules/app_lcd/pcd8544/app_lcd.mk
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

  -> see $MIOS_PATH/apps/examples/lcd7/dog_g