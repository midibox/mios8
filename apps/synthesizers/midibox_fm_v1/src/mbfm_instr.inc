; $Id$
;
; MIDIbox FM
; Instrument Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function returns the base address of an instrument
;;  Input:
;;     o instrument number in MBFM_CURRENT_INSTRUMENT
;;  Output:
;;     o base address in FSR1
;; --------------------------------------------------------------------------
MBFM_INSTR_GetBase
	;; calc base address to instrument record
	SET_BSR	MBFM_BASE
	lfsr	FSR1, MBFM_I1_BASE
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

#if WAVETABLES_IN_RAM
;; --------------------------------------------------------------------------
;;  This function returns the base address to the wavetable of an instrument
;;  Input:
;;     o instrument number in MBFM_CURRENT_INSTRUMENT
;;  Output:
;;     o base address to wavetable in FSR1
;; --------------------------------------------------------------------------
MBFM_INSTR_GetWTBase
	;; calc base address to wavetable
	SET_BSR	MBFM_BASE
	lfsr	FSR1, MBFM_I1_WAVETABLE_BASE
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	mullw	0x80
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return
#endif

;; --------------------------------------------------------------------------
;;  This function returns the base address of an instrument, the bank and
;;  the patch number
;;  Input:
;;     o instrument number in MBFM_CURRENT_INSTRUMENT
;;  Output:
;;     o base address in FSR1
;;     o patch number in MBFM_PATCH
;;     o bank number in MBFM_BANK
;; --------------------------------------------------------------------------
MBFM_INSTR_GetBasePatchAndBank
	;; get base, patch and bank number
	rcall	MBFM_INSTR_GetBase

	movlw	MBFM_Ix_PATCH
	movff	PLUSW1, MBFM_PATCH
	movlw	MBFM_Ix_BANK
	movff	PLUSW1, MBFM_BANK
	return


;; --------------------------------------------------------------------------
;;  This function sends an instrument parameter to OPL3 and saves it into RAM
;;  Input:
;;     o parameter value in WREG
;;     o parameter index in MIOS_PARAMETER1
;;     o instrument number in MBFM_CURRENT_INSTRUMENT
;; --------------------------------------------------------------------------
MBFM_INSTR_SendPar
	;; temporary save value in MIOS_PARAMETER3
	movwf	MIOS_PARAMETER3

	;; get base address to instrument record -> FSR1
	rcall	MBFM_INSTR_GetBase

	;; add parameter index and store value
	movf	MIOS_PARAMETER1, W
	movff	MIOS_PARAMETER3, PLUSW1

	;; copy FSR1 -> FSR0
	movff	FSR1L, FSR0L
	movff	FSR1H, FSR0H

	;; update registers of all assigned voices
	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_VOICE, BANKED
	lfsr	FSR1, MBFM_V1_BASE
MBFM_INSTR_SendParLoop
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	cpfseq	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_INSTR_SendParLoop_Next

	;; check which OPL3 SFRs need to be updated
	;; this code heavily depends on the Ix register assignments in app_defines.inc
	movlw	MBFM_Ix_OP1_FLAGS_MULT-1
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto MBFM_INSTR_SendParLoop_NoSFR
	movlw	MBFM_Ix_WT_RATE
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto MBFM_INSTR_SendParLoop_NoSFR
MBFM_INSTR_SendParLoop_SFR
	movf	MIOS_PARAMETER1, W
	addlw	-MBFM_Ix_OP1_FLAGS_MULT
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MBFM_INSTR_SendParLoop_SFR_0
	rgoto	MBFM_INSTR_SendParLoop_SFR_1
	rgoto	MBFM_INSTR_SendParLoop_SFR_2
	rgoto	MBFM_INSTR_SendParLoop_SFR_3
	rgoto	MBFM_INSTR_SendParLoop_SFR_4
	rgoto	MBFM_INSTR_SendParLoop_SFR_5
	rgoto	MBFM_INSTR_SendParLoop_SFR_6
	rgoto	MBFM_INSTR_SendParLoop_SFR_7

MBFM_INSTR_SendParLoop_SFR_0	;; MBFM_Ix_FLAGS_MULT
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_FLAGS_MULT
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_1	;; MBFM_Ix_OPx_KSL_TL
	;; save current table pointer and MIOS_PARAMETER1 in TMP[123]
	movff	TBLPTRL, TMP1
	movff	TBLPTRH, TMP2
	movff	MIOS_PARAMETER1, TMP3
	;; update registers
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_VOICE_REFRESH
	;; transfer volume to voice registers
	call	MBFM_MIDI_Hlp_Vol
	;; restore table pointer and MIOS_PARAMETER1
	movff	TMP1, TBLPTRL
	movff	TMP2, TBLPTRH
	movff	TMP3, MIOS_PARAMETER1
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_2	;; MBFM_Ix_OPx_AR_DR
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_AR_DR
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_3	;; MBFM_Ix_OPx_SL_RR
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_SL_RR
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_4	;; MBFM_Ix_OPx_WS
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_WS
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_5	;; MBFM_Ix_OPxy_CH_FB_CNT
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_CH_FB_CNT
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_6	;; frequency control registers
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_NOTE_UPDATE_REQ
	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_SFR_7	;; reserved
	;; 	rgoto	MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_NoSFR
MBFM_INSTR_SendParLoop_SFR_Cont

MBFM_INSTR_SendParLoop_Next
	movlw	MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	incf	MBFM_CURRENT_VOICE, F, BANKED
	movlw	MBFM_Vx_NUM - 1
	cpfsgt	MBFM_CURRENT_VOICE, BANKED
	rgoto MBFM_INSTR_SendParLoop

	return
	

;; --------------------------------------------------------------------------
;;  This function maps MBFM_Ix_AUDIO_CHANNELS to MBFM_Ix_OPy_CH_FB_CNT
;;  Input:
;;     o instrument number in MBFM_CURRENT_INSTRUMENT
;;     o audio channels in MBFM_Ix_AUDIO_CHANNELS
;; --------------------------------------------------------------------------
MBFM_INSTR_SendAudioChn
	;; send channel A
	rcall	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_ACHN_OP12
	swapf	PLUSW1, W
	andlw	0xf0
	movwf	MIOS_PARAMETER3

	movlw	MBFM_Ix_OP12_CH_FB_CNT
	movwf	MIOS_PARAMETER1
	movf	PLUSW1, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER3, W
	rcall	MBFM_INSTR_SendPar

	;; send channel B
	rcall	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_ACHN_OP34
	swapf	PLUSW1, W
	andlw	0xf0
	movwf	MIOS_PARAMETER3

	movlw	MBFM_Ix_OP34_CH_FB_CNT
	movwf	MIOS_PARAMETER1
	movf	PLUSW1, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER3, W
	rgoto	MBFM_INSTR_SendPar


;; --------------------------------------------------------------------------
;;  This function initializes an instrument
;;  MBFM_INSTR_Init: patch loaded from EEPROM
;;  MBFM_INSTR_Init_RAM: patch loaded from RAM
;;  Input:
;;     o instrument number in MBFM_CURRENT_INSTRUMENT
;;     o patch number in MBFM_Ix_PATCH
;;     o bank number in MBFM_Ix_BANK
;; --------------------------------------------------------------------------
MBFM_INSTR_Init_RAM
	;; get base, patch and bank number
	rcall	MBFM_INSTR_GetBasePatchAndBank

	rgoto	MBFM_INSTR_Init_Cont
	
MBFM_INSTR_Init
	;; get base, patch and bank number
	rcall	MBFM_INSTR_GetBasePatchAndBank

	;; load patch from EEPROM/BankStick
 	call	MBFM_BANK_LoadP
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetWTBase
	call	MBFM_BANK_LoadW
#endif
	
MBFM_INSTR_Init_Cont
	;; update all voices which are assigned to the instrument
	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_VOICE, BANKED
	lfsr	FSR1, MBFM_V1_BASE
MBFM_INSTR_InitLoop
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	cpfseq	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_INSTR_InitLoop_Next

	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_VOICE_REFRESH

	;; transfer volume to voice registers
	call	MBFM_MIDI_Hlp_Vol

MBFM_INSTR_InitLoop_Next
	movlw	MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	incf	MBFM_CURRENT_VOICE, F, BANKED
	movlw	MBFM_Vx_NUM - 1
	cpfsgt	MBFM_CURRENT_VOICE, BANKED
	rgoto MBFM_INSTR_InitLoop

	;; send audio channels
	rcall	MBFM_INSTR_SendAudioChn

#if CS_ENABLED
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
#endif
	return


;; --------------------------------------------------------------------------
;;  This function initializes all instruments
;;  Input:
;;     o patch number in MBFM_Ix_PATCH
;;     o bank number in MBFM_Ix_BANK
;; --------------------------------------------------------------------------
MBFM_INSTR_InitAll
	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
	rcall	MBFM_INSTR_Init
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	rcall	MBFM_INSTR_Init
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	rcall	MBFM_INSTR_Init
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	rgoto	MBFM_INSTR_Init

;; --------------------------------------------------------------------------
;;  This function initializes all instruments from RAM
;;  Input:
;;     o patch number in MBFM_Ix_PATCH
;;     o bank number in MBFM_Ix_BANK
;; --------------------------------------------------------------------------
MBFM_INSTR_InitAll_RAM
	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
	rcall	MBFM_INSTR_Init_RAM
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	rcall	MBFM_INSTR_Init_RAM
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	rcall	MBFM_INSTR_Init_RAM
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	rgoto	MBFM_INSTR_Init_RAM
