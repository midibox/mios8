; $Id$
;
; MIDIboxSEQ
; MIDI Remote Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This handler is called from USER_MPROC_NotifyReceivedEvent
;; 
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;;
;;  Output: ZERO flag cleared if in remote mode (following MIDI parsers should ignore MIDI event)
;; --------------------------------------------------------------------------
SEQ_REMOTE_Handler
	SET_BSR	SEQ_BASE

	;; exit if receive channel disabled
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	skpnz
	return

	;; only continue of MIDI channel matches
	decf	SEQ_MIDI_CHANNEL, W, BANKED
	xorwf	MIOS_PARAMETER1, W
	andlw	0x0f
	bnz	SEQ_REMOTE_Handler_Failed

#if DEFAULT_MIDI_REMOTE_CC
	;; check for Remote CC
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0xb0
	bnz	SEQ_REMOTE_Handler_NoCC
SEQ_REMOTE_Handler_CC
	movf	MIOS_PARAMETER2, W
	xorlw	DEFAULT_MIDI_REMOTE_CC
	bnz	SEQ_REMOTE_Handler_NoCC
	;; values >= 64 activates, values < 64 deactivate remote function
	BRA_IFSET MIOS_PARAMETER3, 6, ACCESS, SEQ_REMOTE_Handler_CC_On
SEQ_REMOTE_Handler_CC_Off
	;; deactivate MIDI remote function
	bcf	SEQ_STATE, SEQ_STATE_REMOTE, BANKED
	;; send depress event to all buttons
	rcall	SEQ_REMOTE_AllOff
	rgoto	SEQ_REMOTE_End
SEQ_REMOTE_Handler_CC_On
	;; activate MIDI remote function and exit
	bsf	SEQ_STATE, SEQ_STATE_REMOTE, BANKED
	rgoto	SEQ_REMOTE_End

SEQ_REMOTE_Handler_NoCC
#endif

	;; check for Note On/Off
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x80
	bz	SEQ_REMOTE_Handler_NoteOff
	xorlw	0x90 ^ 0x80
	bz	SEQ_REMOTE_Handler_NoteOn
	rgoto	SEQ_REMOTE_Handler_Failed

SEQ_REMOTE_Handler_NoteOn
	movf	MIOS_PARAMETER3, W	; branch to NoteOff if velocity is zero
	skpnz
	rgoto	SEQ_REMOTE_Handler_NoteOff

	;; if MIDI remote key has been pressed
	movlw	DEFAULT_MIDI_REMOTE_KEY
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_REMOTE_Handler_NoteOn_NoRem
SEQ_REMOTE_Handler_NoteOn_Rem
	;; activate MIDI remote function and exit
	bsf	SEQ_STATE, SEQ_STATE_REMOTE, BANKED
	rgoto	SEQ_REMOTE_End
SEQ_REMOTE_Handler_NoteOn_NoRem

	;; continue if MIDI remote function active, else exit here with "fail" status
	BRA_IFCLR SEQ_STATE, SEQ_STATE_REMOTE, BANKED, SEQ_REMOTE_Handler_Failed
	rgoto	SEQ_REMOTE_Handler_Cont

SEQ_REMOTE_Handler_NoteOff

	;; if MIDI remote key has been released
	movlw	DEFAULT_MIDI_REMOTE_KEY
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_REMOTE_Handler_NoteOff_NoRem
SEQ_REMOTE_Handler_NoteOff_Rem
	;; deactivate MIDI remote function
	bcf	SEQ_STATE, SEQ_STATE_REMOTE, BANKED
	;; send depress event to all buttons
	rcall	SEQ_REMOTE_AllOff
	rgoto	SEQ_REMOTE_End
SEQ_REMOTE_Handler_NoteOff_NoRem

	;; continue if MIDI remote function active, else exit here with "fail" status
	BRA_IFCLR SEQ_STATE, SEQ_STATE_REMOTE, BANKED, SEQ_REMOTE_Handler_Failed

	;; ensure that velocity value is zero
	clrf	MIOS_PARAMETER3
	;; 	rgoto	SEQ_REMOTE_Handler_Cont

SEQ_REMOTE_Handler_Cont
	;; ignore keys < 0x24 (C-2)
	movlw	0x24-1
	cpfsgt	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_REMOTE_Ignore

	;; ignore keys >= 0x24+0x40 = 0x64 (E-7)
	movlw	0x64
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_REMOTE_Ignore

	;; copy key number - 0x24 to MIOS_PARAMETER1 and inverted key state to MIOS_PARAMETER2
	;; to keep it compatible with the SEQ_BUTTON handlers
	movf	MIOS_PARAMETER2, W
	addlw	-0x24
	movwf	MIOS_PARAMETER1

	clrf	MIOS_PARAMETER2
	movf	MIOS_PARAMETER3, W
	skpnz
	setf	MIOS_PARAMETER2

	;; call button function
	movf	MIOS_PARAMETER1, W
	rcall	SEQ_REMOTE_Handler_Sub

SEQ_REMOTE_Ignore
SEQ_REMOTE_End
	;; ...and exit with ZERO flag cleared (-> don't forward to other MIDI parser functions)
	iorlw	0xff
	return

SEQ_REMOTE_Handler_Failed
	;; exit with ZERO flag cleared (-> continue remaining MIDI parser functions)
	andlw	0x00
	return


;; --------------------------------------------------------------------------
;;  Call table for remote function - each key has a "rgoto" entry
;;  Note that "goto" won't work here (to save space), the distance between
;;  this address and the target function must be +/- 1024
;;  IN: function number in WREG (0x00-0x3f)
;;      MIOS_PARAMETER2 == 0 if button pressed, 0xff if button depressed
;; --------------------------------------------------------------------------
SEQ_REMOTE_Handler_Sub
	;; branch depending on pressed key
	andlw	0x3f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_BUTTON_GP1		; C-2 (0x24)
	rgoto	SEQ_BUTTON_LayerGate	; C#2 (0x25)
	rgoto	SEQ_BUTTON_GP2		; D-2 (0x26)
	rgoto	SEQ_BUTTON_LayerAux	; D#2 (0x27)
	rgoto	SEQ_BUTTON_GP3		; E-2 (0x28)
	rgoto	SEQ_BUTTON_GP4		; F-2 (0x29)
	rgoto	SEQ_BUTTON_SectionA	; F#2 (0x2a)
	rgoto	SEQ_BUTTON_GP5		; G-2 (0x2b)
	rgoto	SEQ_BUTTON_SectionB	; G#2 (0x2c)
	rgoto	SEQ_BUTTON_GP6		; A-2 (0x2d)
	rgoto	SEQ_BUTTON_SectionC	; A#2 (0x2e)
	rgoto	SEQ_BUTTON_GP7		; B-2 (0x2f)

	rgoto	SEQ_BUTTON_GP8		; C-3 (0x30)
	rgoto	SEQ_BUTTON_SectionD	; C#3 (0x31)
	rgoto	SEQ_BUTTON_GP9		; D-3 (0x32)
	rgoto	SEQ_BUTTON_Live		; D#3 (0x33)
	rgoto	SEQ_BUTTON_GP10		; E-3 (0x34)
	rgoto	SEQ_BUTTON_GP11		; F-3 (0x35)
	rgoto	SEQ_REMOTE_Ignore	; F#3 (0x36)
	rgoto	SEQ_BUTTON_GP12		; G-3 (0x37)
	rgoto	SEQ_REMOTE_Ignore	; G#3 (0x38)
	rgoto	SEQ_BUTTON_GP13		; A-3 (0x39)
	rgoto	SEQ_REMOTE_Ignore	; G-4 (0x43)
	rgoto	SEQ_BUTTON_GP14		; B-3 (0x3b)

	rgoto	SEQ_BUTTON_GP15		; C-4 (0x3c)
	rgoto	SEQ_BUTTON_Group1	; C#4 (0x3d)
	rgoto	SEQ_BUTTON_GP16		; D-4 (0x3e)
	rgoto	SEQ_BUTTON_Group2	; D#4 (0x3f)
	rgoto	SEQ_REMOTE_Ignore	; E-4 (0x40)
	rgoto	SEQ_REMOTE_Ignore	; F-4 (0x41)
	rgoto	SEQ_BUTTON_Group3	; F#4 (0x42)
	rgoto	SEQ_REMOTE_Ignore	; G-4 (0x43)
	rgoto	SEQ_BUTTON_Group4	; G#4 (0x44)
	rgoto	SEQ_REMOTE_Ignore	; A-4 (0x45)
	rgoto	SEQ_REMOTE_Ignore	; A#4 (0x46)
	rgoto	SEQ_REMOTE_Ignore	; B-4 (0x47)

	rgoto	SEQ_BUTTON_Edit		; C-5 (0x48)
	rgoto	SEQ_BUTTON_Solo		; C#5 (0x49)
	rgoto	SEQ_BUTTON_Mute		; D-5 (0x4a)
	rgoto	SEQ_BUTTON_All		; D#5 (0x4b)
	rgoto	SEQ_BUTTON_Pattern	; E-5 (0x4c)
	rgoto	SEQ_BUTTON_Song		; F-5 (0x4d)
	rgoto	SEQ_BUTTON_Fast		; F#5 (0x4e)
	rgoto	SEQ_BUTTON_Metronome	; G-5 (0x4f)
	rgoto	SEQ_BUTTON_Scrub	; G#5 (0x50)
	rgoto	SEQ_BUTTON_F3		; A-5 (0x51)
	rgoto	SEQ_BUTTON_F4		; A#5 (0x52)
	rgoto	SEQ_BUTTON_Stop		; B-5 (0x53)

	rgoto	SEQ_BUTTON_Play		; C-6 (0x54)
	rgoto	SEQ_BUTTON_Pause	; C#6 (0x55)
	rgoto	SEQ_BUTTON_Rew		; D-6 (0x56)
	rgoto	SEQ_BUTTON_Fwd		; D#6 (0x57)
	rgoto	SEQ_BUTTON_F1		; E-6 (0x58)
	rgoto	SEQ_BUTTON_F2		; F-6 (0x59)
	rgoto	SEQ_REMOTE_Ignore	; F#6 (0x5a)
	rgoto	SEQ_BUTTON_Alt		; G-6 (0x5b)
	rgoto	SEQ_BUTTON_Shift	; G#6 (0x5c)
	rgoto	SEQ_BUTTON_Alt		; A-6 (0x5d)
	rgoto	SEQ_BUTTON_LeftRemote	; A#6 (0x5e)
	rgoto	SEQ_BUTTON_RightRemote	; B-6 (0x5f)

	rgoto	SEQ_REMOTE_Ignore	; C-7 (0x60) --- Remote Key
	rgoto	SEQ_BUTTON_F4		; C#7 (0x61)
	rgoto	SEQ_BUTTON_F4		; D-7 (0x62)
	rgoto	SEQ_BUTTON_F4		; D#7 (0x63)


;; --------------------------------------------------------------------------
;;  This function sends a "off" event to all buttons
;;  used when the MIDI remote button has been depressed
;; --------------------------------------------------------------------------
SEQ_REMOTE_AllOff
	;; loop through all 64 remote entries
	SET_BSR	SEQ_BASE
	clrf	SEQ_REMOTE_CTR, BANKED
SEQ_REMOTE_AllOffLoop
	setf	MIOS_PARAMETER2
	movf	SEQ_REMOTE_CTR, W, BANKED
	rcall	SEQ_REMOTE_Handler_Sub
	
	SET_BSR	SEQ_BASE
	incf	SEQ_REMOTE_CTR, F, BANKED
	BRA_IFCLR SEQ_REMOTE_CTR, 6, BANKED, SEQ_REMOTE_AllOffLoop

	return
