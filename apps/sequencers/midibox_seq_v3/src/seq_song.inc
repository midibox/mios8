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

;; action modes as returned by SEQ_SONG_GetActnMode
SEQ_SONG_ACTN_MODE_STOP		EQU	0
SEQ_SONG_ACTN_MODE_LOOPS	EQU	1
SEQ_SONG_ACTN_MODE_JMP_POS	EQU	2
SEQ_SONG_ACTN_MODE_JMP_SONG	EQU	3
SEQ_SONG_ACTN_MODE_SEL_MIXERMAP	EQU	4

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

SEQ_SONG_FetchPosNext
	SET_BSR	SEQ_BASE
	incf	SEQ_SONG_RECURSION_CTR, F, BANKED
	;; stop song once we reached 64 loops
	BRA_IFSET SEQ_SONG_RECURSION_CTR, 6, BANKED, SEQ_SONG_FetchPos_Stop

	;; set initial pattern
	movlw	SEQ_SBANK_SECTOR_ACTN
	movwf	SEQ_SBANK_SECTOR, BANKED

#if 0
	;; if not in song mode, fetch patterns directly
	BRA_IFSET SEQ_SONG, 7, BANKED, SEQ_SONG_FetchPosDirect
#endif

	;; get action
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	movwf	TABLAT		; temporary store it in TABLAT
	rcall	SEQ_SONG_GetActnMode
	bz	SEQ_SONG_FetchPos_Stop	; SEQ_SONG_ACTN_MODE_STOP
	addlw	-1
	bz	SEQ_SONG_FetchPos_Loops	; SEQ_SONG_ACTN_MODE_LOOPS
	addlw	-1
	bz	SEQ_SONG_FetchPos_JmpPos; SEQ_SONG_ACTN_MODE_JMP_POS
	addlw	-1
	bz	SEQ_SONG_FetchPos_JmpSong; SEQ_SONG_ACTN_MODE_JMP_SONG
	addlw	-1
	bz	SEQ_SONG_FetchPos_Mixer	; SEQ_SONG_ACTN_MODE_SEL_MIXERMAP
	;; 	rgoto	SEQ_SONG_FetchPos_Stop	; not supported - stop

SEQ_SONG_FetchPos_Stop
	;; stop sequencer and exit
	call	SEQ_CORE_Stop
	rgoto	SEQ_SONG_FetchPos_End

SEQ_SONG_FetchPos_Loops
	;; play entry is between 0x01 and 0x10: set loop counter
	decf	TABLAT, W	; (TABLAT contains action entry), decrement it to get number of loops
	andlw	0x0f
	movwf	SEQ_SONG_LOOP, BANKED
	movwf	SEQ_SONG_LOOP_MAX, BANKED
	rgoto	SEQ_SONG_FetchPosDirect

SEQ_SONG_FetchPos_JmpPos
	;; target position in G0
	movlw	SEQ_SBANK_SECTOR_G0
	movwf	SEQ_SBANK_SECTOR, BANKED
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	andlw	0x7f		; 0-127
	movwf	SEQ_SONG_POS, BANKED
	rgoto	SEQ_SONG_FetchPosNext

	
SEQ_SONG_FetchPos_JmpSong
	;; target song in G0
	movlw	SEQ_SBANK_SECTOR_G0
	movwf	SEQ_SBANK_SECTOR, BANKED
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	andlw	0x0f		; 0-15
	movwf	SEQ_SONG, BANKED
	clrf	SEQ_SONG_POS, BANKED

	;; if we are in song page, we have to check if edit values need to be stored in EEPROM
	movlw	CS_MENU_PAGE_SONG
	cpfseq	CS_MENU_POS, ACCESS
	rgoto SEQ_SONG_FetchPos_NoChngSongPage
SEQ_SONG_FetchPos_JmpSong_Pg
	call	CS_M_SONG_WriteValueChk
	;; move sequencer position (this cannot be disabled via SEQ_CFG0_FOLLOW_SONG to avoid inconsistencies!)
	movff	SEQ_SONG_POS, SEQ_EDIT_SONG_POS
	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
SEQ_SONG_FetchPos_JmpSong_NotPg
	rgoto	SEQ_SONG_FetchPosNext

	
SEQ_SONG_FetchPos_Mixer
	;; mixer map in G0
	movlw	SEQ_SBANK_SECTOR_G0
	movwf	SEQ_SBANK_SECTOR, BANKED
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	andlw	0x7f		; 0-127
	movwf	CS_MIXER_MAP
	call	CS_M_MIXER_LoadMap
	call	CS_M_MIXER_DumpMap

	;; increment song position
	SET_BSR	SEQ_SONG_POS
	incf	SEQ_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	SEQ_SONG_POS, BANKED
	rgoto	SEQ_SONG_FetchPosNext

	
SEQ_SONG_FetchPosDirect
	;; fetch pattern numbers
	clrf	SEQ_GROUP, BANKED
SEQ_SONG_FetchPosLoop
	movlw	SEQ_SBANK_SECTOR_G0
	addwf	SEQ_GROUP, W, BANKED
	movwf	SEQ_SBANK_SECTOR, BANKED
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	movwf	SEQ_PATTERN, BANKED

	movlw	SEQ_SBANK_SECTOR_BNK_G01
	btfsc	SEQ_GROUP, 1, BANKED
	movlw SEQ_SBANK_SECTOR_BNK_G23
	movwf	SEQ_SBANK_SECTOR, BANKED
	movff	SEQ_SONG_POS, EEADR
	call	SEQ_SBANK_Read
	btfsc	SEQ_GROUP, 0, BANKED
	swapf	WREG, W
	andlw	0x07
	movwf	SEQ_PATTERN_BANK, BANKED

 	call	SEQ_CORE_ChangePattern_Song	; change pattern

	SET_BSR	SEQ_BASE
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_SONG_FetchPosLoop

SEQ_SONG_FetchPos_End
	;; change song pos in CS if in song page and this function is not disabled
	movlw	CS_MENU_PAGE_SONG
	cpfseq	CS_MENU_POS, ACCESS
	rgoto SEQ_SONG_FetchPos_NoChngSongPage
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_FOLLOW_SONG, BANKED, SEQ_SONG_FetchPos_NoChngSongPage
SEQ_SONG_FetchPos_ChngSongPage
	call	CS_M_SONG_WriteValueChk
	movff	SEQ_SONG_POS, SEQ_EDIT_SONG_POS
	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
SEQ_SONG_FetchPos_NoChngSongPage
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


;; --------------------------------------------------------------------------
;;  this function returns an action mode depending on WREG, which contains
;;  either the SEQ_EDIT_SONG_ACTN, or the value read from SEQ_SBANK_SECTOR_ACTN
;; --------------------------------------------------------------------------
SEQ_SONG_GetActnMode
	andlw	0xff		; (ensure that ZERO flag is set)
	bz	SEQ_SONG_GetActnMode_0
	addlw	-0x11
	bz	SEQ_SONG_GetActnMode_2
	addlw	-1
	bz	SEQ_SONG_GetActnMode_3
	addlw	-1
	bz	SEQ_SONG_GetActnMode_4
	rgoto	SEQ_SONG_GetActnMode_1

SEQ_SONG_GetActnMode_0
	movlw	SEQ_SONG_ACTN_MODE_STOP
	rgoto	SEQ_SONG_GetActnMode_End

SEQ_SONG_GetActnMode_1
	movlw	SEQ_SONG_ACTN_MODE_LOOPS
	rgoto	SEQ_SONG_GetActnMode_End

SEQ_SONG_GetActnMode_2
	movlw	SEQ_SONG_ACTN_MODE_JMP_POS
	rgoto	SEQ_SONG_GetActnMode_End

SEQ_SONG_GetActnMode_3
	movlw	SEQ_SONG_ACTN_MODE_JMP_SONG
	rgoto	SEQ_SONG_GetActnMode_End

SEQ_SONG_GetActnMode_4
	movlw	SEQ_SONG_ACTN_MODE_SEL_MIXERMAP
	;; 	rgoto	SEQ_SONG_GetActnMode_End

SEQ_SONG_GetActnMode_End
	andlw	0xff		; (ensure that ZERO flag is set)
	return
