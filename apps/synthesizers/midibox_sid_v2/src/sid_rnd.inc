; $Id$
;
; SID Patch Randomizer
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
;;  OUT: new random number in SID_RANDOM_SEED_[LH]
;; --------------------------------------------------------------------------
SID_RND_GenRandomNumber
	SET_BSR	SID_BASE
	movf	SID_RANDOM_SEED_L, W, BANKED
	mulwf	SID_RANDOM_SEED_H, BANKED
	movf	TMR0L, W
	addwf	PRODL, W
	movwf	SID_RANDOM_SEED_L, BANKED
	movlw	0x69
	addwfc	TMR1L, W
	addwfc	PRODH, W
	movwf	SID_RANDOM_SEED_H, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function returns a scaled 8bit random number
;;  IN: min value in MIOS_PARAMETER1, max value in MIOS_PARAMETER2
;;  OUT: scaled value in WREG
;; --------------------------------------------------------------------------
SID_RND_GetScaled
	;; generate new random number
	rcall	SID_RND_GenRandomNumber

	;; 16bit seed -> 8bit value
	SET_BSR	SID_BASE
	rrf	SID_RANDOM_SEED_H, W, BANKED
	xorwf	SID_RANDOM_SEED_L, W, BANKED
	movwf	PRODH

	;; range: max-min+1 = MIOS_PARAMETER2 - MIOS_PARAMETER1 + 1
	movf	MIOS_PARAMETER1, W
	subwf	MIOS_PARAMETER2, W
	addlw	1

	;; skip scaling if zero (-> range = 256)
	bz	SID_RND_GetScaled_SkipMul

SID_RND_GetScaled_Mul
	mulwf	PRODH, ACCESS

SID_RND_GetScaled_SkipMul
	movf	PRODH, W
	addwf	MIOS_PARAMETER1, W
	return


;; --------------------------------------------------------------------------
;;  This function randomizes a given range from FSR1 to FSR0
;;  modification mask in WREG
;; --------------------------------------------------------------------------
SID_RND_GenerateRange
	movlw	0xff		; (all bits will be modified)
SID_RND_GenerateRange_Masked	; (selected bits will be modified)
	movwf	MIOS_PARAMETER3
	clrf	MIOS_PARAMETER1	; min
	setf	MIOS_PARAMETER2	; max
SID_RND_GenerateRangeLoop
	rcall	SID_RND_GetScaled

	;; only modify selected bits
	andwf	MIOS_PARAMETER3, W
	movwf	PRODL
	comf	MIOS_PARAMETER3, W
	andwf	INDF1, W
	iorwf	PRODL, W
	movwf	POSTINC1

	movf	FSR1L, W
	cpfseq	FSR0L, ACCESS
	rgoto SID_RND_GenerateRangeLoop
	movf	FSR1H, W
	cpfseq	FSR0H, ACCESS
	rgoto SID_RND_GenerateRangeLoop
	return


;; --------------------------------------------------------------------------
;;  This function returns a randomized SID_Ix_Vx_WAVEFORM parameter
;;  OUT: waveform in WREG
;; --------------------------------------------------------------------------
SID_RND_GetIxVxWaveform
	clrf	MIOS_PARAMETER1		; tri..noise (9 possible values)
	movlw	8
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	TABLAT
	
	movlw	3			; Sync/Ring
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	swapf	WREG, W
	rlf	WREG, W
	andlw	0x60
	iorwf	TABLAT, W
	return

;; --------------------------------------------------------------------------
;;  This function returns a randomized SID_Ix_LFOx_MODE parameter
;;  OUT: LFO mode in WREG
;; --------------------------------------------------------------------------
SID_RND_GetIxLFOxMode
	clrf	MIOS_PARAMETER1		; lower nibble (On/Off, Sync, etc)
	movlw	15			
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	TABLAT
	
	movlw	8			; 9 waveforms
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	swapf	WREG, W
	iorwf	TABLAT, W
	return


;; --------------------------------------------------------------------------
;;  This function returns a randomized SID_Ix_MODx_SRCx parameter
;;  OUT: MOD source in WREG
;; --------------------------------------------------------------------------
SID_RND_GetIxMODxSrc
	movlw	1			; 1.. number of sources+1
	movwf	MIOS_PARAMETER1		
	movlw	SID_MOD_SRC_NUM+1
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	TABLAT

	addlw	-(SID_MOD_SRC_NUM+1)	; last one is a "constant value"
	bnz	SID_RND_GetIxMODxSrc_NoConst
SID_RND_GetIxMODxSrc_Const
	movlw	0x7f			; get 0x00..0x7f value
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	iorlw	0x80
	return

SID_RND_GetIxMODxSrc_NoConst
	movf	TABLAT, W
	return


;; --------------------------------------------------------------------------
;;  This function returns a randomized SID_Ix_MODx_OP parameter
;;  OUT: MOD operator in WREG
;; --------------------------------------------------------------------------
SID_RND_GetIxMODxOP
	movlw	1			; 1..12
	movwf	MIOS_PARAMETER1		
	movlw	12
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	TABLAT

	movlw	0x0f			; upper nibble is random
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	swapf	WREG, W
	iorwf	TABLAT, W
	return


;; --------------------------------------------------------------------------
;;  This function returns a randomized SID_Ix_MODx_TARGx parameter
;;  OUT: MOD target in WREG
;; --------------------------------------------------------------------------
SID_RND_GetIxMODxTarg
	movlw	7			; 7.. number of targets (skip off and pitch)
	movwf	MIOS_PARAMETER1		
	movlw	SID_MOD_TARG_NUM
	movwf	MIOS_PARAMETER2
	rgoto	SID_RND_GetScaled


;; --------------------------------------------------------------------------
;;  Partial Generate
;;  index in WREG must match with the CS_MENU_TABLE_RND menu entries
;; --------------------------------------------------------------------------
SID_RND_GeneratePart
	JUMPTABLE_2BYTES 17 ; entries
	rgoto	SID_RND_GenerateAll
	rgoto	SID_RND_GenerateOsc
	rgoto	SID_RND_GenerateFil
	rgoto	SID_RND_GenerateLFO
	rgoto	SID_RND_GenerateENV
	rgoto	SID_RND_GenerateMod
	rgoto	SID_RND_GenerateTrg
	rgoto	SID_RND_GenerateWTC
	rgoto	SID_RND_GenerateWTE
	rgoto	SID_RND_GenerateKnb
	rgoto	SID_RND_GenerateExt
	rgoto	SID_RND_GenerateNte	; only for bassline
	rgoto	SID_RND_GenerateGte	; only for bassline
	rgoto	SID_RND_GenerateSld	; only for bassline
	rgoto	SID_RND_GenerateAcc	; only for bassline
	rgoto	SID_RND_GeneratePar	; only for bassline
	rgoto	SID_RND_GenerateO23	; only for bassline
	rgoto	SID_RND_GenerateDSeq	; only for drums

;; ------------------------------------------------------------------
;; Oscillators
;; ------------------------------------------------------------------
SID_RND_GenerateOsc
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_RND_GenerateOsc_DM
SID_RND_GenerateOsc_LB
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateOsc_B

SID_RND_GenerateOsc_L
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_S1V1_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_S1V1_BASE + 2*(SID_Ix_L_S2V1_BASE-SID_Ix_L_S1V1_BASE)
	rcall	SID_RND_GenerateRange

	;; Phase between oscillators always 0 (no sync)
	SET_BSR	SID_EDIT_BUFFER
	clrf	SID_EDIT_BUFFER + SID_Ix_L_OSC_PHASE, BANKED

	;; detune between 0..31
	movlw	0x1f
	andwf	SID_EDIT_BUFFER + SID_Ix_L_OSC_DETUNE, BANKED

	;; detune mode always 0
	SET_BSR	SID_EDIT_BUFFER
	clrf	SID_EDIT_BUFFER + SID_Ix_L_OSC_DTM, BANKED

	;; loop through left channel voices
	SET_BSR	SID_BASE
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_S1V1_BASE
	movlw	3
	movwf	TMP1
SID_RND_GenerateOsc_L_Loop
	rcall	SID_RND_GenerateOsc_Gen
	movlw	0x10
	addwf	FSR1L, F
	decfsz	TMP1, F
	rgoto	SID_RND_GenerateOsc_L_Loop

	;; SIDR is just a copy of SIDL to improve modulated stereo effects :)
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_S1V1_BASE
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_S2V1_BASE
	movlw	3*0x10
	movwf	TMP1
SID_RND_GenerateOsc_L_RLoop
	movff	POSTINC0, POSTINC1
	decfsz	TMP1, F
	rgoto	SID_RND_GenerateOsc_L_RLoop
	return

SID_RND_GenerateOsc_B
	;; loop through 2 voices
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateOsc_B_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	0x10		; OSC is range 0x00-0x0f
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwf	FSR1H, F
	rcall	SID_RND_GenerateOsc_Gen

	;; transpose by -2 octaves
	movlw	SID_Ix_Vx_TRANSPOSE
	movf	PLUSW1, W
	addlw	-24
	movwf	PRODL
	movlw	SID_Ix_Vx_TRANSPOSE
	movff	PRODL, PLUSW1

	;; increase portamento
	movlw	SID_Ix_Vx_PORTAMENTO
	movf	PLUSW1, W
	addlw	30
	movwf	PRODL
	movlw	SID_Ix_Vx_PORTAMENTO
	movff	PRODL, PLUSW1

	;; clean ADSR parameters for bassline typical sounds
	movlw	SID_Ix_Vx_AD
	clrf	PLUSW1
	movlw	0xc0
	movwf	PRODL
	movlw	SID_Ix_Vx_SR
	movff	PRODL, PLUSW1

	incf	TMP1, F
	movlw	2-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateOsc_B_Loop
	return


SID_RND_GenerateOsc_DM
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateOsc_M
SID_RND_GenerateOsc_D
	;; loop through all 16 instruments
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateOsc_D_Loop
	movf	TMP1, W
	rcall	SID_RND_GenerateOsc_D_Gen

	incf	TMP1, F
	movlw	16-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateOsc_D_Loop
	return

SID_RND_GenerateOsc_M
	;; loop through all 6 instruments
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateOsc_M_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	0x10		; OSC is range 0x00-0x0f
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwf	FSR1H, F
	rcall	SID_RND_GenerateOsc_Gen

	incf	TMP1, F
	movlw	6-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateOsc_M_Loop
	return


;; help function for Lead and Multi Engine
SID_RND_GenerateOsc_Gen
	clrf	PRODL			; voice delay always 0
	movlw	SID_Ix_Vx_DELAY
	movff	PRODL, PLUSW1

	clrf	MIOS_PARAMETER1		; transpose by +/- 1 octave only
	movlw	3
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	mullw	12
	movlw	0x40 - 1*12
	addwf	PRODL, F
	movlw	SID_Ix_Vx_TRANSPOSE
	movff	PRODL, PLUSW1

	movlw	0x80-10			; finetune: only +/- 10
	movwf	MIOS_PARAMETER1
	movlw	0x80+10
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	PRODL
	movlw	SID_Ix_Vx_FINETUNE
	movff	PRODL, PLUSW1

	movlw	2			; pitchrage always 2 semitones
	movwf	PRODL
	movlw	SID_Ix_Vx_PITCHRANGE
	movff	PRODL, PLUSW1

	rcall	SID_RND_GetIxVxWaveform	; special treatment for waveform
	movwf	PRODL
	movlw	SID_Ix_Vx_WAVEFORM
	movff	PRODL, PLUSW1

	clrf	MIOS_PARAMETER1		; minimize portamento effect
	movlw	15			
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	PRODL
	movlw	SID_Ix_Vx_PORTAMENTO
	movff	PRODL, PLUSW1
	
	clrf	PRODL			; disable ARP
	movlw	SID_Ix_Vx_ARP_MODE
	movff	PRODL, PLUSW1

	;; ensure that GSA flag not set
	movlw	SID_Ix_Vx_FLAGS1
	bcf	PLUSW1, SID_I_V_FLAGS1_GSA
	return

;; help function for Drum Engine (also called from cs_menu_exec.inc)
;; expects instrument number in WREG
SID_RND_GenerateOsc_D_Gen
	;; temporary store instrument in TMP5
	movwf	TMP5

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_D_I1_BASE
	movf	TMP5, W
	mullw	SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	movlw	0x0a		; instrument range 0x00-0x09
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_D_I1_BASE
	movf	TMP5, W
	mullw	SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	;; flags always cleared
	movlw	SID_Ix_Dx_FLAGS1
	clrf	PLUSW1

	;; attack always 0
	movlw	SID_Ix_Dx_AD
	movf	PLUSW1, W
	andlw	0x0f
	movwf	PRODL
	movlw	SID_Ix_Dx_AD
	movff	PRODL, PLUSW1

	;; consider drum model range
	clrf	MIOS_PARAMETER1
	movlw	SID_DMODEL_NUM-1
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	PRODL
	movlw	SID_Ix_Dx_MODEL
	movff	PRODL, PLUSW1

	;; velocity assignment always disabled
	movlw	SID_Ix_Dx_VELOCITY_ASSGN
	clrf	PLUSW1
	return	

;; ------------------------------------------------------------------
;; Filters
;; ------------------------------------------------------------------
SID_RND_GenerateFil
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_S1F_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_S1F_BASE + 2*(SID_Ix_L_S2F_BASE-SID_Ix_L_S1F_BASE)
	rcall	SID_RND_GenerateRange

	;; ------------------------------------------------------------------
	;; Filter CutOff always 1/4 (+/- 255) for better modulation
	;; filter channels/mode identical (for better stereo effects)
	;; 3off and EXT flag always cleared
	;; ------------------------------------------------------------------
	movlw	0x04		; (range 0x00..0x0f)
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_S1F_BASE + SID_Ix_L_Fx_CUTOFF_H
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_S2F_BASE + SID_Ix_L_Fx_CUTOFF_H

	movff	SID_EDIT_BUFFER + SID_Ix_L_S1F_BASE + SID_Ix_L_Fx_CHN_MODE, WREG
	andlw	0x77
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_S1F_BASE + SID_Ix_L_Fx_CHN_MODE
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_S2F_BASE + SID_Ix_L_Fx_CHN_MODE

	movlw	0x00
	movff	SID_EDIT_BUFFER + SID_Ix_L_S1F_BASE + SID_Ix_L_Fx_KEYTRACK, WREG
	movff	SID_EDIT_BUFFER + SID_Ix_L_S2F_BASE + SID_Ix_L_Fx_KEYTRACK, WREG

	return


;; ------------------------------------------------------------------
;; LFOs
;; ------------------------------------------------------------------
SID_RND_GenerateLFO
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_RND_GenerateLFO_DM
SID_RND_GenerateLFO_LB
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateLFO_B

SID_RND_GenerateLFO_L
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_LFO1_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_LFO1_BASE + 6*(SID_Ix_L_LFO2_BASE-SID_Ix_L_LFO1_BASE)
	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_LFO1_BASE
	movlw	6
	movwf	TMP1
SID_RND_GenerateLFOLoop
	rcall	SID_RND_GenerateLFO_Gen
	movlw	5			; loop through all LFOs
	addwf	FSR1L, F
	decfsz	TMP1, F
	rgoto	SID_RND_GenerateLFOLoop
	return

SID_RND_GenerateLFO_B
	;; loop through all 2 voices
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateLFO_B_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_LFO1_MODE
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	SID_Ix_B_Vx_ENV_MODE-SID_Ix_B_Vx_LFO1_MODE	; LFO is range 0x14-0x21
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_LFO1_MODE
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwf	FSR1H, F
	rcall	SID_RND_GenerateLFO_Gen			; LFO1
	movlw	SID_Ix_B_Vx_LFO2_MODE-SID_Ix_B_Vx_LFO1_MODE
	addwf	FSR1L, F
	rcall	SID_RND_GenerateLFO_Gen			; LFO2

	incf	TMP1, F
	movlw	2-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateLFO_B_Loop
	return


SID_RND_GenerateLFO_DM
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateLFO_M
SID_RND_GenerateLFO_D
	;; not relevant
	return

SID_RND_GenerateLFO_M
	;; loop through all 6 instruments
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateLFO_M_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_M_Vx_LFO1_MODE
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	SID_Ix_M_Vx_ENV_MODE-SID_Ix_M_Vx_LFO1_MODE	; LFO is range 0x14-0x21
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_M_Vx_LFO1_MODE
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwf	FSR1H, F
	rcall	SID_RND_GenerateLFO_Gen			; LFO1
	movlw	SID_Ix_M_Vx_LFO2_MODE-SID_Ix_M_Vx_LFO1_MODE
	addwf	FSR1L, F
	rcall	SID_RND_GenerateLFO_Gen			; LFO2

	incf	TMP1, F
	movlw	6-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateLFO_M_Loop
	return


;; help function for Lead and Multi Engine
SID_RND_GenerateLFO_Gen
	rcall	SID_RND_GetIxLFOxMode	; special treatment for LFO mode
	movwf	PRODL
	movlw	SID_Ix_LFOx_MODE
	movff	PRODL, PLUSW1

	clrf	MIOS_PARAMETER1		; only reduced delay (0..31)
	movlw	31
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	PRODL
	movlw	SID_Ix_LFOx_DELAY
	movff	PRODL, PLUSW1
	return

;; ------------------------------------------------------------------
;; ENVs
;; ------------------------------------------------------------------
SID_RND_GenerateENV
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_RND_GenerateENV_DM
SID_RND_GenerateENV_LB
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateENV_B

SID_RND_GenerateENV_L
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_ENV1_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_ENV1_BASE + 2*(SID_Ix_L_ENV2_BASE-SID_Ix_L_ENV1_BASE)
	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_ENV1_BASE
	movlw	2
	movwf	TMP1
SID_RND_GenerateENV_L_Loop
	movlw	SID_Ix_L_ENVx_MODE	; mode currently forced to zero (to allow future extensions w/o incompatibilities)
	movff	PRODL, PLUSW1

	clrf	MIOS_PARAMETER1		; only reduced delay (0..31)
	movlw	31
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	movwf	PRODL
	movlw	SID_Ix_L_ENVx_DELAY
	movff	PRODL, PLUSW1

	movlw	16			; loop through all ENVs
	addwf	FSR1L, F
	decfsz	TMP1, F
	rgoto	SID_RND_GenerateENV_L_Loop
	return


SID_RND_GenerateENV_B
	;; loop through all 2 voices
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateENV_B_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_ENV_MODE
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	(SID_Ix_B_Vx_ENV_CURVE+1)-SID_Ix_B_Vx_ENV_MODE	; ENV is range 0x22-0x2a
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	incf	TMP1, F
	movlw	2-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateENV_B_Loop
	return

SID_RND_GenerateENV_DM
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateENV_M
SID_RND_GenerateENV_D
	;; not relevant
	return

SID_RND_GenerateENV_M
	;; loop through all 6 instruments
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateENV_M_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_M_Vx_ENV_MODE
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	(SID_Ix_M_Vx_ENV_CURVE+1)-SID_Ix_M_Vx_ENV_MODE	; ENV is range 0x22-0x2a
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	incf	TMP1, F
	movlw	6-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateENV_M_Loop
	return



;; ------------------------------------------------------------------
;; Modulators
;; ------------------------------------------------------------------
SID_RND_GenerateMod
	;; exit if lead engine not selected
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	skpz
	return

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE + 8*(SID_Ix_L_MOD2_BASE-SID_Ix_L_MOD1_BASE)
	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_MOD1_BASE
	movlw	8
	movwf	TMP1
SID_RND_GenerateMODLoop
	rcall	SID_RND_GetIxMODxSrc	; special treatment for MOD source
	movwf	PRODL
	movlw	SID_Ix_MODx_SRC1
	movff	PRODL, PLUSW1

	rcall	SID_RND_GetIxMODxSrc
	movwf	PRODL
	movlw	SID_Ix_MODx_SRC2
	movff	PRODL, PLUSW1

	rcall	SID_RND_GetIxMODxOP	; special treatment for MOD operator
	movwf	PRODL
	movlw	SID_Ix_MODx_OP
	movff	PRODL, PLUSW1

	;; 1:2 chance, that we will 1 of 8 multitargets
	clrf	MIOS_PARAMETER1
	movlw	1
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	bz	SID_RND_GenerateMODLoop_Multi
SID_RND_GenerateMODLoop_Norm
	rcall	SID_RND_GetIxMODxTarg	; special treatment for MOD target
	movwf	PRODL
	movlw	SID_Ix_MODx_TARG_X1
	movff	PRODL, PLUSW1

	rcall	SID_RND_GetIxMODxTarg
	movwf	PRODL
	movlw	SID_Ix_MODx_TARG_X2
	movff	PRODL, PLUSW1

	clrf	PRODL			; multitargets disabled
	movlw	SID_Ix_MODx_TARG_L
	movff	PRODL, PLUSW1
	movlw	SID_Ix_MODx_TARG_R
	movff	PRODL, PLUSW1

	rgoto	SID_RND_GenerateMODLoop_Next

SID_RND_GenerateMODLoop_Multi
	clrf	PRODL			; normal targets disabled
	movlw	SID_Ix_MODx_TARG_X1
	movff	PRODL, PLUSW1
	movlw	SID_Ix_MODx_TARG_X2
	movff	PRODL, PLUSW1

	clrf	MIOS_PARAMETER1		; select one of 8 multitargets for SIDL and SIDR
	movlw	7			
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	call	MIOS_HLP_GetBitORMask
	movwf	PRODL
	movlw	SID_Ix_MODx_TARG_L
	movff	PRODL, PLUSW1
	movlw	SID_Ix_MODx_TARG_R
	movff	PRODL, PLUSW1

	;; 	rgoto	SID_RND_GenerateMODLoop_Next

SID_RND_GenerateMODLoop_Next
	movlw	8			; loop through all MODs
	addwf	FSR1L, F
	decfsz	TMP1, F
	rgoto	SID_RND_GenerateMODLoop

	return
	

;; ------------------------------------------------------------------
;; Triggers
;; ------------------------------------------------------------------
SID_RND_GenerateTrg
	;; exit if lead engine not selected
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	skpz
	return

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_TRG_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_TRG_BASE + 14*(SID_Ix_L_TRG_NOf_BASE-SID_Ix_L_TRG_NOn_BASE)
	rcall	SID_RND_GenerateRange

	movlw	0xff		; (ENV and Gates already set on Note On)
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_TRG_NOn_BASE + 0

	movff	SID_EDIT_BUFFER + SID_Ix_L_TRG_NOf_BASE + 0, WREG
	iorlw	0x3f		; (Gates already released on Note Off)
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_TRG_NOf_BASE + 0

	movff	SID_EDIT_BUFFER + SID_Ix_L_TRG_MSt_BASE + 0, WREG
	andlw	0x3f		; (Gates not retriggered on MIDI Start)
	movff	WREG, SID_EDIT_BUFFER + SID_Ix_L_TRG_MSt_BASE + 0

	return


;; ------------------------------------------------------------------
;; Wavetable Configuration
;; ------------------------------------------------------------------
SID_RND_GenerateWTC
	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_RND_GenerateWTC_DM
SID_RND_GenerateWTC_LB
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateWTC_B

SID_RND_GenerateWTC_L
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE + 14*(SID_Ix_L_WT2_BASE-SID_Ix_L_WT1_BASE)
	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_L_WT1_BASE
	clrf	TMP1
SID_RND_GenerateWTLoop
	rcall	SID_RND_GenerateWTC_Gen
	movlw	5			; loop through all WTs
	addwf	FSR1L, F
	incf	TMP1, F
	BRA_IFCLR TMP1, 2, ACCESS, SID_RND_GenerateWTLoop
	return


SID_RND_GenerateWTC_B
	;; don't touch!
	return

SID_RND_GenerateWTC_DM
	BRA_IFSET WREG, 0, ACCESS, SID_RND_GenerateWTC_M
SID_RND_GenerateWTC_D
	;; not relevant
	return

SID_RND_GenerateWTC_M
	;; loop through all 6 instruments
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateWTC_M_Loop
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_M_Vx_WT_SPEED
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	movlw	0x30-SID_Ix_M_Vx_WT_SPEED	; WTC is range 0x2b-0x2f
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_M_I1_BASE + SID_Ix_M_Vx_WT_SPEED
	movf	TMP1, W
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwf	FSR1H, F
	rcall	SID_RND_GenerateWTC_Gen

	incf	TMP1, F
	movlw	6-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateWTC_M_Loop
	return


;; help function for Lead and Multi Engine
SID_RND_GenerateWTC_Gen
	;; reduced speed 0..15, always modify both SID channels
	clrf	MIOS_PARAMETER1
	movlw	15
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	iorlw	0xc0
	movwf	PRODL
	movlw	SID_Ix_WTx_SPEED
	movff	PRODL, PLUSW1

	;; always start at n*0x20, never activate MOD
	movf	TMP1, W
	mullw	0x20
	movlw	SID_Ix_WTx_BEGIN
	movff	PRODL, PLUSW1

	;; always stop at n*0x3f
	movlw	0x1f
	addwf	PRODL, F
	movlw	SID_Ix_WTx_END
	movff	PRODL, PLUSW1

	;; loop point with 0x00..0x1f or oneshot
	movlw	SID_Ix_WTx_LOOP
	movf	PLUSW1, W
	andlw	0x9f
	movwf	TABLAT
	movf	TMP1, W
	mullw	0x20
	movf	TABLAT, W
	addwf	PRODL, F
	movlw	SID_Ix_WTx_LOOP
	movff	PRODL, PLUSW1
	return


;; ------------------------------------------------------------------
;; Wavetable
;; ------------------------------------------------------------------
SID_RND_GenerateWTE
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_WAVETABLE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_WAVETABLE + 0x80
	rgoto	SID_RND_GenerateRange

;; ------------------------------------------------------------------
;; Knobs
;; ------------------------------------------------------------------
SID_RND_GenerateKnb
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_P_K1_BASE
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_P_K1_BASE + 8*(SID_Ix_P_K2_BASE-SID_Ix_P_K1_BASE)
	rgoto	SID_RND_GenerateRange

;; ------------------------------------------------------------------
;; Externals
;; ------------------------------------------------------------------
SID_RND_GenerateExt
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_EXT_PAR1_L
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_EXT_PAR1_L + 0x10
	rgoto	SID_RND_GenerateRange

;; ------------------------------------------------------------------
;; Bassline: Notes
;; ------------------------------------------------------------------
SID_RND_GenerateNte
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SID_RND_GenerateNte_End

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x80
	movlw	0x3f		; select octave and note field
	rcall	SID_RND_GenerateRange_Masked

	;; ensure that note never greater than 0xc
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
SID_RND_GenerateNte_Loop
	movf	INDF1, W
	andlw	0x0f
	movwf	PRODL
	movlw	0xc
	cpfsgt	PRODL, ACCESS
	rgoto SID_RND_GenerateNte_Loop_Next
	movlw	0xf7
	andwf	INDF1, F
SID_RND_GenerateNte_Loop_Next
	movf	POSTINC1, W	; increment pointer, loop until 0x80 reached
	BRA_IFCLR FSR1L, 7, ACCESS, SID_RND_GenerateNte_Loop

SID_RND_GenerateNte_End
	return

;; ------------------------------------------------------------------
;; Bassline/Drums: Gates
;; ------------------------------------------------------------------
SID_RND_GenerateGte
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SID_RND_GenerateGte_End

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x80
	movlw	0x80		; select gate flag
	rcall	SID_RND_GenerateRange_Masked
SID_RND_GenerateGte_End
	return

;; ------------------------------------------------------------------
;; Bassline: Slides
;; ------------------------------------------------------------------
SID_RND_GenerateSld
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SID_RND_GenerateSld_End

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x80
	movlw	0x40		; select slide flag
	rcall	SID_RND_GenerateRange_Masked
SID_RND_GenerateSld_End
	return


;; ------------------------------------------------------------------
;; Drums/Bassline: Accent
;; ------------------------------------------------------------------
SID_RND_GenerateAcc
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SID_RND_GenerateAcc_End

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x80
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x100
	movlw	0x80		; select accent flag
	rcall	SID_RND_GenerateRange_Masked
SID_RND_GenerateAcc_End
	return


;; ------------------------------------------------------------------
;; Bassline: Parameter Track
;; ------------------------------------------------------------------
SID_RND_GeneratePar
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SID_RND_GeneratePar_End

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x80
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x100
	movlw	0x7f		; select parameter bitfield
	rcall	SID_RND_GenerateRange_Masked
SID_RND_GeneratePar_End
	return

;; ------------------------------------------------------------------
;; Bassline: Oscillator 2+3 settings
;; ------------------------------------------------------------------
SID_RND_GenerateO23
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SID_RND_GenerateO23_End

	;; loop through 2 voices
	SET_BSR	SID_BASE
	clrf	TMP1
SID_RND_GenerateO23_Loop
	;; O23 is range 0x40..0x4f
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_V2_WAVEFORM
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	movlw	0x10
	addwf	FSR1L, W
	movwf	FSR0L
	movlw	0x00
	addwfc	FSR1H, W
	movwf	FSR0H
  	rcall	SID_RND_GenerateRange

	;; get offset again
	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE
	movf	TMP1, W
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	rcall	SID_RND_GetIxVxWaveform	; special treatment for waveform
	movwf	PRODL
	movlw	SID_Ix_B_V2_WAVEFORM
	movff	PRODL, PLUSW1

	rcall	SID_RND_GetIxVxWaveform	; special treatment for waveform
	movwf	PRODL
	movlw	SID_Ix_B_V3_WAVEFORM
	movff	PRODL, PLUSW1

	clrf	MIOS_PARAMETER1		; transpose by +/- 1 octave only
	movlw	1
	movwf	MIOS_PARAMETER2
	rcall	SID_RND_GetScaled
	skpnz
	movlw	0x07
	movwf	PRODL
	movlw	SID_Ix_B_V2_OCT_TRANSPOSE
	movff	PRODL, PLUSW1

	rcall	SID_RND_GetScaled
	skpnz
	movlw	0x07
	movwf	PRODL
	movlw	SID_Ix_B_V3_OCT_TRANSPOSE
	movff	PRODL, PLUSW1

	;; no static notes
	movlw	SID_Ix_B_V2_STATIC_NOTE
	clrf	PLUSW1
	movlw	SID_Ix_B_V3_STATIC_NOTE
	clrf	PLUSW1

	incf	TMP1, F
	movlw	2-1
	cpfsgt	TMP1, ACCESS
	rgoto SID_RND_GenerateO23_Loop

SID_RND_GenerateO23_End
	return

;; ------------------------------------------------------------------
;; Drums: Sequencer Tracks
;; ------------------------------------------------------------------
SID_RND_GenerateDSeq
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bnz	SID_RND_GenerateDSeq_End

	lfsr	FSR1, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x000
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_SEQUENCES + 0x100
	rcall	SID_RND_GenerateRange
SID_RND_GenerateDSeq_End
	return


;; --------------------------------------------------------------------------
;;  Randomize all
;; --------------------------------------------------------------------------
SID_RND_GenerateAll
	;; load the patch preset into edit buffer
	call	SID_PATCH_LoadPresetIntoEdit

	;; modify the different sections
	rcall	SID_RND_GenerateOsc
	rcall	SID_RND_GenerateFil
	rcall	SID_RND_GenerateLFO
	rcall	SID_RND_GenerateENV
	rcall	SID_RND_GenerateMod
	rcall	SID_RND_GenerateTrg
	rcall	SID_RND_GenerateWTC
	rcall	SID_RND_GenerateWTE
	rcall	SID_RND_GenerateKnb
	rcall	SID_RND_GenerateExt
	rcall	SID_RND_GenerateNte
	rcall	SID_RND_GenerateGte
	rcall	SID_RND_GenerateSld
	rcall	SID_RND_GenerateAcc
	rcall	SID_RND_GeneratePar
	rcall	SID_RND_GenerateO23
	rcall	SID_RND_GenerateDSeq
	
	;; give it a name
	lfsr	FSR1, SID_EDIT_BUFFER
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	TABLE_ADDR_MUL_W SID_RND_PATCH_NAME, 16
SID_RND_GenerateAll_NameLoop
	tblrd*+
	movff	TABLAT, POSTINC1
	movlw	0x10-1
	cpfsgt	FSR1L, ACCESS
	rgoto SID_RND_GenerateAll_NameLoop
	
	;; thats all
	return

SID_RND_PATCH_NAME
	db	"Random Lead     "
	db	"Random Bassline "
	db	"Random Drums    "
	db	"Random Multi    "
