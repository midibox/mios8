; $Id$
;
; MIDIbox SID
; Voice handling
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
;;  SID_VOICE_INSTR_x, it is especially important for mono voices
;; 
;;  The first voice in the queue is the first which will be taken.
;;  To realize a "drop longest note first" algorithm, the take number should
;;  always be moved to the end of the queue
;; 
;;  IN: -
;;  OUT: -
;;  USES: PRODL, FSR1
;; --------------------------------------------------------------------------
SID_VOICE_Init
	;; init voice queue
	lfsr	FSR1, SID_VOICE_QUEUE_0
	clrf	PRODL
SID_VOICE_InitQueueLoop1
	movff	PRODL, POSTINC1
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_InitQueueLoop1

	;; init INSTR queue
	lfsr	FSR1, SID_VOICE_INSTR_0
	clrf	PRODL
SID_VOICE_InitQueueLoop2
	setf	POSTINC1
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_InitQueueLoop2

	;; init exclusive voice allocation flags
	rcall	SID_VOICE_ReInit

	;; init superpoly note table (4 entries)
	lfsr	FSR1, SID_SUPERPOLY_VOICE_NOTE_TAB
	clrf	POSTINC1
	clrf	POSTINC1
	clrf	POSTINC1
	clrf	POSTINC1

	return

;; --------------------------------------------------------------------------
;;  (Re-)initializes the exclusive voice allocation flags
;; --------------------------------------------------------------------------
SID_VOICE_ReInit
	;; interrupts must be disabled when the voice queue is modified
	;; (in drum mode, voice queue is handled via IRQs)
	IRQ_DISABLE

	;; by default, allow non-exclusive access (INSTR[6] set to 1)
	lfsr	FSR1, SID_VOICE_INSTR_0
	clrf	PRODL
SID_VOICE_ReInit_Loop
	bsf	POSTINC1, 6
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_ReInit_Loop
	
	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_VOICE_ReInit_DM
SID_VOICE_ReInit_LB
	;; exclusive voice assignments not used by Lead/Bassline engine
	rgoto	SID_VOICE_ReInit_End

SID_VOICE_ReInit_DM
	BRA_IFSET WREG, 0, ACCESS, SID_VOICE_ReInit_M
SID_VOICE_ReInit_D
	;; go through 16 instruments
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_D_I1_BASE
	clrf	PRODL
SID_VOICE_ReInit_D_Loop
	movlw	SID_Ix_Dx_FLAGS1	; voice assignment located in [7:4]
	swapf	PLUSW1, W
	andlw	0x0f
	addlw	-3			; check if voice 1..6 exclusively selected
	movwf	PRODH
	movlw	6
	cpfslt	PRODH, ACCESS
	rgoto SID_VOICE_ReInit_D_Loop_Next
	;; exclusive voice assignment found -> clear flag #6 of INSTR record
	rcall	SID_VOICE_ReInit_Sub
SID_VOICE_ReInit_D_Loop_Next
	movlw	(SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE)
	addwf	FSR1L, F
	incf	PRODL, F
	BRA_IFCLR PRODL, 4, ACCESS, SID_VOICE_ReInit_D_Loop
	rgoto	SID_VOICE_ReInit_End
	
SID_VOICE_ReInit_M
	;; go through 6 instruments
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I1_BASE
	clrf	PRODL
SID_VOICE_ReInit_M_Loop
	movlw	SID_Ix_M_Vx_VOICE_ASSGN	; voice assignment located in [3:0]
	movf	PLUSW1, W
	andlw	0x0f
	addlw	-3			; check if voice 1..6 exclusively selected
	movwf	PRODH
	movlw	6
	cpfslt	PRODH, ACCESS
	rgoto SID_VOICE_ReInit_M_Loop_Next
	;; exclusive voice assignment found -> clear flag #6 of INSTR record
	rcall	SID_VOICE_ReInit_Sub
SID_VOICE_ReInit_M_Loop_Next
	movlw	(SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE)
	addwf	FSR1L, F
	movlw	0
	addwfc	FSR1H, F

	incf	PRODL, F
	movlw	6-1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_ReInit_M_Loop
	;; 	rgoto	SID_VOICE_ReInit_End

SID_VOICE_ReInit_End
	IRQ_ENABLE
	return

	;; help routine which searches for INSTR entry of specific voice and
	;; clears flag #6 of this entry, so that it is exclusively assigned
	;; IN: voice number in PRODH
SID_VOICE_ReInit_Sub
	lfsr	FSR0, SID_VOICE_QUEUE_0
	clrf	TABLAT
SID_VOICE_ReInit_Sub_Loop
	movf	TABLAT, W
	movf	PLUSW0, W
	andlw	0x7f
	xorwf	PRODH, W
	bz	SID_VOICE_ReInit_Sub_Found
	incf	TABLAT, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	TABLAT, ACCESS
	rgoto SID_VOICE_ReInit_Sub_Loop
	;; very strange error which should never happen!!! issue an reset for debugging purposes
	goto	MIOS_Reset
SID_VOICE_ReInit_Sub_Found
	movf	TABLAT, W
	addlw	SID_VOICE_INSTR_OFFSET
	bcf	PLUSW0, 6
	return


;; --------------------------------------------------------------------------
;;  (Re-)initializes the superpoly voice allocation
;; --------------------------------------------------------------------------
SID_VOICE_SP_ReInit
	;; if lead engine selected:
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bnz	SID_VOICE_SP_ReInit_NoLead
SID_VOICE_SP_ReInit_Lead
	;; re-initialize voice queues
	rcall	SID_VOICE_Init
SID_VOICE_SP_ReInit_NoLead
	return


;; --------------------------------------------------------------------------
;;  This function searches for a voice which is not allocated, or drops the
;;  voice which played the longest note
;;  IN:  o instrument number in SID_CURRENT_MIDIVOICE
;;       o voice assignment mode in SID_CURRENT_VOICE_ASSG[2:0]
;;       o number of voices (1..6) in SID_CURRENT_VOICE_ASSG[6:4]
;;  OUT: o SID_CURRENT_VOICE contains the index of the found voice
;;       o FSR1 contains the base pointer to the found voice
;;  USES: PROD[LH], TABLAT, FSR1
;; --------------------------------------------------------------------------
SID_VOICE_Get
	SET_BSR	SID_BASE

	;; number of voices -> TABLAT
	swapf	SID_CURRENT_VOICE_ASSG, W, BANKED
	andlw	0x07
	movwf	TABLAT

	;; branch depending on voice assignment mode
	movf	SID_CURRENT_VOICE_ASSG, W, BANKED
	andlw	0x0f
	bz	SID_VOICE_Get_LR
	addlw	-1
	bz	SID_VOICE_Get_L
	addlw	-1
	bz	SID_VOICE_Get_R
	addlw	-1

SID_VOICE_Get_S
	;; get single voice, number in SID_CURRENT_VOICE_ASSG[3:0]  (-3)
	movwf	PRODH

	;; only take one of the first n voices (n = TABLAT)
	decf	TABLAT, W
	cpfsgt	PRODH, ACCESS
	rgoto SID_VOICE_Get_S_Ok
	;; decrement number of available voices
	;; e.g.: n=3, PRODH=4 (voice 5) -> PRODH += ((comf n) + 1) -> PRODH += 0xfc+1) -> PRODH=1 (voice 2)
	comf	WREG, W
	addlw	1
	addwf	PRODH, F
	;; ensure that PRODH is never > n (corner case where SID_CURRENT_VOICE_ASSG[2:0] contains invalid value)
	movf	TABLAT, W
	cpfslt	PRODH, ACCESS
	clrf PRODH
SID_VOICE_Get_S_Ok

	;; search for the voice and allocate it
	lfsr	FSR1, SID_VOICE_QUEUE_0
	clrf	PRODL
SID_VOICE_Get_S_Loop
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	xorwf	PRODH, W
	bz	SID_VOICE_Get_Found
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_S_Loop
SID_VOICE_Get_S_NotFound
	;; very strange error which should never happen!!! issue an reset for debugging purposes
	goto	MIOS_Reset


	;; ------------------------------------------------------------------
SID_VOICE_Get_L
	;; only Voice 1..3 allowed!
	movlw	3
	cpfslt	TABLAT, ACCESS
	movwf TABLAT

SID_VOICE_Get_LR ; (all voices available)

	lfsr	FSR1, SID_VOICE_QUEUE_0
	clrf	PRODL
SID_VOICE_Get_L_Loop1
	movf	PRODL, W
	movff	PLUSW1, PRODH
	BRA_IFSET PRODH, 7, ACCESS, SID_VOICE_Get_L_Loop1_Next
	;; don't take voice if exclusively allocated
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	BRA_IFCLR PLUSW1, 6, ACCESS, SID_VOICE_Get_L_Loop1_Next
	
	;; check if voice < TABLAT (number of voices)
	decf	TABLAT, W
	cpfsgt	PRODH, ACCESS
	rgoto SID_VOICE_Get_Found
SID_VOICE_Get_L_Loop1_Next
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_L_Loop1
SID_VOICE_Get_L_NotFound


	;; no free voice found, take first note in queue w/ exclusive check
	;; it must be < TABLAT
	clrf	PRODL
SID_VOICE_Get_L_Loop2
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	BRA_IFCLR PLUSW1, 6, ACCESS, SID_VOICE_Get_L_Loop2_Next

	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODH

	;; check if voice < TABLAT (number of voices)
	decf	TABLAT, W
	cpfsgt	PRODH, ACCESS
	rgoto SID_VOICE_Get_Found
SID_VOICE_Get_L_Loop2_Next
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_L_Loop2

	;; still no free voice found, take first note in queue w/o exclusive check
	;; it must be < TABLAT
	clrf	PRODL
SID_VOICE_Get_L_Loop3
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODH

	;; check if voice < TABLAT (number of voices)
	decf	TABLAT, W
	cpfsgt	PRODH, ACCESS
	rgoto SID_VOICE_Get_Found
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_L_Loop3
SID_VOICE_Get_L_NeverFound
	;; very strange error which should never happen!!! issue an reset for debugging purposes
	goto	MIOS_Reset


	;; ------------------------------------------------------------------
SID_VOICE_Get_R
	;; if number of voices <= 3, only take voice of left channel
	movlw	3
	cpfsgt	TABLAT, ACCESS
	rgoto SID_VOICE_Get_LR

	;; search in voice queue for free voice
	;; only Voice 4, 5 or 6 allowed!
	lfsr	FSR1, SID_VOICE_QUEUE_0
	clrf	PRODL
SID_VOICE_Get_R_Loop1
	movf	PRODL, W
	movff	PLUSW1, PRODH
	BRA_IFSET PRODH, 7, ACCESS, SID_VOICE_Get_R_Loop1_Next
	;; don't take voice if exclusively allocated
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	BRA_IFCLR PLUSW1, 6, ACCESS, SID_VOICE_Get_R_Loop1_Next

	;; check if voice >= 3
	movlw	3
	cpfslt	PRODH, ACCESS
	rgoto SID_VOICE_Get_Found
SID_VOICE_Get_R_Loop1_Next
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_R_Loop1
SID_VOICE_Get_R_NotFound


	;; no free voice found, take first note in queue w exclusive check
	;; it must be >= 3
	clrf	PRODL
SID_VOICE_Get_R_Loop2
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	BRA_IFCLR PLUSW1, 6, ACCESS, SID_VOICE_Get_R_Loop2_Next

	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODH
	movlw	3
	cpfslt	PRODH, ACCESS
	rgoto SID_VOICE_Get_Found
SID_VOICE_Get_R_Loop2_Next
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_R_Loop2


	;; no free voice found, take first note in queue w/o exclusive check
	;; it must be >= 3
	clrf	PRODL
SID_VOICE_Get_R_Loop3
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODH
	movlw	3
	cpfslt	PRODH, ACCESS
	rgoto SID_VOICE_Get_Found
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_R_Loop3
SID_VOICE_Get_R_NeverFound
	;; very strange error which should never happen!!! issue an reset for debugging purposes
	goto	MIOS_Reset


	;; ------------------------------------------------------------------
	;; ------------------------------------------------------------------
	;; free voice found - remember index | 0x80
SID_VOICE_Get_Found
	;; entry point for SID_Voice_GetLast if selected voice has been found and is not allocated
SID_VOICE_GetLast_Cont
	movf	PRODL, W	; mask as allocated
	bsf	PLUSW1, 7
	movff	PLUSW1, PRODH

	;; if this is not already the last voice in the queue, shift queue and put the number to the end
	movf	PRODL, W
	xorlw	SID_VOICE_QUEUE_LEN - 1
	bz	SID_VOICE_Get_End
	;; remember instrument setting in SID_CURRENT_VOICE (dirty!!! but we know that this register will be overwritten later)
	;; this is required to hold the exclusive allocation flag
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	movff	PLUSW1, SID_CURRENT_VOICE

SID_VOICE_Get_Loop2
	incf	PRODL, W
	movff	PLUSW1, TABLAT
	addlw	-1
	movff	TABLAT, PLUSW1

	incf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	movff	PLUSW1, TABLAT
	addlw	-1
	movff	TABLAT, PLUSW1

	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 2
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Get_Loop2

	;; put voice number at the end of the voice queue
 	movlw	SID_VOICE_QUEUE_LEN - 1
	movwf	PRODL
	movff	PRODH, PLUSW1

	;; put instrument number at the end of the instrument queue
	;; this is required to hold the exclusive allocation flag
	;; (the usage of SID_CURRENT_VOICE is dirty!!! but we know that this register will be overwritten later)
	movlw	SID_VOICE_INSTR_OFFSET + SID_VOICE_QUEUE_LEN - 1
	movff	SID_CURRENT_VOICE, PLUSW1

SID_VOICE_Get_End
	;; store the instrument number which allocates the voice
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	movf	PLUSW1, W
	andlw	0x40		; (don't touch exclusive allocation flag)
	iorwf	SID_CURRENT_MIDIVOICE, W, BANKED
	movwf	TABLAT
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	movff	TABLAT, PLUSW1

	;; voice number -> SID_CURRENT_VOICE
	movf	PRODH, W
	andlw	0x7f
	movff	WREG, SID_CURRENT_VOICE

	;; create base pointer to voice
	lfsr	FSR1, SIDL_V1_BASE
	mullw	SID_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return


;; --------------------------------------------------------------------------
;;  This function searches for a given voice. If it is already allocated, 
;;  it continues at SID_VOICE_Get, otherwise it returns the pointer to 
;;  the selected voice
;;  IN:  o selected voice in WREG
;;       o voice assignment mode in SID_CURRENT_VOICE_ASSG[2:0]
;;       o number of voices (1..6) in SID_CURRENT_VOICE_ASSG[6:4]
;;       o SID_CURRENT_MIDIVOICE: the instrument (criteria for equal voice)
;;  OUT: o SID_CURRENT_VOICE contains the index of the found voice
;;       o FSR1 contains the base pointer to the found voice
;;  USES: PROD[LH], TABLAT, FSR1
;; --------------------------------------------------------------------------
SID_VOICE_GetLast
	movwf	PRODH		; save voice in PRODH

	SET_BSR	SID_BASE

SID_VOICE_GetLast_SearchVoice
	;; search in voice queue for selected voice
	lfsr	FSR1, SID_VOICE_QUEUE_0
	clrf	PRODL
SID_VOICE_GetLast_Loop1
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	xorwf	PRODH, W
	bz	SID_VOICE_GetLast_Found
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_GetLast_Loop1
	
SID_VOICE_GetLast_NotFound
	;; selected voice not found, continue at SID_VOICE_Get
	rgoto	SID_VOICE_Get

SID_VOICE_GetLast_Found
	;; selected voice found
	;; if instrument number not equal, we should get a new voice
	movf	PRODL, W
	addlw	SID_VOICE_INSTR_OFFSET
	movf	PLUSW1, W
	andlw	0x3f
	xorwf	SID_CURRENT_MIDIVOICE, W, BANKED
	bnz	SID_VOICE_GetLast_NotFound

	;; if number of available voices has changed in meantime (e.g. Stereo->Mono switch):
	;; check that voice number still < n
	swapf	SID_CURRENT_VOICE_ASSG, W, BANKED
	andlw	0x07
	cpfslt	PRODH, ACCESS
	rgoto SID_VOICE_GetLast_NotFound

SID_VOICE_GetLast_FoundGroup
	;; it's mine!
	;; continue at queue handling
	rgoto	SID_VOICE_GetLast_Cont


;; --------------------------------------------------------------------------
;;  This function releases a voice, so that it is free for SID_VOICE_Get
;;  IN:  o voice which should be released in SID_CURRENT_VOICE
;;  OUT: o FSR1 contains the base pointer to the voice
;;  USES: BSR, PROD[LH], TABLAT, FSR1
;; --------------------------------------------------------------------------
SID_VOICE_Release
	;; search in voice queue for the voice
	lfsr	FSR1, SID_VOICE_QUEUE_0
	clrf	PRODL
SID_VOICE_Release_Loop
	movf	PRODL, W
	movf	PLUSW1, W
	andlw	0x7f
	xorwf	SID_CURRENT_VOICE, W, BANKED
	bz	SID_VOICE_Release_Found
	incf	PRODL, F
	movlw	SID_VOICE_QUEUE_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_VOICE_Release_Loop

SID_VOICE_Release_NotFound
	;; very strange error which should never happen!!! issue an reset for debugging purposes
	goto	MIOS_Reset

SID_VOICE_Release_Found
	;; clear allocate marker
	movf	PRODL, W
	bcf	PLUSW1, 7

	;; create base pointer to voice
	lfsr	FSR1, SIDL_V1_BASE
	movf	SID_CURRENT_VOICE, W, BANKED
	mullw	SID_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

