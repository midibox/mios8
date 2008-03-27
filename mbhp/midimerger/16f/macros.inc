; $Id: macros.inc 178 2008-03-08 12:08:54Z tk $
;
; Macro Definitions
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

GOTO_IFSET	MACRO	reg, bit, label
	btfsc	reg, bit
	goto	label
	ENDM


GOTO_IFCLR	MACRO	reg, bit, label
	btfss	reg, bit
	goto	label
	ENDM

CALL_IFSET	MACRO	reg, bit, label
	btfsc	reg, bit
	call	label
	ENDM


CALL_IFCLR	MACRO	reg, bit, label
	btfss	reg, bit
	call	label
	ENDM

;; ==========================================================================
        
IRQ_DISABLE MACRO
        local   loop
loop:
        bcf     INTCON, GIE
	btfsc	INTCON, GIE
	goto	loop
        ENDM

IRQ_ENABLE MACRO
        bsf     INTCON, GIE
        ENDM

;; ==========================================================================
        
FLASH_ADDR MACRO addr
	SWITCHBANK_0_2
	movlw	LOW(addr)
	movwf	EEADR
	movlw	HIGH(addr)
	movwf	EEADRH
	SWITCHBANK_2_0
	ENDM

FLASH_H	MACRO	addr
	SWITCHBANK_0_2
	movlw	HIGH(addr)
	movwf	EEADRH
	SWITCHBANK_2_0
	ENDM

FLASH_WH MACRO
	SWITCHBANK_0_2
	movwf	EEADRH
	SWITCHBANK_2_0
	ENDM

FLASH_L	MACRO	addr
	SWITCHBANK_0_2
	movlw	LOW(addr)
	movwf	EEADR
	SWITCHBANK_2_0
	ENDM

FLASH_WL MACRO
	SWITCHBANK_0_2
	movwf	EEADR
	SWITCHBANK_2_0
	ENDM

FLASH_ADD_W MACRO
	SWITCHBANK_0_2
	addwf	EEADR, F
	skpnc
	incf	EEADRH, F
	SWITCHBANK_2_0
	ENDM

;; ==========================================================================

SWITCHBANK_0_1 MACRO
        bsf     STATUS, RP0
        ENDM
SWITCHBANK_0_2 MACRO
        bsf     STATUS, RP1
        ENDM
SWITCHBANK_0_3 MACRO
        bsf     STATUS, RP0
        bsf     STATUS, RP1
        ENDM

SWITCHBANK_1_0 MACRO
        bcf     STATUS, RP0
        ENDM
SWITCHBANK_1_2 MACRO
        bcf     STATUS, RP0
        bsf     STATUS, RP1
        ENDM
SWITCHBANK_1_3 MACRO
        bsf     STATUS, RP1
        ENDM

SWITCHBANK_2_0 MACRO
        bcf     STATUS, RP1
        ENDM
SWITCHBANK_2_1 MACRO
        bsf     STATUS, RP0
        bcf     STATUS, RP1
        ENDM
SWITCHBANK_2_3 MACRO
        bsf     STATUS, RP0
        ENDM

SWITCHBANK_3_0 MACRO
        bcf     STATUS, RP0
        bcf     STATUS, RP1
        ENDM
SWITCHBANK_3_1 MACRO
        bcf     STATUS, RP1
        ENDM
SWITCHBANK_3_2 MACRO
        bcf     STATUS, RP0
        ENDM

;; ==========================================================================

SWITCHPAGE_0 MACRO
	bcf	PCLATH, 3
	ENDM

SWITCHPAGE_1 MACRO
	bsf	PCLATH, 3
	ENDM

SWITCHPAGE_U MACRO
	bsf	PCLATH, 4
	ENDM

SWITCHPAGE_L MACRO
	bcf	PCLATH, 4
	ENDM

;; ==========================================================================

SWITCH_FSR_LOWER MACRO
        bcf     STATUS, IRP
        ENDM

SWITCH_FSR_UPPER MACRO
        bsf     STATUS, IRP
        ENDM

;; ==========================================================================

XGOTO MACRO label
        if HIGH(label) & 0x8
        SWITCHPAGE_1
        else
        SWITCHPAGE_0
        endif
        goto    label
        ENDM

XCALL MACRO label
        LOCAL   testlabel
testlabel:
        if HIGH(label) & 0x8
        SWITCHPAGE_1
        else
        SWITCHPAGE_0
        endif

        call    label

        if HIGH(testlabel) & 0x8
        SWITCHPAGE_1
        else
        SWITCHPAGE_0
        endif

        ENDM

XXCALL MACRO label
	LOCAL	testlabel
testlabel:
	if HIGH(label) & 0x8
	SWITCHPAGE_1
	else
	SWITCHPAGE_0
	endif
	if HIGH(label) & 0x10
	SWITCHPAGE_U
	else
	SWITCHPAGE_L
	endif

	call	label

	if HIGH(testlabel) & 0x8
	SWITCHPAGE_1
	else
	SWITCHPAGE_0
	endif
	if HIGH(testlabel) & 0x10
	SWITCHPAGE_U
	else
	SWITCHPAGE_L
	endif
	ENDM

XXGOTO MACRO label
	LOCAL	testlabel
testlabel:
	if HIGH(label) & 0x8
	SWITCHPAGE_1
	else
	SWITCHPAGE_0
	endif
	if HIGH(label) & 0x10
	SWITCHPAGE_U
	else
	SWITCHPAGE_L
	endif

	goto	label

	ENDM

