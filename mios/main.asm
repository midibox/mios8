; $Id$
;
; Source Code of MIOS
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
MIOS_VERSION_STR MACRO
; Version:                 <-------------->
	STRING	16, 0x00, "MIOS8 V1.9h     "
	ENDM
; (fixed string length - 16 characters!)
;
;; ==========================================================================
;;  definitions and macros
;; ==========================================================================
#include <mios.h>
#include "mios_defines.h"
#include "mios_config.h"
#include <macros.h>

;; ==========================================================================
;;  MIOS routines, located from 0x0000-0x28fb (maximum!)
;; ==========================================================================
#include "mios_reset.inc"
#include "mios_isr.inc"
#include "mios_timer.inc"
#include "mios_delay.inc"
#include "mios_hlp.inc"
#include "mios_midi.inc"
#include "mios_mproc.inc"
#include "mios_srio.inc"	
#include "mios_din.inc"
#include "mios_dout.inc"
#include "mios_ain.inc"
#include "mios_mf.inc"
#include "mios_enc.inc"
#include "mios_lcd.inc"
#include "mios_clcd.inc"
#include "mios_glcd0.inc"
#include "mios_mlcd.inc"
#include "mios_eeprom.inc"
#include "mios_bankstick.inc"
#include "mios_iic.inc"
#include "mios_flash.inc"
#include "mios_system.inc"
#include "mios_display.inc"
#include "mios_mainloop.inc"
#include "mios_startup.inc"

;; ==========================================================================
;;  fill unused memory space with reset instructions
;; ==========================================================================
	fill	(reset), 0x2c00-$

;; ==========================================================================
;;  up to here the code/data is located to fixed addresses!
;; ==========================================================================
#include "mios__vectors.inc"		; 0x2c00-0x2fff --- OS address range
#include "user_vectors.inc"		; 0x3000-0x307f --- application address range
#include "mios_tables.inc"		; 0x3080-0x32ff --- application address range


;; ==========================================================================
;;  this "dummy" code will display a READY message so long as no application
;;  has been uploaded
;; ==========================================================================
TEXT_MIOS_WELCOME_0	da 0x0600, "READY."

_USER_DISPLAY_Init
	call	MIOS_LCD_Clear

	;; print ready message at first line
	TABLE_ADDR TEXT_MIOS_WELCOME_0
	goto	MIOS_LCD_PrintString
	
	
;; ==========================================================================
;;  Default GLCD Font (can be overwritten by application)
;; ==========================================================================
#include "mios_glcd_font.inc"		; 0x7cfc-0x7fff

;; ==========================================================================
;;  end of code - the assembler will stop here
;; ==========================================================================
	END
