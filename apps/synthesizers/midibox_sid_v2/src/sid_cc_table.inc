; $Id$
;
; MIDIbox SID
; CC Table
; maps CCs to engine specific parameters which are listed in sid_par_table.inc
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function sets a parameter via CC
;;  IN:	SID_MIDI_PARAMETER1 selects CC number
;;      SID_MIDI_PARAMETER2 contains CC value
;;      SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE[1:0] contains engine number
;;  USES: TABLAT, TBLPTR[LHU], PROD[LH], MIOS_PARAMETER[123], TMP1 (!!!)
;; --------------------------------------------------------------------------
SID_CC_TABLE_Set
	SET_BSR	SID_BASE

	;; get pointer to table depending on engine
	movf	SID_MIDI_PARAMETER1, W, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, TABLAT
	BRA_IFSET TABLAT, 1, ACCESS, SID_CC_TABLE_Set_DM
SID_CC_TABLE_Set_LB
	BRA_IFSET TABLAT, 0, ACCESS, SID_CC_TABLE_Set_B
SID_CC_TABLE_Set_L
	TABLE_ADDR_MUL_W SID_CC_TABLE_L, 1
	rgoto	SID_CC_TABLE_Set_Cont
SID_CC_TABLE_Set_B
	TABLE_ADDR_MUL_W SID_CC_TABLE_B, 1
	rgoto	SID_CC_TABLE_Set_Cont

SID_CC_TABLE_Set_DM
	BRA_IFSET TABLAT, 0, ACCESS, SID_CC_TABLE_Set_M
SID_CC_TABLE_Set_D
	TABLE_ADDR_MUL_W SID_CC_TABLE_D, 1
	rgoto	SID_CC_TABLE_Set_Cont
SID_CC_TABLE_Set_M
	TABLE_ADDR_MUL_W SID_CC_TABLE_M, 1
	;; 	rgoto	SID_CC_TABLE_Set_Cont

SID_CC_TABLE_Set_Cont

	tblrd*+
	movf	TABLAT, W
	bz	SID_CC_TABLE_Set_End ; NOP

	;; store parameter in TMP1
	movwf	TMP1

	;; check value range by (mis-)using the scale function
	;; scale down 0xffff, and check if the resulting value is <= 0x7f
	movlw	0xff
	movwf	MIOS_PARAMETER1
	movwf	MIOS_PARAMETER2
	bcf	MIOS_PARAMETER3, 3 ; scale down
	movf	TMP1, W
	call	SID_PARIN_Scale

	movf	MIOS_PARAMETER2, W
	bnz	SID_CC_TABLE_Set_Scaled
	movf	MIOS_PARAMETER1, W
	andlw	0x80
	bnz	SID_CC_TABLE_Set_Scaled

SID_CC_TABLE_Set_Unscaled
	;; pass 7bit value -> MIOS_PARAMETER[12]
	movff	SID_MIDI_PARAMETER2, MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	;; modify both channels -> MIOS_PARAMETER3
	movlw	0x03
	movwf	MIOS_PARAMETER3
	;; perform parameter change
	movf	TMP1, W
	call	SID_PARIN_Set
 	rgoto	SID_CC_TABLE_Set_End

SID_CC_TABLE_Set_Scaled
	;; convert 7bit CC value to 16bit value -> MIOS_PARAMETER[12]
	clrf	MIOS_PARAMETER1
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER2
	;; modify both channels -> MIOS_PARAMETER3
	movlw	0x03
	movwf	MIOS_PARAMETER3
	;; perform parameter change
	movf	TMP1, W
	call	SID_PARIN_Set16
	;; 	rgoto	SID_CC_TABLE_Set_End

SID_CC_TABLE_Set_End
	return


;; --------------------------------------------------------------------------
;;  CC Tables
;;  see also mbsidv2_cc_chart.txt
;; 
;;  Note that following parameters are already mapped and therefore listed
;;  as NOP here:
;;    CC#00: select Bank
;;    CC#01: Knob#1
;;    CC#06: NRPN MSB Value
;;    CC#16: Knob#2
;;    CC#17: Knob#3
;;    CC#18: Knob#4
;;    CC#19: Knob#5
;;    CC#38: NRPN LSB Value
;;    CC#64: Sustain
;;    CC#98: NRPN LSB
;;    CC#99: NRPN MSB
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;; -- Lead Engine
;; --------------------------------------------------------------------------
SID_CC_TABLE_L
SID_CC_TABLE_B
SID_CC_TABLE_D
SID_CC_TABLE_M
	db	0x00, 0x00, 0x02, 0x03, 0x04, 0x05, 0x00, 0x01	; offset 0x00
	db	0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	; offset 0x08
	db	0x00, 0x00, 0x00, 0x00, 0x20, 0x21, 0x22, 0x23	; offset 0x10
	db	0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b	; offset 0x18
	db	0x2c, 0x2d, 0x2e, 0x2f, 0x48, 0x4c, 0x00, 0x00	; offset 0x20
	db	0x00, 0x00, 0xc1, 0xc3, 0xc8, 0xc9, 0xca, 0xd1	; offset 0x28
	db	0xd3, 0xd8, 0xd9, 0xda, 0x88, 0x89, 0x8a, 0x8b	; offset 0x30
	db	0x8c, 0x8d, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95	; offset 0x38
	db	0x00, 0x00, 0x00, 0x00, 0x30, 0x31, 0x32, 0x33	; offset 0x40
	db	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f	; offset 0x48
	db	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47	; offset 0x50
	db	0x00, 0x00, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65	; offset 0x58
	db	0x66, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	; offset 0x60
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	; offset 0x68
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	; offset 0x70
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	; offset 0x78
