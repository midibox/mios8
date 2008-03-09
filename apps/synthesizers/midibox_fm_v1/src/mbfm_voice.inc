; $Id$
;
; MIDIbox FM
; Voice handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function initializes the voice queue and the assigned instruments
;;  The queue contains a number for each voice. So long bit 7 is set, the
;;  voice is assigned to an instrument, if bit 7 is not set, the voice can
;;  be allocated by a new instrument
;; 
;;  The instrument number to which the voice is assigned can be found in
;;  bit 6-4, it is especially important for mono voices
;; 
;;  The first voice in the queue is the first which will be taken.
;;  To realize a "drop longest note first" algorithm, the take number should
;;  always be moved to the end of the queue
;; 
;;  IN: -
;;  OUT: -
;;  USES: PRODL, FSR1
;; --------------------------------------------------------------------------
MBFM_VOICE_Init
	;; clear the whole voice memory
	lfsr	FSR1, MBFM_V1_BASE
	clrf	PRODL
MBFM_VOICE_InitLoop
	clrf	POSTINC1
	incf	PRODL, F
	movlw	(MBFM_Vx_NUM * MBFM_Vx_RECORD_LEN) - 1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_VOICE_InitLoop

	;; select invalid instrument (so that a new allocated voice will be initialized completely)
	SET_BSR	MBFM_V1_BASE
	setf	MBFM_V1_BASE + MBFM_Vx_ASSIGNED_INSTR
	setf	MBFM_V2_BASE + MBFM_Vx_ASSIGNED_INSTR
	setf	MBFM_V3_BASE + MBFM_Vx_ASSIGNED_INSTR
	setf	MBFM_V4_BASE + MBFM_Vx_ASSIGNED_INSTR
	setf	MBFM_V5_BASE + MBFM_Vx_ASSIGNED_INSTR
	setf	MBFM_V6_BASE + MBFM_Vx_ASSIGNED_INSTR	

	;; clear voice queue
	lfsr	FSR1, MBFM_VOICE_QUEUE_0
	clrf	PRODL
MBFM_VOICE_InitQueueLoop
	movff	PRODL, POSTINC1
	incf	PRODL, F
	movlw	MBFM_Vx_NUM - 1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_VOICE_InitQueueLoop


	return

;; --------------------------------------------------------------------------
;;  This function searches for a voice which is not allocated, or drops the
;;  voice which played the longest note
;;  IN:  o instrument number in MBFM_CURRENT_INSTRUMENT
;;  OUT: o MBFM_CURRENT_VOICE contains the index of the found voice
;;       o FSR1 contains the base pointer to the found voice
;;  USES: PROD[LH], TABLAT, FSR1
;; --------------------------------------------------------------------------
MBFM_VOICE_Get
	;; search in voice queue for free voice
	lfsr	FSR1, MBFM_VOICE_QUEUE_0
	clrf	PRODL
MBFM_VOICE_Get_Loop1
	movf	PRODL, W
	BRA_IFCLR PLUSW1, 7, ACCESS, MBFM_VOICE_Get_Found
	incf	PRODL, F
#if SUPERPOLY_EXPERIMENT
	movlw	4-1
#else
	movlw	MBFM_VOICE_QUEUE_LEN - 1
#endif
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_VOICE_Get_Loop1

MBFM_VOICE_Get_NotFound
	;; no free voice found, take first note in queue
	clrf	PRODL

	;; free voice found - remember index | 0x80 | instrument << 4
MBFM_VOICE_Get_Found
	;; entry point for MBFM_Voice_GetLast if selected voice has been found and is not allocated
MBFM_VOICE_GetLast_Cont
	SET_BSR	MBFM_BASE
	swapf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	andlw	0x70
	iorlw	0x80
	movwf	PRODH
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x0f
	iorwf	PRODH, F
	
	movf	PRODL, W
	movff	PRODH, PLUSW1


	;; if this is not already the last voice in the queue, shift queue and put the number to the end
	movf	PRODL, W
#if SUPERPOLY_EXPERIMENT
	xorlw	4-1
#else
	xorlw	MBFM_VOICE_QUEUE_LEN - 1
#endif
	bz	MBFM_VOICE_Get_End
	movf	PRODL, W
	movff	PLUSW1, PRODH
MBFM_VOICE_Get_Loop2
	incf	PRODL, W
	movff	PLUSW1, TABLAT
	movf	PRODL, W
	movff	TABLAT, PLUSW1
	incf	PRODL, F
#if SUPERPOLY_EXPERIMENT
	movlw	4-2
#else
	movlw	MBFM_VOICE_QUEUE_LEN - 2
#endif
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_VOICE_Get_Loop2
#if SUPERPOLY_EXPERIMENT
	movlw	4-1
#else
	movlw	MBFM_VOICE_QUEUE_LEN - 1
#endif
	movff	PRODH, PLUSW1
MBFM_VOICE_Get_End
	movf	PRODH, W
	andlw	0x0f
	movff	WREG, MBFM_CURRENT_VOICE

	;; create base pointer to voice
	lfsr	FSR1, MBFM_V1_BASE
	mullw	MBFM_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return


;; --------------------------------------------------------------------------
;;  This function searches for a given voice. If it is already allocated, 
;;  it continues at MBFM_VOICE_Get, otherwise it returns the pointer to 
;;  the selected voice
;;  IN:  o selected voice in WREG
;;  OUT: o MBFM_CURRENT_VOICE contains the index of the found voice
;;       o FSR1 contains the base pointer to the found voice
;;  USES: PROD[LH], TABLAT, FSR1
;; --------------------------------------------------------------------------
MBFM_VOICE_GetLast
	movwf	PRODH		; save voice in PRODH

	;; search in voice queue for selected voice
	lfsr	FSR1, MBFM_VOICE_QUEUE_0
	clrf	PRODL
MBFM_VOICE_GetLast_Loop1
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x0f
	xorwf	PRODH, W
	bz	MBFM_VOICE_GetLast_Found
	incf	PRODL, F
#if SUPERPOLY_EXPERIMENT
	movlw	4-1
#else
	movlw	MBFM_VOICE_QUEUE_LEN - 1
#endif
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_VOICE_GetLast_Loop1

MBFM_VOICE_GetLast_NotFound
	;; selected voice not found, continue at MBFM_VOICE_Get
	rgoto	MBFM_VOICE_Get

MBFM_VOICE_GetLast_Found
	;; selected voice found if instrument number not equal, we should get a new voice
	SET_BSR	MBFM_BASE
	movf	PRODL, W
	swapf	PLUSW1, W
	xorwf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	andlw	0x07
	bnz	MBFM_VOICE_Get

	;; it's mine!
	;; continue at queue handling
	rgoto	MBFM_VOICE_GetLast_Cont


;; --------------------------------------------------------------------------
;;  This function releases a voice, so that it is free for MBFM_VOICE_Get
;;  IN:  o voice which should be released in MBFM_CURRENT_VOICE
;;  OUT: o FSR1 contains the base pointer to the voice
;;  USES: BSR, PROD[LH], TABLAT, FSR1
;; --------------------------------------------------------------------------
MBFM_VOICE_Release
	;; search in voice queue for the voice
	lfsr	FSR1, MBFM_VOICE_QUEUE_0
	clrf	PRODL
MBFM_VOICE_Release_Loop
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x0f
	xorwf	MBFM_CURRENT_VOICE, W, BANKED
	bz	MBFM_VOICE_Release_Found
	incf	PRODL, F
#if SUPERPOLY_EXPERIMENT
	movlw	4-1
#else
	movlw	MBFM_VOICE_QUEUE_LEN - 1
#endif
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_VOICE_Release_Loop

MBFM_VOICE_Release_NotFound
	;; very strange error which should never happen!!! issue an reset for debugging purposes
	goto	MIOS_Reset

MBFM_VOICE_Release_Found
	;; clear allocate marker
	movf	PRODL, W
	bcf	PLUSW1, 7

	;; create base pointer to voice
	lfsr	FSR1, MBFM_V1_BASE
	movf	MBFM_CURRENT_VOICE, W, BANKED
	mullw	MBFM_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return
