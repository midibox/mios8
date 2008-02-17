; $Id: seq_trg.inc 73 2008-02-03 00:00:52Z tk $
;
; MIDIboxSEQ
; Functions related to Tracks (Track Names, trigger pin assignments, etc..)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; the track information table has been defined in form of a macro,
	;; which will be included here. It's located in the setup_*.asm file
	;; it consists of 16 entries for 16 tracks
SEQ_TRK_INFO_TABLE
	DEFAULT_TRKINFO

	;; I'm not happy to define the table width at a different place (here):
SEQ_TRK_INFO_TABLE_WIDTH EQU 10
	;; but the user shouldn't change it in his setup_*.asm file to avoid inconsistencies

SEQ_TRK_INFO_TABLE_OFFSET_NAME	EQU	0
SEQ_TRK_INFO_TABLE_OFFSET_SR	EQU	6
SEQ_TRK_INFO_TABLE_OFFSET_PIN	EQU	7
SEQ_TRK_INFO_TABLE_OFFSET_MPORT	EQU	8
SEQ_TRK_INFO_TABLE_OFFSET_ACHN	EQU	9

;; --------------------------------------------------------------------------
;;  This function prints the track name
;;  IN: track number (0-15) in WREG
;;  OUT: 6 characters
;; --------------------------------------------------------------------------
SEQ_TRK_PrnName
	TABLE_ADDR_MUL_W SEQ_TRK_INFO_TABLE, SEQ_TRK_INFO_TABLE_WIDTH
	movlw	6
	goto	MIOS_LCD_PrintPreconfString



;; --------------------------------------------------------------------------
;;  This function requests a trigger in SEQ_DRUM_TRIGGERS[01]
;;  IN: trigger number (correlates to track number) in WREG
;;      velocity (for forwardning to AOUT) in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_TRK_ReqTrigger
	lfsr	FSR1, SEQ_DRUM_TRIGGERS0	; select register
	btfsc	WREG, 3
	lfsr	FSR1, SEQ_DRUM_TRIGGERS1
	call	MIOS_HLP_GetBitORMask		; get pin mask
	iorwf	INDF1, F			; set request flag
	return


;; --------------------------------------------------------------------------
;;  This function is called from SEQ_CORE_Clk and SEQ_MIDI_NoteOn to update all triggers
;;  that have been requested at the same time moment
;;  IN: trigger states in SEQ_DRUM_TRIGGERS[01]
;;  OUT: SEQ_DRUM_TRIGGERS[01] will be cleared during update
;; --------------------------------------------------------------------------
SEQ_TRK_SetTriggers
	SET_BSR	SEQ_BASE

	;; get offset to table
	movlw	LOW(SEQ_TRK_INFO_TABLE)
	addlw	SEQ_TRK_INFO_TABLE_OFFSET_SR
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_TRK_INFO_TABLE)
	addwfc	TBLPTRH, F

	;; go through table and set all requested triggers
	clrf	PRODL			; used as loop counter
SEQ_TRK_SetTriggersLoop
	BRA_IFSET SEQ_DRUM_TRIGGERS0, 0, BANKED, SEQ_TRK_SetTriggersLoop_Req
	tblrd*+				; dummy read for SR definition
	tblrd*+				; dummy read for pin definition
	rgoto	SEQ_TRK_SetTriggersLoop_Next
SEQ_TRK_SetTriggersLoop_Req
	tblrd*+				; read shift register number
	movf	TABLAT, W		; skip if zero
	bnz	SEQ_TRK_SetTriggersLoop_Cont
	tblrd*+				; dummy read for pin definition
	rgoto	SEQ_TRK_SetTriggersLoop_Next
SEQ_TRK_SetTriggersLoop_Cont
	addlw	-1			; get value of SR
	call	MIOS_DOUT_SRGet		; result in MIOS_PARAMETER1 and WREG, pointer to SR in FSR1
	
	tblrd*+				; read pin number
	movf	TABLAT, W
	call	MIOS_HLP_GetBitORMask	; get OR mask
	iorwf	INDF1, F		; set bit
SEQ_TRK_SetTriggersLoop_Next
	clrc				; shift-right request flags
	rrf	SEQ_DRUM_TRIGGERS1, F, BANKED
	rrf	SEQ_DRUM_TRIGGERS0, F, BANKED
	incf	PRODL, F		; next entry?
	BRA_IFSET PRODL, 4, ACCESS, SEQ_TRK_SetTriggersLoop_Break
	movlw	SEQ_TRK_INFO_TABLE_WIDTH-2
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F
	rgoto	SEQ_TRK_SetTriggersLoop

SEQ_TRK_SetTriggersLoop_Break
	return	


;; --------------------------------------------------------------------------
;;  This function is called from USER_SR_ServiceFinish to clear the trigger
;;  pins - this simple method allows to generate 1mS pulses, since all pins,
;;  which have been set before (e.g. from the main program), will turn low
;;  with the next SR update cycle (which is performed 1 mS later)
;; 
;;  Note that this routine is called from an interrupt service routine!
;; --------------------------------------------------------------------------
SEQ_TRK_ClearTriggers
	;; get offset to table
	movlw	LOW(SEQ_TRK_INFO_TABLE)
	addlw	SEQ_TRK_INFO_TABLE_OFFSET_SR
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_TRK_INFO_TABLE)
	addwfc	TBLPTRH, F

	;; go through table and clear all triggers
	clrf	IRQ_TMP1		; used as loop counter
SEQ_TRK_ClearTriggersLoop
	tblrd*+				; read shift register number
	movf	TABLAT, W		; skip if zero
	bnz	SEQ_TRK_ClearTriggersLoop_Cont
	tblrd*+				; dummy read for pin definition
	rgoto	SEQ_TRK_ClearTriggersLoop_Next
SEQ_TRK_ClearTriggersLoop_Cont
	addlw	-1			; get value of SR
	call	MIOS_DOUT_SRGet		; result in MIOS_PARAMETER1 and WREG, pointer to SR in FSR1
	
	tblrd*+				; read pin number
	movf	TABLAT, W
	call	MIOS_HLP_GetBitANDMask	; get AND mask
	andwf	INDF1, F		; clear bit
SEQ_TRK_ClearTriggersLoop_Next

	incf	IRQ_TMP1, F		; next entry?
	BRA_IFSET IRQ_TMP1, 4, ACCESS, SEQ_TRK_ClearTriggersLoop_Break
	movlw	SEQ_TRK_INFO_TABLE_WIDTH-2
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F
	rgoto	SEQ_TRK_ClearTriggersLoop

SEQ_TRK_ClearTriggersLoop_Break
	return	


;; --------------------------------------------------------------------------
;;  This function returns the MIDI port of a given track
;;  IN: track number in WREG
;;  OUT: port number + 1 in WREG (if 0: port disabled by user)
;; --------------------------------------------------------------------------
SEQ_TRK_GetMIDIPort
	TABLE_ADDR_MUL_W SEQ_TRK_INFO_TABLE, SEQ_TRK_INFO_TABLE_WIDTH
	movlw	SEQ_TRK_INFO_TABLE_OFFSET_MPORT
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F

	tblrd*+
	movf	TABLAT, W
	andlw	0xff		; (fix ZERO flag)
	return


