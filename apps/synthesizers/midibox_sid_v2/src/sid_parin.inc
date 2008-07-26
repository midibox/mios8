; $Id$
;
; MIDIbox SID
; Parameter Input Handling
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;; Sets a Parameter Value
;;  IN: index of parameter in WREG
;;      16bit value in MIOS_PARAMETER[12]
;;      SID channels which should be modified in MIOS_PARAMETER3[1:0]
;;      Multi Patch: Instrument number in MIOS_PARAMETER3[6:4]
;;      Drum Patch: Instrument number in MIOS_PARAMETER3[7:4]
;; --------------------------------------------------------------------------
SID_PARIN_Set
	BRA_IFCLR INTCON, GIE, ACCESS, SID_PARIN_Set_FromIRQ	; (for the case that IRQ already disabled)
	IRQ_DISABLE				; interrupts must be disabled on 16x16 multiplication!
	rcall	SID_PARIN_Set_FromIRQ
	IRQ_ENABLE				; enable interrupts again
	return

SID_PARIN_Set_FromIRQ				; direct entry for SID_PARIN_Mod and SID_PARIN_SetWT
	;; store parameter index in SID_PAR_IX
	SET_BSR	SID_BASE
	movwf	SID_PAR_IX, BANKED

	;; get table entries
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, TABLAT
	call	SID_PAR_TABLE_GetPtr	; expecting engine in TABLAT[1:0]
	tblrd*+			; skip left string
	tblrd*+			; skip right string
	tblrd*+			
	movff	TABLAT, PRODL	; mod ID -> PRODL
	tblrd*+
	movff	TABLAT, FSR1L	; low byte of target registers -> FSR1L

	;; preselect FSR1H
	movlw	HIGH(SID_PATCH_BUFFER_SHADOW)
	movwf	FSR1H

	;; jump depending on mod ID
	movf	PRODL, W
	JUMPTABLE_2BYTES 0x39+1 ; entries
	rgoto	SID_PARIN_Set_NOP
	rgoto	SID_PARIN_Set_7
	rgoto	SID_PARIN_Set_8
	rgoto	SID_PARIN_Set_PM8
	rgoto	SID_PARIN_Set_4L
	rgoto	SID_PARIN_Set_4U
	rgoto	SID_PARIN_Set_PAR12
	rgoto	SID_PARIN_Set_CUSTOM_SW
	rgoto	SID_PARIN_Set_FIL4L
	rgoto	SID_PARIN_Set_FIL4U
	rgoto	SID_PARIN_Set_FIL12
	rgoto	SID_PARIN_Set_FIL8
	rgoto	SID_PARIN_Set_OSC123_PM7
	rgoto	SID_PARIN_Set_OSC123_PM8
	rgoto	SID_PARIN_Set_OSC123_7
	rgoto	SID_PARIN_Set_OSC123_8
	rgoto	SID_PARIN_Set_OSC123_12
	rgoto	SID_PARIN_Set_OSC123_4L
	rgoto	SID_PARIN_Set_OSC123_6L
	rgoto	SID_PARIN_Set_OSC123_4U
	rgoto	SID_PARIN_Set_OSC123_PB
	rgoto	SID_PARIN_Set_MOD_PM8
	rgoto	SID_PARIN_Set_MOD_B76
	rgoto	SID_PARIN_Set_LFO_4U
	rgoto	SID_PARIN_Set_LFO_PM8
	rgoto	SID_PARIN_Set_LFO_8
	rgoto	SID_PARIN_Set_ENV_PM8
	rgoto	SID_PARIN_Set_ENV_8
	rgoto	SID_PARIN_Set_WT_6
	rgoto	SID_PARIN_Set_WT_7
	rgoto	SID_PARIN_Set_WT_POS
	rgoto	SID_PARIN_Set_NOTE
	rgoto	SID_PARIN_Set_OSC_INS_PM7
	rgoto	SID_PARIN_Set_OSC_INS_PM8
	rgoto	SID_PARIN_Set_OSC_INS_7
	rgoto	SID_PARIN_Set_OSC_INS_8
	rgoto	SID_PARIN_Set_OSC_INS_12
	rgoto	SID_PARIN_Set_OSC_INS_4L
	rgoto	SID_PARIN_Set_OSC_INS_6L
	rgoto	SID_PARIN_Set_OSC_INS_4U
	rgoto	SID_PARIN_Set_OSC_INS_PB
	rgoto	SID_PARIN_Set_OSC_BL_PM7
	rgoto	SID_PARIN_Set_OSC_BL_PM8
	rgoto	SID_PARIN_Set_OSC_BL_P8
	rgoto	SID_PARIN_Set_OSC_BL_7
	rgoto	SID_PARIN_Set_OSC_BL_8
	rgoto	SID_PARIN_Set_OSC_BL_12
	rgoto	SID_PARIN_Set_OSC_BL_4L
	rgoto	SID_PARIN_Set_OSC_BL_6L
	rgoto	SID_PARIN_Set_OSC_BL_4U
	rgoto	SID_PARIN_Set_OSC_BL_PB
	rgoto	SID_PARIN_Set_OSC_BL_FIL12
	rgoto	SID_PARIN_Set_OSC_BL_FIL8
	rgoto	SID_PARIN_Set_DRM_8
	rgoto	SID_PARIN_Set_DRM_PM8
	rgoto	SID_PARIN_Set_DRM_4U
	rgoto	SID_PARIN_Set_DRM_4L
	rgoto	SID_PARIN_Set_NOTE_INS


;; ----------------------------------------------------------
SID_PARIN_SID_FIL_MACRO MACRO
	LOCAL	SID_PARIN_SID_FIL_Sub

	RCALL_IFSET MIOS_PARAMETER3, 0, ACCESS, SID_PARIN_SID_FIL_Sub
	btfss	MIOS_PARAMETER3, 1
	return
	movlw	(SID_Ix_L_S2F_BASE-SID_Ix_L_S1F_BASE)
	addwf	FSR1L, F
SID_PARIN_SID_FIL_Sub
	ENDM
	
SID_PARIN_SID_OSC_MACRO MACRO
	LOCAL	SID_PARIN_SID_OSC_Sub
	LOCAL	SID_PARIN_SID_OSC_Single

	RCALL_IFSET MIOS_PARAMETER3, 0, ACCESS, SID_PARIN_SID_OSC_Sub
	btfss	MIOS_PARAMETER3, 1
	return
	movlw	(SID_Ix_L_S2V1_BASE-SID_Ix_L_S1V1_BASE)
	addwf	FSR1L, F
SID_PARIN_SID_OSC_Sub
	movf	SID_PAR_IX, W, BANKED
	andlw	0x03
	bnz	SID_PARIN_SID_OSC_Single
	rcall	SID_PARIN_SID_OSC_Single
	movlw	(SID_Ix_L_S1V2_BASE-SID_Ix_L_S1V1_BASE)
	addwf	FSR1L, F
	rcall	SID_PARIN_SID_OSC_Single
	movlw	(SID_Ix_L_S1V2_BASE-SID_Ix_L_S1V1_BASE)
	addwf	FSR1L, F
	rcall	SID_PARIN_SID_OSC_Single
	movlw	(SID_Ix_L_S1V1_BASE-SID_Ix_L_S1V3_BASE)
	addwf	FSR1L, F
	return
SID_PARIN_SID_OSC_Single
	ENDM

SID_PARIN_SID_OSC_INS_MACRO MACRO
	LOCAL	SID_PARIN_SID_OSC_INS_All
	LOCAL	SID_PARIN_SID_OSC_INS_Single

	rcall	SID_PARIN_Hlp_GetVoices		; affected voices in PRODH
	RCALL_IFSET PRODH, 0, ACCESS, SID_PARIN_SID_OSC_INS_Single
	rcall	SID_PARIN_Hlp_FSR1_NextVoice	; switches to next voice
	RCALL_IFSET PRODH, 1, ACCESS, SID_PARIN_SID_OSC_INS_Single
	rcall	SID_PARIN_Hlp_FSR1_NextVoice	; switches to next voice
	RCALL_IFSET PRODH, 2, ACCESS, SID_PARIN_SID_OSC_INS_Single
	rcall	SID_PARIN_Hlp_FSR1_NextVoice	; switches to next voice
	RCALL_IFSET PRODH, 3, ACCESS, SID_PARIN_SID_OSC_INS_Single
	rcall	SID_PARIN_Hlp_FSR1_NextVoice	; switches to next voice
	RCALL_IFSET PRODH, 4, ACCESS, SID_PARIN_SID_OSC_INS_Single
	rcall	SID_PARIN_Hlp_FSR1_NextVoice	; switches to next voice
	RCALL_IFSET PRODH, 5, ACCESS, SID_PARIN_SID_OSC_INS_Single
	return
SID_PARIN_SID_OSC_INS_Single
	ENDM

SID_PARIN_SID_OSC_BL_MACRO MACRO
	LOCAL	SID_PARIN_SID_OSC_BL_Single

	rcall	SID_PARIN_Hlp_GetBL		; affected basslines in PRODH
	RCALL_IFSET PRODH, 0, ACCESS, SID_PARIN_SID_OSC_BL_Single
	movlw	(SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE)
	addwf	FSR1L, F
	RCALL_IFSET PRODH, 1, ACCESS, SID_PARIN_SID_OSC_BL_Single
	return
SID_PARIN_SID_OSC_BL_Single
	ENDM

SID_PARIN_SID_OSC_BL_FIL_MACRO MACRO
	LOCAL	SID_PARIN_SID_OSC_BL_FIL_Single

	rcall	SID_PARIN_Hlp_GetBL		; affected basslines in PRODH
	RCALL_IFSET PRODH, 0, ACCESS, SID_PARIN_SID_OSC_BL_FIL_Single
	movlw	(SID_Ix_L_S2F_BASE-SID_Ix_L_S1F_BASE)
	addwf	FSR1L, F
	RCALL_IFSET PRODH, 1, ACCESS, SID_PARIN_SID_OSC_BL_FIL_Single
	return
SID_PARIN_SID_OSC_BL_FIL_Single
	ENDM

SID_PARIN_SID_DRM_MACRO MACRO
	swapf	MIOS_PARAMETER3, W
	andlw	0x0f
	mullw	(SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE)
	movf	PRODL, W
	addwf	FSR1L, F
	ENDM

SID_PARIN_SID_NOTE_MACRO MACRO
	LOCAL	SID_PARIN_SID_NOTE_Single

	movf	SID_PAR_IX, W, BANKED
	andlw	0x03
	bnz	SID_PARIN_SID_NOTE_Single
	rcall	SID_PARIN_SID_NOTE_Single
	movlw	(SIDL_V2_BASE-SIDL_V1_BASE)
	addwf	FSR1L, F
	rcall	SID_PARIN_SID_NOTE_Single
	movlw	(SIDL_V3_BASE-SIDL_V2_BASE)
	addwf	FSR1L, F
	rcall	SID_PARIN_SID_NOTE_Single
	movlw	(SIDL_V1_BASE-SIDL_V3_BASE)
	addwf	FSR1L, F
	return
SID_PARIN_SID_NOTE_Single
	ENDM

;; ----------------------------------------------------------

	
SID_PARIN_Set_NOP
	return

SID_PARIN_Set_7
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	movwf	INDF1
	return

SID_PARIN_Set_8
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_PAR12
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	POSTINC1
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	INDF1
	swapf	MIOS_PARAMETER2, W
	andlw	0xf0
	iorwf	POSTDEC1, F
	return

SID_PARIN_Set_CUSTOM_SW
	movf	SID_PAR_IX, W, BANKED
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, SID_PARIN_Set_CUSTOM_SW_1
SID_PARIN_Set_CUSTOM_SW_0
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F
	return
SID_PARIN_Set_CUSTOM_SW_1
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F
	return

SID_PARIN_Set_FIL4L
	SID_PARIN_SID_FIL_MACRO		; select SIDL/SIDR filter
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	return

SID_PARIN_Set_FIL4U
	SID_PARIN_SID_FIL_MACRO		; select SIDL/SIDR filter
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	return

SID_PARIN_Set_FIL12
	SID_PARIN_SID_FIL_MACRO		; select SIDL/SIDR filter
	movff	MIOS_PARAMETER1, POSTINC1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	POSTDEC1
	return

SID_PARIN_Set_FIL8
	SID_PARIN_SID_FIL_MACRO		; select SIDL/SIDR filter
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_OSC123_PM7
SID_PARIN_Set_OSC123_PM8
SID_PARIN_Set_OSC123_7
SID_PARIN_Set_OSC123_8
	SID_PARIN_SID_OSC_MACRO		; select SIDL/SIDR OSC1/2/3
SID_PARIN_Set_PM8
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_OSC123_12
	SID_PARIN_SID_OSC_MACRO		; select SIDL/SIDR OSC1/2/3
	movff	MIOS_PARAMETER1, POSTINC1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	POSTDEC1
	return

SID_PARIN_Set_OSC123_4L
	SID_PARIN_SID_OSC_MACRO		; select SIDL/SIDR OSC1/2/3
SID_PARIN_Set_4L
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC123_6L
	SID_PARIN_SID_OSC_MACRO		; select SIDL/SIDR OSC1/2/3
	movlw	0xc0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC123_4U
	SID_PARIN_SID_OSC_MACRO		; select SIDL/SIDR OSC1/2/3
SID_PARIN_Set_4U
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC123_PB
	movlw	HIGH(SIDL_V1_BASE)	; select right address range
	movwf	FSR1H

	SID_PARIN_SID_NOTE_MACRO	; select OSC1/2/3

	;; transfer to register
	movf	MIOS_PARAMETER1, W
	xorlw	0x80
	movwf	PRODL

	movlw	SID_Vx_PITCHBENDER
	btfsc	MIOS_PARAMETER3, 0
	movff	PRODL, PLUSW1

	movlw	SID_Vx_PITCHBENDER + (SIDR_V1_BASE-SIDL_V1_BASE)
	btfsc	MIOS_PARAMETER3, 1
	movff	PRODL, PLUSW1

	return

SID_PARIN_Set_MOD_PM8
	incf	FSR1H, F	; (select upper address range)
	movff	MIOS_PARAMETER1, INDF1
	return
	
SID_PARIN_Set_MOD_B76
	incf	FSR1H, F	; (select upper address range)
	movlw	0x3f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	iorwf	INDF1, F
	return
	
SID_PARIN_Set_LFO_4U
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	return

SID_PARIN_Set_LFO_PM8
SID_PARIN_Set_LFO_8
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_ENV_PM8
SID_PARIN_Set_ENV_8
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_WT_6
	incf	FSR1H, F	; (select upper address range)
	movlw	0xc0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	iorwf	INDF1, F
	return

SID_PARIN_Set_WT_7
	incf	FSR1H, F	; (select upper address range)
	movlw	0x80
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	iorwf	INDF1, F
	return

SID_PARIN_Set_WT_POS
	movlw	HIGH(SID_WT1_BASE)	; select right address range
	movwf	FSR1H
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_OSC_INS_PM7
SID_PARIN_Set_OSC_INS_PM8
SID_PARIN_Set_OSC_INS_7
SID_PARIN_Set_OSC_INS_8
	SID_PARIN_SID_OSC_INS_MACRO	; select instrument
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_OSC_INS_12
	SID_PARIN_SID_OSC_INS_MACRO	; select instrument
	movff	MIOS_PARAMETER1, POSTINC1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	POSTDEC1
	return

SID_PARIN_Set_OSC_INS_4L
	SID_PARIN_SID_OSC_INS_MACRO	; select instrument
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC_INS_6L
	SID_PARIN_SID_OSC_INS_MACRO	; select instrument
	movlw	0xc0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC_INS_4U
	SID_PARIN_SID_OSC_INS_MACRO	; select instrument
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC_INS_PB
	;; determine affected instruments
	movf	SID_PAR_IX, W, BANKED
	andlw	0x07
	bz	SID_PARIN_Set_OSC_INS_PB_All
	addlw	-1
	bz	SID_PARIN_Set_OSC_INS_PB_Cur
	addlw	-1
SID_PARIN_Set_OSC_INS_PB_CurCont
	call	MIOS_HLP_GetBitORMask
	rgoto	SID_PARIN_Set_OSC_INS_PB_Cont

SID_PARIN_Set_OSC_INS_PB_All
	movlw	0x3f
	rgoto	SID_PARIN_Set_OSC_INS_PB_Cont

SID_PARIN_Set_OSC_INS_PB_Cur
	swapf	MIOS_PARAMETER3, W
	andlw	0x07
	rgoto	SID_PARIN_Set_OSC_INS_PB_CurCont

SID_PARIN_Set_OSC_INS_PB_Cont
	;; store it in PRODH
	movwf	PRODH

	;; store pitchbender value in MIDI voice record
	movf	MIOS_PARAMETER1, W
	xorlw	0x80
	movwf	PRODL

	btfsc	PRODH, 0
	movff	PRODL, SID_MV1_BASE + SID_MVx_PITCHBENDER
	btfsc	PRODH, 1
	movff	PRODL, SID_MV2_BASE + SID_MVx_PITCHBENDER
	btfsc	PRODH, 2
	movff	PRODL, SID_MV3_BASE + SID_MVx_PITCHBENDER
	btfsc	PRODH, 3
	movff	PRODL, SID_MV4_BASE + SID_MVx_PITCHBENDER
	btfsc	PRODH, 4
	movff	PRODL, SID_MV5_BASE + SID_MVx_PITCHBENDER
	btfsc	PRODH, 5
	movff	PRODL, SID_MV6_BASE + SID_MVx_PITCHBENDER

	return


SID_PARIN_Set_OSC_BL_PM7
SID_PARIN_Set_OSC_BL_PM8
SID_PARIN_Set_OSC_BL_7
SID_PARIN_Set_OSC_BL_8
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movff	MIOS_PARAMETER1, INDF1
	return

SID_PARIN_Set_OSC_BL_12
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movff	MIOS_PARAMETER1, POSTINC1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	POSTDEC1
	return

SID_PARIN_Set_OSC_BL_4L
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC_BL_6L
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movlw	0xc0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC_BL_4U
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	return

SID_PARIN_Set_OSC_BL_P8
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movf	MIOS_PARAMETER1, W
	addlw	0x80
	btfss	WREG, 7
	movlw 0xff
	movwf	INDF1
	return

SID_PARIN_Set_OSC_BL_PB
	rcall	SID_PARIN_Hlp_GetBL		; affected basslines in PRODH

	;; store pitchbender value in MIDI voice record
	movf	MIOS_PARAMETER1, W
	xorlw	0x80
	movwf	PRODL

	btfsc	PRODH, 0
	movff	PRODL, SID_MV1_BASE + SID_MVx_PITCHBENDER
	btfsc	PRODH, 1
	movff	PRODL, SID_MV2_BASE + SID_MVx_PITCHBENDER

	return


SID_PARIN_Set_OSC_BL_FIL12
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movff	MIOS_PARAMETER1, POSTINC1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	POSTDEC1
	return

SID_PARIN_Set_OSC_BL_FIL8
	SID_PARIN_SID_OSC_BL_MACRO	; select BL
	movff	MIOS_PARAMETER1, POSTINC1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	POSTDEC1
	return

SID_PARIN_Set_DRM_8
	SID_PARIN_SID_DRM_MACRO		; select drum
	rgoto	SID_PARIN_Set_8

SID_PARIN_Set_DRM_PM8
	SID_PARIN_SID_DRM_MACRO		; select drum
	rgoto	SID_PARIN_Set_PM8

SID_PARIN_Set_DRM_4U
	SID_PARIN_SID_DRM_MACRO		; select drum
	rgoto	SID_PARIN_Set_4U

SID_PARIN_Set_DRM_4L
	SID_PARIN_SID_DRM_MACRO		; select drum
	rgoto	SID_PARIN_Set_4L

;; Note handler for multi engine
SID_PARIN_Set_NOTE_INS
	;; ensure that only one voice modified
	bcf	MIOS_PARAMETER3, 1

	rcall	SID_PARIN_Hlp_GetVoices		; affected voices in PRODH
	movff	PRODH, MUL_A_L			; copy to MUL_A_L (interrupts are disabled)
	lfsr	FSR1, SIDL_V1_BASE
	RCALL_IFSET MUL_A_L, 0, BANKED, SID_PARIN_Set_NOTE_Sub
	lfsr	FSR1, SIDL_V2_BASE
	RCALL_IFSET MUL_A_L, 1, BANKED, SID_PARIN_Set_NOTE_Sub
	lfsr	FSR1, SIDL_V3_BASE
	RCALL_IFSET MUL_A_L, 2, BANKED, SID_PARIN_Set_NOTE_Sub
	lfsr	FSR1, SIDR_V1_BASE
	RCALL_IFSET MUL_A_L, 3, BANKED, SID_PARIN_Set_NOTE_Sub
	lfsr	FSR1, SIDR_V2_BASE
	RCALL_IFSET MUL_A_L, 4, BANKED, SID_PARIN_Set_NOTE_Sub
	lfsr	FSR1, SIDR_V3_BASE
	RCALL_IFSET MUL_A_L, 5, BANKED, SID_PARIN_Set_NOTE_Sub
	return

	
;; Note handler for lead engine
SID_PARIN_Set_NOTE
	movlw	HIGH(SIDL_V1_BASE)	; select right address range
	movwf	FSR1H

	SID_PARIN_SID_NOTE_MACRO	; select OSC1/2/3

SID_PARIN_Set_NOTE_Sub
	;; do nothing if 0x01 (hold note) is played
	decf	MIOS_PARAMETER1, W
	skpnz
	rgoto	SID_PARIN_Set_NOTE_End

	;; clear gate bit if note value is 0
	movf	MIOS_PARAMETER1, W
	bnz	SID_PARIN_Set_NOTE_On
SID_PARIN_Set_NOTE_Off
	BRA_IFCLR MIOS_PARAMETER3, 0, ACCESS, SID_PARIN_Set_NOTE_Off_NoL
SID_PARIN_Set_NOTE_Off_L
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
SID_PARIN_Set_NOTE_Off_NoL
	BRA_IFCLR MIOS_PARAMETER3, 1, ACCESS, SID_PARIN_Set_NOTE_Off_NoR
SID_PARIN_Set_NOTE_Off_R
	movlw	SID_Vx_STATE + (SIDR_V1_BASE-SIDL_V1_BASE)
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
SID_PARIN_Set_NOTE_Off_NoR

	;; propagate to trigger matrix
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_PARIN_Set_NOTE_Off_Trg_DM
SID_PARIN_Set_NOTE_Off_Trg_LB
	BRA_IFSET WREG, 0, ACCESS, SID_PARIN_Set_NOTE_Off_Trg_B
SID_PARIN_Set_NOTE_Off_Trg_L
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 0, WREG
	andlw	0xc0		; (Gates handled separately)
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	;; don't trigger SID_SE_TRG_EVNT_U (the wavetable events) - wouldn't make sense here!
	rgoto	SID_PARIN_Set_NOTE_End

SID_PARIN_Set_NOTE_Off_Trg_B
	rgoto	SID_PARIN_Set_NOTE_End	; not implemented yet

SID_PARIN_Set_NOTE_Off_Trg_DM
	BRA_IFSET WREG, 0, ACCESS, SID_PARIN_Set_NOTE_Off_Trg_M
SID_PARIN_Set_NOTE_Off_Trg_D
	rgoto	SID_PARIN_Set_NOTE_End	; not implemented yet
SID_PARIN_Set_NOTE_Off_Trg_M
	movlw	0x03		; static assignment: ENV release
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	rgoto	SID_PARIN_Set_NOTE_End	; not implemented yet



SID_PARIN_Set_NOTE_On
	;; TODO: hold mode (via trigger matrix?)
	;; TODO: SIDL/R check?

	;; set gate bit if voice is active and gate not already set
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_PARIN_Set_NOTE_On_NoGate	
	BRA_IFSET PLUSW1, SID_V_STATE_GATE_ACTIVE, ACCESS, SID_PARIN_Set_NOTE_On_NoGate
SID_PARIN_Set_NOTE_On_Gate
	;; set gate bit
	BRA_IFCLR MIOS_PARAMETER3, 0, ACCESS, SID_PARIN_Set_NOTE_On_NoL
SID_PARIN_Set_NOTE_On_L
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ
SID_PARIN_Set_NOTE_On_NoL
	BRA_IFCLR MIOS_PARAMETER3, 1, ACCESS, SID_PARIN_Set_NOTE_On_NoR
SID_PARIN_Set_NOTE_On_R
	movlw	SID_Vx_STATE + (SIDR_V1_BASE-SIDL_V1_BASE)
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ
SID_PARIN_Set_NOTE_On_NoR

	;; propagate to trigger matrix
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_PARIN_Set_NOTE_On_Trg_DM
SID_PARIN_Set_NOTE_On_Trg_LB
	BRA_IFSET WREG, 0, ACCESS, SID_PARIN_Set_NOTE_On_Trg_B
SID_PARIN_Set_NOTE_On_Trg_L
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 0, WREG
	andlw	0xc0		; (Gates handled separately)
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	;; don't trigger SID_SE_TRG_EVNT_U (the wavetable events) - wouldn't make sense here!
	rgoto	SID_PARIN_Set_NOTE_On_Trg_Cont

SID_PARIN_Set_NOTE_On_Trg_B
	rgoto	SID_PARIN_Set_NOTE_On_Trg_Cont	; not implemented yet

SID_PARIN_Set_NOTE_On_Trg_DM
	BRA_IFSET WREG, 0, ACCESS, SID_PARIN_Set_NOTE_On_Trg_M
SID_PARIN_Set_NOTE_On_Trg_D
	rgoto	SID_PARIN_Set_NOTE_On_Trg_Cont	; not implemented yet
SID_PARIN_Set_NOTE_On_Trg_M
	movlw	0xc0		; static assignment: ENV attack
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	;; 	rgoto	SID_PARIN_Set_NOTE_On_Trg_Cont	; not implemented yet
SID_PARIN_Set_NOTE_On_Trg_Cont


SID_PARIN_Set_NOTE_On_NoGate
	;; set new note
	;; (temporary stored in TABLAT)
	movff	MIOS_PARAMETER1, TABLAT

	;; if > 0x7c, play arpeggiator note
	movlw	0x7c-1
	cpfsgt	TABLAT, ACCESS
	rgoto SID_PARIN_Set_NOTE_On_NoArp
SID_PARIN_Set_NOTE_On_Arp
	movff	FSR1L, PRODL		; temporary store FSR1 in PROD[LH]
	movff	FSR1H, PRODH
	lfsr	FSR1, SID_MV1_BASE + SID_MVx_WT_STACK_0		; (sorted stack)
								;; TODO: alternatively take HOLD stack (global option?)
	;; multi engine: select base pointer  depending on instrument number
	;; (note: PROD[LH] already allocated, therefore we cannot multiply here)
	swapf	MIOS_PARAMETER3, W
	andlw	0x07
	bz	SID_PARIN_Set_NOTE_On_Arp_MV_Ok
	lfsr	FSR1, SID_MV2_BASE + SID_MVx_WT_STACK_0
	addlw	-1
	bz	SID_PARIN_Set_NOTE_On_Arp_MV_Ok
	lfsr	FSR1, SID_MV3_BASE + SID_MVx_WT_STACK_0
	addlw	-1
	bz	SID_PARIN_Set_NOTE_On_Arp_MV_Ok
	lfsr	FSR1, SID_MV4_BASE + SID_MVx_WT_STACK_0
	addlw	-1
	bz	SID_PARIN_Set_NOTE_On_Arp_MV_Ok
	lfsr	FSR1, SID_MV5_BASE + SID_MVx_WT_STACK_0
	addlw	-1
	bz	SID_PARIN_Set_NOTE_On_Arp_MV_Ok
	lfsr	FSR1, SID_MV6_BASE + SID_MVx_WT_STACK_0
SID_PARIN_Set_NOTE_On_Arp_MV_Ok
	
	movf	TABLAT, W
	addlw	-0x7c
	movf	PLUSW1, W	; (note 1-4)
	skpnz
	movf	PLUSW1, W	; (first note)
	movwf	TABLAT

	movff	PRODL, FSR1L		; restore FSR1 from PROD[LH]
	movff	PRODH, FSR1H

	movf	TABLAT, W
	bz	SID_PARIN_Set_NOTE_Off
SID_PARIN_Set_NOTE_On_NoArp

	;; transfer new note to SID_Vx_NOTE register
	;; (SID_Vx_NOTE cannot be used due to special handling in SID_MIDI_Hlp_GateOff!)
	movlw	SID_Vx_NOTE
	btfsc	MIOS_PARAMETER3, 0	; SIDL?
	movff	TABLAT, PLUSW1
	movlw	SID_Vx_STATE
	btfsc	MIOS_PARAMETER3, 0
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE	; will be cleared automatically if no portamento enabled

	BRA_IFCLR MIOS_PARAMETER3, 1, ACCESS, SID_PARIN_Set_NOTE_End ; SIDR?
	movlw	SID_Vx_NOTE + (SIDR_V1_BASE-SIDL_V1_BASE)
	movff	TABLAT, PLUSW1
	movlw	SID_Vx_STATE + (SIDR_V1_BASE-SIDL_V1_BASE)
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE	; will be cleared automatically if no portamento enabled
SID_PARIN_Set_NOTE_End
	return


;; --------------------------------------------------------------------------
;; Scales a 16bit value to target range or a source value to 16bit
;;  IN: index of parameter in WREG
;;      16bit value in MIOS_PARAMETER[12]
;;      MIOS_PARAMETER3[3] = 0: scale down (from 16bit), else scale up (to 16bit)
;;  OUT: scaled value in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SID_PARIN_Scale
	;; determine number of bits from tables
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, TABLAT
	call	SID_PAR_TABLE_GetPtr	; expecting engine in TABLAT[1:0]
	tblrd*+			; skip left string
	tblrd*+			; skip right string
	tblrd*+			; now TABLAT contains mod ID

	;; store MAX value in SID_PAR_MAX_[LH]
	movf	TABLAT, W	; (mod ID)
	JUMPTABLE_2BYTES 0x39+1 ; entries
	rgoto	SID_PARIN_Scale_0	;NOP
	rgoto	SID_PARIN_Scale_7	;7
	rgoto	SID_PARIN_Scale_8	;8
	rgoto	SID_PARIN_Scale_8	;PM8
	rgoto	SID_PARIN_Scale_4	;4L
	rgoto	SID_PARIN_Scale_4	;4U
	rgoto	SID_PARIN_Scale_12	;PAR12
	rgoto	SID_PARIN_Scale_1	;CUSTOM_SW
	rgoto	SID_PARIN_Scale_4	;FIL4L
	rgoto	SID_PARIN_Scale_4	;FIL4U
	rgoto	SID_PARIN_Scale_12	;FIL12
	rgoto	SID_PARIN_Scale_8	;FIL8
	rgoto	SID_PARIN_Scale_7	;OSC123_PM7
	rgoto	SID_PARIN_Scale_8	;OSC123_PM8
	rgoto	SID_PARIN_Scale_7	;OSC123_7
	rgoto	SID_PARIN_Scale_8	;OSC123_8
	rgoto	SID_PARIN_Scale_12	;OSC123_12
	rgoto	SID_PARIN_Scale_4	;OSC123_4L
	rgoto	SID_PARIN_Scale_6	;OSC123_6L
	rgoto	SID_PARIN_Scale_4	;OSC123_4U
	rgoto	SID_PARIN_Scale_8	;OSC123_PB
	rgoto	SID_PARIN_Scale_8	;MOD_PM8
	rgoto	SID_PARIN_Scale_2	;MOD_B76
	rgoto	SID_PARIN_Scale_4	;LFO_4U
	rgoto	SID_PARIN_Scale_8	;LFO_PM8
	rgoto	SID_PARIN_Scale_8	;LFO_8
	rgoto	SID_PARIN_Scale_8	;ENV_PM8
	rgoto	SID_PARIN_Scale_8	;ENV_8
	rgoto	SID_PARIN_Scale_6	;WT_6
	rgoto	SID_PARIN_Scale_7	;WT_7
	rgoto	SID_PARIN_Scale_7	;WT_POS
	rgoto	SID_PARIN_Scale_7	;NOTE
	rgoto	SID_PARIN_Scale_7	;OSC_INS_PM7
	rgoto	SID_PARIN_Scale_8	;OSC_INS_PM8
	rgoto	SID_PARIN_Scale_7	;OSC_INS_7
	rgoto	SID_PARIN_Scale_8	;OSC_INS_8
	rgoto	SID_PARIN_Scale_12	;OSC_INS_12
	rgoto	SID_PARIN_Scale_4	;OSC_INS_4L
	rgoto	SID_PARIN_Scale_6	;OSC_INS_6L
	rgoto	SID_PARIN_Scale_4	;OSC_INS_4U
	rgoto	SID_PARIN_Scale_8	;OSC_INS_PB
	rgoto	SID_PARIN_Scale_7	;OSC_BL_PM7
	rgoto	SID_PARIN_Scale_8	;OSC_BL_PM8
	rgoto	SID_PARIN_Scale_7	;OSC_BL_P8
	rgoto	SID_PARIN_Scale_7	;OSC_BL_7
	rgoto	SID_PARIN_Scale_8	;OSC_BL_8
	rgoto	SID_PARIN_Scale_12	;OSC_BL_12
	rgoto	SID_PARIN_Scale_4	;OSC_BL_4L
	rgoto	SID_PARIN_Scale_6	;OSC_BL_6L
	rgoto	SID_PARIN_Scale_4	;OSC_BL_4U
	rgoto	SID_PARIN_Scale_8	;OSC_BL_PB
	rgoto	SID_PARIN_Scale_12	;OSC_BL_FIL12
	rgoto	SID_PARIN_Scale_8	;OSC_BL_FIL8
	rgoto	SID_PARIN_Scale_8	;DRM_8
	rgoto	SID_PARIN_Scale_8	;DRM_PM8
	rgoto	SID_PARIN_Scale_4	;DRM_4U
	rgoto	SID_PARIN_Scale_4	;DRM_4L
	rgoto	SID_PARIN_Scale_7	;NOTE_INS

SID_PARIN_Scale_0
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return

SID_PARIN_Scale_1
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_1_Up
SID_PARIN_Scale_1_Down
	movlw	0x00
	btfsc	MIOS_PARAMETER2, 7
	movlw 0x01
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return
SID_PARIN_Scale_1_Up
	movlw	0x00
	btfsc	MIOS_PARAMETER1, 0
	movlw 0x80
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	return

;; ---
SID_PARIN_Scale_2
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_2_Up
SID_PARIN_Scale_2_Down
	swapf	MIOS_PARAMETER2, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return
SID_PARIN_Scale_2_Up
	swapf	MIOS_PARAMETER1, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	return

;; ---
SID_PARIN_Scale_4
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_4_Up
SID_PARIN_Scale_4_Down
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return
SID_PARIN_Scale_4_Up
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	return

;; ---
SID_PARIN_Scale_6
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_6_Up
SID_PARIN_Scale_6_Down
	rrf	MIOS_PARAMETER2, W
	rrf	WREG, W
	andlw	0x3f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return
SID_PARIN_Scale_6_Up
	rlf	MIOS_PARAMETER1, W
	rlf	WREG, W
	andlw	0xfc
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	return

;; ---
SID_PARIN_Scale_7
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_7_Up
SID_PARIN_Scale_7_Down
	rrf	MIOS_PARAMETER2, W
	andlw	0x7f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return
SID_PARIN_Scale_7_Up
	rlf	MIOS_PARAMETER1, W
	andlw	0xfe
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	return

;; ---
SID_PARIN_Scale_8
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_8_Up
SID_PARIN_Scale_8_Down
	movff	MIOS_PARAMETER2, MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return
SID_PARIN_Scale_8_Up
	movff	MIOS_PARAMETER1, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	return

;; ---
SID_PARIN_Scale_12
	BRA_IFSET MIOS_PARAMETER3, 3, ACCESS, SID_PARIN_Scale_12_Up
SID_PARIN_Scale_12_Down
	swapf	MIOS_PARAMETER2, W
	movwf	PRODL
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER1
	movf	PRODL, W
	andlw	0xf0
	iorwf	MIOS_PARAMETER1, F
	return
SID_PARIN_Scale_12_Up
	swapf	MIOS_PARAMETER1, W
	movwf	PRODL
	andlw	0xf0
	movwf	MIOS_PARAMETER1
	swapf	MIOS_PARAMETER2, W
	andlw	0xf0
	movwf	MIOS_PARAMETER2
	movf	PRODL, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER2, F
	return


;; --------------------------------------------------------------------------
;; Sets a parameter value from 16bit value
;;  IN: index of parameter in WREG
;;      16bit value in MIOS_PARAMETER[12]
;;      SID channels which should be modified in MIOS_PARAMETER3[1:0]
;;      Multi Patch: Instrument number in MIOS_PARAMETER3[6:4]
;;      Drum Patch: Instrument number in MIOS_PARAMETER3[7:4]
;; --------------------------------------------------------------------------
SID_PARIN_Set16
	BRA_IFCLR INTCON, GIE, ACCESS, SID_PARIN_Set16_FromIRQ	; (for the case that IRQ already disabled)
	IRQ_DISABLE				; interrupts must be disabled on 16x16 multiplication!
	rcall	SID_PARIN_Set16_FromIRQ
	IRQ_ENABLE				; enable interrupts again
	return

SID_PARIN_Set16_FromIRQ				; direct entry for SID_PARIN_SetWT
	;; store parameter index in SID_PAR_IX
	SET_BSR	SID_BASE
	andlw	0xff			; fix zero flag
	bz	SID_PARIN_Set16_End	; skip if parameter is zero
	movwf	SID_PAR_IX, BANKED

	;; scale 16bit value in MIOS_PARAMETER[12] to target range
	;; (SID_PAR_IX already in WREG)
	bcf	MIOS_PARAMETER3, 3	; scale down
	rcall	SID_PARIN_Scale

	;; set new value
	movf	SID_PAR_IX, W, BANKED
	rcall	SID_PARIN_Set_FromIRQ	; (IRQ already disabled)

SID_PARIN_Set16_End
	return


;; --------------------------------------------------------------------------
;; Sets a NRPN parameter value (called from CC handler in sid_midi_*.inc)
;;  IN: MIDI channel in SID_CURRENT_CHANNEL
;;      NRPN data MSB in SID_MIDI_PARAMETER2
;;      NRPN data LSB in NRPN_DATA_LSB[SID_CURRENT_CHANNEL]
;;      NRPN address LSB in NRPN_ADDRESS_LSB[SID_CURRENT_CHANNEL]
;;      NRPN address MSB in NRPN_ADDRESS_MSB[SID_CURRENT_CHANNEL]
;;      Multi Patch: Instrument number in MIOS_PARAMETER3[6:4]
;;      Drum Patch: Instrument number in MIOS_PARAMETER3[7:4]
;; --------------------------------------------------------------------------
SID_PARIN_SetNRPN
	SET_BSR	SID_BASE

	;; LSB -> PRODL (temporary)
	lfsr	FSR1, NRPN_ADDRESS_LSB
	movf	SID_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW1, PRODL

	;; MSB mask: ensure that following values are accepted:
	;; 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 - scaled values
	;; 0x40, 0x41, 0x42, 0x43, 0x44, 0x45 - absolute values
	lfsr	FSR1, NRPN_ADDRESS_MSB
	movf	SID_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW1, W
	movwf	PRODH		; temporary store in PRODH
	andlw	0xb8		; (mask out bit [2:0] and bit #6)
	bnz	SID_PARIN_SetNRPN_End

	;; invalid if PRODH[2:1] == 3
	movf	PRODH, W
	andlw	0x06
	xorlw	0x06
	bz	SID_PARIN_SetNRPN_End

	;; if PRODH[2:1] == 0 -> turn it to 3 (L/R channel selected)
	movf	PRODH, W
	andlw	0x06
	movlw	0x06
	skpnz
	iorwf	PRODH, F

	;; now select L/R channel
	rrf	PRODH, W
	andlw	0x03
	iorwf	MIOS_PARAMETER3, F

	;; set 8th bit in parameter number-> PRODL
	btfsc	PRODH, 0
	bsf	PRODL, 7

	;; prepare data LSB -> MIOS_PARAMETER1
	lfsr	FSR1, NRPN_DATA_LSB
	movf	SID_CURRENT_CHANNEL, W, BANKED
	movf	PLUSW1, W
	movwf	MIOS_PARAMETER1

	;; branch depending on scaled/absolute selection
	BRA_IFSET PRODH, 6, ACCESS, SID_PARIN_SetNRPN_Abs
SID_PARIN_SetNRPN_Scaled
	;; expecting left-aligned 16bit data word in MIOS_PARAMETER[12]
	movff	SID_MIDI_PARAMETER2, MIOS_PARAMETER2
	clrc
	rlf	MIOS_PARAMETER1, F
	clrc
	rlf	MIOS_PARAMETER1, F
	rlf	MIOS_PARAMETER2, F

	;; call parameter function
	movf	PRODL, W
	rcall	SID_PARIN_Set16

	rgoto	SID_PARIN_SetNRPN_End

SID_PARIN_SetNRPN_Abs
	;; expecting right-aligned 16bit data word in MIOS_PARAMETER[12]
	movff	SID_MIDI_PARAMETER2, MIOS_PARAMETER2
	rrf	MIOS_PARAMETER2, F
	skpc
	bsf	MIOS_PARAMETER1, 7

	;; call parameter function
	movf	PRODL, W
	call	SID_PARIN_Set

	;; 	rgoto	SID_PARIN_SetNRPN_End

SID_PARIN_SetNRPN_End
	return


;; --------------------------------------------------------------------------
;; Sets a Parameter Value from WT handler
;;  IN: index of parameter in WREG
;;      8bit WT value (from WT table) in MIOS_PARAMETER1
;;      SID channels which should be modified in MIOS_PARAMETER3[1:0]
;;      Multi Patch: Instrument number in MIOS_PARAMETER3[6:4]
;;      Drum Patch: Instrument number in MIOS_PARAMETER3[7:4]
;; --------------------------------------------------------------------------
	;; SHOULD ONLY BE CALLED FROM IRQ (sid_se.inc)
SID_PARIN_SetWT
	;; store parameter index in SID_PAR_IX
	SET_BSR	SID_BASE
	andlw	0xff			; fix zero flag
	bz	SID_PARIN_SetWT_End	; skip if parameter is zero
	movwf	SID_PAR_IX, BANKED

	;; branch depending on relative (bit 7 cleared) or absolute value (bit 7 set)
	BRA_IFSET MIOS_PARAMETER1, 7, ACCESS, SID_PARIN_SetWT_Abs
SID_PARIN_SetWT_Rel
	;; convert signed 7bit to signed 15bit, result in MUL_R_[23]
	clrf	MUL_R_2, BANKED
	clrc
	rlf	MIOS_PARAMETER1, W
	addlw	-0x80
	movwf	MUL_R_3, BANKED
	;; skip modification if offset is 0
	bz	SID_PARIN_SetWT_End

	;; get current value -> MIOS_PARAMETER[12]
	;; take value from shadow buffer!
	bsf	MIOS_PARAMETER3, 2
	movf	SID_PAR_IX, W, BANKED
	call	SID_PAROUT_Get

	;; scale value in MIOS_PARAMETER[12] to 16bit
	movf	SID_PAR_IX, W, BANKED
	bsf	MIOS_PARAMETER3, 3	; scale up
	rcall	SID_PARIN_Scale

	;; add MUL_R_[23] to MIOS_PARAMETER[12] and saturate
	movf	MUL_R_2, W, BANKED
	addwf	MIOS_PARAMETER1, F
	movf	MUL_R_3, W, BANKED
	addwfc	MIOS_PARAMETER2, F
	BRA_IFSET MUL_R_3, 7, BANKED, SID_PARIN_SetWT_SatNeg
SID_PARIN_SetWT_SatPos
	bnc	SID_PARIN_SetWT_NoSat
	setf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	rgoto	SID_PARIN_SetWT_Sat_Cont
SID_PARIN_SetWT_SatNeg
	bc	SID_PARIN_SetWT_NoSat
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	;; 	rgoto	SID_PARIN_SetWT_Sat_Cont
SID_PARIN_SetWT_Sat_Cont
SID_PARIN_SetWT_NoSat

	;; scale 16bit value in MIOS_PARAMETER[12] to target range
	movf	SID_PAR_IX, W, BANKED
	bcf	MIOS_PARAMETER3, 3	; scale down
	rcall	SID_PARIN_Scale

	;; store new value
	movf	SID_PAR_IX, W, BANKED
	goto	SID_PARIN_Set


SID_PARIN_SetWT_Abs
	;; 7bit -> 16bit value
	clrc
	rlf	MIOS_PARAMETER1, W
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	;; set new value
	movf	SID_PAR_IX, W, BANKED
	rcall	SID_PARIN_Set16_FromIRQ

SID_PARIN_SetWT_End
	return



;; --------------------------------------------------------------------------
;; Help routine which determines the instrument->voice assignments depending
;; on the instrument selection in SID_PAR_IX
;; only used by the SID_PARIN_SID_OSC_INS_MACRO to save code
;; IN: instrument selection in SID_PAR_IX[2:0]
;;     current instrument in MIOS_PARAMETER3[6:4]
;; OUT: affected voices in PRODH[5:0]
;; --------------------------------------------------------------------------
SID_PARIN_Hlp_GetVoices
	movf	SID_PAR_IX, W, BANKED
	andlw	0x07
	bz	SID_PARIN_Hlp_GetVoices_All
	addlw	-1
	bz	SID_PARIN_Hlp_GetVoices_Cur
SID_PARIN_Hlp_GetVoices_Single
	addlw	-1
SID_PARIN_Hlp_GetVoices_Cur_Cont
	;; final voice selection in PRODH
	clrf	PRODH
	SET_BSR	SIDL_V1_BASE
	cpfseq	SIDL_V1_BASE + SID_Vx_ASSIGNED_MV, BANKED
	rgoto	SID_PARIN_Hlp_GetVoices_Not0
SID_PARIN_Hlp_GetVoices_0
	bsf	PRODH, 0
SID_PARIN_Hlp_GetVoices_Not0
	cpfseq	SIDL_V2_BASE + SID_Vx_ASSIGNED_MV, BANKED
	rgoto	SID_PARIN_Hlp_GetVoices_Not1
SID_PARIN_Hlp_GetVoices_1
	bsf	PRODH, 1
SID_PARIN_Hlp_GetVoices_Not1
	cpfseq	SIDL_V3_BASE + SID_Vx_ASSIGNED_MV, BANKED
	rgoto	SID_PARIN_Hlp_GetVoices_Not2
SID_PARIN_Hlp_GetVoices_2
	bsf	PRODH, 2
SID_PARIN_Hlp_GetVoices_Not2
	cpfseq	SIDR_V1_BASE + SID_Vx_ASSIGNED_MV, BANKED
	rgoto	SID_PARIN_Hlp_GetVoices_Not3
SID_PARIN_Hlp_GetVoices_3
	bsf	PRODH, 3
SID_PARIN_Hlp_GetVoices_Not3
	cpfseq	SIDR_V2_BASE + SID_Vx_ASSIGNED_MV, BANKED
	rgoto	SID_PARIN_Hlp_GetVoices_Not4
SID_PARIN_Hlp_GetVoices_4
	bsf	PRODH, 4
SID_PARIN_Hlp_GetVoices_Not4
	cpfseq	SIDR_V3_BASE + SID_Vx_ASSIGNED_MV, BANKED
	rgoto	SID_PARIN_Hlp_GetVoices_Not5
SID_PARIN_Hlp_GetVoices_5
	bsf	PRODH, 5
SID_PARIN_Hlp_GetVoices_Not5
	SET_BSR	SID_BASE
	return

SID_PARIN_Hlp_GetVoices_All
	movlw	0x3f
	movwf	PRODH
	return

SID_PARIN_Hlp_GetVoices_Cur
	swapf	MIOS_PARAMETER3, W
	andlw	0x07
	rgoto	SID_PARIN_Hlp_GetVoices_Cur_Cont


;; --------------------------------------------------------------------------
;; Help routine which increments FSR1 to the next voice
;; only used by the SID_PARIN_SID_OSC_INS_MACRO to save code
;; --------------------------------------------------------------------------
SID_PARIN_Hlp_FSR1_NextVoice
	movlw	(SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE)
	addwf	FSR1L, F
	movlw	0
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;; Help routine which determines the BL->voice assignments depending
;; on the instrument selection in SID_PAR_IX
;; only used by the SID_PARIN_SID_OSC_BL_MACRO to save code
;; IN: instrument selection in SID_PAR_IX[1:0]
;;     current instrument in MIOS_PARAMETER3[6:4]
;; OUT: affected voices in PRODH[1:0]
;; --------------------------------------------------------------------------
SID_PARIN_Hlp_GetBL
	movf	SID_PAR_IX, W, BANKED
	andlw	0x03
	bz	SID_PARIN_Hlp_GetBL_All
	addlw	-1
	bz	SID_PARIN_Hlp_GetBL_Cur
	addlw	-1
	bz	SID_PARIN_Hlp_GetBL_L
SID_PARIN_Hlp_GetBL_R
	movlw	0x02
	rgoto	SID_PARIN_Hlp_GetBL_End
SID_PARIN_Hlp_GetBL_L
	movlw	0x01
	rgoto	SID_PARIN_Hlp_GetBL_End
SID_PARIN_Hlp_GetBL_Cur
	swapf	MIOS_PARAMETER3, W
	andlw	0x03
	rgoto	SID_PARIN_Hlp_GetBL_End
SID_PARIN_Hlp_GetBL_All
	movlw	0x03
	;; 	rgoto	SID_PARIN_Hlp_GetBL_End
SID_PARIN_Hlp_GetBL_End
	movwf	PRODH
	return



