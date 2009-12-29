; $Id$
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
;;        o Enc number in MB64E_CURRENT_BUTTON (BANKED access required!)
;;        o first MIDI byte in MIDI_EVNT0 (no BANKED access required)
;;        o second MIDI byte in MIDI_EVNT1 (no BANKED access required)
;;        o button value in MIDI_EVNT_VALUE (no BANKED access required)
;; --------------------------------------------------------------------------
MB64E_META_Handler_Enc

	;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
	;; ADAPT IT FOR YOUR NEEDS!

	;; we are using the same handler for pots and buttons here
	rgoto	MB64E_META_Handler

MB64E_META_Handler_Fader

	;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
	;; ADAPT IT FOR YOUR NEEDS!

	;; we are using the same handler for pots and buttons here
	rgoto	MB64E_META_Handler

MB64E_META_Handler_Button

	;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
	;; ADAPT IT FOR YOUR NEEDS!

	;; we are using the same handler for pots and buttons here
	rgoto	MB64E_META_Handler

;; --------------------------------------------------------------------------
;; THIS IS JUST AN EXAMPLE META EVENT HANDLER
;; ADAPT IT FOR YOUR NEEDS!
;; HINT: if a large number of SysEx strings with the same structure (means:
;; for the same synth) should be sent, it would be more advantageous to
;; use a table which contains the parameter values and refers to the
;; sending routines
;; 
;; More examples are located in the meta_examples directory
;; --------------------------------------------------------------------------
MB64E_META_Handler

	; F0 (sysex start)
	; 01 (factory brand ID)
	; 20 (synth type or ID)
	; 01 (file version)
	; 01 (program parameter)
	; xx (parameter number) varies fom 0 to 127 (in decimal) but is dedicated to one control ie one pot for instance
	; yy (parameter value = LSB nibble)
	; zz (parameter value = MSB nibble)
	; F7 (sysex end)

	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut
	movlw	0x20
	call	MIOS_MIDI_TxBufferPut
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT1, W		; parameter number == number of meta event (e.g. Meta Event 0xf0 0x01 -> MIDI_EVNT1 == 0x01)
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W	; extract lower nibble
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	swapf	MIDI_EVNT_VALUE, W	; extract upper nibble
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	andlw	0xf7
	call	MIOS_MIDI_TxBufferPut

	return
