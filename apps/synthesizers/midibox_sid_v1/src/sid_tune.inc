; $Id$
;
; MIDIbox SID
; Routines to play short tunes (when a BankStick is attached/deattached)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  SID_PLAY Tune #1
;; --------------------------------------------------------------------------
SID_TUNE_Play1
	rcall	SID_TUNE_Init

	movlw	0x48
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4a
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4c
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4d
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4f
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x51
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x53
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x54
	rcall	SID_TUNE_Note_On
	movlw	0x10
	rcall	SID_TUNE_Wait
	rcall	SID_TUNE_Note_Off
	movlw	0x04
	rcall	SID_TUNE_Wait

	rgoto	SID_TUNE_End
	

;; --------------------------------------------------------------------------
;;  SID_PLAY Tune #2
;; --------------------------------------------------------------------------
SID_TUNE_Play2
	rcall	SID_TUNE_Init

	movlw	0x54
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x53
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x51
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4f
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4d
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4c
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x4a
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x48
	rcall	SID_TUNE_Note_On
	movlw	0x10
	rcall	SID_TUNE_Wait
	rcall	SID_TUNE_Note_Off
	movlw	0x04
	rcall	SID_TUNE_Wait

	rgoto	SID_TUNE_End
	
;; --------------------------------------------------------------------------
;;  SID_PLAY Tune #3 (3rd note in W)
;; --------------------------------------------------------------------------
SID_TUNE_Play3_Note
	rcall	SID_TUNE_Init

	movlw	0x54
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x48
	rcall	SID_TUNE_Note_On_Wait01
	movlw	0x04
	rcall	SID_TUNE_Wait
	rcall	SID_TUNE_Note_Off
	movlw	0x02
	rcall	SID_TUNE_Wait

	rgoto	SID_TUNE_End
	

;; --------------------------------------------------------------------------
;;  SID_PLAY Initialization routine
;; --------------------------------------------------------------------------
SID_TUNE_Init
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE; turn off SID software synth part

	SET_BSR SID_BASE
	movlw	0x0f
	movwf	SID_MODE_VOL, BANKED
	clrf	SID_RES_FCHN, BANKED
	clrf	SID_V2_CTRL, BANKED
	clrf	SID_V3_CTRL, BANKED
	clrf	SID_V1_ENV_AD, BANKED
	movlw	0xf6
	movwf	SID_V1_ENV_SR, BANKED
	movlw	0x08
	movwf	SID_V1_PW_H, BANKED
	clrf	SID_V1_PW_L, BANKED
	return

;; --------------------------------------------------------------------------
;;  SID_PLAY End reinitializes the current SID patch
;; --------------------------------------------------------------------------
SID_TUNE_End
	bcf	SID_STAT, SID_STAT_ENGINE_DISABLE; turn on SID software synth part again
	goto	SID_PATCH_Init			; init patch and exit

;; --------------------------------------------------------------------------
;;  SID_PLAY a note (in: Note Number)
;; --------------------------------------------------------------------------
SID_TUNE_Note_On
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	call	SID_FRQ_TABLE_Get

	movff	MIOS_PARAMETER1, SID_V1_FRQ_L
	movff	MIOS_PARAMETER2, SID_V1_FRQ_H
	movlw	0x41
	movff	WREG, SID_V1_CTRL

	goto	_SID_SR_Handler

SID_TUNE_Note_Off
	SET_BSR	SID_BASE
	movlw	0x40
	movwf	SID_V1_CTRL, BANKED
	goto	_SID_SR_Handler

SID_TUNE_Note_On_Wait01
	rcall	SID_TUNE_Note_On
	movlw	0x02
	rgoto	SID_TUNE_Wait


;; --------------------------------------------------------------------------
;;  SID_PLAY wait for a certain time and poll SID_SR Handler
;; --------------------------------------------------------------------------
SID_TUNE_Wait
	movwf	TMP1
	clrwdt
SID_TUNE_WaitLoop
	movlw	10
	call	MIOS_Delay
	decfsz	TMP1, F
	rgoto	SID_TUNE_WaitLoop
	return
