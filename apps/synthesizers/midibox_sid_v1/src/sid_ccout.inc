; $Id$
;
; MIDIbox SID
; CC Output Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  reacts on a CC dump request
;; --------------------------------------------------------------------------
SID_CCOUT_Handler
	;; do nothing if dump has not been requested
	btfss	SID_STAT, SID_STAT_CC_DUMP_REQ
	return

	;; clear request
	bcf	SID_STAT, SID_STAT_CC_DUMP_REQ

	clrf	TMP1			; TMP1 used as loop counter
SID_CCOUT_Handler_Loop
	movf	TMP1, W
	call	SID_CCOUT_Get		; get CC parameter value
	movwf	TMP2			; store value in TMP2
	BRA_IFSET TMP2, 7, ACCESS, SID_CCOUT_Handler_Next ; (returns WREG[7] if non-CC value)

	;; send CC
	movff	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, WREG
	iorlw	0xb0
	call	MIOS_MIDI_TxBufferPut
	movf	TMP1, W
	call	MIOS_MIDI_TxBufferPut
	movf	TMP2, W
	call	MIOS_MIDI_TxBufferPut

	movlw	8			; add a short delay of 8 mS for slow sequencers
	call	MIOS_Delay

SID_CCOUT_Handler_Next
	incf	TMP1, F			; loop until 128 (bit 8 set)
	BRA_IFCLR TMP1, 7, ACCESS, SID_CCOUT_Handler_Loop
	
	return

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SID_CCOUT_Get
	rcall	SID_CC_TABLE_Get	; prepare a CC action
	;; now:
	;; command   is in MIOS_PARAMETER2
	;; parameter is in FSR1 (pointer)

SID_CCOUT_Set_Cmd
	;; branch depending on command
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SID_CCOUT_Cmd_NOP
	rgoto	SID_CCOUT_Cmd_V123
	rgoto	SID_CCOUT_Cmd_7BIT
	rgoto	SID_CCOUT_Cmd_7BIT; (W_PORTA ignored here)
	rgoto	SID_CCOUT_Cmd_7BIT_4L
	rgoto	SID_CCOUT_Cmd_7BIT_4U
	rgoto	SID_CCOUT_Cmd_7BIT_TO_7_4
	rgoto	SID_CCOUT_Cmd_7BIT_TO_3_0
	rgoto	SID_CCOUT_Cmd_SUSKEY
	rgoto	SID_CCOUT_Cmd_VELOCITY
	rgoto	SID_CCOUT_Cmd_AFTERTOUCH
	rgoto	SID_CCOUT_Cmd_MODWHEEL
	rgoto	SID_CCOUT_Cmd_WAVEFORM
	rgoto	SID_CCOUT_Cmd_MONO
	rgoto	SID_CCOUT_Cmd_POLY
	rgoto	SID_CCOUT_Cmd_NOP; (PLAY_NOTE)

; ==========================================================================


;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_NOP
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_V123
	movf	FSR1L, W		; parameter contains first CC, only return this parameter!
	rgoto	SID_CCOUT_Get

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_7BIT
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_7BIT_4L
	movf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_7BIT_4U
	swapf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_7BIT_TO_7_4
	rrf	INDF1, W
	andlw	0x78
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_7BIT_TO_3_0
	rlf	INDF1, W
	rlf	WREG, F
	rlf	WREG, W
	andlw	0x78
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_SUSKEY
	rrf	SID_KEYTRACKING, W, BANKED
	andlw	0x7e

	btfsc	SID_PLAY_MODE, SID_PLAY_MODE_SUS_KEY, BANKED
	movlw 0x01
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_VELOCITY
	rgoto	SID_CCOUT_Cmd_7BIT

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_AFTERTOUCH
	rgoto	SID_CCOUT_Cmd_7BIT

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_MODWHEEL
	rgoto	SID_CCOUT_Cmd_7BIT

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_WAVEFORM
	swapf	INDF1, W
	andlw	0x6f
	movwf	MIOS_PARAMETER1

	movf	FSR1L, W
	xorlw	SID_V3_CTRL & 0xff
	bz	SID_CCOUT_Cmd_Waveform_V3
	movf	FSR1L, W
	xorlw	SID_V2_CTRL & 0xff
	bz	SID_CCOUT_Cmd_Waveform_V2
SID_CCOUT_Cmd_Waveform_V1
	movlw	(SID_V1_BASE + SID_Vx_MODE) & 0xff
	rgoto	SID_CCOUT_Cmd_Waveform_Cont
SID_CCOUT_Cmd_Waveform_V2
	movlw	(SID_V2_BASE + SID_Vx_MODE) & 0xff
	rgoto	SID_CCOUT_Cmd_Waveform_Cont
SID_CCOUT_Cmd_Waveform_V3
	movlw	(SID_V3_BASE + SID_Vx_MODE) & 0xff
SID_CCOUT_Cmd_Waveform_Cont
	movwf	FSR1L
	movf	MIOS_PARAMETER1, W
	btfsc	INDF1, SID_MODE_VOICE_DISABLED
	iorlw 0x10
	return

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_MONO
	return			; (ignored)

;; --------------------------------------------------------------------------
SID_CCOUT_Cmd_POLY
	movlw	0x00
	btfss	SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED
	movlw 0x01
	btfss	SID_PLAY_MODE, SID_PLAY_MODE_ONLY_WT_OFF, BANKED
	movlw 0x03
	btfsc	SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED
	movlw 0x7f
	return
