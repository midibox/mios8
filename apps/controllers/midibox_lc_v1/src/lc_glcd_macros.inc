; $Id$
;
; MIDIbox LC Display Driver for Graphical LCDs
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
SET_GLCD_SMPTE_BEATS MACRO font, x, y
	movlw	x
	movwf	LCD_SMPTE_BEATS_X
	movlw	y
	movwf	LCD_SMPTE_BEATS_Y
	movlw	font & 0xff
	movwf	LCD_SMPTE_BEATS_FONT_L
	movlw	font >> 8
	movwf	LCD_SMPTE_BEATS_FONT_H
	ENDM

	;; font, x-position, y-position, distance between characters
	;; if font == 0, element will not be print
SET_GLCD_MTC MACRO font, x, y, d
	movlw	x
	movwf	LCD_MTC_X
	movlw	y
	movwf	LCD_MTC_Y
	movlw	d
	movwf	LCD_MTC_D
	movlw	font & 0xff
	movwf	LCD_MTC_FONT_L
	movlw	font >> 8
	movwf	LCD_MTC_FONT_H
	ENDM
	
	;; font, x-position, y-position, distance between characters
	;; if font == 0, element will not be print
SET_GLCD_STATUS MACRO font, x, y, d
	movlw	x
	movwf	LCD_STATUS_X
	movlw	y
	movwf	LCD_STATUS_Y
	movlw	d
	movwf	LCD_STATUS_D
	movlw	font & 0xff
	movwf	LCD_STATUS_FONT_L
	movlw	font >> 8
	movwf	LCD_STATUS_FONT_H
	ENDM
	
	;; font, x-position, y-position, distance between characters
	;; if font == 0, element will not be print
SET_GLCD_HOST_MSG MACRO font, x, y, d
	movlw	x
	movwf	LCD_MSG_X
	movlw	y
	movwf	LCD_MSG_Y
	movlw	d
	movwf	LCD_MSG_D
	movlw	font & 0xff
	movwf	LCD_MSG_FONT_L
	movlw	font >> 8
	movwf	LCD_MSG_FONT_H
	ENDM

	;; icons, x-position, y-position, distance between meters
	;; if font == 0, element will not be print
SET_GLCD_METER MACRO icons, x, y, d
	movlw	x
	movwf	LCD_METER_X
	movlw	y
	movwf	LCD_METER_Y
	movlw	d
	movwf	LCD_METER_D
	movlw	icons & 0xff
	movwf	LCD_METER_ICONS_L
	movlw	icons >> 8
	movwf	LCD_METER_ICONS_H
	ENDM
	
	;; icons, x-position, y-position, distance between knobs
	;; if font == 0, element will not be print
SET_GLCD_KNOB MACRO icons, x, y, d
	movlw	x
	movwf	LCD_KNOB_X
	movlw	y
	movwf	LCD_KNOB_Y
	movlw	d
	movwf	LCD_KNOB_D
	movlw	icons & 0xff
	movwf	LCD_KNOB_ICONS_L
	movlw	icons >> 8
	movwf	LCD_KNOB_ICONS_H
	ENDM
	
