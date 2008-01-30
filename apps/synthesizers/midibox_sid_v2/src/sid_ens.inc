; $Id$
;
; SID Ensemble routines
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Init Ensemble
;;  IN: ensemble number in SID_ENSEMBLE
;; --------------------------------------------------------------------------
SID_ENS_Init
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_ENS_Init_NoCS

	;; update CS_MENU_SID_x_PATCH, CS_MENU_SID_x_BANK and CS_MEU_SID_x_CHN directly from EEPROM
	clrf	PRODL		; used as loop counter
	clrf	PRODH		; contains EEADR page offset
SID_ENS_Init_CS_Loop
	;; fetch MIDI channel (only from first instrument)
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	PRODL, W
	addwf	FSR1L, F

	movf	PRODH, W
	addlw	SID_ENSx_I1_BASE + SID_ENSx_Ix_CHN
	movwf	EEADR
	call	SID_EBANK_Read
	andlw	0x0f
	movwf	INDF1

	;; fetch Bank
	lfsr	FSR1, CS_MENU_SID_M_BANK
	movf	PRODL, W
	addwf	FSR1L, F

	movf	PRODH, W
	addlw	SID_ENSx_BANK
	movwf	EEADR
	call	SID_EBANK_Read
	andlw	0x07
	movwf	INDF1


	;; fetch and request patch
	lfsr	FSR1, CS_MENU_SID_M_PATCH
	movf	PRODL, W
	addwf	FSR1L, F

	movf	PRODH, W
	addlw	SID_ENSx_PATCH
	movwf	EEADR
	call	SID_EBANK_Read
	movwf	INDF1

	;; request delayed Tx
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	PRODL, W
	addwf	FSR1L, F
	movlw	0x81
	movwf	INDF1

	incf	PRODL, F
	movlw	0x40
	addwf	PRODH, F
	bnz	SID_ENS_Init_CS_Loop

	;; transfer slave settings
	movlw	1
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDSetup
	call	CS_MENU_MBNET_Tx_SendEns

	movlw	2
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDSetup
	call	CS_MENU_MBNET_Tx_SendEns

	movlw	3
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDSetup
	call	CS_MENU_MBNET_Tx_SendEns

SID_ENS_Init_NoCS
	
	;; load local ensemble settings
	movlw	0
	rcall	SID_ENS_LoadSIDSetup
	rcall	SID_ENS_Init_Local

	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_ENS_Init_NoCS2

	;; re-init ensemble buffer based on selected SID
 	call	CS_MENU_MS_UpdateEnsBuffer

	;; update display
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
SID_ENS_Init_NoCS2
	return



;; --------------------------------------------------------------------------
;;  Local initialisation
;;  Expecting ensemble data in SID_ENS_BUFFER
;; --------------------------------------------------------------------------
SID_ENS_Init_Local
	;; copy local ENS settings offset 0x00-0x0f from SID_ENS_BUFFER to SID_LOCAL_ENS
	lfsr	FSR1, SID_ENS_BUFFER
	lfsr	FSR2, SID_LOCAL_ENS
	clrf	TABLAT		; used as loop counter
SID_ENS_Init_Local_Loop1
	movff	POSTINC1, POSTINC2
	incf	TABLAT, F
	BRA_IFCLR TABLAT, 4, ACCESS, SID_ENS_Init_Local_Loop1

	;; update BPM
 	call	SID_ENS_BPM_Update

	;; MIDI voices
	lfsr	FSR1, SID_ENS_BUFFER + SID_ENSx_I1_BASE
	lfsr	FSR2, SID_MV1_BASE
	clrf	TABLAT		; used as loop counter
SID_ENS_Init_Local_Loop2
	movlw	SID_ENSx_Ix_CHN		; transfer MIDI channel
	movff	PLUSW1, PRODL
	movlw	SID_MVx_MIDI_CHANNEL
	movff	PRODL, PLUSW2

	movlw	SID_ENSx_Ix_SPLIT_L	; transfer lower split point
	movff	PLUSW1, PRODL
	movlw	SID_MVx_SPLIT_LOWER
	movff	PRODL, PLUSW2

	movlw	SID_ENSx_Ix_SPLIT_U	; transfer upper split point
	movff	PLUSW1, PRODL
	movlw	SID_MVx_SPLIT_UPPER
	movff	PRODL, PLUSW2

	movlw	SID_ENSx_Ix_TRN		; transfer transpose value
	movff	PLUSW1, PRODL
	movlw	SID_MVx_TRANSPOSE
	movff	PRODL, PLUSW2

	;; switch to next records
	movlw	SID_ENSx_Ix_RECORD_LEN
	addwf	FSR1L, F
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR2L, F

	;; loop 6 times (prepared for 6 MIDI voices)
	incf	TABLAT, F
	movlw	SID_MVx_NUM-1
	cpfsgt	TABLAT, ACCESS
	rgoto SID_ENS_Init_Local_Loop2

	;; ensure that all AOUTs will be updated
	lfsr	FSR1, AOUT0_H
	movlw	8
	movwf	TABLAT
SID_ENS_Init_Local_Loop3
	bcf	POSTINC1, 7	; request refresh of AOUT register
	movf	POSTINC1, W	; (switch to next AOUTx_H)
	decfsz	TABLAT, F
	rgoto	SID_ENS_Init_Local_Loop3

	return


;; --------------------------------------------------------------------------
;;  Transfers the setup of a specific SID from EEPROM into SID_ENS_BUFFER
;;  IN: SID (0-3) in WREG
;; --------------------------------------------------------------------------
SID_ENS_LoadSIDSetup
	;; calculate address
	swapf	WREG, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	movwf	EEADR

	;; load configuration page
	lfsr	FSR1, SID_ENS_BUFFER
	call	SID_EBANK_ReadPage

	;; compatibility fix: if BPM entry is less than 50, force it to 120
	movff	SID_ENS_BUFFER + SID_ENSx_BPM, PRODL
	movlw	50
	cpfslt	PRODL, ACCESS
	rgoto SID_ENS_LoadSIDSetup_NoBPMFix
	movlw	120
	movff	WREG, SID_ENS_BUFFER + SID_ENSx_BPM
SID_ENS_LoadSIDSetup_NoBPMFix
	return	


;; --------------------------------------------------------------------------
;;  Transfers the setup of a specific SID from SID_ENS_BUFFER into EEPROM
;;  IN: SID (0-3) in WREG
;; --------------------------------------------------------------------------
SID_ENS_StoreSIDSetup
	;; calculate address
	swapf	WREG, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	movwf	EEADR

	;; store configuration page
	lfsr	FSR1, SID_ENS_BUFFER
	goto	SID_EBANK_WritePage


;; --------------------------------------------------------------------------
;;  Transfers a preset of a specific SID from FLASH into SID_ENS_BUFFER
;;  IN: SID (0-3) in WREG
;; --------------------------------------------------------------------------
SID_ENS_LoadSIDPreset
	;; temporary store SID number in PRODH
	movwf	PRODH

	;; calculate address
	swapf	WREG, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0

	addlw	LOW(SID_ENS_PRESET)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SID_ENS_PRESET)
	addwfc	TBLPTRH, F
	clrf	TBLPTRU
	movlw	UPPER(SID_ENS_PRESET)
	addwfc	TBLPTRU, F

	;; load configuration page
	lfsr	FSR1, SID_ENS_BUFFER
	movlw	0x40
	movwf	PRODL
SID_ENS_LoadSIDPresetLoop
	tblrd*+
	movff	TABLAT, POSTINC1
	decfsz	PRODL, F
	rgoto	SID_ENS_LoadSIDPresetLoop

	;; update CS MIDI channel
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	PRODH, W
	movff	SID_ENS_BUFFER + SID_ENSx_I1_BASE + SID_ENSx_Ix_CHN, PLUSW0

	;; update patch and bank in ensemble (won't be overwritten on initialisation)
	lfsr	FSR0, CS_MENU_SID_M_BANK
	movf	PRODH, W
	movff	PLUSW0, SID_ENS_BUFFER + SID_ENSx_BANK

	lfsr	FSR0, CS_MENU_SID_M_PATCH
	movf	PRODH, W
	movf	PLUSW0, W
	andlw	0x7f
	movff	WREG, SID_ENS_BUFFER + SID_ENSx_PATCH

	return	


;; --------------------------------------------------------------------------
;;  Transfers the local setup into SID_ENS_BUFFER
;; --------------------------------------------------------------------------
SID_ENS_LocalSetupIntoBuffer
	;; clear buffer
	lfsr	FSR1, SID_ENS_BUFFER
	clrf	PRODL
SID_ENS_LocalSetupIntoBufferClrL
	clrf	POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 6, ACCESS, SID_ENS_LocalSetupIntoBufferClrL

	;; transfer local ENS settings offset 0x00-0x0f from SID_LOCAL_ENS to SID_ENS_BUFFER
	lfsr	FSR1, SID_ENS_BUFFER
	lfsr	FSR2, SID_LOCAL_ENS
	clrf	TABLAT		; used as loop counter
SID_ENS_LocalSetupIntoBufferLcL
	movff	POSTINC2, POSTINC1
	incf	TABLAT, F
	BRA_IFCLR TABLAT, 4, ACCESS, SID_ENS_LocalSetupIntoBufferLcL

	;; transfer selected local MIDI voice values into buffer
	lfsr	FSR1, SID_ENS_BUFFER + SID_ENSx_I1_BASE
	lfsr	FSR2, SID_MV1_BASE
	clrf	TABLAT		; used as loop counter
SID_ENS_LocalSetupIntoBufferMV_L
	movlw	SID_MVx_MIDI_CHANNEL	; transfer MIDI channel
	movff	PLUSW2, PRODL
	movlw	SID_ENSx_Ix_CHN
	movff	PRODL, PLUSW1

	movlw	SID_MVx_SPLIT_LOWER	; transfer lower split point
	movff	PLUSW2, PRODL
	movlw	SID_ENSx_Ix_SPLIT_L
	movff	PRODL, PLUSW1

	movlw	SID_MVx_SPLIT_UPPER	; transfer upper split point
	movff	PLUSW2, PRODL
	movlw	SID_ENSx_Ix_SPLIT_U
	movff	PRODL, PLUSW1

	movlw	SID_MVx_TRANSPOSE	; transfer transpose value
	movff	PLUSW2, PRODL
	movlw	SID_ENSx_Ix_TRN
	movff	PRODL, PLUSW1

	;; switch to next records
	movlw	SID_ENSx_Ix_RECORD_LEN
	addwf	FSR1L, F
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR2L, F

	;; loop 6 times (prepared for 6 MIDI voices)
	incf	TABLAT, F
	movlw	SID_MVx_NUM-1
	cpfsgt	TABLAT, ACCESS
	rgoto SID_ENS_LocalSetupIntoBufferMV_L

	return


;; --------------------------------------------------------------------------
;;  Stores SID_ENSEMBLE in internal EEPROM, so that it will be loaded again
;;  after power-on
;;  IN: SID_ENSEMBLE
;; --------------------------------------------------------------------------
SID_ENS_StoreDefaultNum
	movlw	LOW(EEPROM_DEFAULT_ENS)
	movwf	EEADR
	movlw	HIGH(EEPROM_DEFAULT_ENS)
	movwf	EEADRH
	movf	SID_ENSEMBLE, W
	goto	MIOS_EEPROM_Write


;; --------------------------------------------------------------------------
;;  Loads SID_ENSEMBLE from internal EEPROM (called after power-on within USER_Init)
;;  OUT: SID_ENSEMBLE
;; --------------------------------------------------------------------------
SID_ENS_LoadDefaultNum
	movlw	LOW(EEPROM_DEFAULT_ENS)
	movwf	EEADR
	movlw	HIGH(EEPROM_DEFAULT_ENS)
	movwf	EEADRH
	call	MIOS_EEPROM_Read
	movwf	SID_ENSEMBLE
	return


;; --------------------------------------------------------------------------
;;  This function update the timer reload value depending on the given
;;  BPM period length
;; --------------------------------------------------------------------------
SID_ENS_BPM_Update
	;; calc address to timer value:
	movlw	(3125000 >> 16) & 0xff
	movwf	MATH_AARGB0
	movlw	(3125000 >> 8) & 0xff
	movwf	MATH_AARGB1
	movlw	(3125000) & 0xff
	movwf	MATH_AARGB2

	clrf	MATH_BARGB0
	movff	SID_LOCAL_ENS + SID_ENSx_BPM, MATH_BARGB1

	call	MATH_DIV24_16

	;; temporary disable interrupts when changing timer reload values
	movff	INTCON, MATH_AARGB0	; store IRQ enable flags in MATH_AAGB0 (-> TABLAT, see math_div24_16.inc)
	IRQ_DISABLE
	
	;; result in MATH_AARGB1:MATH_AARGB2
	;; copy to TIMER0_RELOAD_[LH]
	comf	MATH_AARGB2, W
	movff	WREG, TIMER0_RELOAD_L
	comf	MATH_AARGB1, W
	movff	WREG, TIMER0_RELOAD_H

	movff	MATH_AARGB0, INTCON	; enable IRQs again (if they were enabled before...)

	;; internal clock source, 16bit, prescaler 1:2
 	movlw	(1 << TMR0ON)
	movwf	T0CON

	;; ensure that timer0 interrupt not enabled
	bcf	INTCON, T0IE

	return


;; --------------------------------------------------------------------------
;;  This function loads the ensemble preset into buffer
;; --------------------------------------------------------------------------
SID_ENS_LoadPreset
	;; load preset into master and slaves

	movlw	0
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDPreset
	rcall	SID_ENS_Init_Local

	movlw	1
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDPreset
	call	CS_MENU_MBNET_Tx_SendEns

	movlw	2
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDPreset
	call	CS_MENU_MBNET_Tx_SendEns

	movlw	3
	movwf	CS_MENU_SID
	call	SID_ENS_LoadSIDPreset
	call	CS_MENU_MBNET_Tx_SendEns

	;; determine selected SID number (-> CS_MENU_SID)
	call	CS_MENU_MS_GetSIDNumber

	;; load back edit buffer
	goto	CS_MENU_MS_UpdateEnsBuffer


;; include preset from file
SID_ENS_PRESET
#include "src/sid_preset_ens.inc"

