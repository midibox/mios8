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
; Declare local variables
; ==========================================================================

APP_LCD_VARIABLES	UDATA
USER_LCD_OFFSET_BYTE	res	1
USER_LCD_OFFSET		res	1
USER_LCD_OFFSET_TMP	res	1
USER_LCD_MASK		res	1
USER_LCD_MASK_TMP1	res	1
USER_LCD_MASK_TMP2	res	1
USER_LCD_WORD_TMP1	res	1
USER_LCD_WORD_TMP2	res	1

;; this switch disables absolute variable locations in app_lcd.inc
#define APP_LCD_DONT_LOCATE_VARIABLES

; ==========================================================================
; Start code section and include driver code
; ==========================================================================
APP_LCD CODE
#include "app_lcd.inc"
	END
