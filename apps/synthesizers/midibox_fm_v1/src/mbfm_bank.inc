; $Id$
;
; MIDIbox FM
; Bank Load/Store Routines
;
; Memory map:
;    - CS configuration                      0x0000-0x000f
;    - AOUT mapping                          0x0010-0x001f
;    - reserved                              0x0020-0x03ff
;    - 32 ensembles with 0x40 bytes each:    0x0400-0x0bff
;    - 16 drum sets with 0x40 bytes each:    0x0c00-0x0fff
;    - 128 patches with 0x60 bytes each:     0x1000-0x3fff
;    - 128 wavetables with 0x80 bytes each:  0x4000-0x7fff
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Stores a patch from RAM in EEPROM or BankStick
;;  IN: Instrument Number in MBFM_CURRENT_INSTRUMENT
;;      Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;; --------------------------------------------------------------------------
MBFM_BANK_StoreP
	rcall	MBFM_INSTR_GetBase		; get base address of patch

	rcall	MBFM_BANK_GetPAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	clrf	PRODL				; write 0x60 bytes (using PRODL as loop counter)
MBFM_BANK_StorePLoop
	movf	POSTINC1, W
	CALL_IFSET PRODH, 0, ACCESS, MIOS_BANKSTICK_Write
	CALL_IFCLR PRODH, 0, ACCESS, MIOS_EEPROM_Write
	;; 	bnz	MBFM_BANK_StoreP_Err		; write error?
	incf	PRODL, F
	movlw	0x60-1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_BANK_StorePLoop
	rgoto	MBFM_BANK_StoreP_End

MBFM_BANK_StoreP_Err
	;; here we could add error processing
MBFM_BANK_StoreP_End
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Loads a patch from EEPROM or BankStick into RAM
;;  IN: Instrument Number in MBFM_CURRENT_INSTRUMENT
;;      Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;; --------------------------------------------------------------------------
MBFM_BANK_LoadP
	rcall	MBFM_INSTR_GetBase		; get base address of patch

	rcall	MBFM_BANK_GetPAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	clrf	PRODL				; read remaining 0x60 bytes (using PRODL as loop counter)
MBFM_BANK_LoadPLoop
	CALL_IFSET PRODH, 0, ACCESS, MIOS_BANKSTICK_Read
	CALL_IFCLR PRODH, 0, ACCESS, MIOS_EEPROM_Read
	movwf	POSTINC1
	incf	PRODL, F
	movlw	0x60-1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_BANK_LoadPLoop

	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Returns a single byte from a patch (used by MBFM_LCD_PrintPatchEE)
;;  IN: offset in EEADR
;;      Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;; --------------------------------------------------------------------------
MBFM_BANK_ReadPByte
	rcall	MBFM_BANK_GetPAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	;; if address >= 0x60, return 0x00
	movlw	0x60 - 1
	cpfsgt	EEADR, ACCESS
	rgoto MBFM_BANK_ReadPByte_Cont
	movlw	0x00
	rgoto	MBFM_BANK_ReadPByte_End	

	;; (entry point for other read routines)
MBFM_BANK_ReadPByte_Cont
MBFM_BANK_ReadWByte_Cont

	BRA_IFCLR PRODH, 0, ACCESS, MBFM_BANK_ReadPByteInternal
MBFM_BANK_ReadPByteBankStick
	movf	EEADR, W			; add offset
	addwf	MIOS_PARAMETER1, F
	call	MIOS_BANKSTICK_Read		; read single byte from BankStick
	incf	EEADR, F			; increment EEPROM address
	rgoto	MBFM_BANK_ReadPByte_End

MBFM_BANK_ReadPByteInternal
	call	MIOS_EEPROM_Read		; just read byte

MBFM_BANK_ReadPByte_End
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  If BankStick available:
;;     -> the function sets the BankStick address to offset #0 of a patch entry
;;     -> PRODH[0] is set
;;  else
;;     -> the function sets the EEPROM address to offset #0 of a patch entry
;;     -> PRODH[0] is cleared
;;  IN: patch number in MBFM_PATCH
;;      BankStick number in MBFM_BANK
;;  OUT: address in MIOS_PARAMETER[12] or EEADR
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;;       PRODH[0] == 1 -> BankStick, else internal EEPROM
;; --------------------------------------------------------------------------
MBFM_BANK_GetPAddress
	;; store in internal EEPROM if bankstick is not available
	movf	MBFM_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	bz	MBFM_BANK_GetPAddressInternal

MBFM_BANK_GetPAddressExternal
 	movf	MBFM_PATCH, W			; patch * 0x60
	mullw	0x60
	movlw	0x00				; add 0x1000
	addwf	PRODL, W
	movwf	MIOS_PARAMETER1
	movlw	0x10
	addwfc	PRODH, W
	movwf	MIOS_PARAMETER2

	bsf	PRODH, 0			; notify that BankStick should be used
	
	movf	MBFM_BANK, W			; select bankstick
	andlw	0x07
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet

MBFM_BANK_GetPAddressInternal
	movlw	MBFM_PRESET_PATCH & 0xff	; set EEPROM address
	movwf	EEADR

	bcf	PRODH, 0			; notify that internal EEPROM should be used
	return


; ==========================================================================
; ==========================================================================

#if WAVETABLES_IN_RAM
;; --------------------------------------------------------------------------
;;  Loads a wavetable from BankStick into RAM
;;  IN: Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;;      address to buffer in FSR1 (normaly Ix_WAVETABLE_BASE on PIC18F4685 option)
;; --------------------------------------------------------------------------
MBFM_BANK_LoadW
	clrf	EEADR				; ensure that EEPROM address is zero
	rcall	MBFM_BANK_GetWAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	;; EEPROM not supported for wavetables
	BRA_IFCLR PRODH, 0, ACCESS, MBFM_BANK_LoadW_End

	call	MIOS_BANKSTICK_ReadPage	; reads 0x40 bytes
	movlw	0x40			; switch to next RAM section
	addwf	FSR1L, F
	call	MIOS_BANKSTICK_ReadPage	; reads 0x40 bytes
	;; 	bnz	MBFM_BANK_LoadW_Err		; write error?
	rgoto	MBFM_BANK_LoadW_End

MBFM_BANK_LoadW_Err
	;; here we could add error processing
MBFM_BANK_LoadW_End
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return
#endif

;; --------------------------------------------------------------------------
;;  Stores a wavetable from a RAM buffer in BankStick
;;  IN: Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;;      address to buffer in FSR1 (normaly MBFM_EE_BUFFER or Ix_WAVETABLE_BASE on PIC18F4685 option)
;; --------------------------------------------------------------------------
MBFM_BANK_StoreW
	clrf	EEADR				; ensure that EEPROM address is zero
	rcall	MBFM_BANK_GetWAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	;; EEPROM not supported for wavetables
	BRA_IFCLR PRODH, 0, ACCESS, MBFM_BANK_StoreW_End

	call	MIOS_BANKSTICK_WritePage	; writes 0x40 bytes
	movlw	0x40			; switch to next RAM section
	addwf	FSR1L, F
	call	MIOS_BANKSTICK_WritePage	; writes 0x40 bytes
	;; 	bnz	MBFM_BANK_StoreW_Err		; write error?
	rgoto	MBFM_BANK_StoreW_End

MBFM_BANK_StoreW_Err
	;; here we could add error processing
MBFM_BANK_StoreW_End
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Writes a single byte to a wavetable
;;  IN: offset in EEADR
;;      byte which should be stored in WREG
;;      Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;; --------------------------------------------------------------------------
MBFM_BANK_WriteWByte
	movwf	TABLAT				; temporary save byte in TABLAT

	rcall	MBFM_BANK_GetWAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	;; EEPROM not supported for wavetables
	BRA_IFCLR PRODH, 0, ACCESS, MBFM_BANK_WriteWByte_End

	movf	EEADR, W			; add offset
	andlw	0x7f				; (mask out wavetable bit)
	addwf	MIOS_PARAMETER1, F
	movf	TABLAT, W
	call	MIOS_BANKSTICK_Write		; write single byte to BankStick
	;; 	rgoto	MBFM_BANK_WriteWByte_End

MBFM_BANK_WriteWByte_End
	incf	EEADR, F			; increment EEPROM address
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Returns a single byte from a wavetable
;;  IN: offset in EEADR
;;      Patch Number in MBFM_PATCH
;;      Bank Number in MBFM_BANK
;; --------------------------------------------------------------------------
MBFM_BANK_ReadWByte
	rcall	MBFM_BANK_GetWAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	;; EEPROM not supported for wavetables
	btfss	PRODH, 0
	retlw 0x00

	;; and continue at ReadPByte
	rgoto	MBFM_BANK_ReadWByte_Cont


;; --------------------------------------------------------------------------
;;  If BankStick available:
;;     -> the function sets the BankStick address to offset #0 of a WT entry
;;     -> PRODH[0] is set
;;  else
;;     -> EEPROM address invalid, since wavetables cannot be stored in EEPROM
;;     -> PRODH[0] is cleared
;;  IN: patch number in MBFM_PATCH
;;      BankStick number in MBFM_BANK
;;  OUT: address in MIOS_PARAMETER[12] or EEADR
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;;       PRODH[0] == 1 -> BankStick, else internal EEPROM
;; --------------------------------------------------------------------------
MBFM_BANK_GetWAddress
	;; store in internal EEPROM if bankstick is not available
	movf	MBFM_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	bz	MBFM_BANK_GetWAddressInternal

MBFM_BANK_GetWAddressExternal
 	movf	MBFM_PATCH, W			; patch * 0x80
	mullw	0x80
	movlw	0x00				; add 0x4000
	addwf	PRODL, W
	movwf	MIOS_PARAMETER1
	movlw	0x40
	addwfc	PRODH, W
	movwf	MIOS_PARAMETER2

	bsf	PRODH, 0			; notify that BankStick should be used
	
	movf	MBFM_BANK, W			; select bankstick
	andlw	0x07
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet

MBFM_BANK_GetWAddressInternal
	;; storing wavetables in EEPROM not supported!

	bcf	PRODH, 0			; notify that internal EEPROM should be used
	return


; ==========================================================================
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Stores a drumset from RAM in BankStick
;;  IN: Drumset Number in MBFM_DRUMSET
;;      Bank Number in MBFM_DBANK
;; --------------------------------------------------------------------------
MBFM_BANK_StoreD
	rcall	MBFM_DRUM_GetBase		; get base address of drum registers

	rcall	MBFM_BANK_GetDAddress		; sets the address for BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	clrf	PRODL				; write 0x40 bytes (using PRODL as loop counter)
MBFM_BANK_StoreDLoop
	;; write zero if address > MBFM_DR_LAST_BANKSTICK_ADDR
	movlw	MBFM_DR_LAST_BANKSTICK_ADDR
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_BANK_StoreDLoopNormal
MBFM_BANK_StoreDLoopZero
	movlw	0x00
	rgoto	MBFM_BANK_StoreDLoopZeroCont
MBFM_BANK_StoreDLoopNormal
	movf	POSTINC1, W
MBFM_BANK_StoreDLoopZeroCont
	CALL_IFSET PRODH, 0, ACCESS, MIOS_BANKSTICK_Write
	CALL_IFCLR PRODH, 0, ACCESS, MIOS_EEPROM_Write
	;; 	bnz	MBFM_BANK_StoreD_Err		; write error?
	incf	PRODL, F
	movlw	0x40-1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_BANK_StoreDLoop
	rgoto	MBFM_BANK_StoreD_End

MBFM_BANK_StoreD_Err
	;; here we could add error processing
MBFM_BANK_StoreD_End
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Loads a drumset from EEPROM or BankStick into RAM
;;  IN: Drumset Number in MBFM_DRUMSET
;;      Bank Number in MBFM_DBANK
;; --------------------------------------------------------------------------
MBFM_BANK_LoadD
	rcall	MBFM_DRUM_GetBase		; get base address of drum registers

	rcall	MBFM_BANK_GetDAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	;; read a page of 0x40 bytes
	CALL_IFSET PRODH, 0, ACCESS, MIOS_BANKSTICK_ReadPage
	CALL_IFCLR PRODH, 0, ACCESS, MIOS_EEPROM_ReadPage

MBFM_BANK_LoadDEnd
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  If BankStick available:
;;     -> the function sets the BankStick address to offset #0 of a drumset entry
;;     -> PRODH[0] is set
;;  else
;;     -> the function sets the EEPROM address to offset #0 of a drumset entry
;;     -> PRODH[0] is cleared
;;  IN: patch number in MBFM_DRUMSET
;;      BankStick number in MBFM_DBANK
;;  OUT: address in MIOS_PARAMETER[12] or EEADR
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;;       PRODH[0] == 1 -> BankStick, else internal EEPROM
;; --------------------------------------------------------------------------
MBFM_BANK_GetDAddress
	;; store in internal EEPROM if bankstick is not available or patch is 0
	movf	MBFM_DBANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	bz	MBFM_BANK_GetDAddressInternal

MBFM_BANK_GetDAddressExternal
 	movf	MBFM_DRUMSET, W			; drumset * 0x40
	mullw	0x40
	movlw	0x00				; add 0x0c00
	addwf	PRODL, W
	movwf	MIOS_PARAMETER1
	movlw	0x0c
	addwfc	PRODH, W
	movwf	MIOS_PARAMETER2

	bsf	PRODH, 0			; notify that BankStick should be used
	
	movf	MBFM_DBANK, W			; select bankstick
	andlw	0x07
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet

MBFM_BANK_GetDAddressInternal
	movlw	MBFM_PRESET_DRUMSET & 0xff	; set EEPROM address
	movwf	EEADR

	bcf	PRODH, 0			; notify that internal EEPROM should be used
	return


; ==========================================================================
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Stores an ensemble from RAM in BankStick
;;  IN: ensemble Number in MBFM_ENSEMBLE
;;      Bank Number in MBFM_EBANK
;; --------------------------------------------------------------------------
MBFM_BANK_StoreE
	rcall	MBFM_BANK_GetEAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED	; outer loop counter
MBFM_BANK_StoreELoopO
	movff	PRODH, TABLAT			; save PRODH (will be overwritten by MBFM_INSTR_GetBase)
	call	MBFM_INSTR_GetBase		; get base address of instrument
	movff	TABLAT, PRODH			; restore PRODH

	clrf	PRODL				; inner loop counter (write 0x10 bytes per instrument)
MBFM_BANK_StoreELoopI
	;; zero value if address > MBFM_Ix_ENSEMBLE_LAST
	movlw	MBFM_Ix_ENSEMBLE_END - MBFM_Ix_ENSEMBLE_BEGIN + 1
	cpfslt	PRODL, ACCESS
	rgoto MBFM_BANK_StoreELoopI_Zero
MBFM_BANK_StoreELoopI_Normal
	;; get value of register
	movf	PRODL, W
	addlw	MBFM_Ix_ENSEMBLE_BEGIN
	movf	PLUSW1, W
	rgoto	MBFM_BANK_StoreELoopI_Cont
MBFM_BANK_StoreELoopI_Zero
	movlw	0x00
MBFM_BANK_StoreELoopI_Cont

	;; write value
	CALL_IFSET PRODH, 0, ACCESS, MIOS_BANKSTICK_Write
	CALL_IFCLR PRODH, 0, ACCESS, MIOS_EEPROM_Write

MBFM_BANK_StoreELoopI_Next
	incf	PRODL, F
	movlw	0x10-1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_BANK_StoreELoopI

	SET_BSR	MBFM_BASE
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM-1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_BANK_StoreELoopO

MBFM_BANK_StoreE_End
	;; 	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Loads an ensemble from EEPROM or BankStick into RAM
;;  IN: Ensemble Number in MBFM_ENSEMBLE
;;      Bank Number in MBFM_EBANK
;; --------------------------------------------------------------------------
MBFM_BANK_LoadE
	rcall	MBFM_BANK_GetEAddress		; sets the address for EEPROM or BankStick
	;; returns PRODH[0] to notify if BankStick or EEPROM should be used

	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED	; outer loop counter
MBFM_BANK_LoadELoopO
	movff	PRODH, TABLAT			; save PRODH (will be overwritten by MBFM_INSTR_GetBase)
	call	MBFM_INSTR_GetBase		; get base address of instrument
	movff	TABLAT, PRODH			; restore PRODH

	clrf	PRODL				; inner loop counter (write 0x10 bytes per instrument)
MBFM_BANK_LoadELoopI
	CALL_IFSET PRODH, 0, ACCESS, MIOS_BANKSTICK_Read
	CALL_IFCLR PRODH, 0, ACCESS, MIOS_EEPROM_Read
	movwf	TABLAT

	;; skip if address > MBFM_Ix_ENSEMBLE_LAST
	movlw	MBFM_Ix_ENSEMBLE_END - MBFM_Ix_ENSEMBLE_BEGIN + 1
	cpfslt	PRODL, ACCESS
	rgoto MBFM_BANK_LoadELoopI_Next

	;; write to ensemble register
	movf	PRODL, W
	addlw	MBFM_Ix_ENSEMBLE_BEGIN
	movff	TABLAT, PLUSW1

MBFM_BANK_LoadELoopI_Next
	incf	PRODL, F
	movlw	0x10-1
	cpfsgt	PRODL, ACCESS
	rgoto MBFM_BANK_LoadELoopI

	SET_BSR	MBFM_BASE
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM-1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_BANK_LoadELoopO

MBFM_BANK_LoadE_End
	SET_BSR	MBFM_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  If BankStick available:
;;     -> the function sets the BankStick address to offset #0 of an ensemble entry
;;     -> PRODH[0] is set
;;  else
;;     -> the function sets the EEPROM address to offset #0 of an ensemble entry
;;     -> PRODH[0] is cleared
;;  IN: patch number in MBFM_ENSEMBLE
;;      BankStick number in MBFM_EBANK
;;  OUT: address in MIOS_PARAMETER[12] or EEADR
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;;       PRODH[0] == 1 -> BankStick, else internal EEPROM
;; --------------------------------------------------------------------------
MBFM_BANK_GetEAddress
	;; store in internal EEPROM if bankstick is not available or patch is 0
	movf	MBFM_EBANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	bz	MBFM_BANK_GetEAddressInternal

MBFM_BANK_GetEAddressExternal
 	movf	MBFM_ENSEMBLE, W		; drumset * 0x40
	mullw	0x40
	movlw	0x00				; add 0x0400
	addwf	PRODL, W
	movwf	MIOS_PARAMETER1
	movlw	0x04
	addwfc	PRODH, W
	movwf	MIOS_PARAMETER2

	bsf	PRODH, 0			; notify that BankStick should be used
	
	movf	MBFM_EBANK, W			; select bankstick
	andlw	0x07
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet

MBFM_BANK_GetEAddressInternal
	movlw	MBFM_PRESET_ENSEMBLE & 0xff	; set EEPROM address
	movwf	EEADR

	bcf	PRODH, 0			; notify that internal EEPROM should be used
	return


; ==========================================================================
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Stores the AOUT mapping into the first BankStick (if available)
;;  IN: -
;; --------------------------------------------------------------------------
MBFM_BANK_StoreA
	btfss	MBFM_BANKSTICK_STATUS, 0
	return

	movlw	0x00	; (disable verify for faster write accesses)
	call	MIOS_BANKSTICK_CtrlSet

	movlw	0x10		; stored from 0x10-0x1f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	lfsr	FSR1, MBFM_AOUT_BASE
MBFM_BANK_StoreALoop
	movf	POSTINC1, W
	call	MIOS_BANKSTICK_Write
	movf	POSTINC1, W
	call	MIOS_BANKSTICK_Write
	
	movf	MIOS_PARAMETER1, W	; loop 16 times
	andlw	0x0f
	bnz	MBFM_BANK_StoreALoop

	return

	
;; --------------------------------------------------------------------------
;;  Loads the AOUT mapping from the first BankStick (if available)
;;  IN: -
;; --------------------------------------------------------------------------
MBFM_BANK_LoadA
	btfss	MBFM_BANKSTICK_STATUS, 0
	return

	movlw	0x00	; (disable verify for faster write accesses)
	call	MIOS_BANKSTICK_CtrlSet

	movlw	0x10		; stored from 0x10-0x1f
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	lfsr	FSR1, MBFM_AOUT_BASE
MBFM_BANK_LoadALoop
	call	MIOS_BANKSTICK_Read
	movwf	POSTINC1
	call	MIOS_BANKSTICK_Read
	movwf	POSTINC1
	
	movf	MIOS_PARAMETER1, W	; loop 16 times
	andlw	0x0f
	bnz	MBFM_BANK_LoadALoop

	return

	
; ==========================================================================
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear the appr. flag in
;;  MBFM_BANKSTICK_STATUS if BankStick not available
;; --------------------------------------------------------------------------
MBFM_BANK_CheckStick
	;; increment check counter, wrap at 8
	incf	MBFM_BANKSTICK_CHK_CTR, W
	andlw	0x07
	movwf	MBFM_BANKSTICK_CHK_CTR

	;; select BankStick
	call	MIOS_BANKSTICK_CtrlSet

	;; try to read from BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	;; this sets the MIOS_BOX_STAT_BS_AVAILABLE flag

	;; save old BankStick status in TMP1
	movff	MBFM_BANKSTICK_STATUS, TMP1

	;; modify the available flag of current bankstick
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MBFM_BANK_CheckStick_AccPassed
MBFM_BANK_CheckStick_AccFailed
	movf	MBFM_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitANDMask
	andwf	MBFM_BANKSTICK_STATUS, F
	rgoto	MBFM_BANK_CheckStick_AccCont
MBFM_BANK_CheckStick_AccPassed
	movf	MBFM_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitORMask
	iorwf	MBFM_BANKSTICK_STATUS, F
MBFM_BANK_CheckStick_AccCont

	;; check if flag has been changed
	movf	MBFM_BANKSTICK_STATUS, W
	xorwf	TMP1, W
	bz	MBFM_BANK_CheckStick_End		; no change

	;; flag changed - branch depending on available status
	movf	MBFM_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	bnz	MBFM_BANK_CheckStick_Ext

	;; --> Internal Patch
MBFM_BANK_CheckStick_Int
MBFM_BANK_CheckStick_IntReload
	;; 	TABLE_ADDR TEXT_INTBANK_0		; print message
	;; 	call	MIOS_LCD_PrintMessage
	;; 	call	MIOS_LCD_PrintMessage

	;; reload patches
	rgoto	MBFM_BANK_CheckStick_ReloadPatch

	;; --> External Patch
MBFM_BANK_CheckStick_Ext
	;; 	TABLE_ADDR TEXT_EXTBANK_0		; print message
	;; 	call	MIOS_LCD_PrintMessage
	;; 	call	MIOS_LCD_PrintMessage

	;; reload patches
	rcall	MBFM_BANK_CheckStick_ReloadPatch

MBFM_BANK_CheckStrick_Ext_NoMsg
	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	movlw	50				; wait some ms to get a stable status
	call	MIOS_Delay			

	clrf	MIOS_PARAMETER1			; read first byte from BankStick
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MAGIC0		; branch to unformatted message if number not equal
	bnz	MBFM_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MAGIC1		; branch to unformatted message if number not equal
	bz	MBFM_BANK_CheckStick_ReloadPatch
MBFM_BANK_CheckStick_Unformatted

	movff	MBFM_BANKSTICK_CHK_CTR, MBFM_BANK
	rcall	MBFM_BANK_FormatStick		; --> format bankstick

MBFM_BANK_CheckStick_ReloadPatch
	;; reload ensemble and drumset
	call	MBFM_ENS_Init
	;; if first bankstick has been detected: reload AOUT mapping
	movf	MBFM_BANKSTICK_CHK_CTR, W
	skpnz
	rcall	MBFM_BANK_LoadA
	
MBFM_BANK_CheckStick_End
	return

;; --------------------------------------------------------------------------
;;  Format Stick: copy the internal patch into the currently selected BankStick Bank
;; --------------------------------------------------------------------------
MBFM_BANK_FormatStick
	;; print format message
	TABLE_ADDR TEXT_FORMATBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print bank number
	movlw	0x44
	call	MIOS_LCD_CursorSet
	movf	MBFM_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	
	;; ------------------------------------------------------------------
	;; copy internal Patch to Bank (128 times)
	movlw	MBFM_PRESET_PATCH & 0xff	; EEPROM source address
	movwf	TMP1
	movlw	0x10				; BankStick destination high-byte
	movwf	TMP2
	movlw	0x60				; patch size
	movwf	TMP3
	movlw	128				; number of patches
	movwf	TMP4
	movlw	'P'				; type identifier
	rcall	MBFM_BANK_FormatStick_CopyIt

	;; ------------------------------------------------------------------
	;; copy internal Drumset to Bank (16 times)
	movlw	MBFM_PRESET_DRUMSET & 0xff	; EEPROM source address
	movwf	TMP1
	movlw	0x0c				; BankStick destination high-byte
	movwf	TMP2
	movlw	0x40				; patch size
	movwf	TMP3
	movlw	16				; number of patches
	movwf	TMP4
	movlw	'D'				; type identifier
	rcall	MBFM_BANK_FormatStick_CopyIt

	;; ------------------------------------------------------------------
	;; copy internal ensemble to Bank (32 times)
	movlw	MBFM_PRESET_ENSEMBLE & 0xff	; EEPROM source address
	movwf	TMP1
	movlw	0x04				; BankStick destination high-byte
	movwf	TMP2
	movlw	0x40				; patch size
	movwf	TMP3
	movlw	32				; number of patches
	movwf	TMP4
	movlw	'E'				; type identifier
	rcall	MBFM_BANK_FormatStick_CopyIt

	;; ------------------------------------------------------------------
	;; write magic bytes to address 0x0000
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	BANKSTICK_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MAGIC1
	call	MIOS_BANKSTICK_Write

	;; write initial AOUT mapping to address 0x0010-0x001f
	goto	MBFM_BANK_StoreA

;; --------------------------------------------------------------------------

	;; IN: start address in EEPROM in TMP1
	;;     start address in BANKSTICK in TMP2
	;;     patch size in TMP3
	;;     number of patches in TMP4
	;;     type identifier ('P', 'E', 'D') in WREG
MBFM_BANK_FormatStick_CopyIt
	movwf	TMP5		; print identifier
	movlw	0x42
	call	MIOS_LCD_CursorSet
	movf	TMP5, W
	call	MIOS_LCD_PrintChar

	clrf	MBFM_PATCH	; begin with patch 0
MBFM_BANK_FormatStick_CopyLoopO
	clrwdt

	movlw	0x45		; print patch number
	call	MIOS_LCD_CursorSet
	incf	MBFM_PATCH, W
	call	MIOS_LCD_PrintBCD3

	;; calculate address to BankStick
	movf	MBFM_PATCH, W
	mulwf	TMP3, ACCESS
	movff	PRODL, MIOS_PARAMETER1
	movf	PRODH, W
	addwf	TMP2, W
	movwf	MIOS_PARAMETER2

	;; copy loop
	movff	TMP1, EEADR
	movff	TMP3, TMP5
MBFM_BANK_FormatStick_CopyLoopI
	call	MIOS_EEPROM_Read
	call	MIOS_BANKSTICK_Write
	decfsz	TMP5, F
	rgoto	MBFM_BANK_FormatStick_CopyLoopI

	;; loop until last patch reached
	incf	MBFM_PATCH, F
	decf	TMP4, W
	cpfsgt	MBFM_PATCH, ACCESS
	rgoto MBFM_BANK_FormatStick_CopyLoopO

	return


;; --------------------------------------------------------------------------

TEXT_FORMATBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATBANK_1 STRING 16, 0x40, "* P x  0        "

