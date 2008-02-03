; $Id$
;
; MIDIO128
; MIDIO DOUT functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function sets a DOUT pin depending on its polarity
;;  IN: pin number in WREG
;; --------------------------------------------------------------------------
MIDIO_DOUT_Set
	SET_BSR	MIDIO_BASE
	GOTO_IFSET MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED, MIOS_DOUT_PinSet0
	goto	MIOS_DOUT_PinSet1

;; --------------------------------------------------------------------------
;;  This function clears a DOUT pin depending on its polarity
;;  IN: pin number in WREG
;; --------------------------------------------------------------------------
MIDIO_DOUT_Clear
	SET_BSR	MIDIO_BASE
	GOTO_IFSET MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED, MIOS_DOUT_PinSet1
	goto	MIOS_DOUT_PinSet0

;; --------------------------------------------------------------------------
;;  This function returns the current status of the DOUT pin depending on
;;  its polarity
;;  IN: pin number in WREG
;; --------------------------------------------------------------------------
MIDIO_DOUT_Get
	call	MIOS_DOUT_PinGet
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED
	xorlw 0x01
	return

;; --------------------------------------------------------------------------
;;  This function inverts all DOUT registers - used when the polarity has
;;  been changed
;; --------------------------------------------------------------------------
MIDIO_DOUT_Invert
	;; get pointer to first SR entry
	movlw	0x00
	call	MIOS_DOUT_SRGet

	;; pointer now in FSR1
	;; loop 16 times
	clrf	TMP1
MIDIO_DOUT_InvertLoop
	comf	POSTINC1, F
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MIDIO_DOUT_InvertLoop
	
	return


;; --------------------------------------------------------------------------
;;  This function fetches the DIN pins and copies the values into the DOUT
;;  registers if FORWARD_IO function is active.
;; 
;;  It always copies the state into the MIDIO_DIN_VALUES_SRx array
;; 
;;  Has to be requested with MB_STAT, MB_STAT_FETCH_DIN_REQ
;;  Will be called from USER_SR_Service_Finish
;; 
;;  ATTENTION: interrupt service routine --- use IRQ_TMPx instead of TMPx!
;; --------------------------------------------------------------------------
MIDIO_DOUT_FetchDIN
	;; loop 16 times
	clrf	IRQ_TMP1
MIDIO_DOUT_FetchDINLoop
	;; get content of DIN SR
	movf	IRQ_TMP1, W
	call	MIOS_DIN_SRGet
	;; result in WREG and MIOS_PARAMETER1

	;; invert value if INVERSE_DIN set
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DIN, BANKED
	comf	MIOS_PARAMETER1, F
	
	;; copy value to MIDIO_DIN_VALUES_SRx array
	lfsr	FSR1, MIDIO_DIN_VALUES_SR0
	movf	IRQ_TMP1, W
	movff	MIOS_PARAMETER1, PLUSW1
	comf	PLUSW1, F	; (must be low-active)

	;; copy value to DOUT if FORWARD_IO is active
	BRA_IFCLR MIDIO_CFG0, MIDIO_CFG0_FORWARD_IO, BANKED, MIDIO_DOUT_FetchDINLoopNext

	;; invert value again if INVERSE_DOUT set
	SET_BSR	MIDIO_BASE
	btfsc	MIDIO_CFG0, MIDIO_CFG0_INVERSE_DOUT, BANKED
	comf	MIOS_PARAMETER1, F

	;; write value to DOUT SR
	movf	IRQ_TMP1, W
	call	MIOS_DOUT_SRSet

	;; next
MIDIO_DOUT_FetchDINLoopNext
	incf	IRQ_TMP1, F
	BRA_IFCLR IRQ_TMP1, 4, ACCESS, MIDIO_DOUT_FetchDINLoop
	return
