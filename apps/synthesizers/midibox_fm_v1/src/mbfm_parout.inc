; $Id$
;
; MIDIbox FM
; Parameter Output Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, instrument in MBFM_CURRENT_INSTRUMENT
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
MBFM_PAROUT_Get
	call	MBFM_PAR_TABLE_Get
	;; now:
	;; command   is in MIOS_PARAMETER2
	;; parameter is in FSR1 (pointer)

MBFM_PAROUT_Set_Cmd
	;; branch depending on command
	movf	MIOS_PARAMETER2, W
	JUMPTABLE_2BYTES 17 ; parameters
	rgoto	MBFM_PAROUT_Cmd_NOP
	rgoto	MBFM_PAROUT_Cmd_7BIT
	rgoto	MBFM_PAROUT_Cmd_7BIT_TO_7_4
	rgoto	MBFM_PAROUT_Cmd_7BIT_TO_3_0
	rgoto	MBFM_PAROUT_Cmd_INV_7BIT_TO_7_4
	rgoto	MBFM_PAROUT_Cmd_INV_7BIT_TO_3_0
	rgoto	MBFM_PAROUT_Cmd_7BIT_TO_7_6
	rgoto	MBFM_PAROUT_Cmd_INV_7BIT_TO_5_0
	rgoto	MBFM_PAROUT_Cmd_7BIT_TO_3_1
	rgoto	MBFM_PAROUT_Cmd_7BIT_TO_CNT
	rgoto	MBFM_PAROUT_Cmd_7BIT_TO_0
	rgoto	MBFM_PAROUT_Cmd_ACHN_ALL
	rgoto	MBFM_PAROUT_Cmd_ACHN
	rgoto	MBFM_PAROUT_Cmd_MODWHEEL
	rgoto	MBFM_PAROUT_Cmd_VELOCITY
	rgoto	MBFM_PAROUT_Cmd_AFTERTOUCH
	rgoto	MBFM_PAROUT_Cmd_WTPAR

; =========================================================================

;; --------------------------------------------------------------------------
;;  This function behaves like MBFM_PAROUT_Get with the difference that
;;  address 0x00-0x0f reads the patch name
;; --------------------------------------------------------------------------
MBFM_PAROUT_SysEx_Get
	movwf	TABLAT
	movlw	0x10
	cpfslt	TABLAT, ACCESS
	rgoto MBFM_PAROUT_SysEx_Get_Norm

MBFM_PAROUT_SysEx_Get_Name
	call	MBFM_INSTR_GetBase
	movf	TABLAT, W
	movf	PLUSW1, W
	return

MBFM_PAROUT_SysEx_Get_Norm
	movf	TABLAT, W
	rgoto	MBFM_PAROUT_Get

; ==========================================================================

MBFM_PAROUT_Cmd_NOP
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT
MBFM_PAROUT_Cmd_WTPAR
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT_TO_7_4
	swapf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT_TO_3_0
	movf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_INV_7BIT_TO_7_4
	swapf	INDF1, W
	xorlw	0x0f
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_INV_7BIT_TO_3_0
	comf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT_TO_7_6
	swapf	INDF1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_INV_7BIT_TO_5_0
	comf	INDF1, W
	andlw	0x3f
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT_TO_3_1
	rrf	INDF1, W
	andlw	0x07
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT_TO_CNT
	movlw	0x00
	btfsc	POSTINC1, 0
	iorlw	0x01
	btfsc	POSTDEC1, 0
	iorlw	0x02
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_7BIT_TO_0
	movf	INDF1, W
	andlw	0x01
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_ACHN_ALL
	movf	POSTINC1, W
	iorwf	POSTDEC1, W
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_ACHN
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
MBFM_PAROUT_Cmd_MODWHEEL
MBFM_PAROUT_Cmd_VELOCITY
MBFM_PAROUT_Cmd_AFTERTOUCH
	rgoto	MBFM_PAROUT_Cmd_7BIT

;; --------------------------------------------------------------------------
