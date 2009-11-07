;; -----------
;; note_on.inc
;; -----------

;; THERE WAS A *NOTE ON* MIDI EVENT

; Note On
MIDI_EVNT_IS_9x	
	
	;; CHECKING: VELOCITY = 0? 
	;; JUMPING TO NOTE OFF if *VELOCITY* = 0
	movlw 0x00
	cpfseq MIDI_EVNT_VALUE
		
		;; VELOCITY IS *NOT* Zero
		;; so go on with NOTE ON handling
		rgoto MIDI_EVNT_IS_9x_AND_VELOCITY_IS_NOT_ZERO
		
		;; VELOCITY *IS* ZERO = NOTE OFF
		;; [MIDI_EVNT_IS_8x] => [note_off.inc]
		goto MIDI_EVNT_IS_8x 


;; Let's GO ON WITH *NOTE ON* PROCESSING
MIDI_EVNT_IS_9x_AND_VELOCITY_IS_NOT_ZERO

		

	;; NEXT: WRITE TUNE VALUES
      	;; COASE TUNE TO MIOS_PARAMETER1
      	;; FINE  TUNE TO MIOS_PARAMETER2
      	;; SEE => note_to_hz.inc 
	goto MIDI_EVNT_IS_9x_NOTE_ON_WRITE_TUNE_VALUES



