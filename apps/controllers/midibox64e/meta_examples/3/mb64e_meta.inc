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
	;; send MMC command:
	;; F0 7F 00 06 xx F7
	;; With Meta event "F0 01" you will send MMC Stop
	;; With Meta event "F0 02" you will send MMC Play
	;; With Meta event "F0 03" you will send MMC FFwd
	;; With Meta event "F0 04" you will send MMC Rwd
	;; With Meta event "F0 05" you will send MMC Rec
	;; button mode should be set to "OnOnly"

	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	movlw	0x06
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT1, W
	call	MIOS_MIDI_TxBufferPut
	movlw	0xf7
	call	MIOS_MIDI_TxBufferPut

	return
