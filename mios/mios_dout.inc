; $Id$
;
; MIOS Service Routines for the DOUT chain
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DOUT_SRSet
;;  C_DECLARATION: void MIOS_DOUT_SRSet(unsigned char sr, unsigned char sr_value)
;;  DESCRIPTION: sets value of DOUT shift register
;;  IN:	  number of shift register in WREG
;;        value in MIOS_PARAMETER1
;;  C_IN:  number of shift register in <sr>
;;        value in <sr_value>
;;  OUT:  pointer to shift register in FSR1
;;  C_OUT: -
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DOUT_SRSet
	andlw	0x0f
	lfsr	FSR1, MIOS_SR_DOUT_0
	addwf	FSR1L, F
	movff	MIOS_PARAMETER1, INDF1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DOUT_SRGet
;;  C_DECLARATION: unsigned char MIOS_DOUT_SRGet(unsigned char sr)
;;  DESCRIPTION: returns value of DOUT shift register
;;  IN:	  number of shift register in WREG
;;  C_IN:  number of shift register in <sr>
;;  OUT:  value of shift register in WREG and MIOS_PARAMETER1,
;;        pointer to shift register in FSR1
;;  C_OUT: value of shift register
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DOUT_SRGet
	andlw	0x0f
	lfsr	FSR1, MIOS_SR_DOUT_0
	addwf	FSR1L, F
	movf	INDF1, W
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DOUT_PinGet
;;  C_DECLARATION: unsigned char MIOS_DOUT_PinGet(unsigned char pin)
;;  DESCRIPTION: returns value from a DOUT Pin
;;  IN:	  Pin number in MIOS_PARAMETER1 and WREG
;;  C_IN: Pin number in <pin>
;;  OUT:  1 if pin is +5V, 0 if pin is 0V
;;  C_OUT:  1 if pin is +5V, 0 if pin is 0V
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DOUT_PinGet
	andlw	0x7f
	lfsr	FSR1, MIOS_SR_DOUT_0
	rgoto	MIOS_SRIO_Get

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DOUT_PinSet0
;;  C_DECLARATION: void MIOS_DOUT_PinSet0(unsigned char pin)
;;  DESCRIPTION: set pin to 0V
;;  IN:	  Pin number in WREG
;;  C_IN:  Pin number in WREG
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DOUT_PinSet0
	andlw	0x7f
	lfsr	FSR1, MIOS_SR_DOUT_0
	rgoto	MIOS_SRIO_Set0

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DOUT_PinSet1
;;  C_DECLARATION: void MIOS_DOUT_PinSet1(unsigned char pin)
;;  DESCRIPTION: set pin to 5V
;;  IN:	  Pin number in WREG
;;  C_IN: Pin number in <pin>
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DOUT_PinSet1
	andlw	0x7f
	lfsr	FSR1, MIOS_SR_DOUT_0
	rgoto	MIOS_SRIO_Set1

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DOUT_PinSet
;;  C_DECLARATION: void MIOS_DOUT_PinSet(unsigned char pin, unsigned char value)
;;  DESCRIPTION: set pin to 0 or 5V
;;  IN:	  Pin number in WREG, value in MIOS_PARAMETER1
;;  C_IN: Pin number in <pin>, value in <value>
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DOUT_PinSet
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, MIOS_DOUT_PinSet1
	rgoto	MIOS_DOUT_PinSet0
