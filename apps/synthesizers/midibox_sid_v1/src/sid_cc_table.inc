; $Id$
;
; MIDIbox SID
; CC Table
; used by sid_ccin.inc and sid_ccout.inc
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

SET_CC_NOP		EQU	0x00
SET_CC_V123		EQU	0x01
SET_CC_7BIT		EQU	0x02
SET_CC_7BIT_W_PORTA	EQU	0x03
SET_CC_7BIT_4L		EQU	0x04
SET_CC_7BIT_4U		EQU	0x05
SET_CC_7BIT_TO_7_4	EQU	0x06
SET_CC_7BIT_TO_3_0	EQU	0x07
SET_CC_SUSKEY		EQU	0x08
SET_CC_VELOCITY		EQU	0x09
SET_CC_AFTERTOUCH	EQU	0x0a
SET_CC_MODWHEEL		EQU	0x0b
SET_CC_WAVEFORM		EQU	0x0c
SET_CC_MONO		EQU	0x0d
SET_CC_POLY		EQU	0x0e
SET_CC_PLAY_NOTE        EQU     0x0f

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: reads CC table and returns command in MIOS_PARAMETER2 and parameter
;; --------------------------------------------------------------------------
SID_CC_TABLE_Get
	SET_BSR	SID_BASE		; prepare BSR for SID register access
	lfsr	FSR1, SID_BASE		; prepare FSR1

	clrc
	rlf	WREG, W
	addlw	SID_CC_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SID_CC_TABLE >> 8
	addwfc	TBLPTRH, F

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2	; -> command
	tblrd*+
	movff	TABLAT, FSR1L		; -> parameter
	return


; ==========================================================================

SID_CC_TABLE
	;; 0x00-0x0f
	db	SET_CC_NOP,					0x00
	db	SET_CC_MODWHEEL,				SID_LAST_MODWHEEL & 0xff
	db	SET_CC_VELOCITY,				SID_INIT_VELOCITY & 0xff
	db	SET_CC_MODWHEEL,				SID_INIT_MODWHEEL & 0xff
	db	SET_CC_AFTERTOUCH,				SID_INIT_AFTERTOUCH & 0xff
	db	SET_CC_V123,					0x1d
	db	SET_CC_NOP,					0x00
	db	SET_CC_7BIT_TO_3_0,				SID_MODE_VOL & 0xff
	db	SET_CC_V123,					0x09
	db	SET_CC_PLAY_NOTE,				0x00; (Voice 1)
	db	SET_CC_PLAY_NOTE,				0x01; (Voice 2)
	db	SET_CC_PLAY_NOTE,				0x02; (Voice 3)
	db	SET_CC_7BIT,					SID_WT_PATCH & 0xff
	db	SET_CC_VELOCITY,				SID_DEPTH_VELOCITY & 0xff
	db	SET_CC_MODWHEEL,				SID_DEPTH_MODWHEEL & 0xff
	db	SET_CC_AFTERTOUCH,				SID_DEPTH_AFTERTOUCH & 0xff

	;; 0x10-0x1f
	db	SET_CC_V123,					0x11
	db	SET_CC_7BIT_W_PORTA,				(SID_V1_BASE + SID_Vx_TRANSPOSE) & 0xff
	db	SET_CC_7BIT_W_PORTA,				(SID_V2_BASE + SID_Vx_TRANSPOSE) & 0xff
	db	SET_CC_7BIT_W_PORTA,				(SID_V3_BASE + SID_Vx_TRANSPOSE) & 0xff
	db	SET_CC_V123,					0x15
	db	SET_CC_7BIT_W_PORTA,				(SID_V1_BASE + SID_Vx_FINETUNE) & 0xff
	db	SET_CC_7BIT_W_PORTA,				(SID_V2_BASE + SID_Vx_FINETUNE) & 0xff
	db	SET_CC_7BIT_W_PORTA,				(SID_V3_BASE + SID_Vx_FINETUNE) & 0xff
	db	SET_CC_V123,					0x19
	db	SET_CC_7BIT_W_PORTA,				(SID_V1_BASE + SID_Vx_PITCHRANGE) & 0xff
	db	SET_CC_7BIT_W_PORTA,				(SID_V2_BASE + SID_Vx_PITCHRANGE) & 0xff
	db	SET_CC_7BIT_W_PORTA,				(SID_V3_BASE + SID_Vx_PITCHRANGE) & 0xff
	db	SET_CC_V123,					0x1d
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_PORTA_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_PORTA_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_PORTA_RATE) & 0xff

	;; 0x20-0x2f
	db	SET_CC_V123,					0x21
	db	SET_CC_WAVEFORM,				SID_V1_CTRL & 0xff
	db	SET_CC_WAVEFORM,				SID_V2_CTRL & 0xff
	db	SET_CC_WAVEFORM,				SID_V3_CTRL & 0xff
	db	SET_CC_V123,					0x25
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_PW_VALUE) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_PW_VALUE) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_PW_VALUE) & 0xff
	db	SET_CC_V123,					0x29
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_ARP_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_ARP_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_ARP_RATE) & 0xff
	db	SET_CC_7BIT_4L,					SID_RES_FCHN & 0xff
	db	SET_CC_7BIT_4U,					SID_MODE_VOL & 0xff
	db	SET_CC_7BIT,					SID_FC_VALUE & 0xff
	db	SET_CC_7BIT,					SID_RESONANCE_7BIT & 0xff
	
	;; 0x30-0x3f
	db	SET_CC_V123,					0x31
	db	SET_CC_7BIT_TO_7_4,				SID_V1_ENV_AD & 0xff
	db	SET_CC_7BIT_TO_7_4,				SID_V2_ENV_AD & 0xff
	db	SET_CC_7BIT_TO_7_4,				SID_V3_ENV_AD & 0xff
	db	SET_CC_V123,					0x35
	db	SET_CC_7BIT_TO_3_0,				SID_V1_ENV_AD & 0xff
	db	SET_CC_7BIT_TO_3_0,				SID_V2_ENV_AD & 0xff
	db	SET_CC_7BIT_TO_3_0,				SID_V3_ENV_AD & 0xff
	db	SET_CC_V123,					0x39
	db	SET_CC_7BIT_TO_7_4,				SID_V1_ENV_SR & 0xff
	db	SET_CC_7BIT_TO_7_4,				SID_V2_ENV_SR & 0xff
	db	SET_CC_7BIT_TO_7_4,				SID_V3_ENV_SR & 0xff
	db	SET_CC_V123,					0x3d
	db	SET_CC_7BIT_TO_3_0,				SID_V1_ENV_SR & 0xff
	db	SET_CC_7BIT_TO_3_0,				SID_V2_ENV_SR & 0xff
	db	SET_CC_7BIT_TO_3_0,				SID_V3_ENV_SR & 0xff

	;; 0x40-0x4f
	db	SET_CC_7BIT,					(SID_LFO1_BASE + SID_LFOx_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO2_BASE + SID_LFOx_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO3_BASE + SID_LFOx_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO4_BASE + SID_LFOx_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO5_BASE + SID_LFOx_RATE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO6_BASE + SID_LFOx_RATE) & 0xff
	db	SET_CC_NOP,					0x00
	db	SET_CC_NOP,					0x00
	db	SET_CC_7BIT,					(SID_LFO1_BASE + SID_LFOx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_LFO2_BASE + SID_LFOx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_LFO3_BASE + SID_LFOx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_LFO4_BASE + SID_LFOx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_LFO5_BASE + SID_LFOx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_LFO6_BASE + SID_LFOx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_ENV1_BASE + SID_ENVx_DEPTH) & 0xff
	db	SET_CC_7BIT,					(SID_ENV2_BASE + SID_ENVx_DEPTH) & 0xff
	
	;; 0x50-0x5f
	db	SET_CC_7BIT,					(SID_LFO1_BASE + SID_LFOx_MODE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO2_BASE + SID_LFOx_MODE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO3_BASE + SID_LFOx_MODE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO4_BASE + SID_LFOx_MODE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO5_BASE + SID_LFOx_MODE) & 0xff
	db	SET_CC_7BIT,					(SID_LFO6_BASE + SID_LFOx_MODE) & 0xff
	db	SET_CC_7BIT,					(SID_ENV1_BASE + SID_ENVx_CURVE) & 0xff
	db	SET_CC_7BIT,					(SID_ENV2_BASE + SID_ENVx_CURVE) & 0xff
	db	SET_CC_7BIT,					(SID_ENV1_BASE + SID_ENVx_ATTACK) & 0xff
	db	SET_CC_7BIT,					(SID_ENV1_BASE + SID_ENVx_DECAY) & 0xff
	db	SET_CC_7BIT,					(SID_ENV1_BASE + SID_ENVx_SUSTAIN) & 0xff
	db	SET_CC_7BIT,					(SID_ENV1_BASE + SID_ENVx_RELEASE) & 0xff
	db	SET_CC_7BIT,					(SID_ENV2_BASE + SID_ENVx_ATTACK) & 0xff
	db	SET_CC_7BIT,					(SID_ENV2_BASE + SID_ENVx_DECAY) & 0xff
	db	SET_CC_7BIT,					(SID_ENV2_BASE + SID_ENVx_SUSTAIN) & 0xff
	db	SET_CC_7BIT,					(SID_ENV2_BASE + SID_ENVx_RELEASE) & 0xff

	;; 0x60-0x6f
	db	SET_CC_V123,					0x61
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_PITCH_LFOS) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_PITCH_LFOS) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_PITCH_LFOS) & 0xff
	db	SET_CC_V123,					0x65
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_PW_LFOS) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_PW_LFOS) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_PW_LFOS) & 0xff
	db	SET_CC_V123,					0x69
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_ENVS) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_ENVS) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_ENVS) & 0xff
	db	SET_CC_7BIT,					SID_FILTER_LFOS & 0xff
	db	SET_CC_7BIT,					SID_FILTER_ENVS & 0xff
	db	SET_CC_7BIT,					SID_ENV_CURVES & 0xff
	db	SET_CC_7BIT,					SID_SE_OPTION & 0xff
	
	;; 0x70-0x7f
	db	SET_CC_V123,					0x71
	db	SET_CC_7BIT,					(SID_V1_BASE + SID_Vx_NOTE_DELAY) & 0xff
	db	SET_CC_7BIT,					(SID_V2_BASE + SID_Vx_NOTE_DELAY) & 0xff
	db	SET_CC_7BIT,					(SID_V3_BASE + SID_Vx_NOTE_DELAY) & 0xff
	db	SET_CC_SUSKEY,					0x00
	db	SET_CC_7BIT,					SID_ASSIGN_VELOCITY & 0xff
	db	SET_CC_7BIT,					SID_ASSIGN_MODWHEEL & 0xff
	db	SET_CC_7BIT,					SID_ASSIGN_AFTERTOUCH & 0xff
	db	SET_CC_7BIT,					SID_WT_RATE & 0xff
	db	SET_CC_7BIT,					SID_ASSIGN_WT_P1 & 0xff
	db	SET_CC_7BIT,					SID_ASSIGN_WT_P2 & 0xff
	db	SET_CC_7BIT,					SID_ASSIGN_WT_P3 & 0xff
	db	SET_CC_7BIT,					SID_OSC_PHASE_SYNC & 0xff
	db	SET_CC_7BIT_4L,					SID_MIDI_SYNC & 0xff
	db	SET_CC_MONO,					0x00
	db	SET_CC_POLY,					0x00

; ==========================================================================
