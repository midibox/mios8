; $Id$
;
; MIDIbox FM
; Parameter Input Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  IN: index of parameter in WREG, value in MIOS_PARAMETER1,
;;	instrument in MBFM_CURRENT_INSTRUMENT
;; --------------------------------------------------------------------------
MBFM_PARIN_Set
	;; update shadow parameter as well
	bsf	MBFM_STAT, MBFM_STAT_SHADOW_UPDATE
	rgoto	MBFM_PARIN_Set_Cont

MBFM_PARIN_Set_NoShadowUpdate
	;; don't update shadow parameter (for changed based on CC assignments)
	bcf	MBFM_STAT, MBFM_STAT_SHADOW_UPDATE
	;; 	rgoto	MBFM_PARIN_Set_Cont

MBFM_PARIN_Set_Cont
	;; remember parameter index for shadow operations
	SET_BSR	MBFM_BASE
	movwf	MBFM_PAR_IX, BANKED

	call	MBFM_PAR_TABLE_Get
	;; now:
	;; value     is in MIOS_PARAMETER1
	;; command   is in MIOS_PARAMETER2
	;; parameter is in FSR1 (pointer)

MBFM_PARIN_Set_Cmd
	;; branch depending on command
	movf	MIOS_PARAMETER2, W
	JUMPTABLE_2BYTES 17 ; parameters
	rgoto	MBFM_PARIN_Cmd_NOP
	rgoto	MBFM_PARIN_Cmd_7BIT
	rgoto	MBFM_PARIN_Cmd_7BIT_TO_7_4
	rgoto	MBFM_PARIN_Cmd_7BIT_TO_3_0
	rgoto	MBFM_PARIN_Cmd_INV_7BIT_TO_7_4
	rgoto	MBFM_PARIN_Cmd_INV_7BIT_TO_3_0
	rgoto	MBFM_PARIN_Cmd_7BIT_TO_7_6
	rgoto	MBFM_PARIN_Cmd_INV_7BIT_TO_5_0
	rgoto	MBFM_PARIN_Cmd_7BIT_TO_3_1
	rgoto	MBFM_PARIN_Cmd_7BIT_TO_CNT
	rgoto	MBFM_PARIN_Cmd_7BIT_TO_0
	rgoto	MBFM_PARIN_Cmd_ACHN_ALL
	rgoto	MBFM_PARIN_Cmd_ACHN
	rgoto	MBFM_PARIN_Cmd_MODWHEEL
	rgoto	MBFM_PARIN_Cmd_VELOCITY
	rgoto	MBFM_PARIN_Cmd_AFTERTOUCH
	rgoto	MBFM_PARIN_Cmd_WTPAR

; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function behaves like MBFM_PARIN_Set with the difference that
;;  address 0x00-0x0f writes the patch name
;; --------------------------------------------------------------------------
MBFM_PARIN_SysEx_Set
	movwf	TABLAT
	movlw	0x10
	cpfslt	TABLAT, ACCESS
	rgoto MBFM_PARIN_SysEx_Set_Norm

MBFM_PARIN_SysEx_Set_Name
	call	MBFM_INSTR_GetBase
	movf	TABLAT, W
	movff	MIOS_PARAMETER1, PLUSW1
	return

MBFM_PARIN_SysEx_Set_Norm
	movf	TABLAT, W
	rgoto	MBFM_PARIN_Set

; ==========================================================================

;; --------------------------------------------------------------------------
;; used for modwheel/aftertouch/velocity control of parameters
;; Input:
;;    - modulate value in MIOS_PARAMETER1
;;    - depth in MIOS_PARAMETER2
;;    - init value in MIOS_PARAMETER3
;; Output:
;;    - resulting value (which can be forwarded to MBFM_PARIN_Set) in WREG
;; --------------------------------------------------------------------------
MBFM_PARIN_Modulate
	movf	MIOS_PARAMETER2, W	; multiply depth with value
	andlw	0x3f
	btfss	MIOS_PARAMETER2, 6
	sublw 0x3f
	clrc
	rlf	WREG, F
	rlf	WREG, W
	mulwf	MIOS_PARAMETER1, ACCESS

	movf	PRODH, W
	BRA_IFSET MIOS_PARAMETER2, 6, ACCESS, MBFM_PARIN_Modulate_Pos
MBFM_PARIN_Modulate_Neg
	xorlw	0xff
	addwf	MIOS_PARAMETER3, W
	btfsc	WREG, 7
	movlw 0x00
	goto	MBFM_PARIN_Modulate_Cont
MBFM_PARIN_Modulate_Pos
	addwf	MIOS_PARAMETER3, W
	btfsc	WREG, 7
	movlw 0x7f
MBFM_PARIN_Modulate_Cont
	return

; ==========================================================================


;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_NOP
	return

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT
	movff	MIOS_PARAMETER1, INDF1
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT_TO_7_4
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT_TO_3_0
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_INV_7BIT_TO_7_4
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0xf0
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_INV_7BIT_TO_3_0
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorlw	0x0f
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT_TO_7_6
	movlw	0x3f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_INV_7BIT_TO_5_0
	movlw	0xc0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	xorlw	0x3f
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT_TO_3_1
	movlw	0xf1
	andwf	INDF1, F
	rlf	MIOS_PARAMETER1, W
	andlw	0x0e
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT_TO_CNT
	bcf	INDF1, 0
	btfsc	MIOS_PARAMETER1, 0
	bsf	INDF1, 0
	movf	POSTINC1, W	; increment
	bcf	INDF1, 0
	btfsc	MIOS_PARAMETER1, 1
	bsf	INDF1, 0
	movf	POSTDEC1, W	; decrement

	;; send first byte
	rcall	MBFM_PARIN_Send

	;; EXTRA sending routine for second byte: dirty but ok here
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_OP34_CH_FB_CNT
	movwf	MIOS_PARAMETER1
	movf	PLUSW1, W
	goto	MBFM_INSTR_SendPar

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_7BIT_TO_0
	movlw	0xfe
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x01
	iorwf	INDF1, F
	rgoto	MBFM_PARIN_Send

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_ACHN_ALL
	movff	MIOS_PARAMETER1, POSTINC1
	movff	MIOS_PARAMETER1, POSTDEC1
	rgoto	MBFM_PARIN_Cmd_ACHN_End

MBFM_PARIN_Cmd_ACHN
	movff	MIOS_PARAMETER1, INDF1

MBFM_PARIN_Cmd_ACHN_End
	goto	MBFM_INSTR_SendAudioChn

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_MODWHEEL
	movff	MIOS_PARAMETER1, INDF1

#if USE_SHADOW_BUFFERS
	;; copy new value into shadow buffer
	BRA_IFCLR MBFM_STAT, MBFM_STAT_SHADOW_UPDATE, ACCESS, MBFM_PARIN_Cmd_MODWHEEL_AsgNoSh
MBFM_PARIN_Cmd_MODWHEEL_AsgSh
	movlw	HIGH(MBFM_I1_SHADOW-MBFM_I1_BASE)
	addwf	FSR1H, F
	movff	MIOS_PARAMETER1, INDF1
MBFM_PARIN_Cmd_MODWHEEL_AsgNoSh

	;; ignore if modwheel assigned to itself
	movf	MIOS_PARAMETER1, W
	xorlw	MBFM_Ix_MODWHEEL_ASSIGN
	bz	MBFM_PARIN_Cmd_MODWHEEL_NoAsg

	;; if any assignment changed: restore complete shadow buffer (not only the changed parameter... keep it simple!)
	movf	FSR1L, W
	andlw	0x7f
	xorlw	MBFM_Ix_MODWHEEL_ASSIGN
	bnz	MBFM_PARIN_Cmd_MODWHEEL_NoAsg
MBFM_PARIN_Cmd_MODWHEEL_Asg
	;; copy all parameters from shadow buffer
	call	MBFM_INSTR_CopyFromShadow
MBFM_PARIN_Cmd_MODWHEEL_NoAsg
#endif

	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_MODWHEEL_ASSIGN	; exit when modwheel assigned to 0
	movf	PLUSW1, W
	bz	MBFM_PARIN_Cmd_MODWHEEL_End
	xorlw	1			; prevent endless loop
	bz	MBFM_PARIN_Cmd_MODWHEEL_End
	xorlw	1 ^ MBFM_Px_MODWHEEL_ASSIGN	 ; or to itself
	bz	MBFM_PARIN_Cmd_MODWHEEL_End
	xorlw	MBFM_Px_MODWHEEL_DEPTH ^ MBFM_Px_MODWHEEL_ASSIGN; or to itself
	bz	MBFM_PARIN_Cmd_MODWHEEL_End
	xorlw	MBFM_Px_MODWHEEL_INIT ^ MBFM_Px_MODWHEEL_DEPTH ; or to itself
	bz	MBFM_PARIN_Cmd_MODWHEEL_End

	movlw	MBFM_Ix_MODWHEEL
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	MBFM_Ix_MODWHEEL_DEPTH
	movff	PLUSW1, MIOS_PARAMETER2
	movlw	MBFM_Ix_MODWHEEL_INIT
	movff	PLUSW1, MIOS_PARAMETER3
	rcall	MBFM_PARIN_Modulate
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Ix_MODWHEEL_ASSIGN
	movf	PLUSW1, W
	rcall	MBFM_PARIN_Set_NoShadowUpdate

MBFM_PARIN_Cmd_MODWHEEL_End
	return


;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_VELOCITY
	movff	MIOS_PARAMETER1, INDF1

#if USE_SHADOW_BUFFERS
	;; copy new value into shadow buffer
	BRA_IFCLR MBFM_STAT, MBFM_STAT_SHADOW_UPDATE, ACCESS, MBFM_PARIN_Cmd_VELOCITY_AsgNoSh
MBFM_PARIN_Cmd_VELOCITY_AsgSh
	movlw	HIGH(MBFM_I1_SHADOW-MBFM_I1_BASE)
	addwf	FSR1H, F
	movff	MIOS_PARAMETER1, INDF1
MBFM_PARIN_Cmd_VELOCITY_AsgNoSh

	;; ignore if velocity assigned to itself
	movf	MIOS_PARAMETER1, W
	xorlw	MBFM_Ix_VELOCITY_ASSIGN
	bz	MBFM_PARIN_Cmd_VELOCITY_NoAsg

	;; if any assignment changed: restore complete shadow buffer (not only the changed parameter... keep it simple!)
	movf	FSR1L, W
	andlw	0x7f
	xorlw	MBFM_Ix_VELOCITY_ASSIGN
	bnz	MBFM_PARIN_Cmd_VELOCITY_NoAsg
MBFM_PARIN_Cmd_VELOCITY_Asg
	;; copy all parameters from shadow buffer
	call	MBFM_INSTR_CopyFromShadow
MBFM_PARIN_Cmd_VELOCITY_NoAsg
#endif

MBFM_PARIN_Cmd_VELOCITY_MIDI	; direct entry from mbfm_midi.inc
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_VELOCITY_ASSIGN	; exit when velocity assigned to 0
	movf	PLUSW1, W
	bz	MBFM_PARIN_Cmd_VELOCITY_End
	xorlw	MBFM_Px_VELOCITY_ASSIGN	 ; or to itself
	bz	MBFM_PARIN_Cmd_VELOCITY_End
	xorlw	MBFM_Px_VELOCITY_DEPTH ^ MBFM_Px_VELOCITY_ASSIGN; or to itself
	bz	MBFM_PARIN_Cmd_VELOCITY_End
	xorlw	MBFM_Px_VELOCITY_INIT ^ MBFM_Px_VELOCITY_DEPTH ; or to itself
	bz	MBFM_PARIN_Cmd_VELOCITY_End

	movlw	MBFM_Ix_VELOCITY
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	MBFM_Ix_VELOCITY_DEPTH
	movff	PLUSW1, MIOS_PARAMETER2
	movlw	MBFM_Ix_VELOCITY_INIT
	movff	PLUSW1, MIOS_PARAMETER3
	rcall	MBFM_PARIN_Modulate
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Ix_VELOCITY_ASSIGN
	movf	PLUSW1, W
	rgoto	MBFM_PARIN_Set_NoShadowUpdate

MBFM_PARIN_Cmd_VELOCITY_End
	return


;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_AFTERTOUCH
	movff	MIOS_PARAMETER1, INDF1

#if USE_SHADOW_BUFFERS
	;; copy new value into shadow buffer
	BRA_IFCLR MBFM_STAT, MBFM_STAT_SHADOW_UPDATE, ACCESS, MBFM_PARIN_Cmd_AFTERTOUCH_AsgNoSh
MBFM_PARIN_Cmd_AFTERTOUCH_AsgSh
	movlw	HIGH(MBFM_I1_SHADOW-MBFM_I1_BASE)
	addwf	FSR1H, F
	movff	MIOS_PARAMETER1, INDF1
MBFM_PARIN_Cmd_AFTERTOUCH_AsgNoSh

	;; ignore if aftertouch assigned to itself
	movf	MIOS_PARAMETER1, W
	xorlw	MBFM_Ix_AFTERTOUCH_ASSIGN
	bz	MBFM_PARIN_Cmd_AFTERTOUCH_NoAsg

	;; if any assignment changed: restore complete shadow buffer (not only the changed parameter... keep it simple!)
	movf	FSR1L, W
	andlw	0x7f
	xorlw	MBFM_Ix_AFTERTOUCH_ASSIGN
	bnz	MBFM_PARIN_Cmd_AFTERTOUCH_NoAsg
MBFM_PARIN_Cmd_AFTERTOUCH_Asg
	;; copy all parameters from shadow buffer
	call	MBFM_INSTR_CopyFromShadow
MBFM_PARIN_Cmd_AFTERTOUCH_NoAsg
#endif

MBFM_PARIN_Cmd_AFTERTOUCH_MIDI	; direct entry from mbfm_midi.inc

	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_AFTERTOUCH_ASSIGN	; exit when aftertouch assigned to 0
	movf	PLUSW1, W
	bz	MBFM_PARIN_Cmd_AFTERTOUCH_End
	xorlw	MBFM_Px_AFTERTOUCH_ASSIGN	 ; or to itself
	bz	MBFM_PARIN_Cmd_AFTERTOUCH_End
	xorlw	MBFM_Px_AFTERTOUCH_DEPTH ^ MBFM_Px_AFTERTOUCH_ASSIGN; or to itself
	bz	MBFM_PARIN_Cmd_AFTERTOUCH_End
	xorlw	MBFM_Px_AFTERTOUCH_INIT ^ MBFM_Px_AFTERTOUCH_DEPTH ; or to itself
	bz	MBFM_PARIN_Cmd_AFTERTOUCH_End

	movlw	MBFM_Ix_AFTERTOUCH
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	MBFM_Ix_AFTERTOUCH_DEPTH
	movff	PLUSW1, MIOS_PARAMETER2
	movlw	MBFM_Ix_AFTERTOUCH_INIT
	movff	PLUSW1, MIOS_PARAMETER3
	rcall	MBFM_PARIN_Modulate
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Ix_AFTERTOUCH_ASSIGN
	movf	PLUSW1, W
	rgoto	MBFM_PARIN_Set_NoShadowUpdate

MBFM_PARIN_Cmd_AFTERTOUCH_End
	return

;; --------------------------------------------------------------------------
MBFM_PARIN_Cmd_WTPAR
	movff	MIOS_PARAMETER1, INDF1

#if USE_SHADOW_BUFFERS
	movf	FSR1L, W  ; ignore if assigned to itself
	andlw	0x7f
	xorwf	MIOS_PARAMETER1, W
	bz	MBFM_PARIN_Cmd_WTPAR_NoAsg

	BRA_IFCLR MBFM_STAT, MBFM_STAT_SHADOW_UPDATE, ACCESS, MBFM_PARIN_Cmd_WTPAR_NoSh
MBFM_PARIN_Cmd_WTPAR_Sh
	;; copy new assignment into shadow buffer
	movff	INDF1, PRODH
	movlw	HIGH(MBFM_I1_SHADOW-MBFM_I1_BASE)
	addwf	FSR1H, F
	movff	PRODH, POSTINC1
MBFM_PARIN_Cmd_WTPAR_NoSh
MBFM_PARIN_Cmd_WTPAR_Asg
	;; copy all parameters from shadow buffer
	call	MBFM_INSTR_CopyFromShadow
MBFM_PARIN_Cmd_WTPAR_NoAsg
#endif

	rgoto	MBFM_PARIN_Send


;; --------------------------------------------------------------------------

	;; sends the new parameter to the OPL3 module
	;; IN: value in INDF1
	;;     Ix/DRx/Ex offset in MIOS_PARAMETER3
MBFM_PARIN_Send
#if USE_SHADOW_BUFFERS
	BRA_IFCLR MBFM_STAT, MBFM_STAT_SHADOW_UPDATE, ACCESS, MBFM_PARIN_Send_NoShadowUpdate
MBFM_PARIN_Send_ShadowUpdate
	movff	INDF1, PRODL
	movlw	HIGH(MBFM_I1_SHADOW-MBFM_I1_BASE)
	addwf	FSR1H, F
	movff	PRODL, INDF1
	movlw	HIGH(MBFM_I1_BASE-MBFM_I1_SHADOW)
	addwf	FSR1H, F
MBFM_PARIN_Send_NoShadowUpdate
#endif

	;; don't reinit instruments during sysex write action to save performance
	movlw	0x02
	cpfseq	MBFM_SYSEX_ACTION, BANKED
	rgoto MBFM_PARIN_Send_Ok
	return

MBFM_PARIN_Send_Ok
	movff	MIOS_PARAMETER3, MIOS_PARAMETER1

	;; branch depending on Instrument/Drum/Ensemble
	BRA_IFSET MIOS_PARAMETER3, 7, ACCESS, MBFM_PARIN_Send_Drum
	movlw	MBFM_Ix_ENSEMBLE_BEGIN
	cpfslt	MIOS_PARAMETER3, ACCESS
	rgoto MBFM_PARIN_Send_Ens
MBFM_PARIN_Send_Instr
	movf	INDF1, W
	goto	MBFM_INSTR_SendPar

MBFM_PARIN_Send_Drum
	movf	INDF1, W
	bcf	MIOS_PARAMETER1, 7
	goto	MBFM_DRUM_SendPar

MBFM_PARIN_Send_Ens
	;; patch/bank: re-init patch
	movf	MIOS_PARAMETER3, W
	andlw	0x0e
	bnz	MBFM_PARIN_Send_Ens_NoBP
MBFM_PARIN_Send_Ens_BP
	goto	MBFM_INSTR_Init
	
MBFM_PARIN_Send_Ens_NoBP
	;; (temporary force voice parameter update on a dirty way)
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_FINETUNE
	movwf	MIOS_PARAMETER1
	movf	PLUSW1, W
	goto	MBFM_INSTR_SendPar
