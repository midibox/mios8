; $Id$
;
; MIDImon Display functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_Timer every mS to flash the cursor
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
MON_DISPLAY_Timer
	;; using a 8-bit register as prescaler
	incf	MON_DISPLAY_CURSOR_CTR, F

	;; update display every 128 seconds
	movf	MON_DISPLAY_CURSOR_CTR, W
	andlw	0x7f
	skpnz
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

	;; the offset position will be visible when bit #7 of MON_DISPLAY_CURSOR_CTR is set
	return
	

;; --------------------------------------------------------------------------
;;  This handler is called from DISPLAY_Tick
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
MON_DISPLAY_Handler
	;; if 1st line update request has been requested, call MON_DISPLAY_Handler_1stL
	RCALL_IFSET MB_STAT, MB_STAT_DISPLAY_UPDATE_1L_REQ, ACCESS, MON_DISPLAY_Handler_1stL

	;; don't continue of no display update has been requested
	btfss	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	return
	;; clear flag
	bcf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

	;; trace memory offset in MON_CURRENT_ENTRY_PTR, will be decremented with every iteration
	movf	MON_DISPLAY_OFFSET, W
	subwf	MON_TRACE_PTR, W
	movwf	MON_CURRENT_ENTRY_PTR
#if DEFAULT_TRACE_MEM_ENTRIES < 256
	movlw	DEFAULT_TRACE_MEM_ENTRIES-1
	cpfsgt	MON_CURRENT_ENTRY_PTR, ACCESS
	rgoto MON_DISPLAY_Handler_NoWrap
	movlw	DEFAULT_TRACE_MEM_ENTRIES
	addwf	MON_CURRENT_ENTRY_PTR, F
MON_DISPLAY_Handler_NoWrap
#endif

	;; loop counter in MON_DISPLAY_LOOP_CTR, beginning at last entry
	movlw	DEFAULT_NUMBER_OF_LINES-1
	movwf	MON_DISPLAY_LOOP_CTR
MON_DISPLAY_Handler_PrintLoop
	movf	MON_DISPLAY_LOOP_CTR, W
	BRA_IFSET MB_STAT, MB_STAT_DISPLAY_GLCD, ACCESS, MON_DISPLAY_Handler_PrintLoopG
MON_DISPLAY_Handler_PrintLoopC
	mullw	0x40
	movf	PRODL, W
	call	MIOS_LCD_CursorSet
	rgoto	MON_DISPLAY_Handler_PrintLoopCnt
MON_DISPLAY_Handler_PrintLoopG
	movwf	MIOS_PARAMETER1	; Y-Pos
	movlw	0x00		; X-Pos
	call	MIOS_GLCD_GCursorSet
MON_DISPLAY_Handler_PrintLoopCnt

	call	MON_LCD_PrintEntry

	;; decrement trace memory offset, wrap at DEFAULT_TRACE_MEM_ENTRIES
	decf	MON_CURRENT_ENTRY_PTR, F
	movlw	DEFAULT_TRACE_MEM_ENTRIES-1
	skpc
	movwf	MON_CURRENT_ENTRY_PTR

MON_DISPLAY_Handler_PrintLoopNxt
	;; branch depending on MTC/MIDICLK flag
	BRA_IFSET MB_STAT, MB_STAT_SHOW_MTC, ACCESS, MON_DISPLAY_Handler_PrintLoopNxC
	BRA_IFSET MB_STAT, MB_STAT_SHOW_MIDICLK, ACCESS, MON_DISPLAY_Handler_PrintLoopNxC

MON_DISPLAY_Handler_PrintLoopNxN
	;; decrement loop counter until 0xff is reached
	decf	MON_DISPLAY_LOOP_CTR, F
	bc	MON_DISPLAY_Handler_PrintLoop
	rgoto	MON_DISPLAY_Handler_PrintOff

MON_DISPLAY_Handler_PrintLoopNxC
	;; if MIDI clock or MTC enabled: decrement loop counter until 0x00 is reached
	;; (so that the first line won't be overwritten)
	decfsz	MON_DISPLAY_LOOP_CTR, F
	rgoto	MON_DISPLAY_Handler_PrintLoop
	;; 	rgoto	MON_DISPLAY_Handler_PrintOff

MON_DISPLAY_Handler_PrintOff
	;; print offset if != 0
	movf	MON_DISPLAY_OFFSET, W
	bz	MON_DISPLAY_Handler_End

	;; don't print if MON_DISPLAY_CURSOR_CTR[7] == 0
	btfss	MON_DISPLAY_CURSOR_CTR, 7
	return

	movlw	0x00 + DEFAULT_OFFSET_SHIFT_INDICATOR
	btfsc	MB_STAT, MB_STAT_SHOW_MTC
	movlw 0x40 + DEFAULT_OFFSET_SHIFT_INDICATOR
	btfsc	MB_STAT, MB_STAT_SHOW_MIDICLK
	movlw 0x40 + DEFAULT_OFFSET_SHIFT_INDICATOR
	call	MIOS_LCD_CursorSet

	movlw	'['
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movf	MON_DISPLAY_OFFSET, W
	call	MIOS_LCD_PrintBCD3
	movlw	']'
	call	MIOS_LCD_PrintChar

MON_DISPLAY_Handler_End
	return

;; --------------------------------------------------------------------------
;; called when update of 1st line has been requested
;; --------------------------------------------------------------------------
MON_DISPLAY_Handler_1stL
	bcf	MB_STAT, MB_STAT_DISPLAY_UPDATE_1L_REQ

	;; if MIDI clock should be displayed, do so
	BRA_IFCLR MB_STAT, MB_STAT_SHOW_MIDICLK, ACCESS, MON_DISPLAY_Handler_NoClk
MON_DISPLAY_Handler_Clk
	;; note: either clock or MTC is enabled
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MON_LCD_PrintClk
MON_DISPLAY_Handler_NoClk

	;; if MTC should be displayed, do so
	BRA_IFCLR MB_STAT, MB_STAT_SHOW_MTC, ACCESS, MON_DISPLAY_Handler_NoMTC
MON_DISPLAY_Handler_MTC
	;; note: either clock or MTC is enabled
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MON_LCD_PrintMTC
MON_DISPLAY_Handler_NoMTC
	return
