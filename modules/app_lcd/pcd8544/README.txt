$Id: $

Custom Driver for up to 8 * PCD8544 (every display provides a resolution of 84x48)

; This display can mostly be found in Nokia mobile phones
;
; This driver allows to drive up to 8 of them, every display is connected
; to a dedicated chip select line at port B. They can be addressed with
; following (graphical) cursor positions:
; 
; 
; CS at PortB.0     CS at PortB.1     CS at PortB.2
; +--------------+  +--------------+  +--------------+
; |              |  |              |  |              |
; | X =   0.. 83 |  | X =  84..167 |  | X = 168..251 |  
; | Y =   0..  5 |  | Y =   0..  5 |  | Y =   0..  5 |
; |              |  |              |  |              |
; +--------------+  +--------------+  +--------------+  
;
; CS at PortB.3     CS at PortB.4     CS at PortB.5
; +--------------+  +--------------+  +--------------+
; |              |  |              |  |              |
; | X =   0.. 83 |  | X =  84..167 |  | X = 168..251 |  
; | Y =   6.. 11 |  | Y =   6.. 11 |  | Y =   6.. 11 |
; |              |  |              |  |              |
; +--------------+  +--------------+  +--------------+  
;
; CS at PortB.6     CS at PortB.7   
; +--------------+  +--------------+
; |              |  |              |
; | X =   0.. 83 |  | X =  84..167 |
; | Y =  12.. 17 |  | Y =  12.. 17 |
; |              |  |              |
; +--------------+  +--------------+
;
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

  -> see $MIOS_PATH/apps/examples/lcd7/pcd8544
