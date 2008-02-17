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
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_SONG_Reset_End
SEQ_SONG_Reset_InSongMode
	rcall	SEQ_SONG_FetchPos

	;; if recursion counter is > 64, clear position and loops again
	SET_BSR	SEQ_BASE
	movf	SEQ_SONG_RECURSION_CTR, W, BANKED
	andlw	0xc0
	bz	SEQ_SONG_Reset_End

	clrf	SEQ_SONG_POS, BANKED
	clrf	SEQ_SONG_LOOP, BANKED
	clrf	SEQ_SONG_LOOP_MAX, BANKED

SEQ_SONG_Reset_End
	return

;; --------------------------------------------------------------------------
;;  This function fetches the pos entries of a song
;; --------------------------------------------------------------------------
SEQ_SONG_FetchPos
	;; this counter is used to avoid a live lock when we have a big loop over different positions and songs
	;; the function will break at 64 recursions
	SET_BSR	SEQ_BASE
	clrf	SEQ_SONG_RECURSION_CTR, BANKED

	;; TODO
	return

SEQ_SONG_FetchPosNext
	SET_BSR	SEQ_BASE
	incf	SEQ_SONG_RECURSION_CTR, F, BANKED
	;; stop song once we reached 64 loops
	BRA_IFSET SEQ_SONG_RECURSION_CTR, 6, BANKED, SEQ_SONG_FetchPos_Stop

	;; TODO
	rgoto	SEQ_SONG_FetchPosNext

SEQ_SONG_FetchPos_Stop
	return


;; --------------------------------------------------------------------------
;;  call this function to enter song mode
;; --------------------------------------------------------------------------
SEQ_SONG_EnterSongMode
	;; enable song mode
	SET_BSR	SEQ_BASE
	bsf	SEQ_STATE, SEQ_STATE_SONG, BANKED

	;; reset song
	rcall	SEQ_SONG_Reset

	;; don't continue if song disabled
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, SEQ_SONG_ExitSongMode

	;; exit if song bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SONG_ExitSongMode

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


