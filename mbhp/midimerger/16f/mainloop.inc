; $Id: $
;
; Main Loop
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================

	;; Ringbuffer Handling Macro
	;; we are using this macro twice for both ringbuffers
	;; why no subroutine? Because we have enough code space and want
	;; to process the events as fast as possible

;; ==========================================================================

BUFFER_HANDLER MACRO RINGBUFFERx_A, RINGBUFFERx_B, RINGBUFFERx, RINGBUFFERx_END, MIDIx_RUNSTATUS
	LOCAL	Buffer_LoopReset, Buffer_Loop, Buffer_Loop_TimeOut, Buffer_Loop_NoTimeOut, Buffer_ReceiveHandler
	LOCAL	Buffer_NoReset, Buffer_RunningData, Buffer_ForwardSysEx, Buffer_GotCommand, Buffer_GotSystemCommand, Buffer_GotCommandLTF8, Buffer_GotOneByteSyx
	LOCAL	Buffer_SendEnd, Buffer_Loop_End

	clrf	MIDI_EXPBYTES			; clear number of expected bytes

Buffer_LoopReset

	clrf	MIDI_TIMEOUT_LOW		; load timeout counter
	clrf	MIDI_TIMEOUT_HIGH
	movlw	0xe0
	movwf	MIDI_TIMEOUT_HHIGH

Buffer_Loop
	clrwdt					; feed the watchdog

	;; check for time out
	incf	MIDI_TIMEOUT_LOW, F
	skpnz
	incf	MIDI_TIMEOUT_HIGH, F
	skpnz
	incf	MIDI_TIMEOUT_HHIGH, F
	
	movf	MIDI_TIMEOUT_LOW, W
	bnz	Buffer_Loop_NoTimeOut
	movf	MIDI_TIMEOUT_HIGH, W
	bnz	Buffer_Loop_NoTimeOut
	movf	MIDI_TIMEOUT_HHIGH, W
	bz	Buffer_Loop_TimeOut	;; Time Out !!!

Buffer_Loop_NoTimeOut
	;; check for new/next byte in Ringbuffer
	movf	RINGBUFFERx_A, W
	xorwf	RINGBUFFERx_B, W
	bnz	Buffer_ReceiveHandler

	;; if no new byte: loop if waiting for complete MIDI event
	movf	MIDI_EXPBYTES, W
	bnz	Buffer_Loop

	goto	Buffer_Loop_End	; no new value and not waiting for complete event

Buffer_ReceiveHandler	
	;; get byte of SUART Ringbuffer
	incf	RINGBUFFERx_B, F
	movf	RINGBUFFERx_B, W
	xorlw	RINGBUFFERx_END+1
	bnz	Buffer_NoReset
	movlw	RINGBUFFERx
	movwf	RINGBUFFERx_B	
Buffer_NoReset
	movf	RINGBUFFERx_B, W
	movwf	FSR

	;; analyse incoming byte
	GOTO_IFSET INDF, 7, Buffer_GotCommand

	movf	MIDIx_RUNSTATUS, W	; on sysex: don't take care for the expected bytes
	xorlw	0xf0
	bz	Buffer_ForwardSysEx

	movf	MIDI_EXPBYTES, W
	bnz	Buffer_RunningData

	movf	MIDIx_RUNSTATUS, W	; get number of expected bytes
	call	MIDI_GetCMax
	movwf	MIDI_EXPBYTES		; store it in MIDI_EXPBYTES

#if RUNNING_STATUS_OPTIMIZATION == 1
	;; check if MIDI Runstatus has to be send
	movf	MIDIx_RUNSTATUS, W
	xorwf	MIDIO_RUNSTATUS, W
	bz	Buffer_RunningData	; running status still active, don't send it again
#endif

	movf	MIDIx_RUNSTATUS, W	; new running status - store it
	movwf	MIDIO_RUNSTATUS
	call	_MIDI_SendByte		; and send it

Buffer_RunningData
	decf	MIDI_EXPBYTES, F
Buffer_ForwardSysEx
	movf	INDF, W			; send received byte
	call	_MIDI_SendByte
	goto	Buffer_SendEnd

Buffer_GotCommand
	;; Command >= 0xf8?
	movf	INDF, W
	andlw	0xf8
	xorlw	0xf8
	bnz	Buffer_GotCommandLTF8

Buffer_GotSystemCommand
	;; System >= F8: don't change Running Status and number of expected bytes!
	movf	INDF, W			; get received byte
	call	_MIDI_SendByte		; and send it out
	goto	Buffer_SendEnd

Buffer_GotCommandLTF8
	;; command less than F8: store running status, send byte, calc number of expected bytes
	movf	INDF, W			; get received byte
	movwf	MIDIx_RUNSTATUS
	movwf	MIDIO_RUNSTATUS
	call	_MIDI_SendByte		; and send it out

	movf	INDF, W			; get number of expected bytes
	call	MIDI_GetCMax
	movwf	MIDI_EXPBYTES		; store it in MIDI_EXPBYTES
	
Buffer_SendEnd

	;; reset Time Out counter and loop again
	goto	Buffer_LoopReset

	;; we got a Time Out?
Buffer_Loop_TimeOut
#if 1
	movlw	0xff
	call	_MIDI_SendByte
#endif
	
Buffer_Loop_End
	ENDM

;; ==========================================================================

MainLoop
	;; feed the watchdog
	clrwdt
	BUFFER_HANDLER RINGBUFFER0_A, RINGBUFFER0_B, RINGBUFFER0, RINGBUFFER0_END, MIDI0_RUNSTATUS
	BUFFER_HANDLER RINGBUFFER1_A, RINGBUFFER1_B, RINGBUFFER1, RINGBUFFER1_END, MIDI1_RUNSTATUS

	goto	MainLoop 

