; $Id$
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
	;; temporary store trigger number in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2

	;; request trigger
	lfsr	FSR1, SEQ_DRUM_TRIGGERS0	; select register
	btfsc	WREG, 3
	lfsr	FSR1, SEQ_DRUM_TRIGGERS1
	call	MIOS_HLP_GetBitORMask		; get pin mask
	iorwf	INDF1, F			; set request flag

	;; check for AOUT assignment
	movf	MIOS_PARAMETER2, W	; pin number was temporary stored in MIOS_PARAMETER2
	TABLE_ADDR_MUL_W SEQ_TRK_INFO_TABLE, SEQ_TRK_INFO_TABLE_WIDTH
	movlw	SEQ_TRK_INFO_TABLE_OFFSET_ACHN
	addwf	TBLPTRL, F
	movlw	0
	addwfc	TBLPTRH, F

	tblrd*+
	movf	TABLAT, W
	bz	SEQ_TRK_ReqTrigger_NoAOUT
SEQ_TRK_ReqTrigger_AOUT
	decf	TABLAT, W		; check for CV DOUT channel number in range 0..15
	andlw	0xf0
	bnz	SEQ_TRK_ReqTrigger_CV_Invalid
SEQ_TRK_ReqTrigger_CV
	lfsr	FSR1, SEQ_CV_DOUT_VALUE1
	decf	TABLAT, W		; assigned CV channel number
	andlw	0x0f
	addwf	FSR1L, F
	rrf	MIOS_PARAMETER1, W	; use 6bit resolution 
	andlw	0x3f
	movwf	INDF1
SEQ_TRK_ReqTrigger_CV_Invalid

	decf	TABLAT, W		; check for AOUT channel number in range 0..7
	andlw	0xf8
	bnz	SEQ_TRK_ReqTrigger_AOUT_Invalid
	decf	TABLAT, W		; assigned AOUT channel number
	call	AOUT_Pin7bitSet		; velocity passed in MIOS_PARAMETER1
SEQ_TRK_ReqTrigger_AOUT_Invalid
SEQ_TRK_ReqTrigger_NoAOUT

	return


;; --------------------------------------------------------------------------
;;  This function is called from SEQ_CORE_Clk and SEQ_MIDI_NoteOn to update all triggers
;;  that have been requested at the same moment
;;  IN: trigger states in SEQ_DRUM_TRIGGERS[01]
;;  OUT: SEQ_DRUM_TRIGGERS[01] will be cleared during update
;; --------------------------------------------------------------------------
SEQ_TRK_SetTriggers
	SET_BSR	SEQ_BASE

	;; interrupts must be disabled during DOUT update to ensure, that all triggers
	;; are set during the same SRIO update cycle
	movff	INTCON, PRODH		; store current interrupt status in PRODH
	IRQ_DISABLE			; disable IRQs
	
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
	xorlw	0x07			; DOUT pins are mirrored!
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

	movff	PRODH, INTCON		; restore interrupt status from PRODH

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
	xorlw	0x07			; DOUT pins are mirrored!
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


;; --------------------------------------------------------------------------
;;  This function is called periodically by USER_SR_ServicePrepare to
;;  transfer CV channel values to DOUT registers
;; --------------------------------------------------------------------------
SEQ_TRK_CV_DOUT_Update

	;; help macro
SEQ_TRK_CV_DOUT_Update_SR MACRO cv, DOUT_SR
  IF DOUT_SR && DOUT_SR <= 16
	movlw	DOUT_SR-1
	call	MIOS_DOUT_SRGet
	andlw	0xc0
	SET_BSR	SEQ_BASE
	iorwf	SEQ_CV_DOUT_VALUE1+cv, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	DOUT_SR-1
	call	MIOS_DOUT_SRSet
  ENDIF
	ENDM


	SEQ_TRK_CV_DOUT_Update_SR 0, DEFAULT_CV_DOUT_SR1
	SEQ_TRK_CV_DOUT_Update_SR 1, DEFAULT_CV_DOUT_SR2
	SEQ_TRK_CV_DOUT_Update_SR 2, DEFAULT_CV_DOUT_SR3
	SEQ_TRK_CV_DOUT_Update_SR 3, DEFAULT_CV_DOUT_SR4
	SEQ_TRK_CV_DOUT_Update_SR 4, DEFAULT_CV_DOUT_SR5
	SEQ_TRK_CV_DOUT_Update_SR 5, DEFAULT_CV_DOUT_SR6
	SEQ_TRK_CV_DOUT_Update_SR 6, DEFAULT_CV_DOUT_SR7
	SEQ_TRK_CV_DOUT_Update_SR 7, DEFAULT_CV_DOUT_SR8
	SEQ_TRK_CV_DOUT_Update_SR 8, DEFAULT_CV_DOUT_SR9
	SEQ_TRK_CV_DOUT_Update_SR 9, DEFAULT_CV_DOUT_SR10
	SEQ_TRK_CV_DOUT_Update_SR 10, DEFAULT_CV_DOUT_SR11
	SEQ_TRK_CV_DOUT_Update_SR 11, DEFAULT_CV_DOUT_SR12
	SEQ_TRK_CV_DOUT_Update_SR 12, DEFAULT_CV_DOUT_SR13
	SEQ_TRK_CV_DOUT_Update_SR 13, DEFAULT_CV_DOUT_SR14
	SEQ_TRK_CV_DOUT_Update_SR 14, DEFAULT_CV_DOUT_SR15
	SEQ_TRK_CV_DOUT_Update_SR 15, DEFAULT_CV_DOUT_SR16

	return
