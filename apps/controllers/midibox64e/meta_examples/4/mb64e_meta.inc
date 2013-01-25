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

	;; send:
	; 0xb0, 0x0x, vv                    fader value, MSB (of a 10bit value)
	; 0xb0, 0x2x, vv                    fader value, LSB (of a 10bit value)
	;
	; x can be specified with the second byte of the meta event.
	; 
	; MIDI_EVNT_VALUE is 7bit, MSB/LSB represent a 10bit value which is splitted into two 7bit values, accordingly the
	; - MSB will contain the upper 3 bits
	; - LSB will contain the lower 7 bits (padded by 0)
	;
	; this leads to following implementation:

	movlw	0xb0
	call	MIOS_MIDI_TxBufferPut

	SET_BSR MIDI_EVNT1		; addresses the MSB
	movf	MIDI_EVNT1, W
	call	MIOS_MIDI_TxBufferPut

	rrf	MIDI_EVNT_VALUE, W	; rightshifted value into WREG, the leftmost bit is now at #5 (bits counted from 0)
	rrf	WREG, W			; rightshift again, the leftmost bit is now at #4 (bits counted from 0)
	rrf	WREG, W			; rightshift again, the leftmost bit is now at #3 (bits counted from 0)
	rrf	WREG, W			; rightshift again, the leftmost bit is now at #2 (bits counted from 0)
	andlw	0x07			; mask value (3 bits)
	call	MIOS_MIDI_TxBufferPut	; send MSB value

	;; note: no need to send 0xb0 again, we get use of the "running status"
	
	SET_BSR MIDI_EVNT1		; addresses the LSB
	movf	MIDI_EVNT1, W
	addlw	0x20
	call	MIOS_MIDI_TxBufferPut

	rlf	MIDI_EVNT_VALUE, W	; leftshifted value into WREG, we've to shift 3 times
	rlf	WREG, W			; again...
	rlf	WREG, W			; ... and again.
	andlw	0x78			; mask value (5 bits)
	call	MIOS_MIDI_TxBufferPut	; send LSB value

	return
