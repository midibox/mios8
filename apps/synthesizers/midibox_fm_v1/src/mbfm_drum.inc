; $Id$
;
; MIDIbox FM
; Drum Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns the base address of the drum records
;;  Output:
;;     o base address in FSR1
;; --------------------------------------------------------------------------
MBFM_DRUM_GetBase
	lfsr	FSR1, MBFM_DR_BASE
	return

;; --------------------------------------------------------------------------
;;  This function sends a drum parameter to OPL3 and saves it into RAM
;;  Input:
;;     o parameter value in WREG
;;     o parameter index in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MBFM_DRUM_SendPar
	SET_BSR	MBFM_BASE

	;; temporary save value in MIOS_PARAMETER3
	movwf	MIOS_PARAMETER3

	;; get base address to drum record -> FSR1
	rcall	MBFM_DRUM_GetBase
	
	;; add parameter index and store value
	movf	MIOS_PARAMETER1, W
	movff	MIOS_PARAMETER3, PLUSW1

	;; (used by MBFM_DRUM_Init:
	;;  register offset in MIOS_PARAMETER1, value in MIOS_PARAMETER3)
MBFM_DRUM_SendParDirectly
	;; get corresponding OPL3 register
	movlw	MBFM_DRUM_OPL3_REG_MAP & 0xff
	addwf	MIOS_PARAMETER1, W
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	MBFM_DRUM_OPL3_REG_MAP >> 8
	addwfc	TBLPTRH, F

	tblrd*+

	;; do nothing if register address is 0x00
	movf	TABLAT, W
	bz	MBFM_DRUM_SendPar_NOP
	
	;; update BD frequency if 0x01 is specified
	addlw	-1
	bz	MBFM_DRUM_SendPar_BD_Frq

	;; update HH frequency if 0x02 is specified
	addlw	-1
	bz	MBFM_DRUM_SendPar_HH_Frq

	;; update TOM frequency if 0x03 is specified
	addlw	-1
	bz	MBFM_DRUM_SendPar_TOM_Frq

	;; else write value into OPL3 register
	movff	TABLAT, MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movf	MIOS_PARAMETER3, W
	goto	MBFM_REG_Write


MBFM_DRUM_SendPar_NOP
	return

MBFM_DRUM_SendPar_BD_Frq
	IRQ_DISABLE		; (since frequency is controlled by Sound Engine)
	movf	MIOS_PARAMETER3, W
	rcall	MBFM_DRUM_SetBDFrq
	rgoto	MBFM_DRUM_SendPar_FNUM_End

MBFM_DRUM_SendPar_HH_Frq
	IRQ_DISABLE		; (since frequency is controlled by Sound Engine)
	movf	MIOS_PARAMETER3, W
	rcall	MBFM_DRUM_SetHHFrq
	rgoto	MBFM_DRUM_SendPar_FNUM_End

MBFM_DRUM_SendPar_TOM_Frq
	IRQ_DISABLE		; (since frequency is controlled by Sound Engine)
	movf	MIOS_PARAMETER3, W
	rcall	MBFM_DRUM_SetTOMFrq
	;; 	rgoto	MBFM_DRUM_SendPar_FNUM_End

MBFM_DRUM_SendPar_FNUM_End
	IRQ_ENABLE
	return


	
;; --------------------------------------------------------------------------
;; This table maps the MBFM_DR* registers to the corresponding OPL3 registers
;; 0x01, 0x02, 0x03 are special tokens to map the frequency values
MBFM_DRUM_OPL3_REG_MAP
	;;      FLAGS_MULT                                   KSL_TL
	db	0x30, 0x33, 0x31, 0x31, 0x34, 0x32, 0x35,    0x50, 0x53, 0x51, 0x51, 0x54, 0x52, 0x55
	;;      AR_DR                                        SL_RR
	db	0x70, 0x73, 0x71, 0x71, 0x74, 0x72, 0x75,    0x90, 0x93, 0x91, 0x91, 0x94, 0x92, 0x95
	;;      WS                                           CH_FB_CNT
	db	0xf0, 0xf3, 0xf1, 0xf1, 0xf4, 0xf2, 0xf5,    0xc6, 0xc7, 0xc8

	;;	FRQ/DECR
	db	0x01, 0x00, 0x02, 0x00, 0x03, 0x00

	;;	SPLIT_LOWER/SPLIT_UPPER
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

	;; rest (fill to 0x3f)
	db	0x00, 0x00
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00

	
;; --------------------------------------------------------------------------
;;  This function initializes the drums instruments
;;  MBFM_DRUM_Init: init from EEPROM
;;  MBFM_DRUM_Init_RAM: init from RAM
;;  IN: drumset in MBFM_DRUMSET
;;      bank in MBFM_DBANK
;; --------------------------------------------------------------------------
MBFM_DRUM_Init_RAM
	;; disable interrupts to suspend sound engine
	IRQ_DISABLE

	rgoto	MBFM_DRUM_Init_Cont
	
MBFM_DRUM_Init
	;; disable interrupts to suspend sound engine
	IRQ_DISABLE
	
	;; load drumset from EEPROM
	call	MBFM_BANK_LoadD

MBFM_DRUM_Init_Cont
	;; clear trigger requests
	SET_BSR	MBFM_BASE
	clrf	MBFM_DRUM_GATES_TRIGGER, BANKED
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_GATES

	;; enable interrupts again
	IRQ_ENABLE

	;; update OPL3 registers
	clrf	TMP1
MBFM_DRUM_Init_Loop
	rcall	MBFM_DRUM_GetBase
	movf	TMP1, W
	movwf	MIOS_PARAMETER1
	movff	PLUSW1, MIOS_PARAMETER3
	rcall	MBFM_DRUM_SendParDirectly

	incf	TMP1, F
	movlw	0x40-1
	cpfsgt	TMP1, ACCESS
	rgoto MBFM_DRUM_Init_Loop

	return


;; --------------------------------------------------------------------------
;;  This function sets the BD frequency
;;  frq in WREG
;; --------------------------------------------------------------------------
MBFM_DRUM_SetBDFrq
	SET_BSR	MBFM_BASE

	rrf	WREG, W
	andlw	0x3f
	movwf	MBFM_DR_BD_FRQH, BANKED
	clrf	MBFM_DR_BD_FRQL, BANKED
	rrf	MBFM_DR_BD_FRQL, F, BANKED

	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_BD_FNUM, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function sets the HH/SD frequency
;;  frq in WREG
;; --------------------------------------------------------------------------
MBFM_DRUM_SetHHFrq
	SET_BSR	MBFM_BASE

	rrf	WREG, W
	andlw	0x3f
	movwf	MBFM_DR_HH_FRQH, BANKED
	movlw	0x20	;; (frequency 0 not always working, >0x20 seems to be ok...)
	movwf	MBFM_DR_HH_FRQL, BANKED
	rrf	MBFM_DR_HH_FRQL, F, BANKED

	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_FNUM, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function sets the TOM/Cymbal frequency
;;  frq in WREG
;; --------------------------------------------------------------------------
MBFM_DRUM_SetTOMFrq
	SET_BSR	MBFM_BASE

	rrf	WREG, W
	andlw	0x3f
	movwf	MBFM_DR_TOM_FRQH, BANKED
	clrf	MBFM_DR_TOM_FRQL, BANKED
	rrf	MBFM_DR_TOM_FRQL, F, BANKED

	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_TOM_FNUM, BANKED
	return

