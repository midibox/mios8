; $Id$
;********************************************************************
; From the Microchip AN526 Application Note
;
; Binary To BCD Conversion Routine
;
; This routine converts the 8 bit binary number in the W Register
; to a 2 digit BCD number.
; The least significant digit is returned in location LSD and
; the most significant digit is returned in location MSD.
;
; Performance :
; Program Memory : 10
; Clock Cycles : 81 (worst case when W = 63 Hex )
; ( i.e max Decimal number 99 )
;
; Program: BIN8BCD.ASM
; Revision Date:
; 1-13-97 Compatibility with MPASMWIN 1.40
;
;*******************************************************************
;
LSD	EQU	TMP1
MSD	EQU	TMP2
CONVERT_Bin2BCD 
	clrf	MSD
	movwf	LSD
CONVERT_Bin2BCD_Loop
	movlw	10
	subwf	LSD, W
	bnc	CONVERT_Bin2BCD_End
	movwf	LSD
	incf	MSD, F
	rgoto	CONVERT_Bin2BCD_Loop
CONVERT_Bin2BCD_End
	return
