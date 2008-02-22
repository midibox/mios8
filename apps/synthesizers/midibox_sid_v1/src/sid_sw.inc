; $Id$
;
; MIDIbox SID
; Software Synthesizer Engine
; mostly optimized for PIC16F - special adaption for PIC18F to be done
;  
; Activate this #define to measure the performance with a scope
; (connect the probe to J14)
#define SID_SW_MEASURE_PERFORMANCE 0
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Idea for ENV Curve Parameter and OSC synchronization by Jess D. Skov-Nielsen
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  SID Flags
;; ==========================================================================

SID_MODE_VOICE_ACTIVE	EQU	0
SID_MODE_VOICE_DISABLED EQU	1
SID_MODE_PORTA_ENABLE   EQU	2
SID_MODE_GATE_SET_REQ   EQU	3
SID_MODE_GATE_CLR_REQ   EQU	4
SID_MODE_GATE_ACTIVE    EQU	5
SID_MODE_SYNC_UPDATE    EQU	6
SID_MODE_ACCENT		EQU	7

LFO_MODE_ENABLE		EQU	0
LFO_MODE_RESERVED	EQU	1
LFO_MODE_SYNC		EQU	2
LFO_MODE_SYNC_ALL	EQU	3
LFO_MODE_WAVEFORM0	EQU	4
LFO_MODE_WAVEFORM1	EQU	5
LFO_MODE_WAVEFORM2	EQU	6
LFO_MODE_DECINC		EQU	7

ASSIGNED_LFOS_1		EQU	0
ASSIGNED_LFOS_2		EQU	1
ASSIGNED_LFOS_3		EQU	2
ASSIGNED_LFOS_4		EQU	3
ASSIGNED_LFOS_5		EQU	4
ASSIGNED_LFOS_6		EQU	5
ASSIGNED_ENVS_1		EQU	6
ASSIGNED_ENVS_2		EQU	7

ENV_MODE_ATTACK		EQU	0
ENV_MODE_DECAY		EQU	1
ENV_MODE_SUSTAIN	EQU	2
ENV_MODE_RELEASE	EQU	3


WT_STATE_STOP		EQU	0
WT_STATE_RESET		EQU	1

WT_STATE_GATE		EQU	4	; for TB303 option
WT_STATE_SLIDE		EQU	5
WT_STATE_SLIDE_PREV	EQU	6
WT_STATE_PLAY_2ND	EQU	7

SE_OPTION_TB303		EQU	0
SE_OPTION_FIP		EQU	1
SE_OPTION_ENV2PORTA	EQU	2
SE_OPTION_ENV2VOL	EQU	3
SE_OPTION_GSA		EQU	4

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  SID Software Handler: Software Synthesizer part for the SID
;;  called by User Timer every 800 us
;; --------------------------------------------------------------------------
SIDSW_Handler
#if SID_SW_MEASURE_PERFORMANCE
	bsf	PORTD, 4
#endif

	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; return immediately if engine has been disabled
	btfsc	SID_STAT, SID_STAT_ENGINE_DISABLE
	return

	;; handle with MIDI clock
SIDSW_Clk
	incf	SID_MIDI_SYNC_CTR, F, BANKED

	;; register last counter value on 0xf8 or if SID_MIDI_SYNC_CTR == 0xff (no clock received)
	bcf	SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_1, BANKED
	incf	SID_MIDI_SYNC_CTR, W, BANKED
	bz	SIDSW_Clk_F8
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_F8, BANKED, SIDSW_Clk_NoF8
 	bcf	SID_MIDI_SYNC, SID_MIDI_SYNC_F8, BANKED
SIDSW_Clk_F8
	bsf	SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_1, BANKED
	movff	SID_MIDI_SYNC_CTR, SID_MIDI_SYNC_CTR_REG
	clrf	SID_MIDI_SYNC_CTR, BANKED
SIDSW_Clk_NoF8

	;; handle with double resolution of SIDSW clock
	bcf	SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_2, BANKED

	;; if MIDI sync enabled: clock LFOs/ENVs two times on every MIDI clock event
	movf	SID_MIDI_SYNC_CTR, W, BANKED
	skpnz
	bsf	SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_2, BANKED

	clrc
	rrf	SID_MIDI_SYNC_CTR_REG, W, BANKED
	xorwf	SID_MIDI_SYNC_CTR, W, BANKED
	skpnz
	bsf	SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_2, BANKED

	;; A MIDI clock start event restarts all LFOs
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_FA, BANKED, SIDSW_Clk_NoFA
	bcf	SID_MIDI_SYNC, SID_MIDI_SYNC_FA, BANKED
SIDSW_Clk_FA
	call	SIDSW_Hlp_SyncAllLFOs

	btfsc	SID_SE_OPTION, SE_OPTION_TB303, BANKED
	bsf	SID_WT_STATE, WT_STATE_RESET, BANKED

SIDSW_Clk_NoFA

	movff	PRODL, SAVED_PRODL	; save PROD[LH] - we are in an interrupt routine
	movff	PRODH, SAVED_PRODH

	;; generate new pseudo-random number
	movf	SID_LFO_RANDOM_SEED_L, W, BANKED
	mulwf	SID_LFO_RANDOM_SEED_H, BANKED
	movf	TMR0L, W
	addwf	PRODL, W
	movwf	SID_LFO_RANDOM_SEED_L, BANKED
	movlw	0x69
	addwfc	PRODH, W
	movwf	SID_LFO_RANDOM_SEED_H, BANKED


	;; wavetable handler: check for MIDI Sync
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_WT_ARP, BANKED, SIDSW_Wt
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_1, BANKED, SIDSW_Wt_End
SIDSW_Wt
	movf	SID_WT_RATE, W, BANKED
	bz	SIDSW_Wt_End

	movf	SID_WT_CTR, W, BANKED
	bnz	SIDSW_Wt_Next

	comf	SID_WT_RATE, W, BANKED
	andlw	0x7f
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_WT_ARP, BANKED, SIDSW_Wt_IntClk
SIDSW_Wt_ExtClk
	btfss	SID_SE_OPTION, SE_OPTION_TB303, BANKED
	addlw 1
	movwf	SID_WT_CTR, BANKED	
	rgoto	SIDSW_Wt_PlayNext
SIDSW_Wt_IntClk
	skpnz			; never use 0x00 (avoid wdt reset on overloaded engine)
	addlw	1
	movwf	SID_WT_CTR, BANKED
	clrc
	rlf	SID_WT_CTR, F, BANKED
	;; 	rgoto	SIDSW_Wt_PlayNext

SIDSW_Wt_PlayNext
	incf	SID_WT_CLK_REQ_CTR, F, BANKED
	rgoto	SIDSW_Wt_End

SIDSW_Wt_Next
	decf	SID_WT_CTR, F, BANKED
SIDSW_Wt_End

	;; ARPs: check for MIDI Sync
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_WT_ARP, BANKED, SIDSW_ARPs
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_1, BANKED, SIDSW_ARPs_End
SIDSW_ARPs
	lfsr	FSR1, SID_V1_BASE
	rcall	SIDSW_Arp
	lfsr	FSR1, SID_V2_BASE
	rcall	SIDSW_Arp
	lfsr	FSR1, SID_V3_BASE
	rcall	SIDSW_Arp
SIDSW_ARPs_End

	;; LFOs: check for MIDI Sync
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_LFO, BANKED, SIDSW_LFOs
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_2, BANKED, SIDSW_LFOs_End
SIDSW_LFOs
	clrf	SID_SW_LFO_NUMBER, BANKED
	rcall	SIDSW_LFO
	incf	SID_SW_LFO_NUMBER, F, BANKED
	rcall	SIDSW_LFO
	incf	SID_SW_LFO_NUMBER, F, BANKED
	rcall	SIDSW_LFO
	incf	SID_SW_LFO_NUMBER, F, BANKED
	rcall	SIDSW_LFO
	incf	SID_SW_LFO_NUMBER, F, BANKED
	rcall	SIDSW_LFO
	incf	SID_SW_LFO_NUMBER, F, BANKED
	rcall	SIDSW_LFO
SIDSW_LFOs_End

	;; ENVs: check for MIDI Sync
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_ENV, BANKED, SIDSW_ENVs
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_TICK_2, BANKED, SIDSW_ENVs_End
SIDSW_ENVs
	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_ENVs_NotTB303
SIDSW_ENVs_TB303
	;; move ACCENT bit to SID_ENV_CURVES.3 and SID_ENV_CURVES.7
	bcf	SID_ENV_CURVES, 3, BANKED
	bcf	SID_ENV_CURVES, 7, BANKED
	lfsr	FSR1, SID_V1_BASE
	movlw	SID_Vx_MODE
	btfsc	PLUSW1, SID_MODE_ACCENT
	bsf	SID_ENV_CURVES, 3, BANKED
	btfsc	PLUSW1, SID_MODE_ACCENT
	bsf	SID_ENV_CURVES, 7, BANKED
SIDSW_ENVs_NotTB303

	clrf	SID_SW_ENV_NUMBER, BANKED
	movf	SID_ENV_CURVES, W, BANKED
	rcall	SIDSW_ENV

	incf	SID_SW_ENV_NUMBER, BANKED
	swapf	SID_ENV_CURVES, W, BANKED
	rcall	SIDSW_ENV

	;; ensure that SID_ENV_CURVES.3 and SID_ENV_CURVES.7 is always cleared
	bcf	SID_ENV_CURVES, 3, BANKED
	bcf	SID_ENV_CURVES, 7, BANKED
SIDSW_ENVs_End


	clrf	SID_SW_VOICE, BANKED	; loop counter
	lfsr	FSR1, SID_V1_BASE
SIDSW_VoiceLoop
	movlw	SID_Vx_NOTE
	movf	PLUSW1, W
	bz	SIDSW_VoiceLoop_NoPitchChange
SIDSW_VoiceLoop_PitchChange
	rcall	SIDSW_Note
	rcall	SIDSW_Pitch
SIDSW_VoiceLoop_NoPitchChange
	rcall	SIDSW_Porta
	rcall	SIDSW_PWM
SIDSW_VoiceLoop_Next
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_SW_VOICE, F, BANKED
	movlw	3-1
	cpfsgt	SID_SW_VOICE, BANKED
	rgoto SIDSW_VoiceLoop


	;; sound engine option: ENV2 controls volume
	BRA_IFCLR SID_SE_OPTION, SE_OPTION_ENV2VOL, BANKED, SIDSW_NoENV2Vol
SIDSW_ENV2Vol
	movlw	0xf0
	andwf	SID_MODE_VOL, BANKED
	rrf	SID_ENV2_BASE + SID_ENVx_RVALUE_H, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	iorwf	SID_MODE_VOL, F, BANKED
SIDSW_NoENV2Vol

SIDSW_Filter
	;; modulate filter value
	;; assigned LFOs and ENVs in WREG
	swapf	SID_FILTER_ENVS, W, BANKED
	rlf	WREG, F
	rlf	WREG, W
	andlw	0xc0
	iorwf	SID_FILTER_LFOS, W, BANKED
	call	SIDSW_Hlp_GetMOD
	;; result in IRQ_TMP[123]

	;; value *2 & saturate
	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSW_Filter_NoSaturate
	BRA_IFSET IRQ_TMP3, 0, ACCESS, SIDSW_Filter_SaturateNeg
SIDSW_Filter_SaturatePos
	setf	IRQ_TMP1
	setf	IRQ_TMP2
	rgoto	SIDSW_Filter_SaturateCont
SIDSW_Filter_SaturateNeg
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
	rgoto	SIDSW_Filter_SaturateCont
SIDSW_Filter_NoSaturate
	clrc
	btfsc	TMP3, 0
	setc
	rlf	IRQ_TMP1, F
	rlf	IRQ_TMP2, F
SIDSW_Filter_SaturateCont

	;; add offset
	clrf	PRODL
	clrc					; key tracking: add KEYTRACKING * NOTE (converted to 8bit values)
 	rlf	SID_KEYTRACKING, W, BANKED
	movwf	PRODH
	clrc
	rlf	SID_V1_BASE + SID_Vx_NOTE, W, BANKED
	mulwf	PRODH, ACCESS

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_Filter_NotTB303
SIDSW_Filter_TB303
	rrf	SID_FC_VALUE, W, BANKED
	addwf	PRODH, F
	skpnc					; saturate
	setf	PRODH

	rrf	PRODL, F
	rrf	PRODH, W
	rrf	PRODL, F
	andlw	0x1f
#if 0
	addlw	0x10
#else
	addlw	0x00
#endif
	addwf	PRODH, F
	skpnc					; saturate
	setf	PRODH
	call	SIDSW_Hlp_AddOffset16
	rgoto	SIDSW_Filter_TB303_Cont

SIDSW_Filter_NotTB303
	clrc
	rlf	SID_FC_VALUE, W, BANKED		; unconverted cutoff
	addwf	PRODH, F
	skpnc					; saturate
	setf	PRODH
	call	SIDSW_Hlp_AddOffset16
SIDSW_Filter_TB303_Cont
	;; result in IRQ_TMP[12]

	;; direct forward to AOUT
	;; TODO: implement a special AOUT matrix
#if ENABLE_AOUT
	;; (note: will be converted and interpolated below!!!)
	movff	IRQ_TMP1, AOUT0_L
	movff	IRQ_TMP2, AOUT0_H
#endif

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_Filter_ConvNotTB303
SIDSW_Filter_ConvTB303
#if (DEFAULT_FILTER_TYPE&1) || (DEFAULT_FILTER_TYPE_SWITCH && (DEFAULT_FILTER_TYPE2&1))
#if DEFAULT_FILTER_TYPE_SWITCH && (DEFAULT_FILTER_TYPE&1) && !(DEFAULT_FILTER_TYPE2&1)
	BRA_IFSET DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN, ACCESS, SIDSW_Filter_NoTB303CBM8580
#endif
#if DEFAULT_FILTER_TYPE_SWITCH && !(DEFAULT_FILTER_TYPE&1) && (DEFAULT_FILTER_TYPE2&1)
	BRA_IFSET DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN, ACCESS, SIDSW_Filter_NoTB303CBM8580
#endif
	clrc
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	clrc
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
SIDSW_Filter_NoTB303CBM8580
#endif
	rgoto	SIDSW_Filter_ConvTB303Cont
SIDSW_Filter_ConvNotTB303

#if (DEFAULT_FILTER_TYPE&1) || (DEFAULT_FILTER_TYPE_SWITCH && (DEFAULT_FILTER_TYPE2&1))
#if DEFAULT_FILTER_TYPE_SWITCH && (DEFAULT_FILTER_TYPE&1) && !(DEFAULT_FILTER_TYPE2&1)
	BRA_IFSET DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN, ACCESS, SIDSW_Filter_NoCBM8580
#endif
#if DEFAULT_FILTER_TYPE_SWITCH && !(DEFAULT_FILTER_TYPE&1) && (DEFAULT_FILTER_TYPE2&1)
	BRA_IFSET DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN, ACCESS, SIDSW_Filter_NoCBM8580
#endif
SIDSW_Filter_CBM8580
	;; MIDIbox SID V1.6
	rrf	IRQ_TMP2, W
	call	SID_FRQ_TABLE_Get
	movff	MIOS_PARAMETER1, IRQ_TMP1
	movff	MIOS_PARAMETER2, IRQ_TMP2
SIDSW_Filter_NoCBM8580
#endif
SIDSW_Filter_ConvTB303Cont

	;; interpolation of IRQ_TMP[12] when FIP flag not disabled
	lfsr	FSR2, FIP1_BASE
	CALL_IFSET SID_SE_OPTION, SE_OPTION_FIP, BANKED, SIDSW_Hlp_IP

#if DEFAULT_FILTER_TYPE == 2 || DEFAULT_FILTER_TYPE == 3
	;; scale between min/max value
	movlw	LOW((DEFAULT_FILTER_CALI_MAX-DEFAULT_FILTER_CALI_MIN) << 5)
#if DEFAULT_FILTER_TYPE_SWITCH
	btfsc	DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN
	movlw LOW((DEFAULT_FILTER_CALI_MAX2-DEFAULT_FILTER_CALI_MIN2) << 5)
#endif
	movwf	MUL_A_L
	movlw	HIGH((DEFAULT_FILTER_CALI_MAX-DEFAULT_FILTER_CALI_MIN) << 5)
#if DEFAULT_FILTER_TYPE_SWITCH
	btfsc	DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN
	movlw HIGH((DEFAULT_FILTER_CALI_MAX2-DEFAULT_FILTER_CALI_MIN2) << 5)
#endif
	movwf	MUL_A_H
	movff	IRQ_TMP1, MUL_B_L
	movff	IRQ_TMP2, MUL_B_H
	;; calc MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)
	movlw	LOW(DEFAULT_FILTER_CALI_MIN)
#if DEFAULT_FILTER_TYPE_SWITCH
	btfsc	DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN
	movlw LOW(DEFAULT_FILTER_CALI_MIN2)
#endif
	addwf	MUL_R_2, W
	movwf	IRQ_TMP1
	movlw	HIGH(DEFAULT_FILTER_CALI_MIN)
#if DEFAULT_FILTER_TYPE_SWITCH
	btfsc	DEFAULT_FILTER_TYPE_SWITCH_PORT, DEFAULT_FILTER_TYPE_SWITCH_PIN
	movlw HIGH(DEFAULT_FILTER_CALI_MIN2)
#endif
	addwfc	MUL_R_3, W
	movwf	IRQ_TMP2
#endif

	;; copy IRQ_TMP_[12] into SID Filter registers
	swapf	IRQ_TMP1, F
	rrf	IRQ_TMP1, W
	andlw	0x07
	movwf	SID_FC_L, BANKED
	movff	IRQ_TMP2, SID_FC_H

	;; special interpolation for AOUT filter value
#if ENABLE_AOUT
	movff	AOUT0_L, IRQ_TMP1
	movff	AOUT0_H, IRQ_TMP2

	lfsr	FSR2, FIP2_BASE
	CALL_IFSET SID_SE_OPTION, SE_OPTION_FIP, BANKED, SIDSW_Hlp_IP

	;; convert to 12bit
	swapf	IRQ_TMP2, W
	andlw	0x0f
	movwf	AOUT0_H

	swapf	IRQ_TMP2, W
	andlw	0xf0
	movwf	AOUT0_L
	swapf	IRQ_TMP1, W
	andlw	0x0f
	iorwf	AOUT0_L, F
#endif


SIDSW_Handler_End

	;; TODO: implement a special AOUT matrix
#if ENABLE_AOUT
	SET_BSR	SID_BASE

	;; AOUT1: resonance
	movf	SID_RESONANCE_7BIT, W, BANKED
	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_Handler_Res_NoAcc
	BRA_IFCLR SID_V1_BASE + SID_Vx_MODE, SID_MODE_ACCENT, BANKED, SIDSW_Handler_Res_NoAcc
	addlw	0x40
	btfsc	WREG, 7
	movlw 0x7f
SIDSW_Handler_Res_NoAcc
	mullw	32
	movff	PRODL, AOUT1_L
	movff	PRODH, AOUT1_H

	;; AOUT2/3/4: PW OSC1/2/3
	movff	SID_V1_PW_L, AOUT2_L
	movff	SID_V1_PW_H, AOUT2_H
	movff	SID_V2_PW_L, AOUT3_L
	movff	SID_V2_PW_H, AOUT3_H
	movff	SID_V3_PW_L, AOUT4_L
	movff	SID_V3_PW_H, AOUT4_H

	;; AOUT5/6/7: Pitch OSC1/2/3
	swapf	SID_V1_FRQ_L, W, BANKED
	andlw	0x0f
	movwf	AOUT5_L
	swapf	SID_V1_FRQ_H, W, BANKED
	andlw	0xf0
	iorwf	AOUT5_L
	swapf	SID_V1_FRQ_H, W, BANKED
	andlw	0x0f
	movwf	AOUT5_H

	swapf	SID_V2_FRQ_L, W, BANKED
	andlw	0x0f
	movwf	AOUT6_L
	swapf	SID_V2_FRQ_H, W, BANKED
	andlw	0xf0
	iorwf	AOUT6_L, F
	swapf	SID_V2_FRQ_H, W, BANKED
	andlw	0x0f
	movwf	AOUT6_H

	swapf	SID_V3_FRQ_L, W, BANKED
	andlw	0x0f
	movwf	AOUT7_L
	swapf	SID_V3_FRQ_H, W, BANKED
	andlw	0xf0
	iorwf	AOUT7_L, F
	swapf	SID_V3_FRQ_H, W, BANKED
	andlw	0x0f
	movwf	AOUT7_H
#endif

	;; copy 7-bit value of resonance to SID register
	movlw	0x0f
	andwf	SID_RES_FCHN, F, BANKED
	movf	SID_RESONANCE_7BIT, W, BANKED
	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_Handler_Res2_NoAcc
	BRA_IFCLR SID_V1_BASE + SID_Vx_MODE, SID_MODE_ACCENT, BANKED, SIDSW_Handler_Res2_NoAcc
	addlw	0x40
	btfsc	WREG, 7
	movlw 0x7f
SIDSW_Handler_Res2_NoAcc
	rlf	WREG, W
	andlw	0xf0
	iorwf	SID_RES_FCHN, F, BANKED

	;; thats all
	movff	SAVED_PRODL, PRODL		; copy back saved PROD[LH]
	movff	SAVED_PRODH, PRODH

#if SID_SW_MEASURE_PERFORMANCE
	bcf	PORTD, 4
#endif

	return





;; --------------------------------------------------------------------------
;; This function handles the arpeggiator
;; --------------------------------------------------------------------------
SIDSW_Arp
	;; skip if arpeggiator rate == 0
	movlw	SID_Vx_ARP_RATE
	movf	PLUSW1, W
	bz	SIDSW_Arp_End

	;; a counter is incremented on each function call
	;; arpeggiator is stepped forward once the counter has reached the
	;; compare value: rate^0x7f + 1, multiply by 2 if MIDI sync not enabled
	xorlw	0x7f
	addlw	1
	clrc
	btfss	SID_MIDI_SYNC, SID_MIDI_SYNC_WT_ARP, BANKED	; (*2 reduce the rate a little if no MIDI sync)
	rlf	WREG, W
	movwf	IRQ_TMP1	; compare value => IRQ_TMP1

	movlw	1
	movwf	IRQ_TMP2	; incrementer => IRQ_TMP2

	;; special option: constant time arp cycle
	;; if 1 key is pressed, use the original incrementer
	;; if 2 keys are pressed, increment by 2
	;; if 3 keys are pressed, increment by 3
	;; if 4 keys are pressed, increment by 4
	movlw	SID_Vx_NOTE_STACK_1
	movf	PLUSW1, W
	skpz
	incf	IRQ_TMP2, F

	movlw	SID_Vx_NOTE_STACK_2
	movf	PLUSW1, W
	skpz
	incf	IRQ_TMP2, F
	
	movlw	SID_Vx_NOTE_STACK_3
	movf	PLUSW1, W
	skpz
	incf	IRQ_TMP2, F

	;; increment counter
	movlw	SID_Vx_ARP_CTR
	movf	PLUSW1, W
	addwf	IRQ_TMP2, F
	bc	SIDSW_Arp_Overrun
	movlw	SID_Vx_ARP_CTR
	movff	IRQ_TMP2, PLUSW1

	;; branch to the end so long the counter is less than the compare value
	movf	IRQ_TMP2, W
	subwf	IRQ_TMP1, W
	bc	SIDSW_Arp_End

SIDSW_Arp_Overrun
	;; clear counter
	movlw	SID_Vx_ARP_CTR
	clrf	PLUSW1

	;; increment note number (1 of 4)
	movlw	SID_Vx_ARP_NOTE_NUMBER
	incf	PLUSW1, W
	andlw	0x03
	movwf	IRQ_TMP1

	;; reset note number if last one reached (SID_Vx_NOTE_STACK_x is zero)
	movlw	SID_Vx_NOTE_STACK_0
	addwf	IRQ_TMP1, W
	movf	PLUSW1, W
	skpnz
	clrf	IRQ_TMP1

	;; save new note number
	movlw	SID_Vx_ARP_NOTE_NUMBER
	movff	IRQ_TMP1, PLUSW1
	
	;; select note
	movlw	SID_Vx_ARP_NOTE_0
	addwf	IRQ_TMP1, W

	;; save new note number if != zero and != last note
	movf	PLUSW1, W
	bz	SIDSW_Arp_NoNewNote
	movwf	IRQ_TMP1
	movlw	SID_Vx_NOTE
	movf	PLUSW1, W
	xorwf	IRQ_TMP1, W
	bz	SIDSW_Arp_NoNewNote
SIDSW_Arp_NewNote
	movlw	SID_Vx_NOTE
	movff	IRQ_TMP1, PLUSW1

	;; enable portamento if rate is > 0 or alternative portamento enabled
	BRA_IFSET SID_SE_OPTION, SE_OPTION_ENV2PORTA, BANKED, SIDSW_Arp_Porta
	movlw	SID_Vx_PORTA_RATE
	movf	PLUSW1, W
	bz	SIDSW_Arp_NoPorta
SIDSW_Arp_Porta
	movlw	SID_Vx_MODE
	bsf	PLUSW1, SID_MODE_PORTA_ENABLE

	;; store current frequency in SID_Vx_PORTA_FRQ_L
	lfsr	FSR2, SID_V1_FRQ_L
	movf	FSR1L, W
	xorlw	SID_V2_BASE & 0xff
	skpnz
	lfsr	FSR2, SID_V2_FRQ_L
	movf	FSR1L, W
	xorlw	SID_V3_BASE & 0xff
	skpnz
	lfsr	FSR2, SID_V3_FRQ_L

	movlw	SID_Vx_PORTA_FRQ_L
	movff	POSTINC2, PLUSW1
	movlw	SID_Vx_PORTA_FRQ_H
	movff	POSTDEC2, PLUSW1

	;; reset ENV2 if voice 1 (!) and SE flag enabled
	movf	FSR1L, W
	xorlw	SID_V1_BASE & 0xff
	bnz	SIDSW_Arp_NoV1
	CALL_IFSET SID_SE_OPTION, SE_OPTION_ENV2PORTA, BANKED, SID_SW_Hlp_ENV2Reset
SIDSW_Arp_NoV1

SIDSW_Arp_NoPorta

SIDSW_Arp_NoNewNote

SIDSW_Arp_End
	return


;; --------------------------------------------------------------------------
;; This function handles the gates and initial note frequency
;; --------------------------------------------------------------------------
SIDSW_Note
	;; store pointer to SID_Vx_CTRL in FSR2
	lfsr	FSR2, SID_V1_CTRL
	btfsc	SID_SW_VOICE, 0, BANKED
	lfsr	FSR2, SID_V2_CTRL
	btfsc	SID_SW_VOICE, 1, BANKED
	lfsr	FSR2, SID_V3_CTRL

	;; check note delay counter, set/clear gate bit
	movlw	SID_Vx_NOTE_DELAY_CTR
	movf	PLUSW1, W
	bz	SIDSW_Note_NoDelay
	movlw	SID_Vx_NOTE_DELAY_CTR
	decf	PLUSW1, F
	rgoto	SIDSW_Note_DelayCont
SIDSW_Note_NoDelay
	movlw	SID_Vx_MODE
	BRA_IFSET PLUSW1, SID_MODE_GATE_CLR_REQ, ACCESS, SIDSW_Note_GateClrReq
	BRA_IFSET PLUSW1, SID_MODE_GATE_SET_REQ, ACCESS, SIDSW_Note_GateSetReq
	rgoto	SIDSW_Note_DelayCont
SIDSW_Note_GateClrReq
	bcf	PLUSW1, SID_MODE_GATE_CLR_REQ
	btfss	SID_SE_OPTION, SE_OPTION_GSA, BANKED; (don't clear gate bit if GSA (gate stays active) option enabled)
	bcf	INDF2, 0	; SID_Vx_CTRL.0 is the gate bit

	bcf	INDF2, 3	; SID_Vx_CTRL.3 is the test bit which allows to sync an oscillator
	movlw	SID_Vx_MODE
	bcf	PLUSW1, SID_MODE_GATE_ACTIVE

	;; sync assigned ENVs (take also the assigned filter channel into account!)
	movlw	0x01
	btfsc	SID_SW_VOICE, 0, BANKED
	movlw 0x02
	btfsc	SID_SW_VOICE, 1, BANKED
	movlw 0x04
	andwf	SID_RES_FCHN, W, BANKED
	bz	SIDSW_Note_NoFilterREnv
	movf	SID_FILTER_ENVS, W, BANKED
SIDSW_Note_NoFilterREnv
#if ENABLE_AOUT
	movlw	0x03
#endif
	movwf	IRQ_TMP1
	movlw	SID_Vx_ENVS
	movf	PLUSW1, W
	iorwf	IRQ_TMP1, W
	btfsc	SID_SE_OPTION, SE_OPTION_ENV2VOL, BANKED
	iorlw 0x02
	call	SIDSW_Hlp_ENVRelease

	movlw	SID_Vx_MODE
	bsf	PLUSW1, SID_MODE_SYNC_UPDATE	; (cleared by SID_SR_Handler)
	rgoto	SIDSW_Note_DelayCont

SIDSW_Note_GateSetReq
	movlw	SID_Vx_MODE
	BRA_IFSET PLUSW1, SID_MODE_SYNC_UPDATE, ACCESS, SIDSW_Note_DelayCont
	bcf	PLUSW1, SID_MODE_GATE_SET_REQ
	BRA_IFSET PLUSW1, SID_MODE_VOICE_DISABLED, ACCESS, SIDSW_Note_GateSetReqSkp ; dont set if voice disabled

	;; set gate or test bit depending on SID_MODE_OSC_PHASE_SYNC
	movlw	0x01
	btfsc	SID_SW_VOICE, 0, BANKED
	movlw 0x02
	btfsc	SID_SW_VOICE, 1, BANKED
	movlw 0x04
	andwf	SID_OSC_PHASE_SYNC, W, BANKED
	bz	SIDSW_Note_GateSet_NoOSCSync
SIDSW_Note_GateSet_OSCSync
	bsf	INDF2, 3	; SID_Vx_CTRL.3 is the test bit which allows to sync an oscillator
	rgoto	SIDSW_Note_GateSet_OSCSyncCont
SIDSW_Note_GateSet_NoOSCSync
	bsf	INDF2, 0	; SID_Vx_CTRL.0 is the gate bit
	;; 	rgoto	SIDSW_Note_GateSet_OSCSyncCont
SIDSW_Note_GateSet_OSCSyncCont

SIDSW_Note_GateSetReqSkp
	movlw	SID_Vx_MODE
	bsf	PLUSW1, SID_MODE_GATE_ACTIVE

	;; sync assigned LFOs (take also the assigned filter channel into account!)
	movlw	0x01
	btfsc	SID_SW_VOICE, 0, BANKED
	movlw 0x02
	btfsc	SID_SW_VOICE, 1, BANKED
	movlw 0x04
	movf	SID_RES_FCHN, W, BANKED
	bz	SIDSW_Note_NoFilterLFO
	movf	SID_FILTER_LFOS, W, BANKED
SIDSW_Note_NoFilterLFO
	movwf	IRQ_TMP1
	
	movlw	SID_Vx_PITCH_LFOS
	movf	PLUSW1, W
	iorwf	IRQ_TMP1, F
	
	movlw	SID_Vx_PW_LFOS
	movf	PLUSW1, W
	iorwf	IRQ_TMP1, W
	call	SIDSW_Hlp_SyncLFOs

	;; sync assigned ENVs (take also the assigned filter channel into account!)
	movlw	0x01
	btfsc	SID_SW_VOICE, 0, BANKED
	movlw 0x02
	btfsc	SID_SW_VOICE, 1, BANKED
	movlw 0x04
	andwf	SID_RES_FCHN, W, BANKED
	bz	SIDSW_Note_NoFilterAEnv
	movf	SID_FILTER_ENVS, W, BANKED
SIDSW_Note_NoFilterAEnv
#if ENABLE_AOUT
	movlw	0x03
#endif
	movwf	IRQ_TMP1
	movlw	SID_Vx_ENVS
	movf	PLUSW1, W
	iorwf	IRQ_TMP1, W
	btfsc	SID_SE_OPTION, SE_OPTION_ENV2VOL, BANKED
	iorlw 0x02
	call	SIDSW_Hlp_ENVAttack

SIDSW_Note_DelayCont

	movlw	SID_Vx_TRANSPOSE
	movf	PLUSW1, W
	sublw	0x40
	xorlw	0xff
	movwf	IRQ_TMP1
	movlw	SID_Vx_NOTE
	movf	PLUSW1, W
	addwf	IRQ_TMP1, W
	addlw	1
	movwf	IRQ_TMP2
	sublw	0x7d
	bc	SIDSW_Note_NoOverflow
	movlw	SID_Vx_TRANSPOSE
	BRA_IFSET PLUSW1, 6, ACCESS, SIDSW_Note_PosSaturation
SIDSW_Note_NegSaturation
	clrf	IRQ_TMP2
	rgoto	SIDSW_Note_NoOverflow
SIDSW_Note_PosSaturation
	;; (will be done below)
SIDSW_Note_NoOverflow

	;; set target frequency depending on note number
	movf	IRQ_TMP2, W
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	call	SID_FRQ_TABLE_Get
	;; result: low-byte in WREG and MIOS_PARAMETER1, high-byte in MIOS_PARAMETER2
	movlw	SID_Vx_TARGET_FRQ_L
	movff	MIOS_PARAMETER1, PLUSW1
	movlw	SID_Vx_TARGET_FRQ_H
	movff	MIOS_PARAMETER2, PLUSW1
	return


;; --------------------------------------------------------------------------
;; This function handles the Note Pitch
;; --------------------------------------------------------------------------
SIDSW_Pitch
	;; skip Pitchbender+Finetune processing if PITCHRANGE == zero
	movlw	SID_Vx_PITCHRANGE
	movf	PLUSW1, W
	bz	SIDSW_Pitch_MOD

	;; result stored in IRQ_TMP[12]
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2

	;; calc IRQ_TMP[12] = pitchbender+finetune (9-bit signed)
	movlw	SID_Vx_PITCHBENDER
	movf	PLUSW1, W
	movwf	IRQ_TMP3
	bz	SIDSW_Pitch_NoPitchBender
SIDSW_Pitch_PitchBender
	addwf	IRQ_TMP1, F
	movlw	0x00
	btfsc	IRQ_TMP3, 7
	movlw 0xff
	addwfc	IRQ_TMP2, F
SIDSW_Pitch_NoPitchBender

	clrc
	movlw	SID_Vx_FINETUNE
	rlf	PLUSW1, W
	movwf	IRQ_TMP3
	xorlw	0x80
	bz	SIDSW_Pitch_NoFineTune
SIDSW_Pitch_FineTune
	addwf	IRQ_TMP1, F
	movlw	0x00
	btfss	IRQ_TMP3, 7
	movlw 0xff
	addwfc	IRQ_TMP2, F
SIDSW_Pitch_NoFineTune

	;; skip tuning if IRQ_TMP[12] == zero
	movf	IRQ_TMP1, W
	iorwf	IRQ_TMP2, W
	bz	SIDSW_Pitch_MOD

	;; else convert IRQ_TMP[12] to unsigned integer
	rcall	SIDSW_Hlp_GetAbs16
	;; result in IRQ_TMP[12], sign in IRQ_TMP3[0]
	clrc			; multiply with 2
	btfsc	IRQ_TMP3, 0
	setc
	rlf	IRQ_TMP1, W
	movwf	MUL_B_L, BANKED
	rlf	IRQ_TMP2, W
	movwf	MUL_B_H, BANKED

	;; get f_in[target], save it in IRQ_TMP[12]
	movlw	SID_Vx_TRANSPOSE
	movf	PLUSW1, W
	sublw	0x40
	xorlw	0xff
	movwf	IRQ_TMP4
	movlw	SID_Vx_NOTE
	movf	PLUSW1, W
	addwf	IRQ_TMP4, W

	;; add pitchrange depending on direction with saturation
	BRA_IFSET IRQ_TMP3, 0, ACCESS, SIDSW_Pitch_Decrease
SIDSW_Pitch_Increase
	addlw	1
	movwf	IRQ_TMP2
	movlw	SID_Vx_PITCHRANGE
	movf	PLUSW1, W
	addwf	IRQ_TMP2, W
	btfsc	WREG, 7
	movlw 0x7d
	movwf	IRQ_TMP2
	rgoto	SIDSW_Pitch_Increase_Cont
SIDSW_Pitch_Decrease
	movwf	IRQ_TMP2
	movlw	SID_Vx_PITCHRANGE
	decf	PLUSW1, W
	subwf	IRQ_TMP2, F
	btfsc	IRQ_TMP2, 7
	clrf IRQ_TMP2
SIDSW_Pitch_Increase_Cont

	movf	IRQ_TMP2, W
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	call	SID_FRQ_TABLE_Get
	;; result: low-byte in WREG and MIOS_PARAMETER1, high-byte in MIOS_PARAMETER2

	;; add and multiply to target frequency
	movff	FSR1H, FSR2H
	movf	FSR1L, W
	addlw	SID_Vx_TARGET_FRQ_L
	movwf	FSR2L
	call	SIDSW_Hlp_AddMul

SIDSW_Pitch_MOD
	;; get output from modulation matrix
	;; assigned LFOs and ENVs in WREG
	movlw	SID_Vx_ENVS
	swapf	PLUSW1, W
	rlf	WREG, F
	rlf	WREG, W
	andlw	0xc0
	movwf	IRQ_TMP1
	movlw	SID_Vx_PITCH_LFOS
	movf	PLUSW1, W
	iorwf	IRQ_TMP1, W
	call	SIDSW_Hlp_GetMOD
	;; unsigned result in IRQ_TMP[12]
	;; sign in IRQ_TMP3[0]

	;; skip tuning if IRQ_TMP[12] == zero
	movf	IRQ_TMP1, W
	iorwf	IRQ_TMP2, W
	bz	SIDSW_Pitch_CopyFrq

	;; add to target frequency
	movff	FSR1H, FSR2H
	movf	FSR1L, W
	addlw	SID_Vx_TARGET_FRQ_L
	movwf	FSR2L
	call	SIDSW_Hlp_Add16

SIDSW_Pitch_CopyFrq
	movlw	SID_Vx_MODE
	BRA_IFSET PLUSW1, SID_MODE_PORTA_ENABLE, ACCESS, SIDSW_Pitch_End

	;; select pointer to SID_Vx_FRQ_L in FSR2
	lfsr	FSR2, SID_V1_FRQ_L
	btfsc	SID_SW_VOICE, 0, BANKED
	lfsr	FSR2, SID_V2_FRQ_L
	btfsc	SID_SW_VOICE, 1, BANKED
	lfsr	FSR2, SID_V3_FRQ_L

	movlw	SID_Vx_TARGET_FRQ_L
	movff	PLUSW1, POSTINC2
	movlw	SID_Vx_TARGET_FRQ_H
	movff	PLUSW1, POSTDEC2
SIDSW_Pitch_End
	return


;; --------------------------------------------------------------------------
;; This function handles the Portamento
;; --------------------------------------------------------------------------
SIDSW_Porta
	movlw	SID_Vx_MODE
	BRA_IFCLR PLUSW1, SID_MODE_PORTA_ENABLE, ACCESS, SIDSW_Porta_End

	;; select pointer to SID_Vx_FRQ_L in FSR2
	lfsr	FSR2, SID_V1_FRQ_L
	btfsc	SID_SW_VOICE, 0, BANKED
	lfsr	FSR2, SID_V2_FRQ_L
	btfsc	SID_SW_VOICE, 1, BANKED
	lfsr	FSR2, SID_V3_FRQ_L

	;; branch depending on portamento option
	BRA_IFCLR SID_SE_OPTION, SE_OPTION_ENV2PORTA, BANKED, SIDSW_Porta_NORM

	;; ------------------------------------------------------------------
	;; ENV2 controls portamento (constant glide time)
SIDSW_Porta_ENV2

	;; target frequency -> MIOS_PARAMETER[12]
	movlw	SID_Vx_TARGET_FRQ_L
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	SID_Vx_TARGET_FRQ_H
	movff	PLUSW1, MIOS_PARAMETER2

	;; ENV2 counter value -> MUL_A_[LH]
	movff	SID_ENV2_BASE + SID_ENVx_CTR_L, MUL_A_L
	movff	SID_ENV2_BASE + SID_ENVx_CTR_H, MUL_A_H

	;; get difference between target and previous frequency -> IRQ_TMP[12]
	movlw	SID_Vx_PORTA_FRQ_L
	movf	PLUSW1, W
	subwf	MIOS_PARAMETER1, W
	movwf	IRQ_TMP1
	movlw	SID_Vx_PORTA_FRQ_H
	movf	PLUSW1, W
	subwfb	MIOS_PARAMETER2, W
	movwf	IRQ_TMP2

	;; convert IRQ_TMP[12] to absolute value
	rcall	SIDSW_Hlp_GetAbs16
	;; result in IRQ_TMP[12], sign in IRQ_TMP3[0]
	;; increment four to ensure that target will be reached
	movlw	4
	addwf	IRQ_TMP1, F
	movlw	0
	addwfc	IRQ_TMP2, F
	movff	IRQ_TMP1, MUL_B_L
	movff	IRQ_TMP2, MUL_B_H

	;; calc MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)

	;; branch depending on direction
	BRA_IFSET IRQ_TMP3, 0, ACCESS, SIDSW_Porta_ENV2_Down
SIDSW_Porta_ENV2_Up
	;; add scaled value to starting frequency
	movlw	SID_Vx_PORTA_FRQ_L
	movf	PLUSW1, W
	addwf	MUL_R_2, W, BANKED
	movwf	POSTINC2

	movlw	SID_Vx_PORTA_FRQ_H
	movf	PLUSW1, W
	addwfc	MUL_R_3, W, BANKED
	movwf	POSTDEC2

	;; continue at normal portamento routine
	rgoto	SIDSW_Porta_ENV2_Up_Cont

SIDSW_Porta_ENV2_Down
	;; subtract scaled value from starting frequency
	movlw	SID_Vx_PORTA_FRQ_L
	movff	PLUSW1, POSTINC2
	movlw	SID_Vx_PORTA_FRQ_H
	movff	PLUSW1, POSTDEC2

	movf	MUL_R_2, W, BANKED
	subwf	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	subwfb	POSTDEC2, F

	;; continue at normal portamento routine
	rgoto	SIDSW_Porta_ENV2_Down_Cont


	;; ------------------------------------------------------------------
	;; "normal" portamento mode (non-constant glide time)
SIDSW_Porta_NORM

	;; multiply rate with current frequency
	;; get portamento multiplier from envelope table -> MUL_A
	movlw	SID_Vx_PORTA_RATE
	movf	PLUSW1, W
	call	SID_ENV_TABLE_Get
	movff	MIOS_PARAMETER1, MUL_A_L
	movff	MIOS_PARAMETER2, MUL_A_H

	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2

	;; get current frequency -> MUL_B
	movff	POSTINC2, MUL_B_L
	movff	POSTDEC2, MUL_B_H

	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)
	;; ensure that result is != 0
	movf	MUL_R_2, W, BANKED
	iorwf	MUL_R_3, W, BANKED
	skpnz
	incf	MUL_R_2, F, BANKED

	;; SID_Vx_FRQ += result (depending on Portamento Direction)
	movff	MUL_R_2, IRQ_TMP1

	;; store target frequency in MIOS_PARAMETER[12]
	movlw	SID_Vx_TARGET_FRQ_L
	movff	PLUSW1, MIOS_PARAMETER1
	movlw	SID_Vx_TARGET_FRQ_H
	movff	PLUSW1, MIOS_PARAMETER2

	;; branch depending on portamento direction
	;; check if value > current value
	movf	MIOS_PARAMETER1, W
	subwf	POSTINC2, W
	movf	MIOS_PARAMETER2, W
	subwfb	POSTDEC2, W
	bc	SIDSW_Porta_Down

SIDSW_Porta_Up
	movf	MUL_R_2, W
	addwf	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTDEC2, F

SIDSW_Porta_ENV2_Up_Cont	; re-used by ENV2 option
	;; check if value > MAX_VALUE
	movf	POSTINC2, W
	subwf	MIOS_PARAMETER1, W
	movf	POSTDEC2, W
	subwfb	MIOS_PARAMETER2, W
	bc	SIDSW_Porta_End		; branch to end if MAX_VALUE not reached
	rgoto	SIDSW_Porta_Up_Cont		; else copy MAX_VALUE into value and finish portamento

SIDSW_Porta_Down
	movf	MUL_R_2, W
	subwf	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	subwfb	POSTDEC2, F

SIDSW_Porta_ENV2_Down_Cont	; re-used by ENV2 option
	;; check if value < MIN_VALUE
	movf	MIOS_PARAMETER1, W
	subwf	POSTINC2, W
	movf	MIOS_PARAMETER2, W
	subwfb	POSTDEC2, W
	bc	SIDSW_Porta_End		; branch to end if MIN_VALUE not reached
	; else copy MIN_VALUE into value and finish portamento
SIDSW_Porta_Up_Cont
	movlw	SID_Vx_TARGET_FRQ_L
	movff	PLUSW1, POSTINC2
	movlw	SID_Vx_TARGET_FRQ_H
	movff	PLUSW1, POSTDEC2

	movlw	SID_Vx_MODE
	bcf	PLUSW1, SID_MODE_PORTA_ENABLE
SIDSW_Porta_End
	return



;; --------------------------------------------------------------------------
;; This function handles the pulsewidth
;; --------------------------------------------------------------------------
SIDSW_PWM
	;; modulate pulsewidth
	;; assigned LFOs and ENVs in WREG
	movlw	SID_Vx_ENVS
	rlf	PLUSW1, W
	rlf	WREG, W
	andlw	0xc0
	movwf	IRQ_TMP1
	movlw	SID_Vx_PW_LFOS
	movf	PLUSW1, W
	iorwf	IRQ_TMP1, W
	call	SIDSW_Hlp_GetMOD
	;; result in IRQ_TMP[123]

	;; add offset
	movlw	SID_Vx_PW_VALUE
	movf	PLUSW1, W
	call	SIDSW_Hlp_AddOffset
	;; result in IRQ_TMP[12]

	;; select pointer to SID_Vx_PW_L in FSR2
	lfsr	FSR2, SID_V1_PW_L
	btfsc	SID_SW_VOICE, 0, BANKED
	lfsr	FSR2, SID_V2_PW_L
	btfsc	SID_SW_VOICE, 1, BANKED
	lfsr	FSR2, SID_V3_PW_L

	;; copy IRQ_TMP_[12] into SID PW registers
	swapf	IRQ_TMP2, W
	andlw	0xf0
	movwf	INDF2
	swapf	IRQ_TMP1, W
	andlw	0x0f
	iorwf	POSTINC2, F
	swapf	IRQ_TMP2, W
	andlw	0x0f
	movwf	INDF2

	return


;; --------------------------------------------------------------------------
;; This function handles the LFOs
;; --------------------------------------------------------------------------
SIDSW_LFO
	;; LFO number in SID_SW_LFO_NUMBER - calculate base address
	lfsr	FSR1, SID_LFO1_BASE
	movf	SID_SW_LFO_NUMBER, W, BANKED
	mullw	SID_LFOx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F

	;; clear result register and skip LFO if not enabled
	movlw	SID_LFOx_MODE
	BRA_IFSET PLUSW1, LFO_MODE_ENABLE, ACCESS, SIDSW_LFO_Enabled
SIDSW_LFO_Disabled
	movlw	SID_LFOx_RVALUE_L
	clrf	PLUSW1
	movlw	SID_LFOx_RVALUE_H
	clrf	PLUSW1
	rgoto	SIDSW_LFO_End
SIDSW_LFO_Enabled
	
	;; increment step counter, store result also in IRQ_TMP1
	movlw	SID_LFOx_CTR
	incf	PLUSW1, F
	movff	PLUSW1, IRQ_TMP1

	;; get CTR/ADD entry from LFO table depending on LFO Rate
	movlw	SID_LFOx_RATE
	movf	PLUSW1, W
	call	SID_LFO_TABLE_Get
	;; result: CTR value in WREG and MIOS_PARAMETER1, ADD value in MIOS_PARAMETER2

	;; exit if max step counter value (CTR) not reached
	;; 	movf	MIOS_PARAMETER1, W
	subwf	IRQ_TMP1, W	; result of LFOx_CTR in IRQ_TMP1
	bnc	SIDSW_LFO_End

	;; else clear step counter
	movlw	SID_LFOx_CTR
	clrf	PLUSW1

	;; skip multiply routine if LFO_x_DEPTH is 0x40 (zero depth)
	movlw	SID_LFOx_DEPTH
	movf	PLUSW1, W
	xorlw	0x40
	bnz	SIDSW_LFO_DepthOk
SIDSW_LFO_Depth40
	;; clear 16bit result value registers and exit
	movlw	SID_LFOx_RVALUE_L
	clrf	PLUSW1
	movlw	SID_LFOx_RVALUE_H
	clrf	PLUSW1
	rgoto	SIDSW_LFO_End
SIDSW_LFO_DepthOk

	;; add or subtract ADD value to linear LFO value
	movlw	SID_LFOx_VALUE
	movff	PLUSW1, IRQ_TMP3

	movlw	SID_LFOx_MODE
	BRA_IFSET PLUSW1, LFO_MODE_DECINC, ACCESS, SIDSW_LFO_Dec
SIDSW_LFO_Inc
	movf	MIOS_PARAMETER2, W		; get ADD value
	addwf	IRQ_TMP3, F			; add to linear LFO value
	bnc	SIDSW_LFO_Cont			; skip next if max value (0xff) not reached
	comf	IRQ_TMP3, W			; subtract the missing ticks
	addwf	IRQ_TMP3, F
	movlw	SID_LFOx_MODE
	bsf	PLUSW1, LFO_MODE_DECINC		; switch to decrement
	rgoto	SIDSW_LFO_Cont

SIDSW_LFO_Dec
	movf	MIOS_PARAMETER2, W		; get ADD value
	subwf	IRQ_TMP3, F			; decrement from linear LFO value
	bc	SIDSW_LFO_Cont			; skip next if min value (0x00) not reached
	comf	IRQ_TMP3, W			; add the missing ticks
	addlw	1
	addwf	IRQ_TMP3, F
	movlw	SID_LFOx_MODE
	bcf	PLUSW1, LFO_MODE_DECINC		; switch to increment
SIDSW_LFO_Cont

	;; write back IRQ_TMP3 -> SID_LFOx_VALUE
	movlw	SID_LFOx_VALUE
	movff	IRQ_TMP3, PLUSW1

	;; convert linear LFO value to waveform by using the SIDSW_LFO_Hlp_Waveform function
	;; LFO mode in IRQ_TMP1
	movlw	SID_LFOx_MODE
	movff	PLUSW1, IRQ_TMP1
	;; LFO depth in IRQ_TMP2
	movlw	SID_LFOx_DEPTH
	movff	PLUSW1, IRQ_TMP2
	;; linear LFO value in WREG
	movf	IRQ_TMP3, W
	;; process waveform
	call	SIDSW_LFO_Hlp_Waveform
	;; store 16bit result in RVALUE registers
	movlw	SID_LFOx_RVALUE_L
	movff	PRODL, PLUSW1
	movlw	SID_LFOx_RVALUE_H
	movff	PRODH, PLUSW1
SIDSW_LFO_End
	return


;; --------------------------------------------------------------------------
;; This function handles the ENVs
;; expects SID_ENV_CURVES bitfield (lower or upper nibble) in WREG
;; 
;; SID_ENV_CURVES.7 and SID_ENV_CURVES.3 contain the ACCENT flag which
;; is used in TB303 mode (copied to IRQ_TMP4.3
;; --------------------------------------------------------------------------
SIDSW_ENV
	andlw	0x0f
	movwf	IRQ_TMP4

	;; ENV number in SID_SW_ENV_NUMBER - calculate base address
	lfsr	FSR1, SID_ENV1_BASE
	movf	SID_SW_ENV_NUMBER, W, BANKED
	mullw	SID_ENVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F

	;; prepare call of SIDSW_ENV_GetBendedValue
	movlw	SID_ENVx_CURVE
	movff	PLUSW1, IRQ_TMP2
	movlw	SID_ENVx_CTR_H
	movff	PLUSW1, IRQ_TMP1

	;; branch depending on ENV state
	movlw	SID_ENVx_MODE
	BRA_IFSET PLUSW1, ENV_MODE_RELEASE, ACCESS, SIDSW_ENV_Release
	BRA_IFCLR PLUSW1, ENV_MODE_ATTACK, ACCESS, SIDSW_ENV_Calc
	BRA_IFSET PLUSW1, ENV_MODE_SUSTAIN, ACCESS, SIDSW_ENV_Sustain
	BRA_IFSET PLUSW1, ENV_MODE_DECAY, ACCESS, SIDSW_ENV_Decay

SIDSW_ENV_Attack
	;; get attack rate depending on curve setting
	movlw	SID_ENVx_ATTACK
	movff	PLUSW1, IRQ_TMP3
	movf	IRQ_TMP4, W
	andlw	0x01
	call	SIDSW_ENV_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2

	;; add to ENV counter
	movlw	SID_ENVx_CTR_L
	movf	PLUSW1, W
	addwf	MIOS_PARAMETER1, F
	movlw	SID_ENVx_CTR_H
	movf	PLUSW1, W
	addwfc	MIOS_PARAMETER2, F
	bnc	SIDSW_ENV_Calc

	;; if value >= 0xffff: set to 0xffff, switch to Decay
	setf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	movlw	SID_ENVx_MODE
	bsf	PLUSW1, ENV_MODE_DECAY
	rgoto	SIDSW_ENV_Calc

SIDSW_ENV_Decay
	;; get decay rate depending on curve setting
	movlw	SID_ENVx_DECAY
	movff	PLUSW1, IRQ_TMP3

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_ENV_Decay_NotTB303
SIDSW_ENV_Decay_TB303
	;; accent in TB303 mode: decrease decay
	BRA_IFCLR IRQ_TMP4, 3, ACCESS, SIDSW_ENV_Decay_NoAcc
	movlw	-20
	addwf	IRQ_TMP3, F
	btfsc	IRQ_TMP3, 7
	clrf	IRQ_TMP3
SIDSW_ENV_Decay_NoAcc
SIDSW_ENV_Decay_NotTB303

	movf	IRQ_TMP4, W
	andlw	0x02
	call	SIDSW_ENV_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2

	;; subtraction with current counter value
	movlw	SID_ENVx_CTR_L
	movff	PLUSW1, IRQ_TMP1
	movlw	SID_ENVx_CTR_H
	movff	PLUSW1, IRQ_TMP2

	movf	MIOS_PARAMETER1, W
	subwf	IRQ_TMP1, W
	movwf	MIOS_PARAMETER1
	movf	MIOS_PARAMETER2, W
	subwfb	IRQ_TMP2, W
	movwf	MIOS_PARAMETER2
	bnc	SIDSW_ENV_Sustain

	;; check if counter value < sustain value
	movlw	SID_ENVx_SUSTAIN
	rlf	PLUSW1, W
	andlw	0xfe
	movwf	IRQ_TMP1
	movlw	0x00
	subwf	MIOS_PARAMETER1, W
	movf	IRQ_TMP1, W
	subwfb	MIOS_PARAMETER2, W
	bc	SIDSW_ENV_Calc

SIDSW_ENV_Sustain
	;; write sustain value into counter
	movlw	SID_ENVx_SUSTAIN
	rlf	PLUSW1, W
	andlw	0xfe
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	movlw	SID_ENVx_MODE
	bsf	PLUSW1, ENV_MODE_SUSTAIN
	rgoto	SIDSW_ENV_Calc

SIDSW_ENV_Release
	;; get release rate depending on curve setting
	movlw	SID_ENVx_RELEASE
	movff	PLUSW1, IRQ_TMP3
	movf	IRQ_TMP4, W
	andlw	0x04
	call	SIDSW_ENV_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	
	;; subtraction with current counter value
	movlw	SID_ENVx_CTR_L
	movff	PLUSW1, IRQ_TMP1
	movlw	SID_ENVx_CTR_H
	movff	PLUSW1, IRQ_TMP2

	movf	MIOS_PARAMETER1, W
	subwf	IRQ_TMP1, W
	movwf	MIOS_PARAMETER1
	movf	MIOS_PARAMETER2, W
	subwfb	IRQ_TMP2, W
	movwf	MIOS_PARAMETER2
	bc	SIDSW_ENV_Calc

	;; zero reached
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	SID_ENVx_MODE
	clrf	PLUSW1
SIDSW_ENV_Calc
	;; copy MIOS_PARAMETER[12] to SID_ENVx_CTR_[LH]
	movlw	SID_ENVx_CTR_L
	movff	MIOS_PARAMETER1, PLUSW1
	movlw	SID_ENVx_CTR_H
	movff	MIOS_PARAMETER2, PLUSW1

	;; calculate envelope value depending on envelope rate
	
	;; clear ENV ResultValue Registers
	movlw	SID_ENVx_RVALUE_L
	clrf	PLUSW1
	movlw	SID_ENVx_RVALUE_H
	clrf	PLUSW1

	;; skip multiply routine if ENV_x_DEPTH is 0x40
	;; (0x00 in TB303 Mode)
	movlw	SID_ENVx_DEPTH
	movf	PLUSW1, W
	btfss	SID_SE_OPTION, SE_OPTION_TB303, BANKED
	xorlw 0x40
	bz	SIDSW_ENV_End

	;; convert linear ENV value to waveform by using the SIDSW_ENV_Hlp_Waveform function
	;; depth in IRQ_TMP2
	movlw	SID_ENVx_DEPTH
	movff	PLUSW1, IRQ_TMP2
	;; linear ENV value in WREG
	movlw	SID_ENVx_CTR_H
	rrf	PLUSW1, W
	;; process waveform
	call	SIDSW_ENV_Hlp_Waveform
	;; store 16bit result in RVALUE registers
	movlw	SID_ENVx_RVALUE_L
	movff	PRODL, PLUSW1
	movlw	SID_ENVx_RVALUE_H
	movff	PRODH, PLUSW1
SIDSW_ENV_End
	return


;; --------------------------------------------------------------------------
;; Help Function: add modulation values depending on enabled sources
;; In: Enabled LFOs and ENCs in WREG, 7-bit offset in IRQ_TMP4
;; Out: signed 16bit result value in IRQ_TMP[12]
;; --------------------------------------------------------------------------
SIDSW_Hlp_GetMOD
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
	clrf	IRQ_TMP3
	movwf	IRQ_TMP4	; save assigned LFOs and ENVs in IRQ_TMP4

	;; add all enabled LFO values (16 bit -> 24 bit)
	BRA_IFCLR IRQ_TMP4, ASSIGNED_LFOS_1, ACCESS, SIDSW_Hlp_GetMOD_NoLFO1
SIDSW_Hlp_GetMOD_LFO1
	movf	SID_LFO1_BASE + SID_LFOx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_LFO1_BASE + SID_LFOx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_LFO1_BASE + SID_LFOx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoLFO1

	BRA_IFCLR IRQ_TMP4, ASSIGNED_LFOS_2, ACCESS, SIDSW_Hlp_GetMOD_NoLFO2
SIDSW_Hlp_GetMOD_LFO2
	movf	SID_LFO2_BASE + SID_LFOx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_LFO2_BASE + SID_LFOx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_LFO2_BASE + SID_LFOx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoLFO2

	BRA_IFCLR IRQ_TMP4, ASSIGNED_LFOS_3, ACCESS, SIDSW_Hlp_GetMOD_NoLFO3
SIDSW_Hlp_GetMOD_LFO3
	movf	SID_LFO3_BASE + SID_LFOx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_LFO3_BASE + SID_LFOx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_LFO3_BASE + SID_LFOx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoLFO3

	BRA_IFCLR IRQ_TMP4, ASSIGNED_LFOS_4, ACCESS, SIDSW_Hlp_GetMOD_NoLFO4
SIDSW_Hlp_GetMOD_LFO4
	movf	SID_LFO4_BASE + SID_LFOx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_LFO4_BASE + SID_LFOx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_LFO4_BASE + SID_LFOx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoLFO4

	BRA_IFCLR IRQ_TMP4, ASSIGNED_LFOS_5, ACCESS, SIDSW_Hlp_GetMOD_NoLFO5
SIDSW_Hlp_GetMOD_LFO5
	movf	SID_LFO5_BASE + SID_LFOx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_LFO5_BASE + SID_LFOx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_LFO5_BASE + SID_LFOx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoLFO5

	BRA_IFCLR IRQ_TMP4, ASSIGNED_LFOS_6, ACCESS, SIDSW_Hlp_GetMOD_NoLFO6
SIDSW_Hlp_GetMOD_LFO6
	movf	SID_LFO6_BASE + SID_LFOx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_LFO6_BASE + SID_LFOx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_LFO6_BASE + SID_LFOx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoLFO6

	BRA_IFCLR IRQ_TMP4, ASSIGNED_ENVS_1, ACCESS, SIDSW_Hlp_GetMOD_NoENV1
SIDSW_Hlp_GetMOD_ENV1
	movf	SID_ENV1_BASE + SID_ENVx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_ENV1_BASE + SID_ENVx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_ENV1_BASE + SID_ENVx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoENV1

	BRA_IFCLR IRQ_TMP4, ASSIGNED_ENVS_2, ACCESS, SIDSW_Hlp_GetMOD_NoENV2
SIDSW_Hlp_GetMOD_ENV2
	movf	SID_ENV2_BASE + SID_ENVx_RVALUE_L, W, BANKED
	addwf	IRQ_TMP1, F
	movf	SID_ENV2_BASE + SID_ENVx_RVALUE_H, W, BANKED
	addwfc	IRQ_TMP2, F
	movlw	0x00
	btfsc	SID_ENV2_BASE + SID_ENVx_RVALUE_H, 7, BANKED
	movlw	0xff
	skpnc
	addlw	1
	addwf	IRQ_TMP3, F
SIDSW_Hlp_GetMOD_NoENV2
	
	;; saturate to absolute 16 bit-value, keep sign in IRQ_TMP3[0]
	BRA_IFSET IRQ_TMP3, 7, ACCESS, SIDSW_Hlp_GetMOD_Negative
SIDSW_Hlp_GetMOD_Positive
	movf	IRQ_TMP3, W
	bz	SIDSW_Hlp_GetMOD_Cont
	setf	IRQ_TMP1
	setf	IRQ_TMP2
	clrf	IRQ_TMP3	; save sign in IRQ_TMP3
	rgoto	SIDSW_Hlp_GetMOD_Cont

SIDSW_Hlp_GetMOD_Negative
	movf	IRQ_TMP3, W
	andlw	0x7f
	bnz	SIDSW_Hlp_GetMOD_Negative_C
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
SIDSW_Hlp_GetMOD_Negative_C
	comf	IRQ_TMP1, F	; invert result
	comf	IRQ_TMP2, F
	movlw	0x01		; save sign in IRQ_TMP3
	movwf	IRQ_TMP3
SIDSW_Hlp_GetMOD_Cont

	return

;; --------------------------------------------------------------------------
;; Help Function to add a 7-bit offset to IRQ_TMP[123] and saturate
;; In:  offset in WREG, absolute 16-bit value in IRQ_TMP[12], sign in IRQ_TMP3[0]
;; Out: New value in IRQ_TMP[12]
;; --------------------------------------------------------------------------
SIDSW_Hlp_AddOffset
	clrc
	rlf	WREG, W
	movwf	PRODH
	clrf	PRODL
	;; 	rgoto	SIDSW_Hlp_AddOffset16

;; --------------------------------------------------------------------------
;; Help Function to add a 16-bit offset to IRQ_TMP[123] and saturate
;; In:  offset in PROD[LH], absolute 16-bit value in IRQ_TMP[12], sign in IRQ_TMP3[0]
;; Out: New value in IRQ_TMP[12]
;; --------------------------------------------------------------------------
SIDSW_Hlp_AddOffset16
	;; add offset and saturate
	BRA_IFSET IRQ_TMP3, 0, ACCESS, SIDSW_Hlp_AddOffset16_Negative
SIDSW_Hlp_AddOffset16_Positive
	movf	PRODL, W	; add offset
	addwf	IRQ_TMP1, F
	movf	PRODH, W
	addwfc	IRQ_TMP2, F
	bnc	SIDSW_Hlp_AddOffset16_End
	setf	IRQ_TMP1	; saturate
	setf	IRQ_TMP2
	rgoto	SIDSW_Hlp_AddOffset16_End
SIDSW_Hlp_AddOffset16_Negative
	movf	IRQ_TMP1, W	; subtract offset
	subwf	PRODL, W
	movwf	IRQ_TMP1
	movf	IRQ_TMP2, W
	subwfb	PRODH, W
	movwf	IRQ_TMP2
	bc	SIDSW_Hlp_AddOffset16_End
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
SIDSW_Hlp_AddOffset16_End

	return

;; --------------------------------------------------------------------------
;; Help Function: Get absolute value
;; IN:  signed 16-bit value in IRQ_TMP[12]
;; Out: unsigned absolute value in IRQ_TMP[12]
;;      sign in IRQ_TMP3[0]
;; --------------------------------------------------------------------------
SIDSW_Hlp_GetAbs16
	;; convert IRQ_TMP[12] to unsigned integer, keep sign in IRQ_TMP3[0]
	clrf	IRQ_TMP3
	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSW_Hlp_GetABS16_Pos
SIDSW_Hlp_GetABS16_Neg
	bsf	IRQ_TMP3, 0	; memorize sign in IRQ_TMP3[0]
	comf	IRQ_TMP1, F
	comf	IRQ_TMP2, F
	incf	IRQ_TMP1, F
	skpnz
	incf	IRQ_TMP2, F
SIDSW_Hlp_GetABS16_Pos
	return


;; --------------------------------------------------------------------------
;; Help Function for ENV Waveforms (resuses the LFO Waveform routine)
;; In:  ENV_x_VALUE in WREG
;;      ENV_x_DEPTH in IRQ_TMP2,
;;      Accent flag in IRQ_TMP4.3
;; Out: Result in PROD[LH]
;; --------------------------------------------------------------------------
SIDSW_ENV_Hlp_Waveform

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SIDSW_ENV_Hlp_Waveform_NotTB303
SIDSW_ENV_Hlp_Waveform_TB303
	movwf	IRQ_TMP1
	;; in TB303 mode the depth parameter is used as "env mod" which is always positive
	;; modify depth depending on ACCENT flag
	rrf	IRQ_TMP2, W
	andlw	0x3f
	movwf	IRQ_TMP2
	BRA_IFCLR IRQ_TMP4, 3, ACCESS, SIDSW_ENV_Hlp_Waveform_NoAcc
SIDSW_ENV_Hlp_Waveform_Acc
	addlw	0x10
	movwf	IRQ_TMP2
SIDSW_ENV_Hlp_Waveform_NoAcc
	movf	IRQ_TMP1, W
	andlw	0x7f
	mulwf	IRQ_TMP2, ACCESS
	return

SIDSW_ENV_Hlp_Waveform_NotTB303
	;; set mode to 0x10, don't overwrite WREG
	clrf	IRQ_TMP1
	bsf	IRQ_TMP1, 4	; (triangle waveform)

	;; convert linear envelope value
	andlw	0x7f
	btfsc	IRQ_TMP2, 6; shift positive values to > 0x0000
	addlw 0x80	
	btfss	IRQ_TMP2, 6; inversion if depth < 0x40
	xorlw 0x7f	
	rgoto	SIDSW_ENV_Hlp_Waveform_Cont

;; --------------------------------------------------------------------------
;; Help Function for LFO and ENV Waveforms
;; In:  LFO_x_VALUE in WREG
;;	LFO_x_MODE in IRQ_TMP1
;;      LFO_x_DEPTH in IRQ_TMP2,
;;      LFO/ENV number in SID_SW_LFO_NUMBER
;;      pointer to LFOx_BASE in FSR1
;; Out: Result in PROD[LH]
;; 	Scaled Value in IRQ_TMP1
;; --------------------------------------------------------------------------
SIDSW_LFO_Hlp_Waveform
	;; invert if negative depth (<0x40)
	btfss	IRQ_TMP2, 6
	xorlw 0xff

	;; envelope waveform routine continues here
SIDSW_ENV_Hlp_Waveform_Cont
	movwf	IRQ_TMP3

	;; get absolute value of depth from SID_DEPTH_TABLE
	;; (to keep it compatible with MIDIbox SID V1.5)
	movf	IRQ_TMP2, W
	rcall	SIDSW_Hlp_Abs7
	addlw	SID_DEPTH_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SID_DEPTH_TABLE >> 8
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	movwf	IRQ_TMP2
	
	;; branch depending on selected waveform
	swapf	IRQ_TMP1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SIDSW_LFO_Hlp_WFBranch_0
	rgoto	SIDSW_LFO_Hlp_WFBranch_1
	rgoto	SIDSW_LFO_Hlp_WFBranch_2
	rgoto	SIDSW_LFO_Hlp_WFBranch_3
	rgoto	SIDSW_LFO_Hlp_WFBranch_4
	rgoto	SIDSW_LFO_Hlp_WFBranch_5
	rgoto	SIDSW_LFO_Hlp_WFBranch_6
	rgoto	SIDSW_LFO_Hlp_WFBranch_7

SIDSW_LFO_Hlp_WFBranch_0	; Sine
SIDSW_LFO_Hlp_WFBranch_6	; (reserved)
SIDSW_LFO_Hlp_WFBranch_7	; (reserved)
	movf	IRQ_TMP3, W
	call	SID_SIN_TABLE_Get
	rgoto	SIDSW_LFO_Hlp_WFBranch_Cont

SIDSW_LFO_Hlp_WFBranch_1	; Triangle
	;; Triangle: return unsigned value
	movf	IRQ_TMP3, W
	btfss	IRQ_TMP3, 7
	xorlw	0x7f
	rgoto	SIDSW_LFO_Hlp_WFBranch_Cont

SIDSW_LFO_Hlp_WFBranch_2	; sawtooth
	;; Sawtooth: x/2, MODE_DECINC is the eight bit
	rrf	IRQ_TMP3, W
	andlw	0x7f
	btfss	IRQ_TMP1, LFO_MODE_DECINC ; (IRQ_TMP1=LFO_x_MODE)
	iorlw	0x80
	rgoto	SIDSW_LFO_Hlp_WFBranch_Cont
SIDSW_LFO_Hlp_WFBranch_3	; pulse
	;; Pulse: 0x00 when Dec, 0xff when Inc, take inversion bit also into account
	movlw	0x00
	btfsc	IRQ_TMP1, LFO_MODE_DECINC; (IRQ_TMP1=LFO_x_MODE)
	movlw 0xff 
	rgoto	SIDSW_LFO_Hlp_WFBranch_Cont
SIDSW_LFO_Hlp_WFBranch_4	; random
	;; each second LFO is in S&H mode
	BRA_IFCLR SID_SW_LFO_NUMBER, 0, BANKED, SIDSW_LFO_Hlp_WFBranch_4_Random
SIDSW_LFO_Hlp_WFBranch_4_S_H
	movf	IRQ_TMP3, W	; latch on period match
	movlw	SID_LFOx_RVALUE_L
	skpnz
	movlw	SID_LFOx_RVALUE_L - SID_LFOx_RECORD_LEN
	movff	PLUSW1, PRODL

	movlw	SID_LFOx_RVALUE_H
	skpnz
	movlw	SID_LFOx_RVALUE_H - SID_LFOx_RECORD_LEN
	movff	PLUSW1, PRODH
	rgoto	SIDSW_LFO_Hlp_Waveform_End

SIDSW_LFO_Hlp_WFBranch_4_Random
	movf	SID_LFO_RANDOM_SEED_H, W, BANKED
	andlw	0x55
	movwf	IRQ_TMP1
	movf	SID_LFO_RANDOM_SEED_L, W, BANKED
	andlw	0xaa
	iorwf	IRQ_TMP1, W
	addwf	TMR1L, W	; super-random ;-)
	xorwf	TMR2, W
	rgoto	SIDSW_LFO_Hlp_WFBranch_Cont

SIDSW_LFO_Hlp_WFBranch_5	; (AIN)
#if ENABLE_AIN_LFO_WAVEFORM
	movf	SID_SW_LFO_NUMBER, W, BANKED
	movff	FSR1L, IRQ_TMP1		; save FSR1
	movff	FSR1H, IRQ_TMP3
	call	MIOS_AIN_PinGet		; get value of analog pin
	movff	IRQ_TMP1, FSR1L		; restore FSR1
	movff	IRQ_TMP3, FSR1H
	SET_BSR	SID_BASE
	rrf	MIOS_PARAMETER2, F	; convert 10bit to 8bit
	rrf	MIOS_PARAMETER1, F
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, W

	;; biased at 0x80
	BRA_IFSET WREG, 7, ACCESS, SIDSW_LFO_Hlp_WFBranch_Cont
	xorlw	0x7f
	addlw	1
	btfsc	WREG, 7
	movlw 0x7f
#else
	movlw	0x80
#endif
	;; 	rgoto	SIDSW_LFO_Hlp_WFBranch_Cont

SIDSW_LFO_Hlp_WFBranch_Cont
	movwf	IRQ_TMP1

	;; process scaling (depth * scaled value)
	andlw	0x7f		; remove sign from value
	mulwf	IRQ_TMP2	; multiply with depth (in IRQ_TMP2)
	;; result in PROD[LH]

	;; invert if DECINC flag not set
	BRA_IFSET IRQ_TMP1, 7, ACCESS, SIDSW_LFO_Hlp_Waveform_End
	comf	PRODL, F
	comf	PRODH, F
SIDSW_LFO_Hlp_Waveform_End
	return


;; --------------------------------------------------------------------------
;; Help Function to sync all LFOs
;; --------------------------------------------------------------------------
SIDSW_Hlp_SyncAllLFOs
	lfsr	FSR2, SID_LFO1_BASE + SID_LFOx_MODE
	movlw	0x06
	movwf	IRQ_TMP1
SIDSW_Hlp_SyncAllLFOs_Loop
	call	SIDSW_Hlp_SyncLFO_Now
	decfsz	IRQ_TMP1, F
	rgoto	SIDSW_Hlp_SyncAllLFOs_Loop
	return

;; --------------------------------------------------------------------------
;; Help Function for gate bit, syncs the LFOs
;; In: SID_Vx_PITCH_LFOS | SID_Vx_PW_LFOS | assigned filter flags
;; --------------------------------------------------------------------------
SIDSW_Hlp_SyncLFOs
	lfsr	FSR2, SID_LFO1_BASE + SID_LFOx_MODE
	movwf	IRQ_TMP1
	rcall	SIDSW_Hlp_SyncSingleLFO
	rrf	IRQ_TMP1, F
	rcall	SIDSW_Hlp_SyncSingleLFO
	rrf	IRQ_TMP1, F
	rcall	SIDSW_Hlp_SyncSingleLFO
	rrf	IRQ_TMP1, F
	rcall	SIDSW_Hlp_SyncSingleLFO
	rrf	IRQ_TMP1, F
	rcall	SIDSW_Hlp_SyncSingleLFO
	rrf	IRQ_TMP1, F
	;; 	rgoto	SIDSW_Hlp_SyncSingleLFO

SIDSW_Hlp_SyncSingleLFO
	BRA_IFSET INDF2, LFO_MODE_SYNC_ALL, ACCESS, SIDSW_Hlp_SyncLFO_Now
	BRA_IFCLR IRQ_TMP1, 0, ACCESS, SIDSW_Hlp_SyncLFO_Skip
	BRA_IFCLR INDF2, LFO_MODE_SYNC, ACCESS, SIDSW_Hlp_SyncLFO_Skip
SIDSW_Hlp_SyncLFO_Now
	bcf	INDF2, LFO_MODE_DECINC
	incf	FSR2L, F	; switch to LFO_x_RATE
	incf	FSR2L, F	; switch to LFO_x_CTR
	clrf	POSTINC2	; clear counter,
	                        ; switch to LFO_x_VALUE
	movlw	0x80		; write 0x80 into value
	movwf	POSTINC2
	                        ; switch to LFO_x_DEPTH
	incf	FSR2L, F	; switch to LFO_x_RVALUE_L
	clrf	POSTINC2	; clear LFO_x_RAVLUE_L
	clrf	POSTINC2	; clear LFO_x_RAVLUE_H

	return
	
SIDSW_Hlp_SyncLFO_Skip
	movlw	0x07		; switch to LFO_x+1_MODE
	addwf	FSR2L, F
	return

;; --------------------------------------------------------------------------
;; Help Function for gate bit, sets ENVs to attack mode
;; In: SID_Vx_ENVS | assigned filter flags
;; --------------------------------------------------------------------------
SIDSW_Hlp_ENVAttack
	;; set envelope generators to attack mode if voice (or filter) has been assigned
	movwf	IRQ_TMP1

	andlw	0x11
	bz	SIDSW_Hlp_ENVAttack_Not1
	movlw	(1 << ENV_MODE_ATTACK)
	movwf	SID_ENV1_BASE + SID_ENVx_MODE, BANKED
SIDSW_Hlp_ENVAttack_Not1

	movf	IRQ_TMP1, W
	andlw	0x22
	bz	SIDSW_Hlp_ENVAttack_Not2
	movlw	(1 << ENV_MODE_ATTACK)
	movwf	SID_ENV2_BASE + SID_ENVx_MODE, BANKED
SIDSW_Hlp_ENVAttack_Not2
	return

;; --------------------------------------------------------------------------
;; Help Function for gate bit, sets ENVs to release mode
;; In: SID_Vx_ENVS | assigned filter flags
;; --------------------------------------------------------------------------
SIDSW_Hlp_ENVRelease
	;; set envelope generators to release mode if voice (or filter) has been assigned
	movwf	IRQ_TMP1

	andlw	0x11
	bz	SIDSW_Hlp_ENVRelease_Not1
	movlw	(1 << ENV_MODE_RELEASE)
	movwf	SID_ENV1_BASE + SID_ENVx_MODE, BANKED
SIDSW_Hlp_ENVRelease_Not1

	movf	IRQ_TMP1, W
	andlw	0x22
	bz	SIDSW_Hlp_ENVRelease_Not2
	movlw	(1 << ENV_MODE_RELEASE)
	movwf	SID_ENV2_BASE + SID_ENVx_MODE, BANKED
SIDSW_Hlp_ENVRelease_Not2
	return

;; --------------------------------------------------------------------------
;; Help Function for SIDSW_Pitch
;; IN: SID_Vx_TARGET_FRQ_LH in FSR2
;; OUT:	new result in SID_Vx_TARGET_FRQ_LH
;; --------------------------------------------------------------------------
SIDSW_Hlp_AddMul
	BRA_IFCLR IRQ_TMP3, 0, ACCESS, SIDSW_Hlp_AddMul_Pos
SIDSW_Hlp_AddMul_Neg
	;; calc MUL_A_[LH] = SID_Vx_FRQ_[LH] - MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	subwf	POSTINC2, W	; SID_Vx_TARGET_FRQ_LH+0, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	MIOS_PARAMETER2, W
	subwfb	POSTDEC2, W	; SID_Vx_TARGET_FRQ_LH+1, W, BANKED
	movwf	MUL_A_H, BANKED

	;; calc MUL_R_[12] = MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; SID_Vx_FRQ -= result
	movf	MUL_R_1, W, BANKED
	subwf	POSTINC2, F	; SID_Vx_TARGET_FRQ_LH+0, F, BANKED
	movf	MUL_R_2, W, BANKED
	subwfb	POSTDEC2, F	; SID_Vx_TARGET_FRQ_LH+1, F, BANKED
	return

SIDSW_Hlp_AddMul_Pos
	;; calc MUL_A_[LH] = MIOS_PARAMETER[12] - SID_Vx_FRQ_[LH]
	movf	POSTINC2, W	; SID_Vx_TARGET_FRQ_LH+0, W, BANKED
	subwf	MIOS_PARAMETER1, W
	movwf	MUL_A_L, BANKED
	movf	POSTDEC2, W	; SID_Vx_TARGET_FRQ_LH+1, W, BANKED
	subwfb	MIOS_PARAMETER2, W
	movwf	MUL_A_H, BANKED

	;; calc MUL_R_[12] = MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; SID_Vx_FRQ += result
	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F	; SID_Vx_TARGET_FRQ_LH+0, F, BANKED
	movf	MUL_R_2, W, BANKED
	addwfc	POSTDEC2, F	; SID_Vx_TARGET_FRQ_LH+1, F, BANKED
	return

;; --------------------------------------------------------------------------
;; Help Function for SIDSW_Pitch
;; IN:  addend and IRQ_TMP[12], SID_Vx_TARGET_FRQ_LH in FSR2
;; OUT:	new result in SID_Vx_TARGET_FRQ_LH
;; --------------------------------------------------------------------------
SIDSW_Hlp_Add16
	;; divide / 4 for a better scaling
	clrc
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	clrc
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
		
	BRA_IFCLR IRQ_TMP3, 0, ACCESS, SIDSW_Hlp_Add16_Pos
SIDSW_Hlp_Add16_Neg
	;; SID_Vx_TARGET_FRQ -= MOD 
	movf	IRQ_TMP1, W
	subwf	POSTINC2, F	; SID_Vx_TARGET_FRQ_LH+0, F, BANKED
	movf	IRQ_TMP2, W
	subwfb	POSTDEC2, F	; SID_Vx_TARGET_FRQ_LH+1, F, BANKED
	;; saturate on overflow
	bc	SIDSW_Hlp_Add16_Neg_End
	clrf	POSTINC2	; SID_Vx_TARGET_FRQ_LH+0, BANKED
	clrf	POSTDEC2	; SID_Vx_TARGET_FRQ_LH+1, BANKED
SIDSW_Hlp_Add16_Neg_End
	return

SIDSW_Hlp_Add16_Pos
	;; SID_Vx_TARGET_FRQ += MOD
	movf	IRQ_TMP1, W
	addwf	POSTINC2, F	; SID_Vx_TARGET_FRQ_LH+0, F, BANKED
	movf	IRQ_TMP2, W
	addwfc	POSTDEC2, F	; SID_Vx_TARGET_FRQ_LH+1, F, BANKED
	;; saturate on overflow (set frequency to zero to avoid unwanted HF beeps)
	bnc	SIDSW_Hlp_Add16_Pos_End
	clrf	POSTINC2	; SID_Vx_TARGET_FRQ_LH+0, BANKED
	clrf	POSTDEC2	; SID_Vx_TARGET_FRQ_LH+1, BANKED
SIDSW_Hlp_Add16_Pos_End
	return


;; --------------------------------------------------------------------------
;; Help Function for SIDSW_ENV, etc.
;; IN:  7-bit signed value in WREG
;; OUT:	absolute value (0x00-0x3f) in WREG
;; --------------------------------------------------------------------------
SIDSW_Hlp_Abs7
	movf	WREG, W
	skpnz
	addlw	1
	btfss	WREG, 6
	sublw 0x40
	andlw	0x3f
	return

;; --------------------------------------------------------------------------
;; Help Function for SIDSW_ENV
;; IN:  ENV_x_CTR_H in IRQ_TMP1
;;	ENV_x_CURVE in IRQ_TMP2
;;	ENV_x_ATTACK/ENV_x_DECAY or ENV_x_SUSTAIN in IRQ_TMP3
;;      WREG != 0: use curve, WREG == 0: don't use curve parameter
;; OUT:	value which should be added to - or subtracted from - ENV_x_CTR_[LH]
;;      low-byte in WREG and MIOS_PARAMETER1; high-byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SIDSW_ENV_GetBendedValue
	bnz	SIDSW_ENV_GetBendedValue_Curve

	;; curve not selected, get value from ENV_TABLE
	movf	IRQ_TMP3, W
	goto	SID_ENV_TABLE_Get

SIDSW_ENV_GetBendedValue_Curve
	;; return ENV_x_DECAY when ENV_x_CURVE == 0x40
	movlw	0x40
	cpfseq	IRQ_TMP2, ACCESS
	rgoto SIDSW_ENV_GetBendedValue_UD
	comf	IRQ_TMP3, W
	rgoto	SIDSW_ENV_GetBendedValue_Cont

SIDSW_ENV_GetBendedValue_UD
	;; feedback: calculate ABS7(CURVE) * ENV_x_CTR_H
	movf	IRQ_TMP2, W		; get absolute value of curve parameter
	rcall	SIDSW_Hlp_Abs7
	mulwf	IRQ_TMP1, ACCESS	; multiply with current counter value
	
	;; when CURVE parameter < 0x40: bend down, else up
	BRA_IFCLR IRQ_TMP2, 6, ACCESS, SIDSW_ENV_GetBendedValue_Down
SIDSW_ENV_GetBendedValue_Up
	comf	IRQ_TMP3, F
	bcf	IRQ_TMP3, 7
	movf	PRODH, W
	subwf	IRQ_TMP3, W
	btfsc	WREG, 7
	movlw 0x00
	rgoto	SIDSW_ENV_GetBendedValue_Cont

SIDSW_ENV_GetBendedValue_Down
	comf	IRQ_TMP3, W
	andlw	0x7f
	addwf	PRODH, W
	btfsc	WREG, 7
	movlw 0x7f
	;; 	rgoto	SIDSW_ENV_GetBendedValue_Cont

SIDSW_ENV_GetBendedValue_Cont
	andlw	0x7f
	goto	SID_FRQ_TABLE_Get


;; --------------------------------------------------------------------------
;; Help Function used from sid_midi.inc and sid_ccin.inc to reset ENV2
;; --------------------------------------------------------------------------
SID_SW_Hlp_ENV2Reset
	SET_BSR	SID_BASE

	movlw	(1 << ENV_MODE_ATTACK)
	movwf	SID_ENV2_BASE + SID_ENVx_MODE, BANKED

	clrf	SID_ENV2_BASE + SID_ENVx_CTR_L, BANKED
	clrf	SID_ENV2_BASE + SID_ENVx_CTR_H, BANKED

	return

;; --------------------------------------------------------------------------
;; Help Function which interpolates a 16bit value
;; IN:  base address of parameter set (FIPx_BASE) in FSR2
;;      target value in IRQ_TMP[12]
;; OUT:	interpolated value in IRQ_TMP[12]
;; USES: PROD[LH]
;; --------------------------------------------------------------------------
SIDSW_Hlp_IP
	;; increment counter which is used to measure time between changes - saturate at 0xff
	movlw	FIPx_CHANGE_CTR
	incf	PLUSW2, F
	skpnz
	setf	PLUSW2

	;; shift-right target value (internally we have to calculate with a signed 16-bit value)
	clrc
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; check for new target value
	movlw	FIPx_TARGET_VALUE_L
	movf	PLUSW2, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SIDSW_Hlp_IP_Change
	movlw	FIPx_TARGET_VALUE_H
	movf	PLUSW2, W
	cpfseq	IRQ_TMP2, ACCESS
	rgoto SIDSW_Hlp_IP_Change
	rgoto	SIDSW_Hlp_IP_Cont
SIDSW_Hlp_IP_Change
	;; copy new target value
	movlw	FIPx_TARGET_VALUE_L
	movff	IRQ_TMP1, PLUSW2
	movlw	FIPx_TARGET_VALUE_H
	movff	IRQ_TMP2, PLUSW2

	;; determine counter value between interpolation steps (CHANGE_CTR / 8)
	movlw	FIPx_CHANGE_CTR
	movf	PLUSW2, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x1f
	skpnz
	movlw	0x01
	movwf	PRODL
	movlw	FIPx_PART_CTR
	movff	PRODL, PLUSW2
	movlw	FIPx_PART_CTR_RELOAD
	movff	PRODL, PLUSW2

	;; clear change counter
	movlw	FIPx_CHANGE_CTR
	clrf	PLUSW2

	;; calculate difference between current value and new target value
	movlw	FIPx_VALUE_L
	movf	PLUSW2, W
	subwf	IRQ_TMP1, W
	movwf	IRQ_TMP1
	movlw	FIPx_VALUE_H
	movf	PLUSW2, W
	subwfb	IRQ_TMP2, W
	movwf	IRQ_TMP2

	;; value which will be added on each step: diverence / 8
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	movf	IRQ_TMP2, W
	andlw	0x1f
	btfsc	WREG, 4
	iorlw 0xe0
	movwf	IRQ_TMP2

	;; should be at least 1 (!)
	movf	IRQ_TMP1, W
	iorwf	IRQ_TMP2, W
	skpnz
	incf	IRQ_TMP1, F

	movlw	FIPx_DIV_VALUE_L
	movff	IRQ_TMP1, PLUSW2
	movlw	FIPx_DIV_VALUE_H
	movff	IRQ_TMP2, PLUSW2

SIDSW_Hlp_IP_Cont

	;; step handler

	;; copy current filter value into IRQ_TMP[12]
	movlw	FIPx_VALUE_L
	movff	PLUSW2, IRQ_TMP1
	movlw	FIPx_VALUE_H
	movff	PLUSW2, IRQ_TMP2
	
	;; do nothing if target value already reached
	movlw	FIPx_TARGET_VALUE_L
	movf	PLUSW2, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SIDSW_Hlp_IP_Cont_Do
	movlw	FIPx_TARGET_VALUE_H
	movf	PLUSW2, W
	cpfseq	IRQ_TMP2, ACCESS
	rgoto SIDSW_Hlp_IP_Cont_Do
	rgoto	SIDSW_Hlp_IP_End
SIDSW_Hlp_IP_Cont_Do

	;; next step?
	movlw	FIPx_PART_CTR
	decf	PLUSW2, F
	bnz	SIDSW_Hlp_IP_End
SIDSW_Hlp_IP_Step
	;; reload part counter
	movlw	FIPx_PART_CTR_RELOAD
	movff	PLUSW2, PRODL
	movlw	FIPx_PART_CTR
	movff	PRODL, PLUSW2

	;; add div value to current value
	movlw	FIPx_DIV_VALUE_L
	movf	PLUSW2, W
	addwf	IRQ_TMP1, F
	movlw	FIPx_DIV_VALUE_H
	movf	PLUSW2, W
	addwfc	IRQ_TMP2, F

	;; end reached on overflow
	BRA_IFSET IRQ_TMP2, 7, ACCESS, SIDSW_Hlp_IP_TargetReached
	;; end reached if value >= target value
	movlw	FIPx_TARGET_VALUE_L
	movf	PLUSW2, W
	subwf	IRQ_TMP1, W
	movlw	FIPx_TARGET_VALUE_H
	movf	PLUSW2, W
	subwfb	IRQ_TMP2, W

	;; branch depending on increment/decrement
	movlw	FIPx_DIV_VALUE_H
	BRA_IFSET PLUSW2, 7, ACCESS, SIDSW_Hlp_IP_Dec
SIDSW_Hlp_IP_Inc
	bnc	SIDSW_Hlp_IP_End
	;; 	rgoto	SIDSW_Hlp_IP_TargetReached

SIDSW_Hlp_IP_TargetReached
	movlw	FIPx_TARGET_VALUE_L
	movff	PLUSW2, IRQ_TMP1
	movlw	FIPx_TARGET_VALUE_H
	movff	PLUSW2, IRQ_TMP2
	rgoto	SIDSW_Hlp_IP_End
	
SIDSW_Hlp_IP_Dec
	bc	SIDSW_Hlp_IP_End
	rgoto	SIDSW_Hlp_IP_TargetReached

SIDSW_Hlp_IP_End
	;; copy back IRQ_TMP[12] to FIPx_VALUE_[LH]
	movlw	FIPx_VALUE_L
	movff	IRQ_TMP1, PLUSW2
	movlw	FIPx_VALUE_H
	movff	IRQ_TMP2, PLUSW2

	;; left-shift result (it's 16bit unsigned again)
	clrc
	rlf	IRQ_TMP1, F
	rlf	IRQ_TMP2, F

	return

