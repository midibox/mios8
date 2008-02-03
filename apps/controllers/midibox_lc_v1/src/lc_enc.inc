; $Id$
;
; MIDIbox LC Encoder Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
LC_ENC_Handler
	;; if encoder number within 0 and 7, send a V-pot event
	movlw	0x07
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto LC_ENC_SendVPot

	;; if encoder number == 8, send jog-wheel event
	movf	MIOS_PARAMETER1, W
	xorlw	0x08
	bz	LC_ENC_SendJogWheel

	;; else do nothing
	return

;; --------------------------------------------------------------------------
;;  Send a V-Pot event (B0 1x vv)
;;     vv = 01: encoder turned clockwise
;;     vv = 41: encoder turned counter clockwise
;; --------------------------------------------------------------------------
LC_ENC_SendVPot

	;; branch depending on GPC mode
	GOTO_IFSET LC_STAT, LC_STAT_GPC_SEL, ACCESS, LC_GPC_SendVPot

LC_ENC_SendVPot_LC
	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; -> first byte
	movlw	0xb0
	call	MIOS_MIDI_TxBufferPut

	;; -> second byte
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	addlw	0x10
	call	MIOS_MIDI_TxBufferPut

	;; -> third byte
	movlw	0x01
	btfsc	MIOS_PARAMETER2, 7
	movlw 0x41
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream
	
	return

;; --------------------------------------------------------------------------
;;  Send a Jogwheel Pot event (B0 3C vv)
;;     vv = 00 + speed: jogwheel turned clockwise
;;     vv = 40 + speed: jogwheel turned counter clockwise
;; --------------------------------------------------------------------------
LC_ENC_SendJogWheel
	;; branch depending on GPC mode
	BRA_IFSET LC_STAT, LC_STAT_GPC_SEL, ACCESS, LC_GPC_SendJogWheel

LC_ENC_SendJogWheel_LC
	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream
	
	;; -> first byte
	movlw	0xb0
	call	MIOS_MIDI_TxBufferPut

	;; -> second byte
	movlw	0x3c
	call	MIOS_MIDI_TxBufferPut

	;; -> third byte
	movf	MIOS_PARAMETER2, W
	btfsc	MIOS_PARAMETER2, 7
	xorlw 0xff
	btfsc	MIOS_PARAMETER2, 7
	addlw 0x41
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream
	
	return

