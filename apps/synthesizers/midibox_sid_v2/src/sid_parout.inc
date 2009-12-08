; $Id$
;
; MIDIbox SID
; Parameter Output Handling
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;; Returns a Parameter Value
;;  IN: index of parameter in WREG
;;      SID channel in MIOS_PARAMETER3[1:0] (if 0, 1 or 3, SIDL register will be returned, with 2 the SIDR register)
;;      Multi Patch: Instrument number in MIOS_PARAMETER3[6:4]
;;      Drum Patch: Instrument number in MIOS_PARAMETER3[7:4]
;;      MIOS_PARAMETER3[2] if 0: take from normal buffer, if 1: take from shadow buffer
;;  OUT: parameter value in MIOS_PARAMETER[12]
;;       low-byte in WREG
;; --------------------------------------------------------------------------
SID_PAROUT_Get
	;; get table entries
	movwf	PRODH		; store index in PRODH
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, TABLAT
	call	SID_PAR_TABLE_GetPtr	; expecting engine in TABLAT[1:0]
	tblrd*+			; skip left string
	tblrd*+			; skip right string
	tblrd*+			
	movff	TABLAT, PRODL	; mod ID -> PRODL
	tblrd*+
	movff	TABLAT, FSR1L	; low byte of target registers -> FSR1L

	;; preselect FSR1H
	movlw	HIGH(SID_PATCH_BUFFER)
	btfsc	MIOS_PARAMETER3, 2
	movlw HIGH(SID_PATCH_BUFFER_SHADOW)
	movwf	FSR1H

	;; pre-clear MIOS_PARAMETER2 (for functions which only return a 8bit value)
	clrf	MIOS_PARAMETER2

	;; jump depending on mod ID
	movf	PRODL, W
	JUMPTABLE_2BYTES 0x3d+1 ; entries
	rgoto	SID_PAROUT_Get_NOP
	rgoto	SID_PAROUT_Get_7
	rgoto	SID_PAROUT_Get_8
	rgoto	SID_PAROUT_Get_PM8
	rgoto	SID_PAROUT_Get_4L
	rgoto	SID_PAROUT_Get_4U
	rgoto	SID_PAROUT_Get_PAR12
	rgoto	SID_PAROUT_Get_CUSTOM_SW
	rgoto	SID_PAROUT_Get_FIL4L
	rgoto	SID_PAROUT_Get_FIL4U
	rgoto	SID_PAROUT_Get_FIL12
	rgoto	SID_PAROUT_Get_FIL12_DIRECT
	rgoto	SID_PAROUT_Get_FIL8
	rgoto	SID_PAROUT_Get_OSC123_PM7
	rgoto	SID_PAROUT_Get_OSC123_PM8
	rgoto	SID_PAROUT_Get_OSC123_7
	rgoto	SID_PAROUT_Get_OSC123_8
	rgoto	SID_PAROUT_Get_OSC123_12
	rgoto	SID_PAROUT_Get_OSC123_4L
	rgoto	SID_PAROUT_Get_OSC123_5L
	rgoto	SID_PAROUT_Get_OSC123_6L
	rgoto	SID_PAROUT_Get_OSC123_4U
	rgoto	SID_PAROUT_Get_OSC123_PB
	rgoto	SID_PAROUT_Get_MOD_PM8
	rgoto	SID_PAROUT_Get_MOD_B76
	rgoto	SID_PAROUT_Get_LFO_4U
	rgoto	SID_PAROUT_Get_LFO_PM8
	rgoto	SID_PAROUT_Get_LFO_8
	rgoto	SID_PAROUT_Get_ENV_PM8
	rgoto	SID_PAROUT_Get_ENV_8
	rgoto	SID_PAROUT_Get_WT_6
	rgoto	SID_PAROUT_Get_WT_7
	rgoto	SID_PAROUT_Get_WT_POS
	rgoto	SID_PAROUT_Get_NOTE
	rgoto	SID_PAROUT_Get_OSC_INS_PM7
	rgoto	SID_PAROUT_Get_OSC_INS_PM8
	rgoto	SID_PAROUT_Get_OSC_INS_7
	rgoto	SID_PAROUT_Get_OSC_INS_8
	rgoto	SID_PAROUT_Get_OSC_INS_12
	rgoto	SID_PAROUT_Get_OSC_INS_4L
	rgoto	SID_PAROUT_Get_OSC_INS_5L
	rgoto	SID_PAROUT_Get_OSC_INS_6L
	rgoto	SID_PAROUT_Get_OSC_INS_4U
	rgoto	SID_PAROUT_Get_OSC_INS_PB
	rgoto	SID_PAROUT_Get_OSC_BL_PM7
	rgoto	SID_PAROUT_Get_OSC_BL_PM8
	rgoto	SID_PAROUT_Get_OSC_BL_P8
	rgoto	SID_PAROUT_Get_OSC_BL_7
	rgoto	SID_PAROUT_Get_OSC_BL_8
	rgoto	SID_PAROUT_Get_OSC_BL_12
	rgoto	SID_PAROUT_Get_OSC_BL_4L
	rgoto	SID_PAROUT_Get_OSC_BL_5L
	rgoto	SID_PAROUT_Get_OSC_BL_6L
	rgoto	SID_PAROUT_Get_OSC_BL_4U
	rgoto	SID_PAROUT_Get_OSC_BL_PB
	rgoto	SID_PAROUT_Get_OSC_BL_FIL12
	rgoto	SID_PAROUT_Get_OSC_BL_FIL8
	rgoto	SID_PAROUT_Get_DRM_8
	rgoto	SID_PAROUT_Get_DRM_PM8
	rgoto	SID_PAROUT_Get_DRM_4U
	rgoto	SID_PAROUT_Get_DRM_4L
	rgoto	SID_PAROUT_Get_NOTE_INS



;; ----------------------------------------------------------
SID_PAROUT_SID_FIL_MACRO MACRO
	LOCAL	SID_PAROUT_SID_FIL_SIDR
	LOCAL	SID_PAROUT_SID_FIL_SIDL

	movf	MIOS_PARAMETER3, W
	andlw	0x03
	xorlw	0x02
	bnz	SID_PAROUT_SID_FIL_SIDL
SID_PAROUT_SID_FIL_SIDR
	movlw	(SID_Ix_L_S2F_BASE-SID_Ix_L_S1F_BASE)
	addwf	FSR1L, F
SID_PAROUT_SID_FIL_SIDL
	ENDM
	
SID_PAROUT_SID_OSC_MACRO MACRO
	LOCAL	SID_PAROUT_SID_OSC_SIDR
	LOCAL	SID_PAROUT_SID_OSC_SIDL

	movf	MIOS_PARAMETER3, W
	andlw	0x03
	xorlw	0x02
	bnz	SID_PAROUT_SID_OSC_SIDL
SID_PAROUT_SID_OSC_SIDR
	movlw	(SID_Ix_L_S2V1_BASE-SID_Ix_L_S1V1_BASE)
	addwf	FSR1L, F
SID_PAROUT_SID_OSC_SIDL
	ENDM

SID_PAROUT_SID_OSC_INS_MACRO MACRO
	LOCAL	SID_PAROUT_SID_OSC_INS_ALL
	LOCAL	SID_PAROUT_SID_OSC_INS_CUR
	LOCAL	SID_PAROUT_SID_OSC_INS_CUR_CONT
	LOCAL	SID_PAROUT_SID_OSC_INS_N

	movf	PRODH, W
	andlw	0x07
	bz	SID_PAROUT_SID_OSC_INS_ALL
	addlw	-1
	bz	SID_PAROUT_SID_OSC_INS_CUR
	rgoto	SID_PAROUT_SID_OSC_INS_N
SID_PAROUT_SID_OSC_INS_CUR
	swapf	MIOS_PARAMETER3, W
	andlw	0x07
	rgoto	SID_PAROUT_SID_OSC_INS_CUR_CONT
SID_PAROUT_SID_OSC_INS_N
	movf	PRODH, W
	andlw	0x07
	addlw	-2
SID_PAROUT_SID_OSC_INS_CUR_CONT
	mullw	(SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE)
	movf	PRODL, F
	addwf	FSR1L, F
	movf	PRODH, F
	andlw	0x07
	addwfc	FSR1H, F
SID_PAROUT_SID_OSC_INS_ALL
	ENDM

SID_PAROUT_SID_OSC_BL_MACRO MACRO
	LOCAL	SID_PAROUT_SID_OSC_BL_END
	LOCAL	SID_PAROUT_SID_OSC_BL_R

	;; select right channel if "current" (#1) and MIOS_PARAMETER3[4] or R (#3)
	BRA_IFCLR PRODH, 0, ACCESS, SID_PAROUT_SID_OSC_BL_END
	BRA_IFSET PRODH, 1, ACCESS, SID_PAROUT_SID_OSC_BL_R
	BRA_IFSET MIOS_PARAMETER3, 4, ACCESS, SID_PAROUT_SID_OSC_BL_END
SID_PAROUT_SID_OSC_BL_R
	movlw	(SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE)
	addwf	FSR1L, F
SID_PAROUT_SID_OSC_BL_END
	ENDM

SID_PAROUT_SID_OSC_BL_FIL_MACRO MACRO
	LOCAL	SID_PAROUT_SID_OSC_BL_FIL_END
	LOCAL	SID_PAROUT_SID_OSC_BL_FIL_R

	;; select right channel if "current" (#1) and MIOS_PARAMETER3[4] or R (#3)
	BRA_IFCLR PRODH, 0, ACCESS, SID_PAROUT_SID_OSC_BL_FIL_END
	BRA_IFSET PRODH, 1, ACCESS, SID_PAROUT_SID_OSC_BL_FIL_R
	BRA_IFSET MIOS_PARAMETER3, 4, ACCESS, SID_PAROUT_SID_OSC_BL_FIL_END
SID_PAROUT_SID_OSC_BL_FIL_R
	movlw	(SID_Ix_L_S2F_BASE-SID_Ix_L_S1F_BASE)
	addwf	FSR1L, F
SID_PAROUT_SID_OSC_BL_FIL_END
	ENDM

SID_PAROUT_SID_DRM_MACRO MACRO
	swapf	MIOS_PARAMETER3, W
	andlw	0x0f
	mullw	(SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE)
	movf	PRODL, W
	addwf	FSR1L, F
	ENDM

;; ----------------------------------------------------------
	
	
SID_PAROUT_Get_NOP
	movlw	0x00
SID_PAROUT_Get_End
	movwf	MIOS_PARAMETER1
	return

SID_PAROUT_Get_7
	movf	INDF1, W
	andlw	0x7f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_8
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_PAR12
	swapf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	swapf	POSTDEC1, W
	andlw	0xf0
	movwf	MIOS_PARAMETER1
	swapf	INDF1, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER1, F
	movf	MIOS_PARAMETER1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_CUSTOM_SW
	movf	PRODH, W
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	skpz
	movlw	0x01
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_FIL4L
	SID_PAROUT_SID_FIL_MACRO	; select SIDL/SIDR filter
	movf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_FIL4U
	SID_PAROUT_SID_FIL_MACRO	; select SIDL/SIDR filter
	swapf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_FIL12
	SID_PAROUT_SID_FIL_MACRO	; for selected SIDL/SIDR filter
SID_PAROUT_Get_FIL12_DIRECT		; for direct reads from SIDL/SIDR Filter value
	movf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	movf	POSTDEC1, W	; (there is no PREDEC1)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_FIL8
	SID_PAROUT_SID_FIL_MACRO	; select SIDL/SIDR filter
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_PM7
SID_PAROUT_Get_OSC123_PM8
SID_PAROUT_Get_OSC123_7
SID_PAROUT_Get_OSC123_8
	SID_PAROUT_SID_OSC_MACRO	; select SIDL/SIDR oscillator
SID_PAROUT_Get_PM8
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_12
	SID_PAROUT_SID_OSC_MACRO	; select SIDL/SIDR oscillator
	movf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	movf	POSTDEC1, W	; (there is no PREDEC1)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_4L
	SID_PAROUT_SID_OSC_MACRO	; select SIDL/SIDR oscillator
SID_PAROUT_Get_4L
	movf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_5L
	SID_PAROUT_SID_OSC_MACRO	; select SIDL/SIDR oscillator
	movf	INDF1, W
	andlw	0x1f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_6L
	SID_PAROUT_SID_OSC_MACRO	; select SIDL/SIDR oscillator
	movf	INDF1, W
	andlw	0x3f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_4U
	SID_PAROUT_SID_OSC_MACRO	; select SIDL/SIDR oscillator
SID_PAROUT_Get_4U
	swapf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC123_PB
	movlw	HIGH(SIDL_V1_BASE)	; select right address range
	movwf	FSR1H
	;; SIDR only selected?
	movf	MIOS_PARAMETER3, W
	andlw	0x03
	xorlw	0x02
	bnz	SID_PAROUT_Get_OSC123_PB_SIDL
SID_PAROUT_Get_OSC123_PB_SIDR
	movlw	SIDR_V1_BASE-SIDL_V1_BASE
	addwf	FSR1L, F
SID_PAROUT_Get_OSC123_PB_SIDL
	movlw	SID_Vx_PITCHBENDER
	movf	PLUSW1, W
	xorlw	0x80
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_MOD_PM8
	incf	FSR1H, F	; (select upper address range)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End
	
SID_PAROUT_Get_MOD_B76
	incf	FSR1H, F	; (select upper address range)
	swapf	INDF1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	rgoto	SID_PAROUT_Get_End
	
SID_PAROUT_Get_LFO_4U
	swapf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_LFO_PM8
SID_PAROUT_Get_LFO_8
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_ENV_PM8
SID_PAROUT_Get_ENV_8
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_WT_6
	incf	FSR1H, F	; (select upper address range)
	movf	INDF1, W
	andlw	0x3f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_WT_7
	incf	FSR1H, F	; (select upper address range)
	movf	INDF1, W
	andlw	0x7f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_WT_POS
	movlw	HIGH(SID_WT1_BASE)	; select right address range
	movwf	FSR1H
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_PM7
SID_PAROUT_Get_OSC_INS_PM8
SID_PAROUT_Get_OSC_INS_7
SID_PAROUT_Get_OSC_INS_8
	SID_PAROUT_SID_OSC_INS_MACRO	; select instrument
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_12
	SID_PAROUT_SID_OSC_INS_MACRO	; select instrument
	movf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	movf	POSTDEC1, W	; (there is no PREDEC1)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_4L
	SID_PAROUT_SID_OSC_INS_MACRO	; select instrument
	movf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_5L
	SID_PAROUT_SID_OSC_INS_MACRO	; select instrument
	movf	INDF1, W
	andlw	0x1f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_6L
	SID_PAROUT_SID_OSC_INS_MACRO	; select instrument
	movf	INDF1, W
	andlw	0x3f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_4U
	SID_PAROUT_SID_OSC_INS_MACRO	; select instrument
	swapf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_INS_PB
	;; TODO
	movlw	HIGH(SIDL_V1_BASE)	; select right address range
	movwf	FSR1H
	;; SIDR only selected?
	movf	MIOS_PARAMETER3, W
	andlw	0x03
	xorlw	0x02
	bnz	SID_PAROUT_Get_OSC_INS_PB_SIDL
SID_PAROUT_Get_OSC_INS_PB_SIDR
	movlw	SIDR_V1_BASE-SIDL_V1_BASE
	addwf	FSR1L, F
SID_PAROUT_Get_OSC_INS_PB_SIDL
	movlw	SID_Vx_PITCHBENDER
	movf	PLUSW1, W
	xorlw	0x80
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_PM7
SID_PAROUT_Get_OSC_BL_PM8
SID_PAROUT_Get_OSC_BL_7
SID_PAROUT_Get_OSC_BL_8
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_12
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	movf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	movf	POSTDEC1, W	; (there is no PREDEC1)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_4L
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	movf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_5L
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	movf	INDF1, W
	andlw	0x1f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_6L
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	movf	INDF1, W
	andlw	0x3f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_4U
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	swapf	INDF1, W
	andlw	0x0f
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_P8
	SID_PAROUT_SID_OSC_BL_MACRO	; select bassline
	movf	INDF1, W
	addlw	-0x80
	btfsc	WREG, 7
	movlw 0x00
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_PB
	;; TODO
	movlw	HIGH(SIDL_V1_BASE)	; select right address range
	movwf	FSR1H
	;; SIDR only selected?
	movf	MIOS_PARAMETER3, W
	andlw	0x03
	xorlw	0x02
	bnz	SID_PAROUT_Get_OSC_BL_PB_SIDL
SID_PAROUT_Get_OSC_BL_PB_SIDR
	movlw	SIDR_V1_BASE-SIDL_V1_BASE
	addwf	FSR1L, F
SID_PAROUT_Get_OSC_BL_PB_SIDL
	movlw	SID_Vx_PITCHBENDER
	movf	PLUSW1, W
	xorlw	0x80
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_FIL12
	SID_PAROUT_SID_OSC_BL_FIL_MACRO	; select bassline
	movf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	movf	POSTDEC1, W	; (there is no PREDEC1)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_OSC_BL_FIL8
	SID_PAROUT_SID_OSC_BL_FIL_MACRO	; select bassline
	movf	PREINC1, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	movf	POSTDEC1, W	; (there is no PREDEC1)
	movf	INDF1, W
	rgoto	SID_PAROUT_Get_End

SID_PAROUT_Get_DRM_8
	SID_PAROUT_SID_DRM_MACRO	; select drum
	rgoto	SID_PAROUT_Get_8

SID_PAROUT_Get_DRM_PM8
	SID_PAROUT_SID_DRM_MACRO	; select drum
	rgoto	SID_PAROUT_Get_PM8

SID_PAROUT_Get_DRM_4U
	SID_PAROUT_SID_DRM_MACRO	; select drum
	rgoto	SID_PAROUT_Get_4U

SID_PAROUT_Get_DRM_4L
	SID_PAROUT_SID_DRM_MACRO	; select drum
	rgoto	SID_PAROUT_Get_4L

SID_PAROUT_Get_NOTE
SID_PAROUT_Get_NOTE_INS		; TODO: not correct for instruments!
	movlw	HIGH(SIDL_V1_BASE)	; select right address range
	movwf	FSR1H
	;; SIDR only selected?
	movf	MIOS_PARAMETER3, W
	andlw	0x03
	xorlw	0x02
	bnz	SID_PAROUT_Get_NOTE_SIDL
SID_PAROUT_Get_NOTE_SIDR
	movlw	SIDR_V1_BASE-SIDL_V1_BASE
	addwf	FSR1L, F
SID_PAROUT_Get_NOTE_SIDL
	movlw	SID_Vx_NOTE
	movf	PLUSW1, W
	rgoto	SID_PAROUT_Get_End
