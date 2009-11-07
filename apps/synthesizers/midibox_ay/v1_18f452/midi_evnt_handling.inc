;; midi_evnt_handling.inc


; Aftertouch
MIDI_EVNT_IS_Ax	
	;; not implemented yet	
	return


;; Program Change - receiving two MIDI bytes
MIDI_EVNT_IS_Cx
	;; not implemented yet	
	return


; Channel Pressure
MIDI_EVNT_IS_Dx	
	;; not implemented yet	
	return


;; System Exclusive messages not really supported, sends only the first byte for realtime events
;; SysEx streams etc. have to be send by the user routines
MIDI_EVNT_IS_Fx
	;; not implemented yet	
	return

