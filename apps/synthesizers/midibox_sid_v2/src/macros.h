;
; Macro Definitions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

BRA_IFSET	MACRO	reg, bit, reg_a, label
	btfsc	reg, bit, reg_a
	bra	label
	ENDM


BRA_IFCLR	MACRO	reg, bit, reg_a, label
	btfss	reg, bit, reg_a
	bra	label
	ENDM

GOTO_IFSET	MACRO	reg, bit, reg_a, label
	btfsc	reg, bit, reg_a
	goto	label
	ENDM


GOTO_IFCLR	MACRO	reg, bit, reg_a, label
	btfss	reg, bit, reg_a
	goto	label
	ENDM

CALL_IFSET	MACRO	reg, bit, reg_a, label
	btfsc	reg, bit, reg_a
	call	label
	ENDM


CALL_IFCLR	MACRO	reg, bit, reg_a, label
	btfss	reg, bit, reg_a
	call	label
	ENDM

RCALL_IFSET	MACRO	reg, bit, reg_a, label
	btfsc	reg, bit, reg_a
	rcall	label
	ENDM


RCALL_IFCLR	MACRO	reg, bit, reg_a, label
	btfss	reg, bit, reg_a
	rcall	label
	ENDM

;; ==========================================================================

IRQ_DISABLE MACRO
        bcf     INTCON, GIE
        ENDM

IRQ_ENABLE MACRO
        bsf     INTCON, GIE
        ENDM

;; ==========================================================================

TABLE_ADDR_FULL MACRO addr
	movlw   LOW(addr)		; store Lo Byte
        movwf   TBLPTRL
        movlw   HIGH(addr)		; store Hi Byte
        movwf   TBLPTRH
        movlw   UPPER(addr)		; store upper Byte
        movwf   TBLPTRU
        ENDM
        
TABLE_ADDR MACRO addr
        movlw   LOW(addr)		; store Lo Byte
        movwf   TBLPTRL
        movlw   HIGH(addr)		; store Hi Byte
        movwf   TBLPTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
        movlw   UPPER(addr)		; store Upper Byte
        movwf   TBLPTRU
#endif
        ENDM
        
TABLE_H MACRO   addr
        movlw   HIGH(addr)		; store Hi Byte
        movwf   TBLPTRH
        ENDM

TABLE_L MACRO   addr
        movlw   LOW(addr)		; store Lo Byte
        movwf   TBLPTRL
        ENDM

TABLE_ADD_W MACRO
	addwf	TBLPTRL, F
	movlw	0x00
	addwfc	TBLPTRH, F
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movlw	0x00
	addwfc	TBLPTRU, F
#endif
        ENDM

TABLE_ADDR_MUL_W MACRO addr, multiplicator
	mullw	multiplicator
	movlw	LOW(addr)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(addr)
	addwfc	PRODH, W
	movwf	TBLPTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	clrf    TBLPTRU
        movlw   UPPER(addr)
	addwfc	TBLPTRU, F
#endif
	ENDM

;; ==========================================================================

JUMPTABLE_2BYTES MACRO max_value
	addlw	-(max_value)	; ensure that jump index is not greater than (max_value-1)
	skpnc
	movlw	-(max_value)
	addlw	max_value
	call	MIOS_HLP_GetIndex_2bytes
	ENDM

JUMPTABLE_2BYTES_UNSECURE MACRO
	call	MIOS_HLP_GetIndex_2bytes
	ENDM

JUMPTABLE_4BYTES MACRO max_value
	addlw	-(max_value)	; ensure that jump index is not greater than (max_value-1)
	skpnc
	return
	addlw	(max_value)
	call	MIOS_HLP_GetIndex_4bytes
	ENDM

JUMPTABLE_4BYTES_UNSECURE MACRO
	call	MIOS_HLP_GetIndex_4bytes
	ENDM

;; ==========================================================================

TWOBYTE_ENTRY MACRO low_byte, high_byte
        dw      (high_byte << 8) | (low_byte)
        ENDM

;; ==========================================================================

#define skpnz	btfsc	STATUS, Z 
#define skpz	btfss	STATUS, Z 
#define skpndc	btfsc	STATUS, DC 
#define skpdc	btfss	STATUS, DC
#define skpnc	btfsc	STATUS, C
#define skpc	btfss	STATUS, C
#define clrc	bcf	STATUS, C
#define setc	bsf	STATUS, C
#define clrz	bcf	STATUS, Z
#define setz	bsf	STATUS, Z
#define rrf	rrcf	
#define rlf	rlcf

rgoto   MACRO   label
        bra     label
        ENDM

SET_BSR MACRO   reg
        movlb   HIGH(reg)
        ENDM

;; ==========================================================================

FONT_ENTRY MACRO width, height, x0, char_offset
	dw	((height) << 8) | (width), ((char_offset) << 8) | (x0)
	ENDM

;; ==========================================================================

STRING MACRO length, pos, str
	da	((length) << 8) | (pos), str
	ENDM

;; ==========================================================================
