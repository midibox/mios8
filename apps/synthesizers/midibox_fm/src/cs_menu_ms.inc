; $Id$
;
; MBFM Control Surface Master/Slave Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Determines the current INS number depending on the selected INSs
;;  Result in WREG and MBFM_CURRENT_INSTRUMENT
;; --------------------------------------------------------------------------
CS_MENU_MS_GetInstrNumber
	;; return value depends on first selected instrument
	movlw	0x03
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 2
	movlw	0x02
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 1
	movlw	0x01
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 0
	movlw	0x00
	movff	WREG, MBFM_CURRENT_INSTRUMENT
	return

;; --------------------------------------------------------------------------
;;  copy base address of instrument to FSR0 depending on CS_MENU_SELECTED_INS
;; --------------------------------------------------------------------------
CS_MENU_MS_GetInstrBase
	lfsr	FSR0, MBFM_I1_BASE
	rcall	CS_MENU_MS_GetInstrNumber
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return

;; --------------------------------------------------------------------------
;;  copy base address of MBFM_CURRENT_INSTRUMENT to FSR0 depending on CS_MENU_SELECTED_INS
;; --------------------------------------------------------------------------
CS_MENU_MS_GetCurInstrBase
	lfsr	FSR0, MBFM_I1_BASE
	movff	MBFM_CURRENT_INSTRUMENT, WREG
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return


;; --------------------------------------------------------------------------
;;  copy patch and bank of currently selected instrument to MBFM_PATCH and MBFM_BANK
;; --------------------------------------------------------------------------
CS_MENU_MS_GetInstrPatch
	rcall	CS_MENU_MS_GetInstrBase
	movlw	MBFM_Ix_BANK
	movff	PLUSW0, MBFM_BANK
	movlw	MBFM_Ix_PATCH
	movff	PLUSW0, MBFM_PATCH
	return


;; --------------------------------------------------------------------------
;;  This help function updates the WT buffer depending on CS_MENU_WT_STEP
;;  (meaning here: WT step)
;;  In: position in CS_MENU_WT_STEP
;; --------------------------------------------------------------------------
CS_MENU_MS_Update_WT_Buffer
	;; copy pos to CS_MENU_WT_STEP_BUFFER
	movff	CS_MENU_WT_STEP, CS_MENU_WT_STEP_BUFFER
	;; clear modified flags
	clrf	CS_MENU_WT_MODIFIED

	;; prepare WT access
	rcall	CS_MENU_MS_Update_WT_Hlp

	clrf	TMP1
CS_MENU_MS_Update_WT_Buffer_Loop
	call	MBFM_BANK_ReadWByte
	andlw	0x7f
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 2, ACCESS, CS_MENU_MS_Update_WT_Buffer_Loop

	;; copy "relative mode" flags
	movlw	-4		; switch back to first byte
	addwf	FSR0L, F
	movff	INDF0, TMP1	; copy first byte to TMP1
	
	movlw	0x01		; copy bit 4 of first byte to bit 7 of second byte
	bcf	PLUSW0, 7
	btfsc	TMP1, 4
	bsf	PLUSW0, 7
	btfss	TMP1, 4		; toggle negate flag if relative mode
	btg	PLUSW0, 6
	movlw	0x02		; copy bit 5 of first byte to bit 7 of third byte
	bcf	PLUSW0, 7
	btfsc	TMP1, 5
	bsf	PLUSW0, 7
	btfss	TMP1, 5		; toggle negate flag if relative mode
	btg	PLUSW0, 6
	movlw	0x03		; copy bit 6 of first byte to bit 7 of fourth byte
	bcf	PLUSW0, 7
	btfsc	TMP1, 6
	bsf	PLUSW0, 7
	btfss	TMP1, 6		; toggle negate flag if relative mode
	btg	PLUSW0, 6

	;; clear flags
	movlw	0x0f
	andwf	INDF0, F
	return

;; -- prepare WT access
CS_MENU_MS_Update_WT_Hlp
	;; get bank/patch
	rcall	CS_MENU_MS_GetInstrPatch

	;; calc pointer to EEPROM: CS_MENU_WT_STEP << 2
	;; if WT step >= 0x20: select first step
	movlw	0x20-1
	cpfsgt	CS_MENU_WT_STEP, ACCESS
	rgoto CS_MENU_MS_Update_WT_Hlp_Normal
CS_MENU_MS_Update_WT_Hlp_Comb
	clrf	EEADR
	rgoto	CS_MENU_MS_Update_WT_Hlp_Cont
CS_MENU_MS_Update_WT_Hlp_Normal
	rlf	CS_MENU_WT_STEP, W
	rlf	WREG, W
	andlw	0x7c
	movwf	EEADR
CS_MENU_MS_Update_WT_Hlp_Cont

	;; get pointer to buffer
	lfsr	FSR0, CS_MENU_WT_MODE
	return

;; --------------------------------------------------------------------------
;;  This help function writes the WT buffer to EEPROM
;;  In: step in CS_MENU_WT_STEP
;; --------------------------------------------------------------------------
CS_MENU_MS_Update_WT_EEPROM
	;; don't write if current step != buffer step
	movf	CS_MENU_WT_STEP_BUFFER, W
	xorwf	CS_MENU_WT_STEP, W
	bnz	CS_MENU_MS_Update_WT_EEPROM_End; (will clear CS_MENU_WT_MODIFIED!)

	;; don't write if nothing has been updated
	movf	CS_MENU_WT_MODIFIED, W
	bz	CS_MENU_MS_Update_WT_EEPROM_End

	clrf	TMP5		; using TMP5 as loop counter if Step == "All"
CS_MENU_MS_Update_WT_EEPROMLoop
	;; prepare WT access
	rcall	CS_MENU_MS_Update_WT_Hlp	; always returns first entry if CS_MENU_WT_STEP >= 0x20
	movf	TMP5, W				; add loop counter to EEADR (will be incremented by 4 on each loop)
	addwf	EEADR, F			; (only relevant if Step == "All"

	;; copy old content to TMP[1234]
	call	MBFM_BANK_ReadWByte
	movwf	TMP1
	call	MBFM_BANK_ReadWByte
	movwf	TMP2
	call	MBFM_BANK_ReadWByte
	movwf	TMP3
	call	MBFM_BANK_ReadWByte
	movwf	TMP4
	movlw	-4		; switch back to first byte
	addwf	EEADR, F

	;; copy all *modified* parameters to TMP[1234]

	;; if modify flag for step mode is set, overwrite bitfield[3:0]
	BRA_IFCLR CS_MENU_WT_MODIFIED, 0, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M0N
CS_MENU_MS_Update_WT_EEPROM_M0
	movlw	0xf0
	andwf	TMP1, F
	movf	INDF0, W
	andlw	0x0f
	iorwf	TMP1, F
CS_MENU_MS_Update_WT_EEPROM_M0N;ot

	;; if modify flag of parameter 1 is set, copy new value to TMP2/TMP1[4]
	incf	FSR0L, F
	BRA_IFCLR CS_MENU_WT_MODIFIED, 1, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M1N
CS_MENU_MS_Update_WT_EEPROM_M1
	bcf	TMP1, 4		; 7th bit of second value to 4th bit of first byte
	btfsc	INDF0, 7
	bsf	TMP1, 4
	movf	INDF0, W	; copy value to TMP2
	btfss	TMP1, 4		; toggle negate flag if relative mode
	xorlw	0x40
	andlw	0x7f
	movwf	TMP2
CS_MENU_MS_Update_WT_EEPROM_M1N;ot

	;; if modify flag of parameter 2 is set, copy new value into TMP3/TMP1[5]
	incf	FSR0L, F
	BRA_IFCLR CS_MENU_WT_MODIFIED, 2, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M2N
CS_MENU_MS_Update_WT_EEPROM_M2
	bcf	TMP1, 5		; 7th bit of third value to 5th bit of first byte
	btfsc	INDF0, 7
	bsf	TMP1, 5
	movf	INDF0, W	; copy value to TMP3
	btfss	TMP1, 5		; toggle negate flag if relative mode
	xorlw	0x40
	andlw	0x7f
	movwf	TMP3
CS_MENU_MS_Update_WT_EEPROM_M2N;ot

	;; if modify flag of parameter 3 is set, copy new value into TMP4/TMP1[6]
	incf	FSR0L, F
	BRA_IFCLR CS_MENU_WT_MODIFIED, 3, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M3N
CS_MENU_MS_Update_WT_EEPROM_M3
	bcf	TMP1, 6		; 7th bit of fourth value to 6th bit of first byte
	btfsc	INDF0, 7
	bsf	TMP1, 6
	movf	INDF0, W	; copy value to TMP4
	btfss	TMP1, 6		; toggle negate flag if relative mode
	xorlw	0x40
	andlw	0x7f
	movwf	TMP4
CS_MENU_MS_Update_WT_EEPROM_M3N;ot

	movf	TMP1, W
	call	MBFM_BANK_WriteWByte
	movf	TMP2, W
	call	MBFM_BANK_WriteWByte
	movf	TMP3, W
	call	MBFM_BANK_WriteWByte
	movf	TMP4, W
	call	MBFM_BANK_WriteWByte

CS_MENU_MS_Update_WT_EEPROMLpNxt
	;; if WT_STEP >= 0x20 ("All"), increment TMP5 by 4 and loop until it has reached 0x80
	movlw	0x20-1
	cpfsgt	CS_MENU_WT_STEP, ACCESS
	rgoto CS_MENU_MS_Update_WT_EEPROM_End
	clrwdt			; feed watchdog (for long loops)
	movlw	4
	addwf	TMP5, F
	BRA_IFCLR TMP5, 7, ACCESS, CS_MENU_MS_Update_WT_EEPROMLoop

CS_MENU_MS_Update_WT_EEPROM_End
	clrf	CS_MENU_WT_MODIFIED	; clear all modified flags
	return
