; $Id$
;
; MIDIbox LC Motorfader Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by a fader has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_MF_FaderEvent

	;; save the fader number in LC_FADER_NUMBER
	movwf	LC_FADER_NUMBER

#if TOUCH_SENSOR_MODE >= 2
	;; in this mode, no value will be sent if touch sensor not active
	movf	LC_FADER_NUMBER, W
	call	MIOS_MF_SuspendGet
	bz	LC_MF_FaderEvent_End
#endif

	;; store 10-bit LSB value in LC_FADER_POS_L
	movff	MIOS_PARAMETER2, LC_FADER_POS_L

	;; store 10-bit MSB value in LC_FADER_POS_H
	movff	MIOS_PARAMETER3, LC_FADER_POS_H

	;; shift the values to the right position like descriped in
	;; the Logic Control MIDI implementation, chapter 12

	;; 10 bit -> 14 bit (LC_FADER_POS_[LH] << 4)
	clrc
	rlf	LC_FADER_POS_L, F	; 1
	rlf	LC_FADER_POS_H, F
	rlf	LC_FADER_POS_L, F	; 2
	rlf	LC_FADER_POS_H, F
	rlf	LC_FADER_POS_L, F	; 3
	rlf	LC_FADER_POS_H, F
	rlf	LC_FADER_POS_L, F	; 4
	rlf	LC_FADER_POS_H, F

	;; MIDI MSB: (LC_FADER_POS_H << 1) | LC_FADER_POS_L[7]
	clrc
	rlf	LC_FADER_POS_H, W
	btfsc	LC_FADER_POS_L, 7
	iorlw 0x01
	movwf	LC_FADER_POS_H

	;; MIDI LSB: LC_FADER_POS_L & 0x7f
	movlw	0x7f
	andwf	LC_FADER_POS_L, F

	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; finally send value: E<fader> LSB MSB
	movf	LC_FADER_NUMBER, W
	andlw	0x0f
#if MOTORFADER0_IS_MASTERFADER != 0
	skpnz
	iorlw	0x08
#endif
	iorlw	0xe0
	call	MIOS_MIDI_TxBufferPut

	movf	LC_FADER_POS_L, W
	call	MIOS_MIDI_TxBufferPut

	movf	LC_FADER_POS_H, W
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream
	
LC_MF_FaderEvent_End
	;; thats all
	return

;; --------------------------------------------------------------------------
;;  This function is called by LC_MPROC_Received_Ex when a fader should
;;  be moved
;;  Input:
;;     o fader number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_MF_FaderMove
	;; save fader number in LC_FADER_NUMBER
	andlw	0x07
	movwf	LC_FADER_NUMBER

#if TOUCH_SENSOR_MODE >= 1
	;; reset touch detection
	call	MIOS_MF_TouchDetectionReset
#endif

	;; derive 10-bit value from received 14-bit value. Store result in LC_FADER_POS_[LH]
	movff	MIOS_PARAMETER2, LC_FADER_POS_L ; MIDI LSB -> LC_FADER_POS_L
	movff	MIOS_PARAMETER3, LC_FADER_POS_H ; MIDI MSB -> LC_FADER_POS_H

	;; LSB = MSB[0] | MIDI LSB
	btfsc	LC_FADER_POS_H, 0
	bsf	LC_FADER_POS_L, 7
	;; MSB = MIDI MSB >> 1
	clrc
	rrf	LC_FADER_POS_H, F

	;; 10-bit value = 14-bit value >> 4
	clrc
	rrf	LC_FADER_POS_H, F	; 1
	rrf	LC_FADER_POS_L, F
	clrc
	rrf	LC_FADER_POS_H, F	; 2
	rrf	LC_FADER_POS_L, F
	clrc
	rrf	LC_FADER_POS_H, F	; 3
	rrf	LC_FADER_POS_L, F
	clrc
	rrf	LC_FADER_POS_H, F	; 4
	rrf	LC_FADER_POS_L, F

	;; finally move fader
	movff	LC_FADER_POS_L, MIOS_PARAMETER1
	movff	LC_FADER_POS_H, MIOS_PARAMETER2
	movf	LC_FADER_NUMBER, W
	goto	MIOS_MF_FaderMove
