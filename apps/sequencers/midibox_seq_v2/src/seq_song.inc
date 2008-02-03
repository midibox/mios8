; $Id$
;
; MIDIboxSEQ
; Sequencer song functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function resets a song
;; --------------------------------------------------------------------------
SEQ_SONG_Reset
	;; clear song position and loop counter
	SET_BSR	SEQ_BASE
	clrf	SEQ_SONG_POS, BANKED
	clrf	SEQ_SONG_LOOP, BANKED
	clrf	SEQ_SONG_LOOP_MAX, BANKED
	;; in song mode: fetch new entries
	BRA_IFSET SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_SONG_FetchPos
	;; else exit
	return

;; --------------------------------------------------------------------------
;;  This function fetches the pos entries of a song
;; --------------------------------------------------------------------------
SEQ_SONG_FetchPos
	SET_BSR	SEQ_BASE

	;; set initial pattern
	movlw	SEQ_SBANK_SECTOR_PLAY
	movwf	SEQ_SBANK_SECTOR, BANKED

	;; if not in song mode, fetch patterns directly
	BRA_IFSET SEQ_SONG, 7, BANKED, SEQ_SONG_FetchPosDirect

	;; set loop counter
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	bnz	SEQ_SONG_FetchPos_Cont
SEQ_SONG_FetchPos_Stop
	;; play entry is 0x00: stop sequencer and exit
	;; but only in SONG mode!
	goto	SEQ_CORE_Stop

SEQ_SONG_FetchPos_Cont
	addlw	-1
	movwf	TABLAT
	andlw	0xf0
	bz	SEQ_SONG_FetchPos_ContL
SEQ_SONG_FetchPos_ContLP
	;; loop point: new song position is TABLAT - 0x10
	movf	TABLAT, W
	addlw	-0x10
	cpfseq	SEQ_SONG_POS, BANKED
	rgoto SEQ_SONG_FetchPos_ContLPOk
	;; illegal: loop point to the same position - stop sequencer
	rgoto	SEQ_SONG_FetchPos_Stop
SEQ_SONG_FetchPos_ContLPOk
	;; loop point ok
	movwf	SEQ_SONG_POS, BANKED
	rgoto	SEQ_SONG_FetchPos

SEQ_SONG_FetchPos_ContL
	;; play entry is between 0x01 and 0x10: set loop counter
	movf	TABLAT, W	; (TABLAT contains play entry - 1)
	andlw	0x0f
	movwf	SEQ_SONG_LOOP, BANKED
	movwf	SEQ_SONG_LOOP_MAX, BANKED
	
SEQ_SONG_FetchPosDirect
	;; fetch pattern numbers
	lfsr	FSR0, SEQ_PATTERN0
	movlw	SEQ_SBANK_SECTOR_P0
	movwf	SEQ_SBANK_SECTOR, BANKED
SEQ_SONG_FetchPosLoop
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	movwf	POSTINC0
	incf	SEQ_SBANK_SECTOR, F, BANKED
	movlw	SEQ_SBANK_SECTOR_P3+1
	cpfsgt	SEQ_SBANK_SECTOR, BANKED
	rgoto SEQ_SONG_FetchPosLoop

	;; change patterns
	goto	SEQ_CORE_ChangePatternSynched

;; --------------------------------------------------------------------------
;;  call this function to enter song mode
;; --------------------------------------------------------------------------
SEQ_SONG_EnterSongMode
	;; reset song
	rcall	SEQ_SONG_Reset

	;; don't continue if song disabled
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, SEQ_SONG_ExitSongMode

	;; exit if song bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SONG_ExitSongMode

	;; enable song mode
	bsf	SEQ_STATE, SEQ_STATE_SONG, BANKED

	return


;; --------------------------------------------------------------------------
;;  call this function to exit song mode
;; --------------------------------------------------------------------------
SEQ_SONG_ExitSongMode
	SET_BSR	SEQ_BASE
	bcf	SEQ_STATE, SEQ_STATE_SONG, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function increments the song pointer
;; --------------------------------------------------------------------------
SEQ_SONG_NextPos
	SET_BSR	SEQ_BASE

	decf	SEQ_SONG_LOOP, F, BANKED
	BRA_IFCLR SEQ_SONG_LOOP, 7, BANKED, SEQ_SONG_NextPos_NInc
SEQ_SONG_NextPos_Inc
	incf	SEQ_SONG_POS, F, BANKED
	;; fetch new pos entries
	rcall	SEQ_SONG_FetchPos
SEQ_SONG_NextPos_NInc
	return


;; --------------------------------------------------------------------------
;;  This function sets the song pointer
;;  Note that it can also used when not in song mode to set the patterns
;; --------------------------------------------------------------------------
SEQ_SONG_SetPos
	SET_BSR	SEQ_BASE
	movwf	SEQ_SONG_POS, BANKED
	;; fetch new pos entries
	rgoto	SEQ_SONG_FetchPos


;; --------------------------------------------------------------------------
;;  This function decrements the song pointer
;; --------------------------------------------------------------------------
SEQ_SONG_PrevPos
	SET_BSR	SEQ_BASE

	incf	SEQ_SONG_LOOP, F, BANKED
	movf	SEQ_SONG_LOOP_MAX, W, BANKED
	cpfsgt	SEQ_SONG_LOOP, BANKED
	rgoto SEQ_SONG_PrevPos_NDec
SEQ_SONG_PrevPos_Dec
	decf	SEQ_SONG_POS, F, BANKED
	BRA_IFCLR SEQ_SONG_POS, 7, BANKED, SEQ_SONG_PrevPos_Dec_NR
SEQ_SONG_PrevPos_Dec_R
	clrf	SEQ_SONG_POS, BANKED
	;; fetch pos entries again
	rcall	SEQ_SONG_FetchPos
	movff	SEQ_SONG_LOOP_MAX, SEQ_SONG_LOOP
	rgoto	SEQ_SONG_PrevPos_NDec

SEQ_SONG_PrevPos_Dec_NR
	;; fetch new pos entries
	rcall	SEQ_SONG_FetchPos
	SET_BSR	SEQ_BASE
	clrf	SEQ_SONG_LOOP, BANKED
SEQ_SONG_PrevPos_NDec
	return


;; --------------------------------------------------------------------------
;;  This function is used by the "Fwd" button
;; --------------------------------------------------------------------------
SEQ_SONG_Fwd
	call	SEQ_CORE_Stop
	call	SEQ_CORE_Reset
	rcall	SEQ_SONG_NextPos
	goto	SEQ_CORE_Cont

;; --------------------------------------------------------------------------
;;  This function is used by the "Rew" button
;; --------------------------------------------------------------------------
SEQ_SONG_Rew
	call	SEQ_CORE_Stop
	call	SEQ_CORE_Reset
	rcall	SEQ_SONG_PrevPos
	goto	SEQ_CORE_Cont
