; $Id$
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

	
;; ==========================================================================

IRQ_DISABLE MACRO
        bcf     INTCON, GIE
        ENDM

IRQ_ENABLE MACRO
        bsf     INTCON, GIE
        ENDM

;; ==========================================================================

TABLE_ADDR_FULL MACRO addr
        movlw   LOW(addr)
        movwf   TBLPTRL
        movlw   HIGH(addr)
        movwf   TBLPTRH
        movlw   (((addr) >> 16) & 0xff) ; store upper Byte
        movwf   TBLPTRU
        ENDM
        
TABLE_ADDR MACRO addr
        movlw   LOW(addr)
        movwf   TBLPTRL
        movlw   HIGH(addr)
        movwf   TBLPTRH
        ENDM

TABLE_ADD_W MACRO
	addwf	TBLPTRL, F
	movlw	0x00
	addwfc	TBLPTRH, F
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

