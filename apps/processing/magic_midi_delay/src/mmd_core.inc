; $Id$
;
; Core functions of the Magic MIDI Processor
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_MPROC_NotifyReceivedEvent
;;  when a Note Off event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MMD_CORE_NoteOff
	;; clear MIOS_PARAMETER3 (velocity)
	clrf	MIOS_PARAMETER3
	;; convert 0x8n to 0x9n for an easier handling
	movlw	0x10
	iorwf	MIOS_PARAMETER1, F

	;; push event into the four queues
	call	MMD_CORE_Push_Queue

	return

;; --------------------------------------------------------------------------
;;  This function is called by USER_MPROC_NotifyReceivedEvent
;;  when a Note On event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MMD_CORE_NoteOn
	;; push event into the four queues
	call	MMD_CORE_Push_Queue

	return

;; --------------------------------------------------------------------------
;;  pushs an event (stored in MIOS_PARAMETER[123] into the queue
;; --------------------------------------------------------------------------
	;; define the push queue code in a macro, we duplicate it 4 times
PUSH_QUEUE MACRO QUEUEx_BEGIN, QUEUEx_END, QUEUEx_HEAD, QUEUEx_TAIL, QUEUEx_OVERALL_DLY, QUEUEx_DLY, QUEUEx_CHN, QUEUEx_TRANSP
	LOCAL	PUSH_QUEUE_TranspAdd
	LOCAL	PUSH_QUEUE_TranspSub
	LOCAL	PUSH_QUEUE_TranspCont
	LOCAL	PUSH_QUEUE_SendImmediately
	LOCAL	PUSH_QUEUE_DontSendImmediately
	LOCAL	PUSH_QUEUE_NotEmpty
	LOCAL	PUSH_QUEUE_Empty
	LOCAL	PUSH_QUEUE_End

	movf	QUEUEx_CHN, W				; don't add if channel has been disabled
	bz	PUSH_QUEUE_End
	addlw	-1					; save QUEUEx_CHN-1 in QUEUE_CHANNEL
	andlw	0x0f					; ensure that channel is in between 0 and 15
	movwf	MMD_QUEUE_CHANNEL

	movf	MIOS_PARAMETER2, W			; add/subtract transpose value and saturate
	movwf	MMD_QUEUE_NOTE
	movf	QUEUEx_TRANSP, W
	addlw	-0x40
	addwf	MMD_QUEUE_NOTE, F
	BRA_IFCLR QUEUEx_TRANSP, 6, ACCESS, PUSH_QUEUE_TranspSub
PUSH_QUEUE_TranspAdd
	BRA_IFCLR MMD_QUEUE_NOTE, 7, ACCESS, PUSH_QUEUE_TranspCont
	movlw	0x7f
	movwf	MMD_QUEUE_NOTE
	rgoto	PUSH_QUEUE_TranspCont
PUSH_QUEUE_TranspSub
	BRA_IFCLR MMD_QUEUE_NOTE, 7, ACCESS, PUSH_QUEUE_TranspCont
	clrf	MMD_QUEUE_NOTE
	;; 	rgoto	PUSH_QUEUE_TranspCont
PUSH_QUEUE_TranspCont

	movf	QUEUEx_DLY, W				; copy Queue delay to QUEUE_DELAY
	movwf	MMD_QUEUE_DELAY
	
	bnz	PUSH_QUEUE_DontSendImmediately		; send event immediately if delay is zero
PUSH_QUEUE_SendImmediately
	movf	MIOS_PARAMETER1, W			; send MIDI event byte 1
	andlw	0xf0					; zero channel value
	addwf	MMD_QUEUE_CHANNEL, W			; add MIDI channel
	call	MIOS_MIDI_TxBufferPut			; send MIDI byte
	movf	MMD_QUEUE_NOTE, W			; send MIDI event byte 2 (note)
	call	MIOS_MIDI_TxBufferPut			; send MIDI byte
	movf	MIOS_PARAMETER3, W			; send MIDI event byte 3
	call	MIOS_MIDI_TxBufferPut			; send MIDI byte
	rgoto	PUSH_QUEUE_End				; jump to the end
PUSH_QUEUE_DontSendImmediately
	
	movf	QUEUEx_HEAD, W				; prevent buffer overrun
	addlw	4					; add 4 and check if tail offset reached
	andlw	QUEUEx_END-QUEUEx_BEGIN
	xorwf	QUEUEx_TAIL, W
	bz	PUSH_QUEUE_End				; jump to the end if tail == (head+4)&(QUEUE_SIZE-1)

	movf	QUEUEx_HEAD, W				; subtract overall delay if head != tail
	xorwf	QUEUEx_TAIL, W
	bz	PUSH_QUEUE_Empty
PUSH_QUEUE_NotEmpty
	movf	QUEUEx_OVERALL_DLY, W			; delay = delay - overall_delay
	subwf	MMD_QUEUE_DELAY, W			; do saturate (if delay < 0 then delay = 0)
	skpc
	movlw	0x00
	movwf	MMD_QUEUE_DELAY
PUSH_QUEUE_Empty
	
	movlw	QUEUEx_END-QUEUEx_BEGIN			; mask out the head with the queue size
	andwf	QUEUEx_HEAD, F

	lfsr	FSR0, QUEUEx_BEGIN			; calc pointer to new queue entry
	movf	QUEUEx_HEAD, W
	addwf	FSR0L, F

	movf	MMD_QUEUE_DELAY, W			; push delay
	movwf	POSTINC0
	movf	MIOS_PARAMETER1, W			; push MIDI event byte 1
	andlw	0xf0					; zero channel value
	addwf	MMD_QUEUE_CHANNEL, W			; push assigned MIDI channel of queue
	movwf	POSTINC0
	movf	MMD_QUEUE_NOTE, W			; push MIDI event byte 2 (note)
	movwf	POSTINC0
	movf	MIOS_PARAMETER3, W			; push MIDI event byte 3
	movwf	POSTINC0

	movlw	4					; add 4 to head offset
	addwf	QUEUEx_HEAD, F

	movf	MMD_QUEUE_DELAY, W			; add current delay to overall delay
	addwf	QUEUEx_OVERALL_DLY, F

PUSH_QUEUE_End
	ENDM

MMD_CORE_Push_Queue
	PUSH_QUEUE MMD_QUEUE0_BEGIN, MMD_QUEUE0_END, MMD_QUEUE0_HEAD, MMD_QUEUE0_TAIL, MMD_QUEUE0_OVERALL_DLY, MMD_QUEUE0_DLY, MMD_QUEUE0_CHN, MMD_QUEUE0_TRANSP
	PUSH_QUEUE MMD_QUEUE1_BEGIN, MMD_QUEUE1_END, MMD_QUEUE1_HEAD, MMD_QUEUE1_TAIL, MMD_QUEUE1_OVERALL_DLY, MMD_QUEUE1_DLY, MMD_QUEUE1_CHN, MMD_QUEUE1_TRANSP
	PUSH_QUEUE MMD_QUEUE2_BEGIN, MMD_QUEUE2_END, MMD_QUEUE2_HEAD, MMD_QUEUE2_TAIL, MMD_QUEUE2_OVERALL_DLY, MMD_QUEUE2_DLY, MMD_QUEUE2_CHN, MMD_QUEUE2_TRANSP
	PUSH_QUEUE MMD_QUEUE3_BEGIN, MMD_QUEUE3_END, MMD_QUEUE3_HEAD, MMD_QUEUE3_TAIL, MMD_QUEUE3_OVERALL_DLY, MMD_QUEUE3_DLY, MMD_QUEUE3_CHN, MMD_QUEUE3_TRANSP
	return


;; --------------------------------------------------------------------------
;;  This function is called when a MIDI clock has been received
;; --------------------------------------------------------------------------

	;; define the handler code in a macro, we duplicate it 4 times
SINGLE_DELAY_HANDLER MACRO QUEUEx_BEGIN, QUEUEx_END, QUEUEx_HEAD, QUEUEx_TAIL, QUEUEx_OVERALL_DLY
	LOCAL	SINGLE_DELAY_HANDLER_Loop
	LOCAL	SINGLE_DELAY_HANDLER_Loop_End
	LOCAL	SINGLE_DELAY_HANDLER_End

	movf	QUEUEx_HEAD, W
	andlw	(QUEUEx_END-QUEUEx_BEGIN)
	movwf	MMD_QUEUE_SNIPPET_HEAD

SINGLE_DELAY_HANDLER_Loop
	movf	MMD_QUEUE_SNIPPET_HEAD, W		; leave handler when snipped_head != tail
	xorwf	QUEUEx_TAIL, W
	bz	SINGLE_DELAY_HANDLER_End

	lfsr	FSR0, QUEUEx_BEGIN			; calc pointer to buffer
	movf	QUEUEx_TAIL, W
	addwf	FSR0L, F
	movf	INDF0, W				; leave loop when first entry (delay) != 0
	bnz	SINGLE_DELAY_HANDLER_Loop_End

	movf	POSTINC0, W				; skip delay entry

	movf	POSTINC0, W
	call	MIOS_MIDI_TxBufferPut			; send first byte
	movf	POSTINC0, W
	call	MIOS_MIDI_TxBufferPut			; send second byte
	movf	POSTINC0, W
	call	MIOS_MIDI_TxBufferPut			; send third byte

	movf	QUEUEx_TAIL, W				; add 4 to tail and mask out with queue size
	addlw	4
	andlw	(QUEUEx_END-QUEUEx_BEGIN)
	movwf	QUEUEx_TAIL
	rgoto	SINGLE_DELAY_HANDLER_Loop		; next round
SINGLE_DELAY_HANDLER_Loop_End

	decf	INDF0, F				; decrement delay value
	decf	QUEUEx_OVERALL_DLY, F			; decrement overall delay
		
SINGLE_DELAY_HANDLER_End
	ENDM

MMD_CORE_ClockHandler
	;; process the handlers
	SINGLE_DELAY_HANDLER MMD_QUEUE0_BEGIN, MMD_QUEUE0_END, MMD_QUEUE0_HEAD, MMD_QUEUE0_TAIL, MMD_QUEUE0_OVERALL_DLY
	SINGLE_DELAY_HANDLER MMD_QUEUE1_BEGIN, MMD_QUEUE1_END, MMD_QUEUE1_HEAD, MMD_QUEUE1_TAIL, MMD_QUEUE1_OVERALL_DLY
	SINGLE_DELAY_HANDLER MMD_QUEUE2_BEGIN, MMD_QUEUE2_END, MMD_QUEUE2_HEAD, MMD_QUEUE2_TAIL, MMD_QUEUE2_OVERALL_DLY
	SINGLE_DELAY_HANDLER MMD_QUEUE3_BEGIN, MMD_QUEUE3_END, MMD_QUEUE3_HEAD, MMD_QUEUE3_TAIL, MMD_QUEUE3_OVERALL_DLY
	return						; thats all ;)
