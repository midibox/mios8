; $Id$
;
; MIOS Service Routines for the DIN chain
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_DIN_NotifyToggle
;;  C_DECLARATION: void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value)
;;  DESCRIPTION: This function is called by MIOS when an button has been 
;;  toggled
;;  IN: Button number in WREG and MIOS_PARAMETER1
;;      Button value MIOS_PARAMETER2:
;;        - 1 if button has been released (=5V)
;;        - 0 if button has been pressed (=0V)
;;  C_IN: Button number in <pin>
;;      Button value <pin_value>:
;;        - 1 if button has been released (=5V)
;;        - 0 if button has been pressed (=0V)
;;  OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DIN_SRGet
;;  C_DECLARATION: unsigned char MIOS_DIN_SRGet(unsigned char sr)
;;  DESCRIPTION: returns value of DIN shift register
;;  IN:	  number of shift register in WREG
;;  C_IN: number of shift register in <sr>
;;  OUT:  value of shift register in MIOS_PARAMETER1 and WREG,
;;        pointer to shift register in FSR1
;;  C_OUT: value of shift register
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_DIN_SRGet
	andlw	0x0f
	lfsr	FSR1, MIOS_SR_DIN_0
	addwf	FSR1L, F
	movf	INDF1, W
	movwf	MIOS_PARAMETER1
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DIN_PinGet
;;  C_DECLARATION: unsigned char MIOS_DIN_PinGet(unsigned char pin)
;;  DESCRIPTION: returns value from a DIN Pin
;;  IN:	  Pin number in WREG
;;  C_IN: Pin number in <pin>
;;  OUT:  1 if pin is +5V, 0 if pin is 0V in MIOS_PARAMETER and WREG
;;  C_OUT: 1 if pin is +5V, 0 if pin is 0V
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DIN_PinGet
	andlw	0x7f
	lfsr	FSR1, MIOS_SR_DIN_0
	rgoto	MIOS_SRIO_Get
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DIN_PinAutoRepeatEnable
;;  C_DECLARATION: void MIOS_DIN_PinAutoRepeatEnable(unsigned char pin)
;;  DESCRIPTION: enables the auto-repeat feature for the appr. pin
;;  IN:	  number of pin in WREG
;;  C_IN:  number of pin in <pin>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_DIN_PinAutoRepeatEnable
	andlw	0x7f
	lfsr	FSR1, MIOS_DIN_REG0_00		; calc pointer to DIN register 00
	addwf	FSR1L, F

	bsf	INDF1, 7			; enable/disable handler
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_AUTOREPEAT; enable the handler
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DIN_PinAutoRepeatDisable
;;  C_DECLARATION: void MIOS_DIN_PinAutoRepeatDisable(unsigned char pin)
;;  DESCRIPTION: disables the auto-repeat feature for the appr. pin
;;  IN:	  number of pin in WREG
;;  C_IN: number of pin in <pin>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_DIN_PinAutoRepeatDisable
	andlw	0x7f
	lfsr	FSR1, MIOS_DIN_REG0_00		; calc pointer to DIN register 00
	addwf	FSR1L, F

	bcf	INDF1, 7			; enable/disable handler
	;; ToDo: here we could disable the autorepeat-handler if this feature
	;; has been cleared for all buttons. However, it's difficult since
	;; also the encoder table has to be taken into account
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_DIN_PinAutoRepeatGet
;;  C_DECLARATION: unsigned char MIOS_DIN_PinAutoRepeatGet(unsigned char pin)
;;  DESCRIPTION: returns != 0 if auto-repeat has been enabled for the appr. pin
;;  IN:	  number of pin in WREG
;;  C_IN:  number of pin in <pin>
;;  OUT:  WREG != 0 if auto-repeat has been enabled for this pin
;;        WREG == 0 if auto-repeat has been disabled for this pin
;;  C_OUT:  != 0 if auto-repeat has been enabled for this pin
;;        0 if auto-repeat has been disabled for this pin
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_DIN_PinAutoRepeatGet
	andlw	0x0f
	lfsr	FSR1, MIOS_SR_DIN_0
	addwf	FSR1L, F
	movlw	0x00
	btfsc	INDF1, 7
	movlw	0x01
	movwf	MIOS_PARAMETER1
	return
	
;; --------------------------------------------------------------------------
;;  INTERNAL
;;  DESCRIPTION: returns value from two DIN Neighbour Pins
;;  IN:	  number of first Pin in WREG
;;  OUT:  0, 1, 2, 3
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DIN_2PinsGet
	andlw	0x7f
	lfsr	FSR1, MIOS_SR_DIN_0
	rgoto	MIOS_SRIO_Get2
	
;; --------------------------------------------------------------------------
;;  INTERNAL
;;  MIOS SRIO DIN CHANGED Set00: zeroes two neighboured DIN changed flags
;;  IN:	  number of first Pin in WREG
;;  OUT:  -
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_DIN_ChangedSet00
	andlw	0x7e
	lfsr	FSR1, MIOS_SR_DIN_CHANGED_0
	rgoto	MIOS_SRIO_Set00
	

;; --------------------------------------------------------------------------
;;  MIOS DIN AutoRepeat Tick
;;  retriggers a pin if enabled
;; --------------------------------------------------------------------------
MIOS_DIN_AutoRepeat_Tick
	btfss	MIOS_BOX_STAT, MIOS_BOX_STAT_AUTOREPEAT
	return

	SET_BSR	MIOS_DIN_AUTO_CTR
	decfsz	MIOS_DIN_AUTO_CTR, F, BANKED
	return

	movlw	0x20
	movwf	MIOS_DIN_AUTO_CTR, BANKED

	movff	FSR0L, IRQ_TMP1			; store FSR0 in temp. register
	movff	FSR0H, IRQ_TMP2
	movff	FSR1L, MIOS_IRQ_FSR1L		; store FSR1 in temp. register
	movff	FSR1H, MIOS_IRQ_FSR1H

	lfsr	FSR0, MIOS_DIN_REG0_00
	lfsr	FSR1, MIOS_DIN_REG1_00
	lfsr	FSR2, MIOS_SR_DIN_0

MIOS_DIN_AutoRepeat_TickLoop

MIOS_DIN_AUTOREPEAT_TICK MACRO bit
	LOCAL	AR_Disabled
	LOCAL	AR_Next
	LOCAL	AR_Depressed

	BRA_IFCLR POSTINC0, 7, ACCESS, AR_Disabled
	BRA_IFSET INDF2, bit, ACCESS, AR_Depressed
	decfsz	INDF1, F
	rgoto	AR_Next
	incf	INDF1, F
	movlw	(MIOS_SR_DIN_CHANGED_0-MIOS_SR_DIN_0) & 0xff
	addwf	FSR2L, F
	bsf	INDF2, bit
	movlw	(MIOS_SR_DIN_0-MIOS_SR_DIN_CHANGED_0) & 0xff
	addwf	FSR2L, F
	rgoto	AR_Next
AR_Depressed
	movlw	0x10
	movwf	INDF1
	;; 	rgoto	AR_Next
AR_Next
AR_Disabled
	movf	POSTINC1, W	; (inc)
	ENDM

	MIOS_DIN_AUTOREPEAT_TICK 0
	MIOS_DIN_AUTOREPEAT_TICK 1
	MIOS_DIN_AUTOREPEAT_TICK 2
	MIOS_DIN_AUTOREPEAT_TICK 3
	MIOS_DIN_AUTOREPEAT_TICK 4
	MIOS_DIN_AUTOREPEAT_TICK 5
	MIOS_DIN_AUTOREPEAT_TICK 6
	MIOS_DIN_AUTOREPEAT_TICK 7
	movf	POSTINC2, W	; (inc)

MIOS_DIN_AutoRepeat_Tick_Next
	BRA_IFCLR FSR0L, 7, ACCESS, MIOS_DIN_AutoRepeat_TickLoop

	movff	MIOS_IRQ_FSR1L, FSR1L		; restore FSR1 from temp. register
	movff	MIOS_IRQ_FSR1H, FSR1H
	movff	IRQ_TMP1, FSR0L			; restore FSR0 from temp. register
	movff	IRQ_TMP2, FSR0H
	return


;; --------------------------------------------------------------------------
;;  MIOS DIN Handler
;;  check DIN_CHANGED flags, call USER_DIN_NotifyToggle when status of DIN
;;  pins have been changed
;; --------------------------------------------------------------------------
MIOS_DIN_Handler
	SET_BSR	MIOS_TMP_CTR
	clrf	MIOS_TMP_CTR, BANKED
MIOS_DIN_Handler_Loop
	lfsr	FSR0, MIOS_SR_DIN_CHANGED_0
	movf	MIOS_TMP_CTR, W, BANKED
	addwf	FSR0L, F
	movf	INDF0, W
	bz	MIOS_DIN_Handler_Loop_Next
	RCALL_IFSET INDF0, 0, ACCESS, MIOS_DIN_NotifyToggle_Pin0	; fastest solution...
	RCALL_IFSET INDF0, 1, ACCESS, MIOS_DIN_NotifyToggle_Pin1
	RCALL_IFSET INDF0, 2, ACCESS, MIOS_DIN_NotifyToggle_Pin2
	RCALL_IFSET INDF0, 3, ACCESS, MIOS_DIN_NotifyToggle_Pin3
	RCALL_IFSET INDF0, 4, ACCESS, MIOS_DIN_NotifyToggle_Pin4
	RCALL_IFSET INDF0, 5, ACCESS, MIOS_DIN_NotifyToggle_Pin5
	RCALL_IFSET INDF0, 6, ACCESS, MIOS_DIN_NotifyToggle_Pin6
	RCALL_IFSET INDF0, 7, ACCESS, MIOS_DIN_NotifyToggle_Pin7
MIOS_DIN_Handler_Loop_Next
	incf	MIOS_TMP_CTR, F, BANKED
	BRA_IFCLR MIOS_TMP_CTR, 4, BANKED, MIOS_DIN_Handler_Loop

	return

	;; ---

MIOS_DIN_NotifyToggle_Pin0
	bcf	INDF0, 0			; clear "changed" flag
	movlw	0x00 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin1
	bcf	INDF0, 1			; clear "changed" flag
	movlw	0x01 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin2
	bcf	INDF0, 2			; clear "changed" flag
	movlw	0x02 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin3
	bcf	INDF0, 3			; clear "changed" flag
	movlw	0x03 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin4
	bcf	INDF0, 4			; clear "changed" flag
	movlw	0x04 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin5
	bcf	INDF0, 5			; clear "changed" flag
	movlw	0x05 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin6
	bcf	INDF0, 6			; clear "changed" flag
	movlw	0x06 << 1
	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin7
	bcf	INDF0, 7			; clear "changed" flag
	movlw	0x07 << 1
	;; 	rgoto	MIOS_DIN_NotifyToggle_Pin_Cont
MIOS_DIN_NotifyToggle_Pin_Cont
	movwf	FSR0H				; use FSR0H as temp. register
	swapf	FSR0L, W			; calc DIN number
	andlw	0xf0
	iorwf	FSR0H, W
	clrc
	rrf	WREG, W
	movwf	MIOS_PARAMETER3

	rcall	MIOS_DIN_PinGet
	movwf	MIOS_PARAMETER2

	;; call hook
	movf	MIOS_PARAMETER3, W
	movwf	MIOS_PARAMETER1
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_DIN_NotifyToggle

	;; reload debounce counter
	movff	MIOS_SRIO_DEBOUNCE, MIOS_SRIO_DEBOUNCE_CTR
	
	lfsr	FSR0, MIOS_SR_DIN_CHANGED_0	; fix FSR0 if it has been overwritten
	SET_BSR	MIOS_TMP_CTR
	movf	MIOS_TMP_CTR, W, BANKED
	addwf	FSR0L, F
	return
