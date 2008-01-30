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
	goto	SID_ENS_Init
