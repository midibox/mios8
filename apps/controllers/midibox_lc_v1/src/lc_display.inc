; $Id$
;
; MIDIbox LC Display Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Display help functions, used by lc_mproc.inc
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_90
;;  when any 90 xx xx event has been received
;;  IN: LED ID in WREG
;; --------------------------------------------------------------------------
LC_DISPLAY_LEDStatusUpdate
	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_LEDStatusUpdate
	rgoto	LC_GLCD_LEDStatusUpdate

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_B04x,
;;  lc_mproc::LC_MPROC_Action_WriteMTC* and some functions of
;;  this file when a MTC digit should be print
;;  When one character LCD:  do nothing
;;  When two character LCDs: print the digits depending on cursor positions
;;  When graphical LCD:      print the digits depending on cursor positions
;;  see also lc_hlp.inc for the cursor positions
;; 
;; IN: digit position in MIOS_PARAMETER2
;;     digit value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_DISPLAY_Print_MTC_Digit
	;; calc pointer to LC_MTC_DIGIT_x
	lfsr	FSR1, LC_MTC_DIGIT_0
	movf	MIOS_PARAMETER2, W
	andlw	0x0f		; prevent overrun
	addwf	FSR1L, F

	;; save digit value
	movf	MIOS_PARAMETER3, W
	movwf	INDF1

	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Print_MTC_Digit
	rgoto	LC_GLCD_Print_MTC_Digit

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Received_B04x,
;;  lc_mproc::LC_MPROC_Action_WriteMTC* and some functions of
;;  this file when a status digit should be print
;;  When one character LCD:  do nothing
;;  When two character LCDs: print the digits depending on cursor positions
;;  When graphical LCD:      print the digits depending on cursor positions
;;  see also lc_hlp.inc for the cursor positions
;; 
;; IN: digit position in MIOS_PARAMETER2
;;     digit value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_DISPLAY_Print_STATUS_Digit
	;; calc pointer to LC_STATUS_DIGIT_x
	lfsr	FSR1, LC_STATUS_DIGIT_A
	movf	MIOS_PARAMETER2, W
	andlw	0x01		; prevent overrun
	addwf	FSR1L, F

	;; save digit value
	movf	MIOS_PARAMETER3, W
	movwf	INDF1

	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Print_STATUS_Digit
	rgoto	LC_GLCD_Print_STATUS_Digit

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by lc_meters.inc::LC_METERS_PatternSet when
;;  a meter should be displayed
;;  IN: meter number in FSR0L & 0x7
;;      pattern index in INDF0
;; --------------------------------------------------------------------------
LC_DISPLAY_Print_Meter
	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Print_Meter
	rgoto	LC_GLCD_Print_Meter

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by lc_meters.inc::LC_METERS_PatternSet when
;;  a meter should be displayed
;;  IN: meter number in FSR0L & 0x7
;;      pattern index in INDF0
;; --------------------------------------------------------------------------
LC_DISPLAY_Print_Knob
	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Print_Knob
	rgoto	LC_GLCD_Print_Knob

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Action_WriteLCD
;;  when a character should be print to the host message section
;;  IN: character in WREG
;;      cursor position in LC_CURSOR
;; --------------------------------------------------------------------------
LC_DISPLAY_Msg_PrintChar
	;; not in cali/GPC mode
	btfsc	LC_STAT, LC_STAT_CALI_SEL
	return
	btfsc	LC_STAT, LC_STAT_GPC_SEL
	return

	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Msg_PrintChar
	rgoto	LC_GLCD_Msg_PrintChar

;; --------------------------------------------------------------------------
;;  This function is called by lc_mproc::LC_MPROC_Action_WriteLCD
;;  to update the cursor on the host message screen
;;  IN: cursor position in LC_CURSOR
;; --------------------------------------------------------------------------
LC_DISPLAY_Msg_CursorUpdate
	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Msg_CursorUpdate
	rgoto	LC_GLCD_Msg_CursorUpdate

;; --------------------------------------------------------------------------
;;  This function is called by LC_DISPLAY_Tick when the host/glcd message
;;  has to be updated
;; --------------------------------------------------------------------------
LC_DISPLAY_Print_Msg
	BRA_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Print_Msg
	rgoto	LC_GLCD_Print_Msg
