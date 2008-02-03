; $Id$
;
; MIDIO128
; DIN handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  the DIN handler of MIDIO
;;  Input:
;;     o DIN number in MIOS_PARAMETER1 (AND NOT IN WREG!)
;;     o DIN value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
MIDIO_DIN_Handler
	;; branch to GP function handler
	rgoto	MIDIO_DIN_GP


;; --------------------------------------------------------------------------
;;  general purpose DIN function
;;  Input:
;;     o DIN number in MIOS_PARAMETER1 (AND NOT IN WREG!)
;;     o DIN value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
MIDIO_DIN_GP
	;; save current DIN number in MIDIO_CURRENT_DIN
	SET_BSR MIDIO_BASE
	movf	MIOS_PARAMETER1, W
	movwf	MIDIO_CURRENT_DIN, BANKED

	;; save it also in MIDIO_LAST_DIN0
	;; if LAST_DIN0 != CURRENT_DIN, shift value to LAST_DIN1
	xorwf	MIDIO_LAST_DIN0, W, BANKED
	skpz
	movff	MIDIO_LAST_DIN0, MIDIO_LAST_DIN1
	movff	MIDIO_CURRENT_DIN, MIDIO_LAST_DIN0

	;; store current DIN value in TMP1
	;; invert it if so configured (note: the pins are already inverted by default)
	movf	MIOS_PARAMETER2, W
	btfss	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DIN, BANKED
	xorlw 0x01
	movwf	TMP1

	;; calc address to DIN mode entry:
	call	MIDIO_ADDR_DINModeEntry

	;; in difference to other MIDIboxes the MIDIO128 stores the mode in a 4-bit field
	call	MIDIO_BANK_Read
	;; select nibble
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CURRENT_DIN, 0, BANKED
	swapf	WREG, W
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MIDIO_DIN_OnOff
	rgoto	MIDIO_DIN_OnOnly
	rgoto	MIDIO_DIN_Toggle
	rgoto	MIDIO_DIN_OnOff

	;; (DIN value stored in TMP1)
MIDIO_DIN_OnOff
	;; save status of DIN
	rcall	MIDIO_DIN_Hlp_SaveStat
	rgoto	MIDIO_DIN_Send

	;; (DIN value stored in TMP1)
MIDIO_DIN_OnOnly
	;; save status of DIN
	rcall	MIDIO_DIN_Hlp_SaveStat

	;; when on: send DIN value defined in dump
	;; when off: send nothing
	BRA_IFSET TMP1, 0, ACCESS, MIDIO_DIN_DontSend
	rgoto	MIDIO_DIN_Send

	;; (DIN value stored in TMP1)
MIDIO_DIN_Toggle
	;; when on: toggle between 0x00 and the DIN value defined in dump
	;; when off: send nothing
	BRA_IFSET TMP1, 0, ACCESS, MIDIO_DIN_NotifyChangeEnd

	;; toggle appr. bit in DIN_VALUE register
	call	MIDIO_FSR_DINValue
	SET_BSR	MIDIO_BASE
	movf	MIDIO_CURRENT_DIN, W, BANKED
	call	MIOS_HLP_GetBitORMask
	xorwf	INDF0, F

	;; 	rgoto	MIDIO_DIN_Send


MIDIO_DIN_Send
	;; send event
	call	MIDIO_MIDI_SendDINEvent
MIDIO_DIN_DontSend

	;; if "FORWARD_IO" flag set: forward DIN value to DOUT
	SET_BSR	MIDIO_BASE
	BRA_IFCLR MIDIO_CFG0, MIDIO_CFG0_FORWARD_IO, ACCESS, MIDIO_DIN_NoForward
MIDIO_DIN_Forward
	call	MIDIO_FSR_DINValue
	SET_BSR	MIDIO_BASE
	movf	MIDIO_CURRENT_DIN, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	bnz	MIDIO_DIN_ForwardClr
MIDIO_DIN_ForwardSet
	movf	MIDIO_CURRENT_DIN, W, BANKED
	call	MIDIO_DOUT_Set
	rgoto	MIDIO_DIN_ForwardCont
MIDIO_DIN_ForwardClr
	movf	MIDIO_CURRENT_DIN, W, BANKED
	call	MIDIO_DOUT_Clear
	;; 	rgoto	MIDIO_DIN_ForwardCont
MIDIO_DIN_ForwardCont
MIDIO_DIN_NoForward

	;; request display update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

MIDIO_DIN_NotifyChangeEnd
	return


;; --------------------------------------------------------------------------
	;; this help function saves the DIN status in the MIDIO_DIN register
MIDIO_DIN_Hlp_SaveStat
	call	MIDIO_FSR_DINValue
	movf	MIDIO_CURRENT_DIN, W, BANKED
	andlw	0x07

	BRA_IFSET TMP1, 0, ACCESS, MIDIO_DIN_Hlp_SaveStat1
MIDIO_DIN_Hlp_SaveStat0
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF0, F
	return
MIDIO_DIN_Hlp_SaveStat1
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF0, F
	return
