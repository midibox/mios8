; $Id$
;
; MIDIbox LC Display Driver for Character LCDs
; Macro Library
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  Macros which are used to initialize the display elements
;; --------------------------------------------------------------------------
	;; font, x-position, y-position
	;; if font == 0, element will not be print
SET_CLCD_SMPTE_BEATS MACRO font, x, y
	movlw	x
	movwf	LCD_SMPTE_BEATS_X
	movlw	y * 0x40
	movwf	LCD_SMPTE_BEATS_Y
	movlw	font & 0xff
	movwf	LCD_SMPTE_BEATS_FONT_H
	ENDM

	;; font, x-position, y-position
	;; if font == 0, element will not be print
SET_CLCD_MTC MACRO font, x, y
	movlw	x
	movwf	LCD_MTC_X
	movlw	y * 0x40
	movwf	LCD_MTC_Y
	movlw	font & 0xff
	movwf	LCD_MTC_FONT_H
	ENDM
	
	;; font, x-position, y-position
	;; if font == 0, element will not be print
SET_CLCD_STATUS MACRO font, x, y
	movlw	x
	movwf	LCD_STATUS_X
	movlw	y * 0x40
	movwf	LCD_STATUS_Y
	movlw	font & 0xff
	movwf	LCD_STATUS_FONT_H
	ENDM
	
	;; font, x-position, y-position, spaces between tracks enable/disable
	;; if font == 0, element will not be print
SET_CLCD_HOST_MSG MACRO font, x, y, d
	movlw	x
	movwf	LCD_MSG_X
	movlw	y * 0x40
	movwf	LCD_MSG_Y
	movlw	d
	movwf	LCD_MSG_D
	movlw	font & 0xff
	movwf	LCD_MSG_FONT_H
	ENDM

	;; icons, x-position, y-position, distance between meters
	;; if font == 0, element will not be print
SET_CLCD_METER MACRO icons, x, y, d
	movlw	x
	movwf	LCD_METER_X
	movlw	y * 0x40
	movwf	LCD_METER_Y
	movlw	d
	movwf	LCD_METER_D
	movlw	icons & 0xff
	movwf	LCD_METER_ICONS_H
	ENDM
	

	;; font, x-position, y-position
	;; if font == 0, element will not be print
SET_CLCD_RSM_MSG MACRO font, x, y
	movlw	x
	movwf	LCD_RSM_X
	movlw	y * 0x40
	movwf	LCD_RSM_Y
	movlw	font & 0xff
	movwf	LCD_RSM_FONT_H
	ENDM

	;; font, x-position, y-position
	;; if font == 0, element will not be print
SET_CLCD_SEL_MSG MACRO font, x, y
	movlw	x
	movwf	LCD_SEL_X
	movlw	y * 0x40
	movwf	LCD_SEL_Y
	movlw	font & 0xff
	movwf	LCD_SEL_FONT_H
	ENDM

