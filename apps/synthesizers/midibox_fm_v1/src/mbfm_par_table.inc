; $Id$
;
; MIDIbox FM
; Parameter Table
; used by mbfm_parin.inc and mbfm_parout.inc
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; must be in-sync with table!
MBFM_Px_TRANSPOSE	EQU	0x02
MBFM_Px_UNISONO		EQU	0x03
MBFM_Px_VOLUME		EQU	0x07
MBFM_Px_ACHN_ALL	EQU	0x0d
MBFM_Px_ACHN_OP12	EQU	0x0e
MBFM_Px_ACHN_OP34	EQU	0x0f

MBFM_Px_OPx_FLAGS	EQU	0x10
MBFM_Px_OPx_MULT	EQU	0x14
MBFM_Px_OPx_KSL		EQU	0x18
MBFM_Px_OPx_TL		EQU	0x1c
MBFM_Px_OPx_AR		EQU	0x20
MBFM_Px_OPx_DR		EQU	0x24
MBFM_Px_OPx_SL		EQU	0x28
MBFM_Px_OPx_RR		EQU	0x2c
MBFM_Px_OPx_WS		EQU	0x30

MBFM_Px_FEEDBACK	EQU	0x38
MBFM_Px_CON		EQU	0x3c
	
MBFM_Px_LFOx_MODE	EQU	0x40
MBFM_Px_LFOx_PHASE	EQU	0x41
MBFM_Px_LFOx_RATE	EQU	0x42
MBFM_Px_LFOx_DEPTH_P	EQU	0x43
MBFM_Px_LFOx_DEPTH_V1	EQU	0x44
MBFM_Px_LFOx_DEPTH_V2	EQU	0x45
MBFM_Px_LFOx_DEPTH_V3	EQU	0x46
MBFM_Px_LFOx_DEPTH_V4	EQU	0x47
MBFM_Px_LFOx_DEPTH_L	EQU	0x48
MBFM_Px_LFOx_DEPTH_A	EQU	0x49

MBFM_Px_VELOCITY_ASSIGN	EQU	0x4c
MBFM_Px_VELOCITY_INIT	EQU	0x4d
MBFM_Px_VELOCITY_DEPTH	EQU	0x4e

MBFM_Px_AFTERTOUCH_ASSIGN EQU	0x5c
MBFM_Px_AFTERTOUCH_INIT	EQU	0x5d
MBFM_Px_AFTERTOUCH_DEPTH EQU	0x5e

MBFM_Px_EGx_MODE	EQU	0x60
MBFM_Px_EGx_ATTACK	EQU	0x61
MBFM_Px_EGx_ATTLVL	EQU	0x62
MBFM_Px_EGx_DECAY1	EQU	0x63
MBFM_Px_EGx_DECLVL	EQU	0x64
MBFM_Px_EGx_DECAY2	EQU	0x65
MBFM_Px_EGx_SUSTAIN	EQU	0x66
MBFM_Px_EGx_RELEASE	EQU	0x67
MBFM_Px_EGx_CURVE	EQU	0x68
MBFM_Px_EGx_DEPTH_P	EQU	0x69
MBFM_Px_EGx_DEPTH_V1	EQU	0x6a
MBFM_Px_EGx_DEPTH_V2	EQU	0x6b
MBFM_Px_EGx_DEPTH_V3	EQU	0x6c
MBFM_Px_EGx_DEPTH_V4	EQU	0x6d
MBFM_Px_EGx_DEPTH_L	EQU	0x6e
MBFM_Px_EGx_DEPTH_A	EQU	0x6f

MBFM_Px_CTRL1_L		EQU	0x70
MBFM_Px_CTRL1_U		EQU	0x71
MBFM_Px_CTRL2_L		EQU	0x72
MBFM_Px_CTRL2_U		EQU	0x73
MBFM_Px_FINETUNE	EQU	0x74
MBFM_Px_PITCHRANGE	EQU	0x75
MBFM_Px_PORTA_RATE	EQU	0x76
MBFM_Px_WT_RATE		EQU	0x77
MBFM_Px_WT_PAR1		EQU	0x78
MBFM_Px_WT_PAR2		EQU	0x79
MBFM_Px_WT_PAR3		EQU	0x7a

MBFM_Px_MODWHEEL_ASSIGN	EQU	0x7c
MBFM_Px_MODWHEEL_INIT	EQU	0x7d
MBFM_Px_MODWHEEL_DEPTH	EQU	0x7e


MBFM_PDx_BD_M_MULT		EQU	0x80 | 0x00
MBFM_PDx_BD_C_MULT		EQU	0x80 | 0x01
MBFM_PDx_HH_O_MULT		EQU	0x80 | 0x02
MBFM_PDx_HH_C_MULT		EQU	0x80 | 0x03
MBFM_PDx_SD_MULT		EQU	0x80 | 0x04
MBFM_PDx_TOM_MULT		EQU	0x80 | 0x05
MBFM_PDx_CYM_MULT		EQU	0x80 | 0x06
	
MBFM_PDx_BD_M_TL		EQU	0x80 | 0x08
MBFM_PDx_BD_C_TL		EQU	0x80 | 0x09
MBFM_PDx_HH_O_TL		EQU	0x80 | 0x0a
MBFM_PDx_HH_C_TL		EQU	0x80 | 0x0b
MBFM_PDx_SD_TL			EQU	0x80 | 0x0c
MBFM_PDx_TOM_TL			EQU	0x80 | 0x0d
MBFM_PDx_CYM_TL			EQU	0x80 | 0x0e

MBFM_PDx_BD_M_AR		EQU	0x80 | 0x10
MBFM_PDx_BD_C_AR		EQU	0x80 | 0x11
MBFM_PDx_HH_O_AR		EQU	0x80 | 0x12
MBFM_PDx_HH_C_AR		EQU	0x80 | 0x13
MBFM_PDx_SD_AR			EQU	0x80 | 0x14
MBFM_PDx_TOM_AR			EQU	0x80 | 0x15
MBFM_PDx_CYM_AR			EQU	0x80 | 0x16

MBFM_PDx_BD_M_DR		EQU	0x80 | 0x18
MBFM_PDx_BD_C_DR		EQU	0x80 | 0x19
MBFM_PDx_HH_O_DR		EQU	0x80 | 0x1a
MBFM_PDx_HH_C_DR		EQU	0x80 | 0x1b
MBFM_PDx_SD_DR			EQU	0x80 | 0x1c
MBFM_PDx_TOM_DR			EQU	0x80 | 0x1d
MBFM_PDx_CYM_DR			EQU	0x80 | 0x1e

MBFM_PDx_BD_M_SL		EQU	0x80 | 0x20
MBFM_PDx_BD_C_SL		EQU	0x80 | 0x21
MBFM_PDx_HH_O_SL		EQU	0x80 | 0x22
MBFM_PDx_HH_C_SL		EQU	0x80 | 0x23
MBFM_PDx_SD_SL			EQU	0x80 | 0x24
MBFM_PDx_TOM_SL			EQU	0x80 | 0x25
MBFM_PDx_CYM_SL			EQU	0x80 | 0x26

MBFM_PDx_BD_M_RR		EQU	0x80 | 0x28
MBFM_PDx_BD_C_RR		EQU	0x80 | 0x29
MBFM_PDx_HH_O_RR		EQU	0x80 | 0x2a
MBFM_PDx_HH_C_RR		EQU	0x80 | 0x2b
MBFM_PDx_SD_RR			EQU	0x80 | 0x2c
MBFM_PDx_TOM_RR			EQU	0x80 | 0x2d
MBFM_PDx_CYM_RR			EQU	0x80 | 0x2e

MBFM_PDx_BD_M_WS		EQU	0x80 | 0x30
MBFM_PDx_BD_C_WS		EQU	0x80 | 0x31
MBFM_PDx_HH_O_WS		EQU	0x80 | 0x32
MBFM_PDx_HH_C_WS		EQU	0x80 | 0x33
MBFM_PDx_SD_WS			EQU	0x80 | 0x34
MBFM_PDx_TOM_WS			EQU	0x80 | 0x35
MBFM_PDx_CYM_WS			EQU	0x80 | 0x36

MBFM_PDx_BD_FEEDBACK		EQU	0x80 | 0x38
MBFM_PDx_BD_CON			EQU	0x80 | 0x39
MBFM_PDx_SD_HH_FEEDBACK		EQU	0x80 | 0x3a
MBFM_PDx_SD_HH_CON		EQU	0x80 | 0x3b
MBFM_PDx_CYM_TOM_FEEDBACK	EQU	0x80 | 0x3c
MBFM_PDx_CYM_TOM_CON		EQU	0x80 | 0x3d

MBFM_PDx_BD_SPLIT_LOWER		EQU	0x80 | 0x41
MBFM_PDx_HH_O_SPLIT_LOWER	EQU	0x80 | 0x42
MBFM_PDx_HH_C_SPLIT_LOWER	EQU	0x80 | 0x43
MBFM_PDx_SD_SPLIT_LOWER		EQU	0x80 | 0x44
MBFM_PDx_TOM_SPLIT_LOWER	EQU	0x80 | 0x45
MBFM_PDx_CYM_SPLIT_LOWER	EQU	0x80 | 0x46

MBFM_PDx_BD_SPLIT_UPPER		EQU	0x80 | 0x49
MBFM_PDx_HH_O_SPLIT_UPPER	EQU	0x80 | 0x4a
MBFM_PDx_HH_C_SPLIT_UPPER	EQU	0x80 | 0x4b
MBFM_PDx_SD_SPLIT_UPPER		EQU	0x80 | 0x4c
MBFM_PDx_TOM_SPLIT_UPPER	EQU	0x80 | 0x4d
MBFM_PDx_CYM_SPLIT_UPPER	EQU	0x80 | 0x4e
	
MBFM_PDx_BD_FRQ			EQU	0x80 | 0x50
MBFM_PDx_BD_FRQ_DECR		EQU	0x80 | 0x51
MBFM_PDx_HH_FRQ			EQU	0x80 | 0x52
MBFM_PDx_HH_FRQ_DECR		EQU	0x80 | 0x53
MBFM_PDx_TOM_FRQ		EQU	0x80 | 0x54
MBFM_PDx_TOM_FRQ_DECR		EQU	0x80 | 0x55

MBFM_PDx_BD_ACHN		EQU	0x80 | 0x58
MBFM_PDx_SD_ACHN		EQU	0x80 | 0x5a
MBFM_PDx_TOM_ACHN		EQU	0x80 | 0x5c


MBFM_PEx_BANK			EQU	0xf0 | 0x00
MBFM_PEx_PATCH			EQU	0xf0 | 0x01
MBFM_PEx_MIDI_CHANNEL		EQU	0xf0 | 0x02
MBFM_PEx_SPLIT_LOWER		EQU	0xf0 | 0x03
MBFM_PEx_SPLIT_UPPER		EQU	0xf0 | 0x04
MBFM_PEx_CTRLE			EQU	0xf0 | 0x05
MBFM_PEx_VOLUME			EQU	0xf0 | 0x06
MBFM_PEx_TRANSPOSE		EQU	0xf0 | 0x07
MBFM_PEx_UNISONO		EQU	0xf0 | 0x08
MBFM_PEx_ACHN_OP12		EQU	0xf0 | 0x09
MBFM_PEx_ACHN_OP34		EQU	0xf0 | 0x0a


; ==========================================================================

CMD_PAR_NOP		EQU	0x00
CMD_PAR_7BIT		EQU	0x01
CMD_PAR_7BIT_TO_7_4	EQU	0x02
CMD_PAR_7BIT_TO_3_0	EQU	0x03
CMD_PAR_INV_7BIT_TO_7_4	EQU	0x04
CMD_PAR_INV_7BIT_TO_3_0	EQU	0x05
CMD_PAR_7BIT_TO_7_6	EQU	0x06
CMD_PAR_INV_7BIT_TO_5_0	EQU	0x07
CMD_PAR_7BIT_TO_3_1	EQU	0x08
CMD_PAR_7BIT_TO_CNT	EQU	0x09
CMD_PAR_7BIT_TO_0	EQU	0x0a
CMD_PAR_ACHN_ALL	EQU	0x0b
CMD_PAR_ACHN		EQU	0x0c
CMD_PAR_MODWHEEL	EQU	0x0d
CMD_PAR_VELOCITY	EQU	0x0e
CMD_PAR_AFTERTOUCH	EQU	0x0f
CMD_PAR_WTPAR		EQU	0x10

;; --------------------------------------------------------------------------
;;  IN: index of parameter in WREG, instrument in MBFM_CURRENT_INSTRUMENT
;;  OUT: reads parameter table and returns command in MIOS_PARAMETER2,
;;       Ix/DRx/Ex offset in MIOS_PARAMETER3
;;          (0x00 >= index >= 0x7f: patch)
;;          (0x80 >= index >= 0xef: drum)
;;          (0xf0 >= index >= 0xff: ensemble)
;;       pointer to parameter in FSR1
;; --------------------------------------------------------------------------
MBFM_PAR_TABLE_Get
	movwf	TABLAT

	TABLE_ADDR_MUL_W MBFM_PAR_TABLE, 2

	;; get base address to instrument/drum/ensemble record -> FSR1
	movf	TABLAT, W
	andlw	0xf0
	xorlw	0xf0
	bz	MBFM_PAR_TABLE_Get_Ix
	BRA_IFSET TABLAT, 7, ACCESS, MBFM_PAR_TABLE_Get_DRx
MBFM_PAR_TABLE_Get_Ix
	call	MBFM_INSTR_GetBase
	rgoto	MBFM_PAR_TABLE_Get_Cont
MBFM_PAR_TABLE_Get_DRx
	call	MBFM_DRUM_GetBase
	;; 	rgoto	MBFM_PAR_TABLE_Get_Cont
MBFM_PAR_TABLE_Get_Cont

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2	; -> command
	tblrd*+
	movf	TABLAT, W		; -> parameter offset
	movwf	MIOS_PARAMETER3		; (save Ix/DRx offset in MIOS_PARAMETER3)
	andlw	0x7f
	addwf	FSR1L, F		; add to FSR1 pointer
	movlw	0x00
	addwfc	FSR1H, F
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function maps the ensemble indices to following addresses:
;;     0x00-0x0f -> I1, 0xf0-0xff
;;     0x10-0x1f -> I2, 0xf0-0xff
;;     0x20-0x2f -> I3, 0xf0-0xff
;;     0x30-0x3f -> I4, 0xf0-0xff
;; --------------------------------------------------------------------------
MBFM_PAR_TABLE_ENS_Map
	SET_BSR	MBFM_BASE

	movwf	TABLAT
	swapf	TABLAT, W
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED
	movf	TABLAT, W
	iorlw	0xf0
	return

; ==========================================================================


MBFM_PAR_TABLE
	;; 0x00-0x0f
	db	CMD_PAR_NOP,			0			; bank change implemented in mbfm_midi.inc
	db	CMD_PAR_MODWHEEL,		MBFM_Ix_MODWHEEL	; modulation wheel
	db	CMD_PAR_7BIT,			MBFM_Ix_TRANSPOSE	; transpose
	db	CMD_PAR_7BIT,			MBFM_Ix_UNISONO		; unisono
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT,			MBFM_Ix_VOLUME		; main volume

	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_ACHN_ALL,		MBFM_Ix_ACHN_OP12	; all audio channels at once
	db	CMD_PAR_ACHN,			MBFM_Ix_ACHN_OP12	; audio channels of OP12
	db	CMD_PAR_ACHN,			MBFM_Ix_ACHN_OP34	; audio channels of OP34

	;; 0x10-0x1f
	db	CMD_PAR_7BIT_TO_7_4,		MBFM_Ix_OP1_FLAGS_MULT	; OP1 AM/VIB/EGT/KSR
	db	CMD_PAR_7BIT_TO_7_4,		MBFM_Ix_OP2_FLAGS_MULT	; OP2 AM/VIB/EGT/KSR
	db	CMD_PAR_7BIT_TO_7_4,		MBFM_Ix_OP3_FLAGS_MULT	; OP4 AM/VIB/EGT/KSR
	db	CMD_PAR_7BIT_TO_7_4,		MBFM_Ix_OP4_FLAGS_MULT	; OP4 AM/VIB/EGT/KSR
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP1_FLAGS_MULT	; OP1 MULT
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP2_FLAGS_MULT	; OP2 MULT
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP3_FLAGS_MULT	; OP3 MULT
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP4_FLAGS_MULT	; OP4 MULT
	db	CMD_PAR_7BIT_TO_7_6,		MBFM_Ix_OP1_KSL_TL	; OP1 KSL
	db	CMD_PAR_7BIT_TO_7_6,		MBFM_Ix_OP2_KSL_TL	; OP2 KSL
	db	CMD_PAR_7BIT_TO_7_6,		MBFM_Ix_OP3_KSL_TL	; OP3 KSL
	db	CMD_PAR_7BIT_TO_7_6,		MBFM_Ix_OP4_KSL_TL	; OP4 KSL
	db	CMD_PAR_INV_7BIT_TO_5_0,	MBFM_Ix_OP1_KSL_TL	; OP1 TL
	db	CMD_PAR_INV_7BIT_TO_5_0,	MBFM_Ix_OP2_KSL_TL	; OP2 TL
	db	CMD_PAR_INV_7BIT_TO_5_0,	MBFM_Ix_OP3_KSL_TL	; OP3 TL
	db	CMD_PAR_INV_7BIT_TO_5_0,	MBFM_Ix_OP4_KSL_TL	; OP4 TL

	;; 0x20-0x2f
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP1_AR_DR	; OP1 AR
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP2_AR_DR	; OP2 AR
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP3_AR_DR	; OP3 AR
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP4_AR_DR	; OP4 AR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP1_AR_DR	; OP1 DR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP2_AR_DR	; OP2 DR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP3_AR_DR	; OP3 DR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP4_AR_DR	; OP4 DR
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP1_SL_RR	; OP1 SL
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP2_SL_RR	; OP2 SL
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP3_SL_RR	; OP3 SL
	db	CMD_PAR_INV_7BIT_TO_7_4,	MBFM_Ix_OP4_SL_RR	; OP4 SL
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP1_SL_RR	; OP1 RR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP2_SL_RR	; OP2 RR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP3_SL_RR	; OP3 RR
	db	CMD_PAR_INV_7BIT_TO_3_0,	MBFM_Ix_OP4_SL_RR	; OP4 RR

	;; 0x30-0x3f
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP1_WS		; OP1 WS
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP2_WS		; OP2 WS
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP3_WS		; OP3 WS
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_OP4_WS		; OP4 WS
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT_TO_3_1,		MBFM_Ix_OP12_CH_FB_CNT	; OP1 feedback
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT_TO_CNT,		MBFM_Ix_OP12_CH_FB_CNT	; OP connections
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved

	;; 0x40-0x4f
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_MODE	; LFO1 mode
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_PHASE	; LFO1 phase
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_RATE	; LFO1 rate
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_P	; LFO1 pitch depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_V1	; LFO1 OP1 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_V2	; LFO1 OP2 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_V3	; LFO1 OP3 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_V4	; LFO1 OP4 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_L	; LFO1 LFO2 depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO1_DEPTH_A	; LFO1 AOUT depth
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
#if USE_SHADOW_BUFFERS
	db	CMD_PAR_VELOCITY,               MBFM_Ix_VELOCITY_ASSIGN	; velocity parameter
#else
	db	CMD_PAR_7BIT,                   MBFM_Ix_VELOCITY_ASSIGN	; velocity parameter
#endif
	db	CMD_PAR_VELOCITY,		MBFM_Ix_VELOCITY_INIT	; initial value
	db	CMD_PAR_VELOCITY,		MBFM_Ix_VELOCITY_DEPTH	; depth
	db	CMD_PAR_NOP,			0			; reserved

	;; 0x50-0x5f
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_MODE	; LFO2 mode
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_PHASE	; LFO2 phase
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_RATE	; LFO2 rate
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_P	; LFO2 pitch depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_V1	; LFO2 OP1 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_V2	; LFO2 OP2 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_V3	; LFO2 OP3 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_V4	; LFO2 OP4 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_L	; LFO2 LFO1 depth
	db	CMD_PAR_7BIT,			MBFM_Ix_LFO2_DEPTH_A	; LFO2 AOUT depth
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
#if USE_SHADOW_BUFFERS
	db	CMD_PAR_AFTERTOUCH,             MBFM_Ix_AFTERTOUCH_ASSIGN ; aftertouch parameter
#else
	db	CMD_PAR_7BIT,                   MBFM_Ix_AFTERTOUCH_ASSIGN ; aftertouch parameter
#endif
	db	CMD_PAR_AFTERTOUCH,		MBFM_Ix_AFTERTOUCH_INIT	; initial value
	db	CMD_PAR_AFTERTOUCH,		MBFM_Ix_AFTERTOUCH_DEPTH ; depth
	db	CMD_PAR_NOP,			0			; reserved

	;; 0x60-0x6f
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_MODE	; EG5 mode
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_ATTACK	; EG5 attack
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_ATTLVL	; EG5 attack level
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DECAY1	; EG5 decay1
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DECLVL	; EG5 decay level
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DECAY2	; EG5 decay2
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_SUSTAIN	; EG5 sustain level
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_RELEASE	; EG5 release
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_CURVE	; EG5 curve
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_P	; EG5 pitch depth
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_V1	; EG5 OP1 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_V2	; EG5 OP2 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_V3	; EG5 OP3 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_V4	; EG5 OP4 volume depth
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_L	; EG5 LFO1 depth
	db	CMD_PAR_7BIT,			MBFM_Ix_EG5_DEPTH_A	; EG5 AOUT depth

	;; 0x70-0x7f
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_CTRL1		; general control
	db	CMD_PAR_7BIT_TO_7_4,		MBFM_Ix_CTRL1		; velocity mode flags
	db	CMD_PAR_7BIT_TO_3_0,		MBFM_Ix_CTRL2		; MIDI sync flags
	db	CMD_PAR_7BIT_TO_7_4,		MBFM_Ix_CTRL2		; reserved
	db	CMD_PAR_7BIT,			MBFM_Ix_FINETUNE	; finetune
	db	CMD_PAR_7BIT,			MBFM_Ix_PITCHRANGE	; pitchrange
	db	CMD_PAR_7BIT,			MBFM_Ix_PORTA_RATE	; portamento rate
	db	CMD_PAR_7BIT,			MBFM_Ix_WT_RATE		; wavetable rate
#if USE_SHADOW_BUFFERS
	db	CMD_PAR_WTPAR,			MBFM_Ix_WT_PAR1		; wt parameter #1
	db	CMD_PAR_WTPAR,			MBFM_Ix_WT_PAR2		; wt parameter #2
	db	CMD_PAR_WTPAR,			MBFM_Ix_WT_PAR3		; wt parameter #3
#else
	db	CMD_PAR_7BIT,			MBFM_Ix_WT_PAR1		; wt parameter #1
	db	CMD_PAR_7BIT,			MBFM_Ix_WT_PAR2		; wt parameter #2
	db	CMD_PAR_7BIT,			MBFM_Ix_WT_PAR3		; wt parameter #3
#endif
	db	CMD_PAR_NOP,			0			; reserved
#if USE_SHADOW_BUFFERS
	db	CMD_PAR_MODWHEEL,               MBFM_Ix_MODWHEEL_ASSIGN ; modwheel parameter
#else
	db	CMD_PAR_7BIT,                   MBFM_Ix_MODWHEEL_ASSIGN ; modwheel parameter
#endif
	db	CMD_PAR_MODWHEEL,		MBFM_Ix_MODWHEEL_INIT	; initial value
	db	CMD_PAR_MODWHEEL,		MBFM_Ix_MODWHEEL_DEPTH	; depth
	db	CMD_PAR_NOP,			0			; reserved

; ==========================================================================

	;; drum 0x00-0x07
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_BD_M_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_BD_C_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_HH_O_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_HH_C_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_SD_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_TOM_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_CYM_FLAGS_MULT - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x08-0x0f
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_BD_M_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_BD_C_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_HH_O_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_HH_C_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_SD_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_TOM_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_5_0,	0x80 | (MBFM_DR_CYM_KSL_TL - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x10-0x17
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_BD_M_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_BD_C_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_HH_O_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_HH_C_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_SD_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_TOM_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_CYM_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x18-0x1f
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_BD_M_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_BD_C_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_HH_O_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_HH_C_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_SD_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_TOM_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_CYM_AR_DR - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x20-0x27
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_BD_M_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_BD_C_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_HH_O_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_HH_C_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_SD_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_TOM_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_7_4,	0x80 | (MBFM_DR_CYM_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x28-0x2f
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_BD_M_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_BD_C_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_HH_O_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_HH_C_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_SD_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_TOM_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_INV_7BIT_TO_3_0,	0x80 | (MBFM_DR_CYM_SL_RR - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x30-0x37
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_BD_M_WS - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_BD_C_WS - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_HH_O_WS - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_HH_C_WS - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_SD_WS - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_TOM_WS - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_0,		0x80 | (MBFM_DR_CYM_WS - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x38-0x3f
	db	CMD_PAR_7BIT_TO_3_1,		0x80 | (MBFM_DR_BD_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_0,		0x80 | (MBFM_DR_BD_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_1,		0x80 | (MBFM_DR_HH_SD_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_0,		0x80 | (MBFM_DR_HH_SD_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_3_1,		0x80 | (MBFM_DR_CYM_TOM_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_7BIT_TO_0,		0x80 | (MBFM_DR_CYM_TOM_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x40-0x47
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_BD_SPLIT_LOWER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_HH_O_SPLIT_LOWER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_HH_C_SPLIT_LOWER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_SD_SPLIT_LOWER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_TOM_SPLIT_LOWER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_CYM_SPLIT_LOWER - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x48-0x4f
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_BD_SPLIT_UPPER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_HH_O_SPLIT_UPPER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_HH_C_SPLIT_UPPER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_SD_SPLIT_UPPER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_TOM_SPLIT_UPPER - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_CYM_SPLIT_UPPER - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x50-0x57
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_BD_FRQ - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_BD_DECR - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_HH_FRQ - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_HH_DECR - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_TOM_FRQ - MBFM_DR_BASE)
	db	CMD_PAR_7BIT,			0x80 | (MBFM_DR_TOM_DECR - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x58-0x5f
	db	CMD_PAR_7BIT_TO_7_4,		0x80 | (MBFM_DR_BD_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT_TO_7_4,		0x80 | (MBFM_DR_HH_SD_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_7BIT_TO_7_4,		0x80 | (MBFM_DR_CYM_TOM_CH_FB_CNT - MBFM_DR_BASE)
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved

	;; drum 0x60-0x6f
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved

; ==========================================================================

	;; ensemble 0x00-0x0f
	db	CMD_PAR_7BIT,			MBFM_Ix_BANK		; bank
	db	CMD_PAR_7BIT,			MBFM_Ix_PATCH		; patch
	db	CMD_PAR_7BIT,			MBFM_Ix_MIDI_CHANNEL	; MIDI channel
	db	CMD_PAR_7BIT,			MBFM_Ix_SPLIT_LOWER	; lower split point
	db	CMD_PAR_7BIT,			MBFM_Ix_SPLIT_UPPER	; upper split point
	db	CMD_PAR_7BIT,			MBFM_Ix_CTRLE		; ensemble control flags
	db	CMD_PAR_7BIT,			MBFM_Ix_VOLUME		; volume
	db	CMD_PAR_7BIT,			MBFM_Ix_TRANSPOSE	; transpose
	db	CMD_PAR_7BIT,			MBFM_Ix_UNISONO		; unisono
	db	CMD_PAR_ACHN,			MBFM_Ix_ACHN_OP12	; audio channels OP12
	db	CMD_PAR_ACHN,			MBFM_Ix_ACHN_OP34	; audio channels OP34
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved
	db	CMD_PAR_NOP,			0			; reserved

; ==========================================================================

#if DISPLAY_CC_ASSIGNMENT_NAMES
;; --------------------------------------------------------------------------
;;  IN: index of parameter in WREG
;;  OUT: pointer to CC Name string in TBLPTR[LHU]
;; --------------------------------------------------------------------------
MBFM_PAR_TABLE_CCName_Get
	TABLE_ADDR_MUL_W MBFM_PAR_TABLE_CC_NAMES, 16
	return

MBFM_PAR_TABLE_CC_NAMES
	;; 	<---------------->
	db	"--------        "	; 0x00
	db	"ModWheel        "	; 0x01
	db	"Transpose   All "	; 0x02
	db	"Unisono         "	; 0x03
	db	"--------        "	; 0x04
	db	"--------        "	; 0x05
	db	"--------        "	; 0x06
	db	"Volume      All "	; 0x07
	db	"--------        "	; 0x08
	db	"--------        "	; 0x09
	db	"--------        "	; 0x0a
	db	"--------        "	; 0x0b
	db	"--------        "	; 0x0c
	db	"AudioChn.   All "	; 0x0d
	db	"AudioChn.   OP12"	; 0x0e
	db	"AudioChn.   OP34"	; 0x0f

	db	"OP Flags     OP1"	; 0x10
	db	"OP Flags     OP2"	; 0x11
	db	"OP Flags     OP3"	; 0x12
	db	"OP Flags     OP4"	; 0x13
	db	"OP Mult.     OP1"	; 0x14
	db	"OP Mult.     OP2"	; 0x15
	db	"OP Mult.     OP3"	; 0x16
	db	"OP Mult.     OP4"	; 0x17
	db	"OP KSL       OP1"	; 0x18
	db	"OP KSL       OP2"	; 0x19
	db	"OP KSL       OP3"	; 0x1a
	db	"OP KSL       OP4"	; 0x1b
	db	"OP Volume    OP1"	; 0x1c
	db	"OP Volume    OP2"	; 0x1d
	db	"OP Volume    OP3"	; 0x1e
	db	"OP Volume    OP4"	; 0x1f

	db	"OP Attack    OP1"	; 0x20
	db	"OP Attack    OP2"	; 0x21
	db	"OP Attack    OP3"	; 0x22
	db	"OP Attack    OP4"	; 0x23
	db	"OP Decay     OP1"	; 0x24
	db	"OP Decay     OP2"	; 0x25
	db	"OP Decay     OP3"	; 0x26
	db	"OP Decay     OP4"	; 0x27
	db	"OP Sustain   OP1"	; 0x28
	db	"OP Sustain   OP2"	; 0x29
	db	"OP Sustain   OP3"	; 0x2a
	db	"OP Sustain   OP4"	; 0x2b
	db	"OP Release   OP1"	; 0x2c
	db	"OP Release   OP2"	; 0x2d
	db	"OP Release   OP3"	; 0x2e
	db	"OP Release   OP4"	; 0x2f

	db	"OP Waveform  OP1"	; 0x30
	db	"OP Waveform  OP2"	; 0x31
	db	"OP Waveform  OP3"	; 0x32
	db	"OP Waveform  OP4"	; 0x33
	db	"--------        "	; 0x34
	db	"--------        "	; 0x35
	db	"--------        "	; 0x36
	db	"--------        "	; 0x37
	db	"OP Feedback  OP1"	; 0x38
	db	"--------        "	; 0x39
	db	"--------        "	; 0x3a
	db	"--------        "	; 0x3b
	db	"OP Algorithm    "	; 0x3c
	db	"--------        "	; 0x3d
	db	"--------        "	; 0x3e
	db	"--------        "	; 0x3f

	db	"Mode        LFO1"	; 0x40
	db	"Phase       LFO1"	; 0x41
	db	"Rate        LFO1"	; 0x42
	db	"Pitch Depth LFO1"	; 0x43
	db	"Vol1 Depth  LFO1"	; 0x44
	db	"Vol2 Depth  LFO1"	; 0x45
	db	"Vol3 Depth  LFO1"	; 0x46
	db	"Vol4 Depth  LFO1"	; 0x47
	db	"LFO2Depth   LFO1"	; 0x48
	db	"AOUTDepth   LFO1"	; 0x49
	db	"--------        "	; 0x4a
	db	"--------        "	; 0x4b
	db	"CC Assign.   Vel"	; 0x4c
	db	"Init Value   Vel"	; 0x4d
	db	"Depth        Vel"	; 0x4e
	db	"--------        "	; 0x4f

	db	"Mode        LFO2"	; 0x50
	db	"Phase       LFO2"	; 0x51
	db	"Rate        LFO2"	; 0x52
	db	"Pitch Depth LFO2"	; 0x53
	db	"Vol1 Depth  LFO2"	; 0x54
	db	"Vol2 Depth  LFO2"	; 0x55
	db	"Vol3 Depth  LFO2"	; 0x56
	db	"Vol4 Depth  LFO2"	; 0x57
	db	"LFO2 Depth  LFO2"	; 0x58
	db	"AOUT Depth  LFO2"	; 0x59
	db	"--------        "	; 0x5a
	db	"--------        "	; 0x5b
	db	"CC Assign.   Aft"	; 0x5c
	db	"Init Value   Aft"	; 0x5d
	db	"Depth        Aft"	; 0x5e
	db	"--------        "	; 0x5f

	db	"Mode         EG5"	; 0x60
	db	"Attack       EG5"	; 0x61
	db	"AttLevel     EG5"	; 0x62
	db	"Decay1       EG5"	; 0x63
	db	"DecLevel     EG5"	; 0x64
	db	"Decay2       EG5"	; 0x65
	db	"Sustain      EG5"	; 0x66
	db	"Release      EG5"	; 0x67
	db	"Curve        EG5"	; 0x68
	db	"Pitch Depth  EG5"	; 0x69
	db	"Vol1 Depth   EG5"	; 0x6a
	db	"Vol2 Depth   EG5"	; 0x6b
	db	"Vol3 Depth   EG5"	; 0x6c
	db	"Vol4 Depth   EG5"	; 0x6d
	db	"LFO1 Depth   EG5"	; 0x6e
	db	"AOUT Depth   EG5"	; 0x6f

	db	"Play Mode       "	; 0x70
	db	"--------        "	; 0x71
	db	"MIDI Sync       "	; 0x72
	db	"WT Mode       WT"	; 0x73
	db	"Finetune        "	; 0x74
	db	"Pitch Range     "	; 0x75
	db	"Portamento Rate "	; 0x76
	db	"WT Rate       WT"	; 0x77
	db	"WT Par1 Asg.  WT"	; 0x78
	db	"WT Par2 Asg.  WT"	; 0x79
	db	"WT Par3 Asg.  WT"	; 0x7a
	db	"--------        "	; 0x7b
	db	"CC Assign.   Mod"	; 0x7c
	db	"Init Value   Mod"	; 0x7d
	db	"Depth        Mod"	; 0x7e
	db	"--------        "	; 0x7f
#endif
