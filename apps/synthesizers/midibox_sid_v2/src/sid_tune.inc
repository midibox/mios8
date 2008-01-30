; $Id$
;
; MIDIbox SID
; Routines to play short tunes (when a BankStick is attached/deattached)
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  SID_PLAY Tune #1
;; --------------------------------------------------------------------------
SID_TUNE_Play1
	rcall	SID_TUNE_NoteGet	; get note depending on BS number (stored in TABLAT)
	rcall	SID_TUNE_Init		; init sound
	movf	TABLAT, W		; play note
	rcall	SID_TUNE_Note_On_Wait01
	rcall	SID_TUNE_Note_Off	; release gate
	movlw	0x04			; wait some mS
	rcall	SID_TUNE_Wait
	rgoto	SID_TUNE_End
	

;; --------------------------------------------------------------------------
;;  SID_PLAY Tune #2
;; --------------------------------------------------------------------------
SID_TUNE_Play2
	rcall	SID_TUNE_NoteGet	; get note depending on BS number (stored in TABLAT)
	rcall	SID_TUNE_Init		; init sound
	movf	TABLAT, W		; play note
	rcall	SID_TUNE_Note_On_Wait01
	rcall	SID_TUNE_Note_Off	; release gate
	movlw	0x04			; wait some mS
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

	SET_BSR SIDL_BASE
	movlw	0x0f
	movwf	SIDL_BASE + SIDx_MODE_VOL, BANKED
	movwf	SIDR_BASE + SIDx_MODE_VOL, BANKED
	clrf	SIDL_BASE + SIDx_RES_FCHN, BANKED
	clrf	SIDR_BASE + SIDx_RES_FCHN, BANKED
	clrf	SIDL_BASE + SIDx_V2_CTRL, BANKED
	clrf	SIDR_BASE + SIDx_V2_CTRL, BANKED
	clrf	SIDL_BASE + SIDx_V3_CTRL, BANKED
	clrf	SIDR_BASE + SIDx_V3_CTRL, BANKED
	movlw	0x88
	clrf	SIDL_BASE + SIDx_V1_ENV_AD, BANKED
	clrf	SIDR_BASE + SIDx_V1_ENV_AD, BANKED
	movwf	SIDL_BASE + SIDx_V1_ENV_SR, BANKED
	movwf	SIDR_BASE + SIDx_V1_ENV_SR, BANKED
	movlw	0x08
	movwf	SIDL_BASE + SIDx_V1_PW_H, BANKED
	movwf	SIDR_BASE + SIDx_V1_PW_H, BANKED
	clrf	SIDL_BASE + SIDx_V1_PW_L, BANKED
	clrf	SIDR_BASE + SIDx_V1_PW_L, BANKED
	return

;; --------------------------------------------------------------------------
;;  SID_PLAY End reinitializes the current SID patch
;; --------------------------------------------------------------------------
SID_TUNE_End
	goto	SID_PATCH_Init			; re-init patch and exit

;; --------------------------------------------------------------------------
;;  SID_PLAY a note (in: Note Number)
;; --------------------------------------------------------------------------
SID_TUNE_Note_On
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	TABLE_ADDR_MUL_W SID_FRQ_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to SIDx_V1_FRQ_[LH]
	movff	TABLAT, SIDL_BASE + SIDx_V1_FRQ_L
	movff	TABLAT, SIDR_BASE + SIDx_V1_FRQ_L
	tblrd*+
	movff	TABLAT, SIDL_BASE + SIDx_V1_FRQ_H
	movff	TABLAT, SIDR_BASE + SIDx_V1_FRQ_H

	movlw	0x41
	movff	WREG, SIDL_BASE + SIDx_V1_CTRL
	movff	WREG, SIDR_BASE + SIDx_V1_CTRL

	goto	_SID_SR_Handler

SID_TUNE_Note_Off
	SET_BSR	SIDL_BASE
	movlw	0x40
	movwf	SIDL_BASE + SIDx_V1_CTRL, BANKED
	movwf	SIDR_BASE + SIDx_V1_CTRL, BANKED
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

;; --------------------------------------------------------------------------
;;  Note Tab for Tune 1/2 (one note for each BS)
;; --------------------------------------------------------------------------
SID_TUNE_NoteGet_TAB
#if 0
	db	0x39, 0x3d, 0x40, 0x45, 0x49, 0x4c, 0x51, 0x55
#else
	db	0x39, 0x3d, 0x40, 0x45, 0x49, 0x4c, 0x51, 0x39	; BS#7 (ENS) plays same note like BS#0 --- suboptimal, but I prefer this tune!
#endif

SID_TUNE_NoteGet
	TABLE_ADDR_MUL_W SID_TUNE_NoteGet_TAB, 1
	tblrd*+
	movf	TABLAT, W
	return
