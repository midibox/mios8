; $Id$
;
; MIOS Shift Register IO Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; pins of SRIO shift registers
MIOS_SRIO_LAT_SCLK	EQU	LATD	; Pin D.3
MIOS_SRIO_PIN_SCLK	EQU	3
MIOS_SRIO_LAT_RCLK	EQU	LATD	; Pin D.2
MIOS_SRIO_PIN_RCLK	EQU	2
MIOS_SRIO_PORT_DIN	EQU	PORTD	; Pin D.1
MIOS_SRIO_PIN_DIN	EQU	1
MIOS_SRIO_LAT_DOUT	EQU	LATD	; Pin D.0
MIOS_SRIO_PIN_DOUT	EQU	0

;; Touch Sensor Pin D.4
MIOS_SRIO_LAT_TS	EQU	LATD
MIOS_SRIO_PIN_TS	EQU	4


;; --------------------------------------------------------------------------
;;  MIOS SRIO Tick
;; --------------------------------------------------------------------------
MIOS_SRIO_Tick
	movff	FSR0L, IRQ_TMP1			; store FSR0 in temp. register
	movff	FSR0H, IRQ_TMP2

	SET_BSR	MIOS_IRQ_TMP_CTR

	;; send a short pulse over port J14 of the core module for capacitive touch sensors
	movf	MIOS_SRIO_TS_SENSITIVITY, W, BANKED
	bnz	MIOS_SRIO_TS_Enabled
MIOS_SRIO_TS_Disabled
        bcf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; register button values
	clrf	MIOS_IRQ_TMP_CTR, BANKED		; init loop counter for register upload
	nop
	nop
	nop
        bsf     MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; release register clock
	rgoto	MIOS_SRIO_TS_Cont			; continue behind TS handler

MIOS_SRIO_TS_Enabled
	movwf	MIOS_IRQ_TMP_CTR, BANKED		; use sensitivity as loop counter

  	bsf	MIOS_SRIO_LAT_TS, MIOS_SRIO_PIN_TS	; touch sensor pin = 1
MIOS_SRIO_TS_Loop
	decfsz	MIOS_IRQ_TMP_CTR, F, BANKED
	rgoto	MIOS_SRIO_TS_Loop
        bcf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; register button values
	clrf	MIOS_IRQ_TMP_CTR, BANKED		; init loop counter for register upload
	nop
	nop
	nop
        bsf     MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; release register clock
	bcf	MIOS_SRIO_LAT_TS, MIOS_SRIO_PIN_TS	; touch sensor pin = 0
MIOS_SRIO_TS_Cont

	lfsr	FSR0, MIOS_SR_DIN_CHANGED_0
	lfsr	FSR1, MIOS_SR_DIN_0
	lfsr	FSR2, MIOS_SR_DOUT_0
	decf	MIOS_SRIO_NUMBER, W, BANKED
	addwf	FSR2L, F

	movf	MIOS_SRIO_NUMBER, W, BANKED
	movwf	MIOS_IRQ_TMP_CTR, BANKED

	;; feed the SRs
MIOS_SRIO_Loop
        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 7
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 0
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 7
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 6
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 1
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 6
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 5
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 2
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 5
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 4
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 3
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 4
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 3
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 4
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 3
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 2
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 5
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 2
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 1
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 6
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 1
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

        btfsc   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bsf	IRQ_TMP5, 0
        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	btfss	INDF2, 7
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
        btfss   MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN
	bcf	IRQ_TMP5, 0
        bsf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	nop

	movf	IRQ_TMP5, W
	xorwf	INDF1, W
	iorwf	POSTINC0, F

        bcf     MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK

	movf	IRQ_TMP5, W
	movwf	POSTINC1

	movf	POSTDEC2, W
MIOS_SRIO_Loop_Next
	decfsz	MIOS_IRQ_TMP_CTR, F, BANKED
	rgoto	MIOS_SRIO_Loop

        bcf     MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; latch LED values

	movff	IRQ_TMP1, FSR0L				; restore FSR0 from temp. register
	movff	IRQ_TMP2, FSR0H				; (should be executed here to stretch the RCLK pulse)

        bsf     MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK	; latch LED values
	
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT	; set SR OUT pin to defined value

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_NumberSet
;;  C_DECLARATION: void MIOS_SRIO_NumberSet(unsigned char number_sr)
;;  DESCRIPTION: sets number of available SR registers
;;  If number > 16, value will be forced to 16
;;  IN:	  number of SRs in WREG
;;  C_IN:  number of SRs in <number_sr>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_NumberSet
	SET_BSR	MIOS_SRIO_NUMBER
	movwf	MIOS_SRIO_NUMBER, BANKED
	andlw	0xf0
	skpnz
	return
	movlw	0x10
	movwf	MIOS_SRIO_NUMBER, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_NumberGet
;;  C_DECLARATION: unsigned char MIOS_SRIO_NumberGet(void)
;;  DESCRIPTION: returns number of available SR registers
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  number of SRs in MIOS_PARAMETER1 and WREG
;;  C_OUT: number of SRs
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_NumberGet
	SET_BSR	MIOS_SRIO_NUMBER
	movf	MIOS_SRIO_NUMBER, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_UpdateFrqSet
;;  C_DECLARATION: void MIOS_SRIO_UpdateFrqSet(unsigned char update_frq)
;;  DESCRIPTION: sets the update frequency of SR registers
;;  IN:	  update frequency (unit: milliseconds) in WREG
;;  C_IN: update frequency (unit: milliseconds) in <update_frq>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_UpdateFrqSet
	SET_BSR	MIOS_SRIO_UPDATE_FRQ
	movwf	MIOS_SRIO_UPDATE_FRQ, BANKED
	movf	MIOS_SRIO_UPDATE_FRQ, W, BANKED
	skpnz
	incf	MIOS_SRIO_UPDATE_FRQ, F, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_UpdateFrqGet
;;  C_DECLARATION: unsigned char MIOS_SRIO_UpdateFrqGet(void)
;;  DESCRIPTION: returns the update frequency of SR registers
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  update frequency (unit: milliseconds) in MIOS_PARAMETER1 and WREG
;;  C_OUT: update frequency (unit: milliseconds)
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_UpdateFrqGet
	SET_BSR	MIOS_SRIO_UPDATE_FRQ
	movf	MIOS_SRIO_UPDATE_FRQ, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_TS_SensitivitySet
;;  C_DECLARATION: void MIOS_SRIO_TS_SensitivitySet(unsigned char sensitivity)
;;  DESCRIPTION: sets the touch sensor sensitivity.<BR>
;;  sensitivity == 0x00 disables the TS so that Pin RD.4 (J14 of the core module) 
;;  won't be driven by MIOS anymore and therefore is free for other purposes
;;  IN:	  sensitivity value in WREG
;;  C_IN: sensitivity value in <sensitivity>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_TS_SensitivitySet
	SET_BSR	MIOS_SRIO_TS_SENSITIVITY
	movwf	MIOS_SRIO_TS_SENSITIVITY, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_TS_SensitivityGet
;;  C_DECLARATION: unsigned char MIOS_SRIO_TS_SensitivityGet(void)
;;  DESCRIPTION: returns the touchsensor sensitivity
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  sensitivity value in MIOS_PARAMETER1 and WREG
;;  C_OUT:  sensitivity value
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_TS_SensitivityGet
	SET_BSR	MIOS_SRIO_TS_SENSITIVITY
	movf	MIOS_SRIO_TS_SENSITIVITY, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_DebounceSet
;;  C_DECLARATION: void MIOS_SRIO_DebounceSet(unsigned char debounce_value)
;;  DESCRIPTION: sets the debounce counter reload value for the DIN SR registers
;;  which are not assigned to rotary encoders to debounce low-quality buttons.<BR><BR>
;; 
;;  Debouncing is realized in the following way: on every button movement the
;;  debounce preload value will be loaded into the debounce counter register.
;;  The counter will be decremented on every SRIO update cycle. So long as this
;;  counter isn't zero, button changes will still be recorded, but they won't trigger
;;  the USER_DIN_NotifyToggle function.<BR><BR>
;; 
;;  No (intended) button movement will get lost, but the latency will be
;;  increased. Example: if the update frequency is set to 1 mS, and the 
;;  debounce value to 32, the first button movement will be regognized with
;;  a worst-case latency of 1 mS. Every additional button movement which happens
;;  within 32 mS will be regognized within a worst-case latency of 32 mS.
;;  After the debounce time has passed, the worst-case latency is 1 mS again.<BR><BR>
;;
;;  Note that in MIOS versions below v1.9c, the debounce counter also affected
;;  the rotary encoders and DOUT registers (they where not serviced).<BR>
;;  With MIOS V1.9c and higher, this problem doesn't exist anymore and the
;;  debouncing feature can be used in nearly all applications.<BR>
;;  Only exception: if the application should record pin changes from digital
;;  sensors which are switching very fast, then debouncing should be ommited.
;; 
;;  IN:	  debounce counter reload value in WREG
;;  C_IN: debounce counter reload value in <debounce_value>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_DebounceSet
	SET_BSR	MIOS_SRIO_DEBOUNCE
	movwf	MIOS_SRIO_DEBOUNCE, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_SRIO_DebounceGet
;;  C_DECLARATION: unsigned char MIOS_SRIO_DebounceGet(void)
;;  DESCRIPTION: returns the debounce counter reload value of the DIN SR registers
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  debounce counter reload value in WREG and MIOS_PARAMETER1
;;  C_OUT:  debounce counter reload value
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_DebounceGet
	SET_BSR	MIOS_SRIO_DEBOUNCE
	movf	MIOS_SRIO_DEBOUNCE, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
		
	
;; --------------------------------------------------------------------------
;;  INTERNAL
;;  MIOS SRIO Get: returns value from a DIN/DOUT register
;;  IN:	  Pin number in WREG, register address in FSR0
;;  OUT:  1 if pin is +5V, 0 if pin is 0V in MIOS_PARAMETER1 and WREG
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_Get
	movwf	MIOS_PARAMETER1

	rlf	WREG, F
	swapf	WREG, W
	andlw	0x0f
	addwf	FSR1L, F

	movf	MIOS_PARAMETER1, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	movlw	0x00
	skpz
	movlw	0x01
	andlw	0x01					; for correct status (Z flag set/not set...)
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  INTERNAL
;;  MIOS SRIO Get2: returns value from two DIN/DOUT Neighbour Pins
;;  IN:	  number of first Pin in WREG, register address in FSR
;;  OUT:  0, 1, 2, 3 in MIOS_PARAMETER1 and WREG
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_Get2
	movwf	MIOS_PARAMETER1

	rlf	WREG, F
	swapf	WREG, W
	andlw	0x0f
	addwf	FSR1L, F

	BRA_IFSET MIOS_PARAMETER1, 2, ACCESS, MIOS_SRIO_Get2_Cont_Pin4567
MIOS_SRIO_Get2_Cont_Pin0123
	BRA_IFSET MIOS_PARAMETER1, 1, ACCESS, MIOS_SRIO_Get2_Cont_Pin23
MIOS_SRIO_Get2_Cont_Pin01
	movlw	0x00
	btfsc	INDF1, 0
	iorlw	0x01
	btfsc	INDF1, 1
	iorlw	0x02
	rgoto	MIOS_SRIO_Get2_End

MIOS_SRIO_Get2_Cont_Pin23
	movlw	0x00
	btfsc	INDF1, 2
	iorlw	0x01
	btfsc	INDF1, 3
	iorlw	0x02
	rgoto	MIOS_SRIO_Get2_End

MIOS_SRIO_Get2_Cont_Pin4567
	BRA_IFSET MIOS_PARAMETER1, 1, ACCESS, MIOS_SRIO_Get2_Cont_Pin67
MIOS_SRIO_Get2_Cont_Pin45
	movlw	0x00
	btfsc	INDF1, 4
	iorlw	0x01
	btfsc	INDF1, 5
	iorlw	0x02
	rgoto	MIOS_SRIO_Get2_End

MIOS_SRIO_Get2_Cont_Pin67
	movlw	0x00
	btfsc	INDF1, 6
	iorlw	0x01
	btfsc	INDF1, 7
	iorlw	0x02
	;; 	rgoto	MIOS_SRIO_Get2_End

MIOS_SRIO_Get2_End
	andlw	0x03					; for correct status (Z flag set/not set...)
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  INTERNAL
;;  MIOS SRIO Set0: sets DIN/DOUT/CHANGED register flag to 0
;;  IN:	  Pin number in WREG, register address in FSR0
;;  OUT:  -
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_Set0
	movwf	MIOS_PARAMETER1

	rlf	WREG, F
	swapf	WREG, W
	andlw	0x0f
	addwf	FSR1L, F

	movf	MIOS_PARAMETER1, W
	andlw	0x07
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F
	return

;; --------------------------------------------------------------------------
;;  INTERNAL
;;  MIOS SRIO Set0: sets DIN/DOUT register flag to 0
;;  IN:	  Pin number in WREG, register address in FSR0
;;  OUT:  -
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_Set1
	movwf	MIOS_PARAMETER1

	rlf	WREG, F
	swapf	WREG, W
	andlw	0x0f
	addwf	FSR1L, F

	movf	MIOS_PARAMETER1, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F
	return

;; --------------------------------------------------------------------------
;;  INTERNAL
;;  MIOS SRIO Set00: clears two neighboured DIN/DOUT/CHANGED register flags
;;  IN:	  Number of first pin in WREG, register address in FSR0
;;  OUT:  -
;;  USES: FSR1 and BSR
;; --------------------------------------------------------------------------
MIOS_SRIO_Set00
	movwf	MIOS_PARAMETER1

	rlf	WREG, F
	swapf	WREG, W
	andlw	0x0f
	addwf	FSR1L, F

	BRA_IFSET MIOS_PARAMETER1, 2, ACCESS, MIOS_SRIO_Set00_Cont_Pin4567
MIOS_SRIO_Set00_Cont_Pin0123
	BRA_IFSET MIOS_PARAMETER1, 1, ACCESS, MIOS_SRIO_Set00_Cont_Pin23
MIOS_SRIO_Set00_Cont_Pin01
	movlw	0xfc
	rgoto	MIOS_SRIO_Set00_End

MIOS_SRIO_Set00_Cont_Pin23
	movlw	0xf3
	rgoto	MIOS_SRIO_Set00_End

MIOS_SRIO_Set00_Cont_Pin4567
	BRA_IFSET MIOS_PARAMETER1, 1, ACCESS, MIOS_SRIO_Set00_Cont_Pin67
MIOS_SRIO_Set00_Cont_Pin45
	movlw	0xcf
	rgoto	MIOS_SRIO_Set00_End

MIOS_SRIO_Set00_Cont_Pin67
	movlw	0x3f
	;; 	rgoto	MIOS_SRIO_Set00_End

MIOS_SRIO_Set00_End
	andwf	INDF1, F
	return
