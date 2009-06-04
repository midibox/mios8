; $Id$
;
; SID Control Surface Bank Handling
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  In difference to the SID_PBANK_Read function CS_BANK_Read reads from
;;  the patch of the current selected SID
;; --------------------------------------------------------------------------
CS_MENU_BANK_Read
	rcall	CS_MENU_MS_SetSIDPatch_C
	goto	SID_PBANK_Read


;; --------------------------------------------------------------------------
;;  In difference to the SID_PBANK_ReadPage function CS_BANK_ReadPage reads from
;;  the patch of the current selected SID
;; --------------------------------------------------------------------------
CS_MENU_BANK_ReadPage
	rcall	CS_MENU_MS_SetSIDPatch_C
	goto	SID_PBANK_ReadPage


;; --------------------------------------------------------------------------
;;  Check restore of ensemble configuration
;; --------------------------------------------------------------------------
CS_MENU_BANK_RestoreCfg_Tst
	decf	CS_MENU_REINIT_CFG_CTR, W
	skpz
	return
	clrf	CS_MENU_REINIT_CFG_CTR

	;; if Ensemble BankStick detected, and default ensemble was set to EInt, switch to E002
	;; this is done for more comfortable usage during application development (EInt will
	;; always be overwritten after app. download, E002 not...)
	
	;; in addition, we switch to EInt if no ensemble BankStick has been detected
	call	SID_EBANK_CheckValid
	bz	CS_MENU_BANK_RestoreCfg_Tst_BSA
CS_MENU_BANK_RestoreCfg_Tst_BSNA
	;; ensemble bankstick not available
	clrf	SID_ENSEMBLE	; ensure that ensemble starts with 0
	rgoto	CS_MENU_BANK_RestoreCfg_Tst_Cont

CS_MENU_BANK_RestoreCfg_Tst_BSA
	;; ensemble bankstick available
	;; switch to ensemble E002 if EInt was selected before
	movf	SID_ENSEMBLE, W
	bnz	CS_MENU_BANK_RestoreCfg_Tst_Cont
	movlw	0x01
	movwf	SID_ENSEMBLE

CS_MENU_BANK_RestoreCfg_Tst_Cont
	goto	SID_ENS_Init
