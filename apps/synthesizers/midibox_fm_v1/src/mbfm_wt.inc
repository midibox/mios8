; $Id$
;
; MIDIbox FM
; Wavetable Sequencer
;
; MEMO - when MIDI sync is enabled, following WT clock rates are usefull:
;  126:	8 steps = 1/4 note
;  123:	8 steps = 1/2 note
;  117:	8 steps = 1 note
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  MBFM Wavetable Handler
;; --------------------------------------------------------------------------
MBFM_WT_Handler
	SET_BSR	MBFM_BASE			; init BSR

	;; service all instruments
	clrf	MBFM_CURRENT_INSTRUMENT
MBFM_WT_Handler_Loop
	;; get base address to instrument
	call	MBFM_INSTR_GetBase

	;; exit handler if wavetable rate == 0
	movlw	MBFM_Ix_WT_RATE
	movf	PLUSW1, W
	bz	MBFM_WT_Handler_Loop_Next

	;; reset requested?
	movlw	MBFM_Ix_WT_STATE
	BRA_IFCLR PLUSW1, MBFM_WT_STATE_RESET, ACCESS, MBFM_WT_NoReset
	bcf	PLUSW1, MBFM_WT_STATE_RESET
	bcf	PLUSW1, MBFM_WT_STATE_STOP

	;; clear counter & position and request first entry
	IRQ_DISABLE
	movlw	MBFM_Ix_CTRL2
	BRA_IFCLR PLUSW1, MBFM_CTRL2_SYNC_WT, ACCESS, MBFM_WT_Handler_IntClk
MBFM_WT_Handler_ExtClk
	movlw	MBFM_Ix_WT_RATE
	comf	PLUSW1, W
	andlw	0x7f
	addlw	1
	rgoto	MBFM_WT_Handler_ExtClk_C
MBFM_WT_Handler_IntClk
	movlw	MBFM_Ix_WT_RATE		; copy inverted rate * 2 into CTR register
	comf	PLUSW1, W
	clrc
	rlf	WREG, W
MBFM_WT_Handler_ExtClk_C
	movwf	TMP1
	movlw	MBFM_Ix_WT_CTR
	movff	TMP1, PLUSW1

	movlw	MBFM_Ix_WT_POS
	clrf	PLUSW1
	movlw	MBFM_Ix_WT_CLK_REQ_CTR
	clrf	PLUSW1
	bsf	PLUSW1, 0
	IRQ_ENABLE
MBFM_WT_NoReset


	;; handle all requested clocks
MBFM_WT_NextClkLoop
	movlw	MBFM_Ix_WT_CLK_REQ_CTR
	movf	PLUSW1, W
	bz	MBFM_WT_Handler_Loop_Next
	movlw	MBFM_Ix_WT_STATE
	BRA_IFSET PLUSW1, MBFM_WT_STATE_STOP, ACCESS, MBFM_WT_Handler_Loop_Next
	movlw	MBFM_Ix_WT_CLK_REQ_CTR
	decf	PLUSW1, F

	;; get first entry
	rcall	MBFM_WT_Hlp_CalcPos
	rcall	MBFM_WT_ReadByte
	movwf	TMP4
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetBase
#endif
MBFM_WT_Loop
	;; branch depending on entry ID
	movf	TMP4, W
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MBFM_WT_Branch_Inc
	rgoto	MBFM_WT_Branch_Play
	rgoto	MBFM_WT_Branch_Goto
	rgoto	MBFM_WT_Branch_End
	
MBFM_WT_Branch_Play
	;; in note step mode: do only one step
	movlw	MBFM_Ix_CTRL2
	BRA_IFCLR PLUSW1, MBFM_CTRL2_SYNC_WT_NOTESTEP, ACCESS, MBFM_WT_Branch_Play_NoS
	movlw	MBFM_Ix_WT_STATE
	BRA_IFSET PLUSW1, MBFM_WT_STATE_STEP_DONE, ACCESS, MBFM_WT_Handler_Loop_Next
MBFM_WT_Branch_Play_NoS

	;; set WT step flag
	movlw	MBFM_Ix_WT_STATE
	bsf	PLUSW1, MBFM_WT_STATE_STEP_DONE

	;; first parameter
	rcall	MBFM_WT_ReadByte
	movwf	TMP2
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetBase
#endif
	btfsc	TMP4, 4
	bsf	TMP2, 7
	movlw	MBFM_Ix_WT_PAR1
	movf	PLUSW1, W
	skpz
	rcall	MBFM_WT_Hlp_Play_P

	;; second parameter
	rcall	MBFM_WT_ReadByte
	movwf	TMP2
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetBase
#endif
	btfsc	TMP4, 5
	bsf	TMP2, 7
	movlw	MBFM_Ix_WT_PAR2
	movf	PLUSW1, W
	skpz
	rcall	MBFM_WT_Hlp_Play_P

	;; third parameter
	rcall	MBFM_WT_ReadByte
	movwf	TMP2
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetBase
#endif
	btfsc	TMP4, 6
	bsf	TMP2, 7
	movlw	MBFM_Ix_WT_PAR3
	movf	PLUSW1, W
	skpz
	rcall	MBFM_WT_Hlp_Play_P

	rgoto	MBFM_WT_Branch_Inc

	;; ---
MBFM_WT_Branch_End
	movlw	MBFM_Ix_WT_STATE
	bsf	PLUSW1, MBFM_WT_STATE_STOP
	rgoto	MBFM_WT_NextClkLoop
	
	;; ---
MBFM_WT_Branch_Goto
	rcall	MBFM_WT_ReadByte
	andlw	0x1f
	movwf	TMP4
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetBase
#endif
	movlw	MBFM_Ix_WT_POS
	movff	TMP4, PLUSW1
	rcall	MBFM_WT_Hlp_CalcPos
	rcall	MBFM_WT_ReadByte
	movwf	TMP4
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetBase
#endif
	movf	TMP4, W
	andlw	0x03
	xorlw	0x02
	bz	MBFM_WT_Branch_End
	rgoto	MBFM_WT_Loop
	
	;; ---
MBFM_WT_Branch_Inc
	movlw	MBFM_Ix_WT_POS
	incf	PLUSW1, W
	andlw	0x1f
	movwf	TMP4
	movlw	MBFM_Ix_WT_POS
	movff	TMP4, PLUSW1
	rgoto	MBFM_WT_NextClkLoop

;; ---
MBFM_WT_Handler_Loop_Next
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM - 1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_WT_Handler_Loop
	return




;; ---
	;; calculate address to WT entry
MBFM_WT_Hlp_CalcPos
	;; select patch and bank
	movlw	MBFM_Ix_PATCH
	movff	PLUSW1, MBFM_PATCH
	movlw	MBFM_Ix_BANK
	movff	PLUSW1, MBFM_BANK

	movlw	MBFM_Ix_WT_POS
	rlf	PLUSW1, W
	rlf	WREG, W
	andlw	0x7c
	movwf	EEADR
	return
;; ---
	;; play a WT entry
MBFM_WT_Hlp_Play_P
	movwf	TMP3	; save PAR number in TMP3

	BRA_IFSET TMP2, 7, ACCESS, MBFM_WT_Hlp_Play_P_Cont

	;; add/subtract with saturation
	call	MBFM_PAROUT_Get

	BRA_IFSET TMP2, 6, ACCESS, MBFM_WT_Hlp_Play_P_Sub
MBFM_WT_Hlp_Play_P_Add
	addwf	TMP2, F
	BRA_IFCLR TMP2, 7, ACCESS, MBFM_WT_Hlp_Play_P_Cont
	movlw	0x7f
	movwf	TMP2	
	goto	MBFM_WT_Hlp_Play_P_Cont	
MBFM_WT_Hlp_Play_P_Sub
	bsf	TMP2, 7
	addwf	TMP2, F
	BRA_IFCLR TMP2, 7, ACCESS, MBFM_WT_Hlp_Play_P_Cont
	clrf	TMP2
	;; 	rgoto	MBFM_WT_Hlp_Play_P_Cont	

MBFM_WT_Hlp_Play_P_Cont	
	movf	TMP2, W
	andlw	0x7f
	movwf	MIOS_PARAMETER1
	movf	TMP3, W
	call	MBFM_PARIN_Set_NoShadowUpdate

	;; restore base address to instrument
	goto	MBFM_INSTR_GetBase



;; --------------------------------------------------------------------------
;;  Reads a byte from wavetable
;;  Expecting instrument in MBFM_CURRENT_INSTRUMENT and WT offset in EEADR
;;  Overwrites FSR1!!!
;; --------------------------------------------------------------------------
MBFM_WT_ReadByte
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetWTBase
	movf	EEADR, W
	movf	PLUSW1, W
	incf	EEADR, F
	return
#else
	goto	MBFM_BANK_ReadWByte
#endif
