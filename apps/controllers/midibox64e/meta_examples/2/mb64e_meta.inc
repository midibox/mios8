; $Id$
;
; MIDIbox64E
; Customized Meta handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MB64E_midi.inc when a meta event (Fx xx) has 
;;  been assigned to a enc or button
;;  You can use this handler to enhance the firmware by your own MIDI events.
;;  Here you are able to send more than one MIDI event (i.E. two Note On
;;  Events to control Cakewalk via MIDI remote with one button), or a
;;  SysEx/NRPN string to your synthesizer, or just to toggle PIC pins
;;  in order to switch relays...
;;  IN:
;;     on enc events (entry point: MB64E_META_Handler_Enc):
;;        o Enc number in MB64E_CURRENT_ENC (BANKED access required!)
;;        o first MIDI byte in MIDI_EVNT0 (no BANKED access required)
;;        o second MIDI byte in MIDI_EVNT1 (no BANKED access required)
;;        o enc value in MIDI_EVNT_VALUE (no BANKED access required)
;;
;;     on button events (entry point: MB64E_META_Handler_Button):
;;        o Button number in MB64E_CURRENT_BUTTON (BANKED access required!)
;;        o first MIDI byte in MIDI_EVNT0 (no BANKED access required)
;;        o second MIDI byte in MIDI_EVNT1 (no BANKED access required)
;;        o button value in MIDI_EVNT_VALUE (no BANKED access required)
;; --------------------------------------------------------------------------
MB64E_META_Handler_Enc
MB64E_META_Handler_Fader

	;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
	;; ADAPT IT FOR YOUR NEEDS!

	;; we are using the same handler for encs and buttons here
	rgoto	MB64E_META_Handler

MB64E_META_Handler_Button

	;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
	;; ADAPT IT FOR YOUR NEEDS!

	;; we are using the same handler for encs and buttons here
	rgoto	MB64E_META_Handler


;; --------------------------------------------------------------------------
;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
;; ADAPT IT FOR YOUR NEEDS!
;; HINT: if a large number of SysEx strings with the same structure (means:
;; for the same synth) should be sent, it would be more advantageous to
;; use a table which contains the parameter values and refers to the
;; sending routines
;; --------------------------------------------------------------------------
MB64E_META_Handler
	;; branch depending on second meta event byte
	movf	MIDI_EVNT1, W
	JUMPTABLE_2BYTES 17	; 17 entries
	rgoto	MB64E_META_Handler_00
	rgoto	MB64E_META_Handler_01
	rgoto	MB64E_META_Handler_02
	rgoto	MB64E_META_Handler_03
	rgoto	MB64E_META_Handler_04
	rgoto	MB64E_META_Handler_05
	rgoto	MB64E_META_Handler_06
	rgoto	MB64E_META_Handler_07
	rgoto	MB64E_META_Handler_08
	rgoto	MB64E_META_Handler_09
	rgoto	MB64E_META_Handler_0A
	rgoto	MB64E_META_Handler_0B
	rgoto	MB64E_META_Handler_0C
	rgoto	MB64E_META_Handler_0D
	rgoto	MB64E_META_Handler_0E
	rgoto	MB64E_META_Handler_0F
	rgoto	MB64E_META_Handler_10

;; --------------------------------------------------------------------------
MB64E_META_Handler_00
	;; this example sends a SysEx string: F0 43 10 5C 10 10 31 vv F7 (AN1x: high pass filter)
	;; META Event: "F0 00" (enc) or "F0 00 7F @OnOff" (Button - 7F may vary)
	movlw	0x31			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_01
	;; this example sends a SysEx string: F0 43 10 5C 10 10 33 vv F7 (AN1x: cutoff frequency)
	;; META Event: "F0 01" (enc) or "F0 01 7F @OnOff" (Button - 7F may vary)	
	movlw	0x33			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_02
	;; this example sends a SysEx string: F0 43 10 5C 10 10 34 vv F7 (AN1x: resonance)
	;; META Event: "F0 02" (enc) or "F0 02 7F @OnOff" (Button - 7F may vary)	
	movlw	0x34			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_03
	;; this example sends a SysEx string: F0 43 10 5C 10 10 29 vv F7 (AN1x: volume VCO1)
	;; META Event: "F0 03" (enc) or "F0 03 7F @OnOff" (Button - 7F may vary)	
	movlw	0x29			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_04
	;; this example sends a SysEx string: F0 43 10 5C 10 10 2A vv F7 (AN1x: volume VCO2)
	;; META Event: "F0 04" (enc) or "F0 04 7F @OnOff" (Button - 7F may vary)	
	movlw	0x2a			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_05
	;; this example sends a SysEx string: F0 43 10 5C 10 10 2C vv F7 (AN1x: volume noise)
	;; META Event: "F0 05" (enc) or "F0 05 7F @OnOff" (Button - 7F may vary)	
	movlw	0x2c			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_06
	;; this example sends a SysEx string: F0 43 10 5C 10 10 3E vv F7 (AN1x: feedback)
	;; META Event: "F0 06" (enc) or "F0 06 7F @OnOff" (Button - 7F may vary)	
	movlw	0x3e			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
MB64E_META_Handler_07
	;; this example sends a SysEx string: F0 43 10 5C 10 10 3F vv F7 (AN1x: volume)
	;; META Event: "F0 07" (enc) or "F0 07 7F @OnOff" (Button - 7F may vary)	
	movlw	0x3f			; store parameter value in W
	rgoto	META_Send_AN1x_SysEx	; branch to SysEx routine

;; --------------------------------------------------------------------------
	;; This is a subroutine which sends a SysEx stream for AN1x synthesizer
	;; IN: parameter address in working register WREG
	;;     enc/button value in MIDI_SEND
META_Send_AN1x_SysEx
	movwf	TMP1		; temporary store parameter value from working register W into TMP1
	call	MIOS_MIDI_BeginStream	; begin stream
	movlw	0xf0			; send 0xf0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x43			; send 0x43
	call	MIOS_MIDI_TxBufferPut
	movlw	0x10			; send 0x10
	call	MIOS_MIDI_TxBufferPut
	movlw	0x5c			; send 0x5c
	call	MIOS_MIDI_TxBufferPut
	movlw	0x10			; send 0x10
	call	MIOS_MIDI_TxBufferPut
	movlw	0x10			; send 0x10
	call	MIOS_MIDI_TxBufferPut
	movf	TMP1, W			; send parameter value, stored in TMP1
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W	; send enc/button value, stored in MIDI_EVNT_VALUE
	call	MIOS_MIDI_TxBufferPut
	movlw	0xf7			; send 0xf7
	call	MIOS_MIDI_TxBufferPut
	goto	MIOS_MIDI_BeginStream	; end stream and exit

;; --------------------------------------------------------------------------

	;; a trick: all labels are pointing to the same routine
MB64E_META_Handler_08
MB64E_META_Handler_09
MB64E_META_Handler_0A
MB64E_META_Handler_0B
MB64E_META_Handler_0C
MB64E_META_Handler_0D
MB64E_META_Handler_0E
MB64E_META_Handler_0F

	;; this example sends two NOTE On or Off Events in order to control the
	;; MIDI Remote Function of Cakewalk (or Cubase or Logic)
	;; META Event: "F0 0x" (enc) or "F0 0x 36 @OnOff" (Button - 36 may vary and is stored in MIDI_EVNT_VALUE, this value is zero when a button is depressed)
	;; x = 08 to 0F

MB64E_META_Handler_DoubleNote
	call	MIOS_MIDI_BeginStream	; begin stream

	;; branch depending on content of MIDI_EVNT_VALUE
	movf	MIDI_EVNT_VALUE, W		; get MIDI_EVNT_VALUE value
	bz	MB64E_META_Handler_DoubleNoteOff	; when Zero (enc value = 00 or button released): send Note Off

MB64E_META_Handler_DoubleNoteOn
	;; NOTE ON
	movlw	0x90		; send 0x90 (Note On Header, channel 0)
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00		; send 0x00 (C-0) -- cakewalk needs this as MIDI Remote indicator
	call	MIOS_MIDI_TxBufferPut
	movlw	0x7f		; send 0x7F (velocity)
	call	MIOS_MIDI_TxBufferPut

	movf	MIDI_EVNT1, W	; send content of second byte (08-0f)
	call	MIOS_MIDI_TxBufferPut
	movlw	0x7f		; send 0x7F (velocity)
	call	MIOS_MIDI_TxBufferPut

	rgoto	MB64E_META_Handler_DoubleNoteEnd

	;; ----
	
MB64E_META_Handler_DoubleNoteOff
	;; NOTE OFF
	movlw	0x80		; send 0x80 (Note Off Header, channel 0)
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00		; send 0x00 (C-0) -- cakewalk needs this as MIDI Remote indicator
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00		; send 0x00 (velocity) -- 0x00 = NOTE OFF
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT1, W	; send content of second byte (08-0f)
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00		; send 0x00 (velocity) -- 0x00 = NOTE OFF
	call	MIOS_MIDI_TxBufferPut

MB64E_META_Handler_DoubleNoteEnd
	goto	MIOS_MIDI_BeginStream	; end stream and exit


	;; ------------------------------------------------------------------
MB64E_META_Handler_10
	;; this example can be used to send a single-byte System Realtime event with a button:
	;;    F8 00 [00-7F]: send MIDI Clock
	;;    FA 00 [00-7F]: send MIDI Start
	;;    FB 00 [00-7F]: send MIDI Continue
	;;    FC 00 [00-7F]: send MIDI Stop
	movf	MIDI_EVNT0, W		; extract status from first byte
	andlw	0x0f
	iorlw	0xf0			; add SysEx status
	goto	MIOS_MIDI_TxBufferPut	; send byte and exit

