; $Id$
;
; SID Patch routines
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Init Routine for patch parameters
;;  IN: patch in SID_PATCH_BUFFER(_SHADOW)
;; --------------------------------------------------------------------------
SID_PATCH_Init
	;; disable SID engine
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE

	;; clear the whole SID SFR memory space
	;; skip this if old and new engine are drum or bassline
	SET_BSR	SID_PATCH_BUFFER + SID_Ix_ENGINE
	movf	SID_PATCH_BUFFER + SID_Ix_ENGINE, W, BANKED
	SET_BSR	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE
	xorwf	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, W, BANKED
	andlw	0x03
	bnz	SID_PATCH_Init_Clear
	movf	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, W, BANKED
	andlw	0x03
	bz	SID_PATCH_Init_Clear ; lead engine
	xorlw	0x03
	bz	SID_PATCH_Init_Clear ; multi engine
	rgoto	SID_PATCH_Init_ClearSkip

SID_PATCH_Init_Clear
	movlw	SIDSE_CLEAR_128B_BLOCKS
	movwf	TABLAT
	lfsr	FSR0, SIDSE_CLEAR_BEGIN
SID_PATCH_Init_ClearLoop
	clrf	POSTINC0
	movf	FSR0L, W
	andlw	0x7f
	bnz	SID_PATCH_Init_ClearLoop
	decfsz	TABLAT, F
	rgoto	SID_PATCH_Init_ClearLoop

	;; clear non-active notes
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_ClrNonActive
	lfsr	FSR2, SID_MV2_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_ClrNonActive
	lfsr	FSR2, SID_MV3_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_ClrNonActive
	lfsr	FSR2, SID_MV4_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_ClrNonActive
	lfsr	FSR2, SID_MV5_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_ClrNonActive
	lfsr	FSR2, SID_MV6_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_ClrNonActive

	;; clear parts of the MIDI voices
	clrf	TABLAT
SID_PATCH_Init_ClearMVLoop
	lfsr	FSR0, SID_MV1_BASE
	movf	TABLAT, W
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addlw	SID_MVx_PITCHBENDER
	addwf	FSR0L, F

	;; clear variable area
	movlw	(SID_MVx_WT_STACK_0-1)-SID_MVx_PITCHBENDER+1
	movwf	PRODL
SID_PATCH_Init_ClearMVLoop_I1
	clrf	POSTINC0
	decfsz	PRODL, F
	rgoto	SID_PATCH_Init_ClearMVLoop_I1

	
	;; clear note stack if new engine != old engine
	SET_BSR	SID_PATCH_BUFFER + SID_Ix_ENGINE
	movf	SID_PATCH_BUFFER + SID_Ix_ENGINE, W, BANKED
	SET_BSR	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE
	xorwf	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, W, BANKED
	andlw	0x03
	bz	SID_PATCH_Init_ClearMVLoop_Next

	;; clear sustain flags
	clrf	SID_MIDIVOICE_SUSTAIN, BANKED

	;; clear note stack (+ptr)
	movlw	SID_MVx_NOTE_STACK_LEN + 1
	movwf	PRODL
SID_PATCH_Init_ClearMVLoop_I2
	clrf	POSTINC0
	decfsz	PRODL, F
	rgoto	SID_PATCH_Init_ClearMVLoop_I2

SID_PATCH_Init_ClearMVLoop_Next
	incf	TABLAT, F
	movlw	SID_MVx_NUM-1
	cpfsgt	TABLAT, ACCESS
	rgoto SID_PATCH_Init_ClearMVLoop

	;; ensure that WT pointer will be reset to avoid,
	;; that a random WT position will be played after patch change
	SET_BSR	SID_BASE
	movlw	0x0f
	movwf	SID_SE_TRG_EVNT_U, BANKED

SID_PATCH_Init_ClearSkip
	SET_BSR	SID_BASE

	;; transfer patch buffer to shadow buffer
	lfsr	FSR0, SID_PATCH_BUFFER
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW
	movlw	2
	movwf	PRODL
SID_PATCH_Init_CopyLoop
	movff	POSTINC0, POSTINC1
	movf	FSR0L, W
	bnz	SID_PATCH_Init_CopyLoop
	decfsz	PRODL, F
	rgoto	SID_PATCH_Init_CopyLoop

#if 0	; disabled - could cause confusion! see also comment on bassline knob function issues at the end of the ChangeLog
	;; update knob values
	call	SID_KNOB_UpdateAll
#endif

	;; already set volume and filter values to right value to avoid crackling sound in some cases
	;; doesn't prevent crackle if filter channels/mode are switched between two patterns!
	;; note: I tried a slow fade-in of volume, but this leads to even worse
	;; results since volume influences the offset voltage at the audio output

	;; works for all engines so long it is ensured, that filters are located at same patch position
	IRQ_DISABLE
	clrf	SID_SE_ELEMENT_NUM, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL1_L
	lfsr	FSR2, SIDL_BASE
	call	SIDSE_Filter

	incf	SID_SE_ELEMENT_NUM, F, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL2_L
	lfsr	FSR2, SIDR_BASE
	call	SIDSE_Filter
	IRQ_ENABLE

	;; force a refresh of all registers
 	call	SID_SR_Refresh

	;; clear voice queue
	call	SID_VOICE_Init

	;; if a note was played during patch change, play it again
	;; works only with lead engine when superpoly not enabled
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bnz	SID_PATCH_Init_NoNote
	movff	SID_LOCAL_ENS + SID_ENSx_SUPERPOLY_CTRL, WREG
	andlw	0x07
	bnz	SID_PATCH_Init_NoNote
	SET_BSR	SID_BASE
	lfsr	FSR0, SID_MV1_BASE
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	bz	SID_PATCH_Init_NoNote
SID_PATCH_Init_PlayNoteAgain

	;; 	lfsr	FSR0, SID_MV1_BASE  ; (already in FSR0)
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_PATCH_Init_PlayNoteAgainLoop
	movlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW0, W
	andlw	0x7f
	movwf	SID_MIDI_PARAMETER1, BANKED	; note number
	movlw	0x64
	movwf	SID_MIDI_PARAMETER2, BANKED	; velocity
	call	SID_MIDI_L_Hlp_NoteOn

	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_PATCH_Init_PlayNoteAgainLoop
SID_PATCH_Init_NoNote

	;; LFOs should start with random offset (for best freerunning results - especially in multi mode)
	;; can be *enabled* with the DOR flag
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFCLR WREG, SID_ENS_CTRL1_DOR, ACCESS, SID_PATCH_Init_NoLFORnd
SID_PATCH_Init_LFORnd
	lfsr	FSR1, SID_LFO1_BASE
	movlw	SID_LFOx_NUM
	movwf	TABLAT		; using TABLAT as counter
SID_PATCH_Init_LFORnd_Loop
	call	SID_RND_GenRandomNumber
	movff	SID_RANDOM_SEED_L, POSTINC1
	movff	SID_RANDOM_SEED_H, POSTDEC1
	movlw	SID_LFOx_RECORD_LEN
	addwf	FSR1L, F
	decfsz	TABLAT, F
	rgoto	SID_PATCH_Init_LFORnd_Loop
SID_PATCH_Init_NoLFORnd

	
	;; enable SID engine again
	bcf	SID_STAT, SID_STAT_ENGINE_DISABLE

	return


;; --------------------------------------------------------------------------
;;  This routine plays a note on all selected instruments (depends on engine)
;;  IN: current instrument number in CS_MENU_SELECTED_INS (used for multi and drum engine)
;;      selected SID flags (L/R) in CS_MENU_SELECTED_SID_LR
;;
;;      If function is called via MBNet (-> SID_MBNET_RxSpecial_NON), the
;;      variables have to be set from there!
;; --------------------------------------------------------------------------
SID_PATCH_NotesOn
	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_PATCH_NotesOn_DM
SID_PATCH_NotesOn_LB
	BRA_IFSET WREG, 0, ACCESS, SID_PATCH_NotesOn_B
SID_PATCH_NotesOn_L
	;; only single instrument
	movlw	0
	rgoto	SID_PATCH_NotesOn_Hlp

SID_PATCH_NotesOn_B
	;; the selected instruments
	;; note or sequence number is selected inside help function
	movlw	0
	RCALL_IFSET CS_MENU_SELECTED_SID_LR, 0, ACCESS, SID_PATCH_NotesOn_Hlp
	movlw	1
	RCALL_IFSET CS_MENU_SELECTED_SID_LR, 1, ACCESS, SID_PATCH_NotesOn_Hlp
	return

SID_PATCH_NotesOn_DM
	BRA_IFSET WREG, 0, ACCESS, SID_PATCH_NotesOn_M
SID_PATCH_NotesOn_D
	;; one instrument
	;; note or sequence number is selected inside help function
	movlw	0
	rcall	SID_PATCH_NotesOn_Hlp
	return

SID_PATCH_NotesOn_M
	;; selected instrument
	movf	CS_MENU_SELECTED_INS, W
	;; 	rgoto	SID_PATCH_NotesOn_Hlp


SID_PATCH_NotesOn_Hlp
	;; temporary store instrument number in MIOS_PARAMETER3
	movwf	MIOS_PARAMETER3

	;; determine pointer to MIDI voice
	lfsr	FSR1, SID_MV1_BASE
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F

	;; exit if MIDI voice disabled (channel > 16)
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW1, W
	movwf	MIOS_PARAMETER1
	andlw	0xf0
	bnz	SID_PATCH_NotesOn_Hlp_End

	movlw	0x90		; MIDI note
	iorwf	MIOS_PARAMETER1, F

	movlw	0x3c		; C-4
	movwf	MIOS_PARAMETER2

	;; increase if split point is higher
	movlw	SID_MVx_SPLIT_LOWER
	movf	PLUSW1, W
	cpfsgt	MIOS_PARAMETER2, ACCESS
	movwf MIOS_PARAMETER2

	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_PATCH_NotesOn_Hlp_DM
SID_PATCH_NotesOn_Hlp_LB
	BRA_IFSET WREG, 0, ACCESS, SID_PATCH_NotesOn_Hlp_B
SID_PATCH_NotesOn_Hlp_L
	;; no additional measure
	rgoto	SID_PATCH_NotesOn_Hlp_Cont

SID_PATCH_NotesOn_Hlp_B
	;; don't increase note number if sequencer disabled
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_FLAGS2, WREG
	btfsc	MIOS_PARAMETER3, 0
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_FLAGS2, WREG
	BRA_IFCLR WREG, SID_I_B_V_FLAGS2_WT_ONLY, ACCESS, SID_PATCH_NotesOn_Hlp_Cont

	;; select sequence
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM, WREG
	btfsc	MIOS_PARAMETER3, 0
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM, WREG
	andlw	0x07	; if sequence disabled, start with first (8 -> 0)
	addwf	MIOS_PARAMETER2, F
	rgoto	SID_PATCH_NotesOn_Hlp_Cont

SID_PATCH_NotesOn_Hlp_DM
	BRA_IFSET WREG, 0, ACCESS, SID_PATCH_NotesOn_Hlp_M
SID_PATCH_NotesOn_Hlp_D
	;; sequencer enabled?
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, WREG
	BRA_IFCLR WREG, SID_I_V_SEQ_ON, ACCESS, SID_PATCH_NotesOn_Hlp_D_NoSeq
SID_PATCH_NotesOn_Hlp_D_Seq
	;; select sequence
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM, WREG
	andlw	0x07	; if sequence disabled, start with first (8 -> 0)
	addwf	MIOS_PARAMETER2, F
	rgoto	SID_PATCH_NotesOn_Hlp_Cont

SID_PATCH_NotesOn_Hlp_D_NoSeq
	;; add instrument number to note
	movf	CS_MENU_SELECTED_INS, W
	addlw	0x0c		; +1 octave
	addwf	MIOS_PARAMETER2, F
	rgoto	SID_PATCH_NotesOn_Hlp_Cont

SID_PATCH_NotesOn_Hlp_M
	;; no additional measure
	;; 	rgoto	SID_PATCH_NotesOn_Hlp_Cont

SID_PATCH_NotesOn_Hlp_Cont
	movlw	0x40		; mid velocity
	movwf	MIOS_PARAMETER3

	;; forward MIDI event
	call	SID_MIDI_NotifyReceivedEvent

SID_PATCH_NotesOn_Hlp_End
	return


;; --------------------------------------------------------------------------
;;  This routine disables all notes, stops the arp and WT
;; --------------------------------------------------------------------------
SID_PATCH_NotesOff
	IRQ_DISABLE

	;; if drum engine:
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bnz	SID_PATCH_NotesOff_NoDrums
SID_PATCH_NotesOff_Drums
	;; request note off for all voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED
  	call	SID_SE_D_NoteOff
	rgoto	SID_PATCH_NotesOff_Drums_Cont

SID_PATCH_NotesOff_NoDrums
	;; disable voices
	SET_BSR	SID_BASE
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_PATCH_NotesOff_DisVLoop
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_PATCH_NotesOff_DisVLoop

SID_PATCH_NotesOff_Drums_Cont
	;; clear all runtime variables of MIDI voices
	clrf	TABLAT
SID_PATCH_NotesOff_ClearMVLoopO
	lfsr	FSR0, SID_MV1_BASE
	movf	TABLAT, W
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addlw	SID_MVx_PITCHBENDER
	addwf	FSR0L, F

	;; clear variable area
	movlw	SID_MVx_RECORD_LEN-SID_MVx_PITCHBENDER
	movwf	PRODL
SID_PATCH_NotesOff_ClearMVLoopI
	clrf	POSTINC0
	decfsz	PRODL, F
	rgoto	SID_PATCH_NotesOff_ClearMVLoopI

	incf	TABLAT, F
	movlw	SID_MVx_NUM-1
	cpfsgt	TABLAT, ACCESS
	rgoto SID_PATCH_NotesOff_ClearMVLoopO

	;; clear all sustain flags
	clrf	SID_MIDIVOICE_SUSTAIN, BANKED

	;; clear sequencer running flag
	SET_BSR	SID_SEQ1_BASE
	bcf	SID_SEQ1_BASE + SID_SEQx_MISC, SID_SEQ_MISC_SEQ_RUNNING, BANKED
	bcf	SID_SEQ2_BASE + SID_SEQx_MISC, SID_SEQ_MISC_SEQ_RUNNING, BANKED
	SET_BSR	SID_BASE

	IRQ_ENABLE

	return


;; --------------------------------------------------------------------------
;;  This routine transfers the patch buffer into the shadow buffer
;; --------------------------------------------------------------------------
SID_PATCH_UpdateShadowBuffer
	movlw	2
	movwf	PRODL
	lfsr	FSR0, SID_PATCH_BUFFER
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW
SID_PATCH_UpdateShadowBufferLoop
	movff	POSTINC0, POSTINC1
	movf	FSR0L, W
	bnz	SID_PATCH_UpdateShadowBufferLoop
	decfsz	PRODL, F
	rgoto	SID_PATCH_UpdateShadowBufferLoop

	;; special measure for pitchbender: since it is not part of the shadow buffer, we should clear it here
	;; for non-destructive knob assignment changes
	SET_BSR	SID_MV1_BASE
	clrf	SID_MV1_BASE + SID_MVx_PITCHBENDER, BANKED
	clrf	SID_MV2_BASE + SID_MVx_PITCHBENDER, BANKED
	clrf	SID_MV3_BASE + SID_MVx_PITCHBENDER, BANKED
	clrf	SID_MV4_BASE + SID_MVx_PITCHBENDER, BANKED
	clrf	SID_MV5_BASE + SID_MVx_PITCHBENDER, BANKED
	clrf	SID_MV6_BASE + SID_MVx_PITCHBENDER, BANKED

	SET_BSR	SIDL_V1_BASE
	clrf	SIDL_V1_BASE + SID_Vx_PITCHBENDER, BANKED
	clrf	SIDL_V2_BASE + SID_Vx_PITCHBENDER, BANKED
	clrf	SIDL_V3_BASE + SID_Vx_PITCHBENDER, BANKED
	clrf	SIDR_V1_BASE + SID_Vx_PITCHBENDER, BANKED
	clrf	SIDR_V2_BASE + SID_Vx_PITCHBENDER, BANKED
	clrf	SIDR_V3_BASE + SID_Vx_PITCHBENDER, BANKED

	;; update all knob values
	goto	SID_KNOB_UpdateAll


;; --------------------------------------------------------------------------
;;  This routine transfers the patch buffer into the edit buffer if the
;;  master SID is selected in the CS
;; --------------------------------------------------------------------------
SID_PATCH_UpdateEditBuffer
	;; copy into edit buffer if SID is selected
	movf	CS_MENU_EDIT_BUFFER_SID, W
	cpfseq	CS_MENU_SID, ACCESS
	rgoto SID_PATCH_UpdateEditBuffer_End
	movlw	2
	movwf	PRODL
	lfsr	FSR0, SID_PATCH_BUFFER
	lfsr	FSR1, SID_EDIT_BUFFER
SID_PATCH_UpdateEditBufferLoop
	movff	POSTINC0, POSTINC1
	movf	FSR0L, W
	bnz	SID_PATCH_UpdateEditBufferLoop
	decfsz	PRODL, F
	rgoto	SID_PATCH_UpdateEditBufferLoop
SID_PATCH_UpdateEditBuffer_End
	return

;; --------------------------------------------------------------------------
;;  This routine loads the EEPROM/BankStick content into the SID Patch Buffer
;;  and initialises the patch
;;  IN: Patch in SID_BANK/SID_PATCH
;; --------------------------------------------------------------------------
SID_PATCH_LoadPatchBuffer
	clrf	EEADR
	clrf	EEADRH
	lfsr	FSR1, SID_PATCH_BUFFER
SID_PATCH_LoadPatchBufferLoop
 	call	SID_PBANK_ReadPage		; transfer 64 bytes

	movlw	0x40				; increment FSR1 pointer
	addwf	FSR1L, F
	movlw	0
	addwfc	FSR1H, F

	movf	EEADR, W			; loop 8 times
	bnz	SID_PATCH_LoadPatchBufferLoop
	movlw	2-1
	cpfsgt	EEADRH, ACCESS
	rgoto SID_PATCH_LoadPatchBufferLoop

	;; update edit buffer if required
	rcall	SID_PATCH_UpdateEditBuffer

	;; re-init patch and exit
	rgoto	SID_PATCH_Init


;; --------------------------------------------------------------------------
;;  This function initialises the edit buffer depending on the selected engine
;; --------------------------------------------------------------------------
SID_PATCH_LoadPresetIntoEdit
	;; select engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SID_PATCH_LoadPresetIntoEdit_DM
SID_PATCH_LoadPresetIntoEdit_LB
	BRA_IFSET WREG, 0, ACCESS, SID_PATCH_LoadPresetIntoEdit_B
SID_PATCH_LoadPresetIntoEdit_L
	TABLE_ADDR SID_PATCH_PRESET_LEAD
	rgoto	SID_PATCH_LoadPresetIntoEdit_Cnt
SID_PATCH_LoadPresetIntoEdit_B
	TABLE_ADDR SID_PATCH_PRESET_BASSLINE
	rgoto	SID_PATCH_LoadPresetIntoEdit_Cnt

SID_PATCH_LoadPresetIntoEdit_DM
	BRA_IFSET WREG, 0, ACCESS, SID_PATCH_LoadPresetIntoEdit_M
SID_PATCH_LoadPresetIntoEdit_D
	TABLE_ADDR SID_PATCH_PRESET_DRUM
	rgoto	SID_PATCH_LoadPresetIntoEdit_Cnt
SID_PATCH_LoadPresetIntoEdit_M
	TABLE_ADDR SID_PATCH_PRESET_MULTI
	;; 	rgoto	SID_PATCH_LoadPresetIntoEdit_Cnt

SID_PATCH_LoadPresetIntoEdit_Cnt

	lfsr	FSR0, SID_EDIT_BUFFER
	movlw	2
	movwf	PRODL
SID_PATCH_LoadPresetIntoEditLoop
	tblrd*+
	movff	TABLAT, POSTINC0
	movf	FSR0L, W
	bnz	SID_PATCH_LoadPresetIntoEditLoop
	decfsz	PRODL, F
	rgoto	SID_PATCH_LoadPresetIntoEditLoop

	return

	;; include the preset tables (512 bytes)
SID_PATCH_PRESET_LEAD
#include "src/sid_preset_lead.inc"
SID_PATCH_PRESET_BASSLINE
#include "src/sid_preset_bassline.inc"
SID_PATCH_PRESET_DRUM
#include "src/sid_preset_drum.inc"
SID_PATCH_PRESET_MULTI
#include "src/sid_preset_multi.inc"

