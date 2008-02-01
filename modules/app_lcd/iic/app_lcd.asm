; $Id: $
;
; Wrapper for app_lcd.inc
; To be used for relocatable programs
;
; ==========================================================================

	radix	dec

#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================
; Import/Export Labels
; ==========================================================================
	global	USER_LCD_Init
	global	USER_LCD_Data
	global	USER_LCD_Cmd
	global	USER_LCD_Clear
	global	USER_LCD_CursorSet
	global	USER_LCD_PrintChar
	global	USER_LCD_SpecialCharInit

; ==========================================================================
; Start code section and include driver code
; ==========================================================================
APP_LCD CODE
#include "app_lcd.inc"
	END
