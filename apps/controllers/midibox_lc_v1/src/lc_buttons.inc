; $Id$
;
; MIDIbox LC Button Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
LC_BUTTON_Handler
	;; in calibration mode, jump to dedicated button handler
	GOTO_IFSET LC_STAT, LC_STAT_CALI_SEL, ACCESS, LC_CALI_BUTTON_Handler
LC_CALI_BUTTON_Handler_Return

	;; calc address to button function: LC_IO_TABLE + (button_nr << 1)
	TABLE_ADDR LC_IO_TABLE_LAYER0	; init table pointer
	movf	MIOS_PARAMETER1, W
	mullw	2
	;; if layer1 selected, increment high byte (-> +256)
	btfsc	LC_STAT, LC_STAT_LAYER_SEL
	incf	PRODH, F

	movf	PRODL, W		; add low-byte of offset to TBLPTRL
	addwf	TBLPTRL, F
	movf	PRODH, W		; add high-byte of offset to TBLPTRL
	addwfc	TBLPTRH, F
	
	;; read first MIDI byte
	tblrd*+

	;; exit routine if no event has been defined for this button (entry = 0xff)
	incf	TABLAT, W
	skpnz
	return

	;; if bit 7 is set, branch to LC_SFB handler!
	movf	TABLAT, W
	andlw	0x7f
	BRA_IFSET TABLAT, 7, ACCESS, LC_SFB_Handler

#if TOUCH_SENSOR_MODE >= 1
	;; if note event matches with any ID_FADER_TOUCH_CHNx, use the MIOS function
	;; to suspend the faders
#if ID_FADER_TOUCH_CHN1 != 0x68 || ID_FADER_TOUCH_CHN2 != 0x69 || ID_FADER_TOUCH_CHN3 != 0x6a || ID_FADER_TOUCH_CHN4 != 0x6b
	.error "inconsistency in ID_FADER_TOUCH_CHNx - the TOUCH_SENSOR_MODE won't work properly!"
#endif
#if  ID_FADER_TOUCH_CHN5 != 0x6c || ID_FADER_TOUCH_CHN6 != 0x6d || ID_FADER_TOUCH_CHN7 != 0x6e || ID_FADER_TOUCH_CHN8 != 0x6f || ID_FADER_TOUCH_MASTER != 0x70
	.error "inconsistency in ID_FADER_TOUCH_CHNx - the TOUCH_SENSOR_MODE won't work properly!"
#endif
	movf	TABLAT, W
	andlw	0xf8
	xorlw	ID_FADER_TOUCH_CHN1
	bz	LC_BUTTON_Handler_MFSuspend
	movf	TABLAT, W
	xorlw	ID_FADER_TOUCH_MASTER
	bnz	LC_BUTTON_Handler_NoMFSuspend
LC_BUTTON_Handler_MFSuspend
	movf	TABLAT, W
	andlw	0x07
	CALL_IFCLR MIOS_PARAMETER2, 0, ACCESS, MIOS_MF_SuspendEnable
	CALL_IFSET MIOS_PARAMETER2, 0, ACCESS, MIOS_MF_SuspendDisable
LC_BUTTON_Handler_NoMFSuspend
#endif

	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; send first MIDI byte, must be 0x90
	movlw	0x90
	call	MIOS_MIDI_TxBufferPut

	;; send second MIDI byte (table entry)
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut

	;; send third MIDI byte: 0x7f if button pressed, else 0x00
	movlw	0x7f
	btfsc	MIOS_PARAMETER2, 0
	movlw 0x00
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream

	;; thats all
	return
