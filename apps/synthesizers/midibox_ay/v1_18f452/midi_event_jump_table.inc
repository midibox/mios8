; midi_event_jump_table.inc
; MIDIBOX AY - MIDI EVENT HANDLING
; =================================
; 
; 
; MIDI DATA:
; ----------
; MIDI_EVNT0		= MIDI DATA BYTE 0 (1)	
; MIDI_EVNT1		= MIDI DATA BYTE 1 (2)
; MIDI_EVNT_VALUE	= MIDI DATA BYTE 2 (3)
;
; MIDI_EVNT_CHANNEL 	= MIDI CHANNEL (0-15 internal / 1-16 external)


LABEL_MIDI_HANDLING_START
	;; Write The MIDI EVENT to 
	;; MIDI_EVNT0, MIDI_EVNT1 AND MIDI_EVNT_VALUE
	
	;; MIDI CHANNEL AND MIDI STATUS HERE 
	movff	MIOS_PARAMETER1, MIDI_EVNT0 
	
	;; MIDI DATA BYTE1 0-127
	movff	MIOS_PARAMETER2, MIDI_EVNT1
	
	;; MIDI DATA BYTE2 0-127
	movff	MIOS_PARAMETER3, MIDI_EVNT_VALUE

	
	
	;; Extract the MIDI CHANNEL:
	;; MIDI CH = MIDI_EVNT0 BIT 0 to 3 => xxxxnnnn
	;; nnnn = 0 to 15 dec. => 0x00 to 0x0F HEX
	;; => MIDI CHANNEL 0-15 (1 to 16)
	movlw 0x0F 		;; => BIN 00001111
  	andwf MIDI_EVNT0, W 	;; AND BIN 00001111 WITH
  	
  	;; WRITE THE MIDI CHANNEL TO => WREG
  	;; WRITE THE CURREND MIDI CHANNEL TO 
  	;; => MIDI_EVNT_CHANNEL
 	movwf MIDI_EVNT_CHANNEL 

 	
	

	;; Branch depending on the MIDI EVENT
	;; THIS IS THE MIDI EVENT JUMP TABLE :-)
	SET_BSR	MIDI_EVNT0
	swapf	MIDI_EVNT0, W, BANKED
	andlw	0x07

	JUMPTABLE_4BYTES 8 ; 8 entries
		goto	MIDI_EVNT_IS_8x	; Note Off 		=> note_off.inc
		goto	MIDI_EVNT_IS_9x	; Note On		=> note_on.inc
		goto	MIDI_EVNT_IS_Ax	; Aftertouch		=> midi_evnt_handling.inc
		goto	MIDI_EVNT_IS_Bx	; Controller		=> midi_controller.inc
		goto	MIDI_EVNT_IS_Cx	; Program Change	=> midi_evnt_handling.inc
		goto	MIDI_EVNT_IS_Dx	; Channel Pressure	=> midi_evnt_handling.inc
		goto	MIDI_EVNT_IS_Ex	; Pitch Bend		=> midi_evnt_pitch_wheel.inc
		goto	MIDI_EVNT_IS_Fx	; not supported, 
				
	
		
		return




;; ------[END OF MIDI EVENT FUNCTION]----

MIDI_EVNT_End

	return
	