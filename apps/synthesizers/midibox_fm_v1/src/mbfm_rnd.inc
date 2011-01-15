; $Id$
;
; MBFM Patch Randomizer
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function generates a new random number
;;  OUT: new random number in MBFM_RANDOM_SEED_[LH]
;; --------------------------------------------------------------------------
MBFM_RND_GenRandomNumber
	SET_BSR	MBFM_BASE
	movf	MBFM_RANDOM_SEED_L, W, BANKED
	mulwf	MBFM_RANDOM_SEED_H, BANKED
	movf	TMR0L, W
	addwf	PRODL, W
	movwf	MBFM_RANDOM_SEED_L, BANKED
	movlw	0x69
	addwfc	TMR1L, W
	addwfc	PRODH, W
	movwf	MBFM_RANDOM_SEED_H, BANKED
	return

#if ENABLE_RANDOM_GENERATOR

;; --------------------------------------------------------------------------
;;  This function returns a scaled 8bit random number
;;  IN: min value in MIOS_PARAMETER1, max value in MIOS_PARAMETER2
;;  OUT: scaled value in WREG
;; --------------------------------------------------------------------------
MBFM_RND_GetScaled
	;; generate new random number
	rcall	MBFM_RND_GenRandomNumber

	;; 16bit seed -> 8bit value
	SET_BSR	MBFM_BASE
	rrf	MBFM_RANDOM_SEED_H, W, BANKED
	xorwf	MBFM_RANDOM_SEED_L, W, BANKED
	movwf	PRODH

	;; range: max-min+1 = MIOS_PARAMETER2 - MIOS_PARAMETER1 + 1
	movf	MIOS_PARAMETER1, W
	subwf	MIOS_PARAMETER2, W
	addlw	1

	;; skip scaling if zero (-> range = 256)
	bz	MBFM_RND_GetScaled_SkipMul

MBFM_RND_GetScaled_Mul
	mulwf	PRODH, ACCESS

MBFM_RND_GetScaled_SkipMul
	movf	PRODH, W
	addwf	MIOS_PARAMETER1, W
	return


;; --------------------------------------------------------------------------
;;  This function randomizes a given range from FSR1 to FSR0
;;  modification mask in WREG
;; --------------------------------------------------------------------------
MBFM_RND_GenerateRange
	movlw	0xff		; (all bits will be modified)
MBFM_RND_GenerateRange_Masked	; (selected bits will be modified)
	movwf	MIOS_PARAMETER3
	clrf	MIOS_PARAMETER1	; min
	setf	MIOS_PARAMETER2	; max
MBFM_RND_GenerateRangeLoop
	rcall	MBFM_RND_GetScaled

	;; only modify selected bits
	andwf	MIOS_PARAMETER3, W
	movwf	PRODL
	comf	MIOS_PARAMETER3, W
	andwf	INDF1, W
	iorwf	PRODL, W
	movwf	POSTINC1

	movf	FSR1L, W
	cpfseq	FSR0L, ACCESS
	rgoto MBFM_RND_GenerateRangeLoop
	movf	FSR1H, W
	cpfseq	FSR0H, ACCESS
	rgoto MBFM_RND_GenerateRangeLoop
	return


;; --------------------------------------------------------------------------
;;  Partial Generate
;;  Expecting instrument number in MBFM_CURRENT_INSTRUMENT
;;  index in WREG must match with the CS_MENU_TABLE_RND menu entries
;; --------------------------------------------------------------------------
MBFM_RND_GeneratePart
	JUMPTABLE_2BYTES 6 ; entries
	rgoto	MBFM_RND_GenerateAll
	rgoto	MBFM_RND_GenerateOP
	rgoto	MBFM_RND_GenerateLFO
	rgoto	MBFM_RND_GenerateEG5
	rgoto	MBFM_RND_GenerateWTC
	rgoto	MBFM_RND_GenerateWTE

;; ------------------------------------------------------------------
;; Oscillators
;; ------------------------------------------------------------------
MBFM_RND_GenerateOP
	call	MBFM_INSTR_GetBase
	movff	FSR1H, FSR0H
	movlw	MBFM_Ix_OP1_FLAGS_MULT
	addwf	FSR1L, F
	movf	FSR1L, W
	addlw	MBFM_Ix_OP34_CH_FB_CNT-MBFM_Ix_OP1_FLAGS_MULT+1
	movwf	FSR0L
	rcall	MBFM_RND_GenerateRange

	;; Modify Mult: better results when OP1/OP3 multiplier between 0..7, and OP2/4 always 0
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_OP1_FLAGS_MULT
	addwf	FSR1L, F

	;; OP1 MULT
	movlw	0xf0
	andwf	INDF1, F

	clrf	MIOS_PARAMETER1
	movlw	7
	movwf	MIOS_PARAMETER2
	rcall	MBFM_RND_GetScaled
	iorwf	POSTINC1, F

	;; OP2 MULT
	movlw	0xf0
	andwf	POSTINC1, F
	
	;; OP3 MULT
	movlw	0xf0
	andwf	INDF1, F

	clrf	MIOS_PARAMETER1
	movlw	7
	movwf	MIOS_PARAMETER2
	rcall	MBFM_RND_GetScaled
	iorwf	POSTINC1, F

	;; OP4 MULT
	movlw	0xf0
	andwf	POSTINC1, F

	;; volume OP1/OP3: between 48..63 to improve results
	;; volume OP2/OP3: always 63 for max output
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_OP1_KSL_TL
	addwf	FSR1L, F

	;; OP1 VOLUME
	movlw	0xc0
	andwf	INDF1, F

	movlw	48
	movwf	MIOS_PARAMETER1
	movlw	63
	movwf	MIOS_PARAMETER2
	rcall	MBFM_RND_GetScaled
	xorlw	0x3f		; (inverted)
	iorwf	POSTINC1, F

	;; OP2 VOLUME
	movlw	0xc0
	andwf	POSTINC1, F
	
	;; OP3 VOLUME
	movlw	0xc0
	andwf	INDF1, F

	movlw	48
	movwf	MIOS_PARAMETER1
	movlw	63
	movwf	MIOS_PARAMETER2
	rcall	MBFM_RND_GetScaled
	xorlw	0x3f		; (inverted)
	iorwf	POSTINC1, F

	;; OP4 VOLUME
	movlw	0xc0		; (inverted)
	andwf	POSTINC1, F


	;; sustain: OP1..4 between 8..15 so that we always hear the OP
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_OP1_SL_RR
	addwf	FSR1L, F

	bcf	POSTINC1, 7	; OP1 sustain: force upper bit, it must be inverted!
	bcf	POSTINC1, 7	; OP2 sustain: force upper bit, it must be inverted!
	bcf	POSTINC1, 7	; OP3 sustain: force upper bit, it must be inverted!
	bcf	POSTINC1, 7	; OP4 sustain: force upper bit, it must be inverted!

	;; WS: ensure that upper bits not set
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_OP1_WS
	addwf	FSR1L, F

	movlw	0x07
	andwf	POSTINC1, F	; OP1 WS [2:0]
	andwf	POSTINC1, F	; OP2 WS [2:0]
	andwf	POSTINC1, F	; OP3 WS [2:0]
	andwf	POSTINC1, F	; OP4 WS [2:0]

	return

;; ------------------------------------------------------------------
;; LFOs
;; ------------------------------------------------------------------
MBFM_RND_GenerateLFO
	call	MBFM_INSTR_GetBase
	movff	FSR1H, FSR0H
	movlw	MBFM_Ix_LFO1_MODE
	addwf	FSR1L, F
	movf	FSR1L, W
	addlw	MBFM_Ix_LFO2_DEPTH_A-MBFM_Ix_LFO1_MODE+1
	movwf	FSR0L
	movlw	0x7f		; only 7bit values
	rcall	MBFM_RND_GenerateRange_Masked

	;; LFO mode: ensure that waveform range is between 0..3
	call	MBFM_INSTR_GetBase

	movlw	MBFM_Ix_LFO1_MODE
	addwf	FSR1L, F
	movlw	0x3f
	andwf	INDF1, F

	movlw	MBFM_Ix_LFO2_MODE-MBFM_Ix_LFO1_MODE
	addwf	FSR1L, F
	movlw	0x3f
	andwf	INDF1, F

	;; apply Vx depth only on OP1 and OP3 (modulator) for better results
	call	MBFM_INSTR_GetBase

	movlw	MBFM_Ix_LFO1_DEPTH_V2
	addwf	FSR1L, F
	movlw	0x40
	movwf	INDF1

	movlw	MBFM_Ix_LFO1_DEPTH_V4-MBFM_Ix_LFO1_DEPTH_V2
	addwf	FSR1L, F
	movlw	0x40
	movwf	INDF1

	movlw	MBFM_Ix_LFO2_DEPTH_V2-MBFM_Ix_LFO1_DEPTH_V4
	addwf	FSR1L, F
	movlw	0x40
	movwf	INDF1

	movlw	MBFM_Ix_LFO2_DEPTH_V4-MBFM_Ix_LFO2_DEPTH_V2
	addwf	FSR1L, F
	movlw	0x40
	movwf	INDF1

	return

;; ------------------------------------------------------------------
;; EG5
;; ------------------------------------------------------------------
MBFM_RND_GenerateEG5
	call	MBFM_INSTR_GetBase
	movff	FSR1H, FSR0H
	movlw	MBFM_Ix_EG5_MODE
	addwf	FSR1L, F
	movf	FSR1L, W
	addlw	MBFM_Ix_EG5_DEPTH_L-MBFM_Ix_EG5_MODE+1
	movwf	FSR0L
	movlw	0x7f		; only 7bit values
	rcall	MBFM_RND_GenerateRange_Masked

	;; mode between 0..15
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_EG5_MODE
	addwf	FSR1L, F
	movlw	0x0f
	andwf	INDF1, F

	;; apply Vx depth only on OP1 and OP3 (modulator) for better results
	call	MBFM_INSTR_GetBase

	movlw	MBFM_Ix_EG5_DEPTH_V2
	addwf	FSR1L, F
	movlw	0x40
	movwf	INDF1

	movlw	MBFM_Ix_EG5_DEPTH_V4-MBFM_Ix_EG5_DEPTH_V2
	addwf	FSR1L, F
	movlw	0x40
	movwf	INDF1

	return


;; ------------------------------------------------------------------
;; Wavetable Configuration
;; ------------------------------------------------------------------
MBFM_RND_GenerateWTC
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_WT_RATE
	addwf	FSR1L, F

	;; generated rate in range 64..127
	;; probability for playing a wavetable: 1:2
	call	MBFM_RND_GenRandomNumber
	iorlw	0x40
	btfsc	WREG, 7
	movlw	0x00
	andlw	0x7f
	movwf	POSTINC1

	;; parameters: select from list
	rcall	MBFM_RND_GenerateWTC_Par
	movwf	POSTINC1	; PAR1
	rcall	MBFM_RND_GenerateWTC_Par
	movwf	POSTINC1	; PAR2
	rcall	MBFM_RND_GenerateWTC_Par
	movwf	POSTINC1	; PAR3
	return

MBFM_RND_GenerateWTC_Par
	TABLE_ADDR MBFM_RND_GenerateWTC_Par_TABLE
	clrf	MIOS_PARAMETER1
	movlw	17-1
	movwf	MIOS_PARAMETER2
	rcall	MBFM_RND_GetScaled
	TABLE_ADD_W
	tblrd*
	movf	TABLAT, W
	return

MBFM_RND_GenerateWTC_Par_TABLE
	db	20, 21, 22, 23, 48, 49, 50, 51, 66, 67, 72, 82, 83, 88, 105, 110, 116

;; ------------------------------------------------------------------
;; Wavetable
;; ------------------------------------------------------------------
MBFM_RND_GenerateWTE
	;; generate 128 random numbers
	;; TODO: write into RAM once feature is available for PIC18F4685

	call	MBFM_INSTR_GetBasePatchAndBank
	movlw	0x80
	movwf	EEADR
MBFM_RND_GenerateWTE_Loop
	call	MBFM_RND_GenRandomNumber
	call	MBFM_BANK_WriteWByte
	BRA_IFSET EEADR, 7, ACCESS, MBFM_RND_GenerateWTE_Loop
	return


;; --------------------------------------------------------------------------
;;  Randomize all
;;  Expecting instrument number in MBFM_CURRENT_INSTRUMENT
;; --------------------------------------------------------------------------
MBFM_RND_GenerateAll
	;; modify the different sections
	rcall	MBFM_RND_GenerateOP
	rcall	MBFM_RND_GenerateLFO
	rcall	MBFM_RND_GenerateEG5
	rcall	MBFM_RND_GenerateWTC
	rcall	MBFM_RND_GenerateWTE
	
	;; give it a name
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_NAME_x
	addwf	FSR1L, F
	TABLE_ADDR MBFM_RND_PATCH_NAME
MBFM_RND_GenerateAll_NameLoop
	tblrd*+
	movff	TABLAT, POSTINC1
	movlw	0x10-1
	cpfsgt	FSR1L, ACCESS
	rgoto MBFM_RND_GenerateAll_NameLoop
	
	;; thats all
	return

MBFM_RND_PATCH_NAME
	db	"Random Patch    "

#endif
