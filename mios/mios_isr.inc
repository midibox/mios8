; $Id$
;
; IRQ Vector and Service Routines of MIOS
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_Timer
;;  C_DECLARATION: void Timer(void)
;;  DESCRIPTION: This function is periodically called by MIOS. The frequency
;;  has to be initialized with MIOS_Timer_Set<BR>
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: yes
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_SR_Service_Prepare
;;  C_DECLARATION: void SR_Service_Prepare(void)
;;  DESCRIPTION: This function is called by MIOS before the shift register 
;;  are loaded.<BR>
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: yes
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_SR_Service_Finish
;;  C_DECLARATION: void SR_Service_Finish(void)
;;  DESCRIPTION: This function is called by MIOS after the shift register 
;;  have been loaded.<BR>
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: yes
;; --------------------------------------------------------------------------


;; ==========================================================================
;;  IRQ Vector
;; ==========================================================================

	;; note: bootloader jumps to 0x0408 on interrupts
	org	0x0408

#if PIC_DERIVATIVE_IRQ_WORKAROUND
	;; interrupt workaround for some buggy PIC devices like PIC18F4620
	call	MIOS_ISR, FAST
MIOS_ISR
	pop
#endif

	;; save FSR2[LH], required for SDCC
	movff	FSR2L, MIOS_IRQ_FSR2L
	movff	FSR2H, MIOS_IRQ_FSR2H

IRQ_Handler
	; which IRQ received?
	BRA_IFSET PIR1, RCIF, ACCESS, IRQ_UART_Rx	; UART Receive IRQ?
	BRA_IFSET PIR1, TMR1IF, ACCESS, IRQ_Timer1        ; Timer1 IRQ?
	BRA_IFSET PIR2, TMR3IF, ACCESS, IRQ_Timer3        ; Timer3 IRQ?

	BRA_IFCLR PIE1, TXIE, ACCESS, IRQ_Handler_NoTxIRQ
	BRA_IFSET PIR1, TXIF, ACCESS, IRQ_UART_Tx	; UART Transmit IRQ?
IRQ_Handler_NoTxIRQ

	;; check T0IF only if interrupt is enabled (allows to use Timer0 as alternative resource)
	BRA_IFCLR INTCON, T0IE, ACCESS, IRQ_Handler_NoT0IRQ
	BRA_IFSET INTCON, T0IF, ACCESS, IRQ_Timer0	; Timer0 IRQ?
IRQ_Handler_NoT0IRQ

IRQEnd

	;; restore FSR2[LH], especially saved for SDCC
	movff	MIOS_IRQ_FSR2L, FSR2L
	movff	MIOS_IRQ_FSR2H, FSR2H

        retfie	FAST

;; ==========================================================================
;;  Used to store all user registers
;; ==========================================================================
MIOS_IRQ_StoreUserRegs	
	movff	FSR1L, MIOS_IRQ_FSR1L		; store FSR1 and TBL* in temp. register
	movff	FSR1H, MIOS_IRQ_FSR1H
	movff	TBLPTRL, MIOS_IRQ_TBLPTRL
	movff	TBLPTRH, MIOS_IRQ_TBLPTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movff	TBLPTRU, MIOS_IRQ_TBLPTRU
#endif
	movff	TABLAT, MIOS_IRQ_TABLAT
	movff	MIOS_PARAMETER1, MIOS_IRQ_PARAMETER1
	movff	MIOS_PARAMETER2, MIOS_IRQ_PARAMETER2
	movff	MIOS_PARAMETER3, MIOS_IRQ_PARAMETER3
	return

;; ==========================================================================
;;  Used to restore all user registers
;; ==========================================================================
MIOS_IRQ_RestoreUserRegs	
	movff	MIOS_IRQ_FSR1L, FSR1L			; restore FSR1 and TBL* from temp. register
	movff	MIOS_IRQ_FSR1H, FSR1H
	movff	MIOS_IRQ_TBLPTRL, TBLPTRL
	movff	MIOS_IRQ_TBLPTRH, TBLPTRH
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movff	MIOS_IRQ_TBLPTRU, TBLPTRU
#endif
	movff	MIOS_IRQ_TABLAT, TABLAT
	movff	MIOS_IRQ_PARAMETER1, MIOS_PARAMETER1
	movff	MIOS_IRQ_PARAMETER2, MIOS_PARAMETER2
	movff	MIOS_IRQ_PARAMETER3, MIOS_PARAMETER3
	return

;; ==========================================================================
;;  IRQ Handler for Timer0, called every 100 us
;; ==========================================================================
IRQ_Timer0
	;; reload timer
	movlw	0x08
	movwf	TMR0L		; (no add, because we need a minimum time for the ADC!)

	;; clear IRQ flag
	bcf	INTCON, T0IF

	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_ALL, ACCESS, IRQ_Timer0_End

#if MIOS_ENABLE_AIN
	call	MIOS_AIN_Tick
#endif

#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
	;; SET_BSR	MIOS_AIN_CTRL	;(already set)
	CALL_IFSET MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED, MIOS_MF_SRM_Tick
#endif

IRQ_Timer0_End
	rgoto	IRQ_Handler
	
;; ==========================================================================
;;  IRQ Handler for Timer1, called every ms
;; ==========================================================================
IRQ_Timer1
	;; clear IRQ flag
	bcf	PIR1, TMR1IF

	; reload T1
	bcf	T1CON, TMR1ON
	movlw	-39-1
	addwf	TMR1H, F
	movlw	-16-1
	addwf	TMR1L, F
	bsf	T1CON, TMR1ON

	;; Decrement Delay Counter
	SET_BSR	MIOS_DELAY_CTR
	movf	MIOS_DELAY_CTR, W, BANKED
	skpz
	decf	MIOS_DELAY_CTR, F, BANKED

	;; Increment GP Counter
	incf	MIOS_GP_CTR, F, BANKED

	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_ALL, ACCESS, IRQ_Timer1_End

	movf	MIOS_GP_CTR, W, BANKED	; every 8th cycle decrement message counter -> 256*8 = ca. 2 seconds
	andlw	0x07
	bnz	IRQ_Timer1_MessageCtrSkip
	;; Decrement message counter if counter != 0
	decf	MIOS_MESSAGE_CTR, W, BANKED
	bz	IRQ_Timer1_MessageCtrSkip	; mainloop reacts when display counter == 1 and sets it to 0 (simple semaphore)
	movf	MIOS_MESSAGE_CTR, W, BANKED
	skpz
	decf	MIOS_MESSAGE_CTR, F, BANKED
IRQ_Timer1_MessageCtrSkip

	;; start SRIO scan once the update counter has reached zero
	SET_BSR	MIOS_SRIO_UPDATE_CTR
	decf	MIOS_SRIO_UPDATE_CTR, F, BANKED
	bnz	IRQ_Timer1_SkipScan
	movf	MIOS_SRIO_UPDATE_FRQ, W, BANKED
	skpnz
	incf	MIOS_SRIO_UPDATE_FRQ, W, BANKED
	movwf	MIOS_SRIO_UPDATE_CTR, BANKED

	;; skip if number of SRIOs is 0
	movf	MIOS_SRIO_NUMBER, W, BANKED
	bz	IRQ_Timer1_SkipScan

	;; decrement debounce counter so long it isn't zero
	movf	MIOS_SRIO_DEBOUNCE_CTR, W, BANKED
	skpz
	decf	MIOS_SRIO_DEBOUNCE_CTR, F, BANKED

IRQ_Timer1_DoScan
	;; do the SRIO scan and handle rotary encoders
	rcall	MIOS_IRQ_StoreUserRegs
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_SR_Service_Prepare	; call prepare hook
	call	MIOS_SRIO_Tick		; SRIO scan
#if MIOS_ENABLE_ENC
	call	MIOS_ENC_Tick		; encoder handler
#endif
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_SR_Service_Finish	; call finish hook

	;; So long debounce counter is != 0, clear all MIOS_SR_DIN_CHANGED_x registers to ignore the button movements at this time.
	;; In order to ensure, that a new final state of a button won't get lost, the DIN values are XORed with the CHANGED registers.
	;; Yes, this ill idea works! :)
	;; Even the encoder handler still works properly, because it clears the appr. _DIN_CHANGED_x flags, so that the _DIN_x flags
	;; won't be touched by the XOR operation
	SET_BSR	MIOS_SRIO_DEBOUNCE_CTR	; GRRRRR! never forget to change the bank after a user call
	movf	MIOS_SRIO_DEBOUNCE_CTR, W, BANKED
	bz	IRQ_Timer1_DoScan_NoDebounce
IRQ_Timer1_DoScan_Debounce
	lfsr	FSR2, MIOS_SR_DIN_CHANGED_0
	lfsr	FSR1, MIOS_SR_DIN_0
	movlw	16
	movwf	IRQ_TMP1
IRQ_Timer1_DoScan_DebounceLoop
	movf	INDF2, W	; XOR _DIN_x with _DIN_CHANGED_x
	xorwf	POSTINC1, F
	clrf	POSTINC2	; clear _DIN_CHANGED_x
	decfsz	IRQ_TMP1, F
	rgoto	IRQ_Timer1_DoScan_DebounceLoop
IRQ_Timer1_DoScan_NoDebounce

	rcall	MIOS_IRQ_RestoreUserRegs

IRQ_Timer1_SkipScan

	;; handle auto repeats if debounce counter != 0
	movf	MIOS_SRIO_DEBOUNCE_CTR, W, BANKED
	skpnz
	call	MIOS_DIN_AutoRepeat_Tick

IRQ_Timer1_End
	rgoto	IRQ_Handler


;; ==========================================================================
;;  IRQ Handler for Timer3, called with user definable period
;; ==========================================================================
IRQ_Timer3
	;; clear IRQ flag
        bcf     PIR2, TMR3IF

	;; reload timer
	SET_BSR	MIOS_TIMER3_RELOAD_H
	bcf	T3CON, TMR3ON
	movf	MIOS_TIMER3_RELOAD_L, W, BANKED
	addwf	TMR3L, F
	movf	MIOS_TIMER3_RELOAD_H, W, BANKED
	addwfc	TMR3H, F
	bsf	T3CON, TMR3ON

	;; if MIOS_TIMER_Stop has been called, skip the rest
	BRA_IFCLR T3CON, TMR3ON, ACCESS, IRQ_Timer3_End
	;; also in suspend mode...
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_ALL, ACCESS, IRQ_Timer3_End
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, IRQ_Timer3_End

	rcall	MIOS_IRQ_StoreUserRegs
	call	USER_Timer
	rcall	MIOS_IRQ_RestoreUserRegs
	
IRQ_Timer3_End
	rgoto	IRQ_Handler


;; ==========================================================================
;;  IRQ Handler for UART Transmitter
;; ==========================================================================
IRQ_UART_Tx
	SET_BSR	MIOS_TX_BUFFER_HEAD
	movf	MIOS_TX_BUFFER_HEAD, W, BANKED	; check if new byte has to be sent
	xorwf	MIOS_TX_BUFFER_TAIL, W, BANKED
	bnz	IRQ_UART_Tx_NextByte

	bcf	PIE1, TXIE			; prevent interrupts until this flag is set again by MIDI_SendByte

IRQ_UART_Tx_End	
	rgoto	IRQ_Handler			; return to interrupt handler

IRQ_UART_Tx_NextByte
	rcall	MIOS_IRQ_StoreUserRegs

	rcall	MIOS_MIDI_TxBufferGet		; get byte from Tx buffer
	movwf	TXREG				; send it

	rcall	MIOS_IRQ_RestoreUserRegs

	rgoto	IRQ_Handler			; return to interrupt handler

;; ==========================================================================
;;  IRQ Handler for UART Receiver
;; ==========================================================================
IRQ_UART_Rx
	;; notify frame error in MIOS_BOX_STAT - this will lead to a timeout (or to an error response during SysEx transfer)
	btfsc	RCSTA, FERR
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_FERR

	;; store received byte in MIOS internal register MIOS_IRQ_TMP_CTR
	;; (unfortunately this name has been choosen before I noticed that
	;; an internal TMP register is also required here)
	movff	RCREG, MIOS_IRQ_TMP_CTR

	rcall	MIOS_IRQ_StoreUserRegs

	SET_BSR	MIOS_IRQ_TMP_CTR
	BRA_IFSET MIOS_IRQ_TMP_CTR, 7, BANKED, IRQ_UART_Rx_Command

	movf	MIOS_MIDI_RUNSTATUS, W, BANKED	; on sysex: don't take care for the expected bytes
	xorlw	0xf0
	bz	IRQ_UART_Rx_ForwardSysEx

	movf	MIOS_MIDI_EXPBYTES, W, BANKED
	bnz	IRQ_UART_Rx_RunningData
	
	movf	MIOS_MIDI_RUNSTATUS, W, BANKED	; send running data status byte
	rcall	MIOS_MIDI_RxBufferPut
	
	movf	MIOS_MIDI_RUNSTATUS, W, BANKED	; store number of expected bytes in MIDI_EXPBYTES
	rcall	IRQ_UART_Rx_Hlp_GetCMax
	movwf	MIOS_MIDI_EXPBYTES, BANKED
	
IRQ_UART_Rx_RunningData
	decf	MIOS_MIDI_EXPBYTES, F, BANKED
IRQ_UART_Rx_ForwardSysEx
	movf	MIOS_IRQ_TMP_CTR, W, BANKED
	rcall	MIOS_MIDI_RxBufferPut
	rgoto	IRQ_UART_Rx_End

IRQ_UART_Rx_Command
	;; Command < 0xf8?
	movlw	0xf8 - 1
	cpfsgt	MIOS_IRQ_TMP_CTR, BANKED
	rgoto IRQ_UART_Rx_CommandLTF8

IRQ_UART_Rx_SystemCommand
	;; System >= F8: don't change Running Status and number of expected bytes!
	movf	MIOS_IRQ_TMP_CTR, W, BANKED	; just forward received byte
	rcall	MIOS_MIDI_RxBufferPut
	rgoto	IRQ_UART_Rx_End

IRQ_UART_Rx_CommandLTF8
	;; command less than F8: store running status, send byte, calc number of expected bytes
	movf	MIOS_IRQ_TMP_CTR, W, BANKED
	movwf	MIOS_MIDI_RUNSTATUS, BANKED
	movf	MIOS_MIDI_RUNSTATUS, W, BANKED
	rcall	IRQ_UART_Rx_Hlp_GetCMax
	movwf	MIOS_MIDI_EXPBYTES, BANKED

	movf	MIOS_MIDI_RUNSTATUS, W, BANKED
	rcall	MIOS_MIDI_RxBufferPut

IRQ_UART_Rx_End	
	bcf	PIR1, RCIF

	rcall	MIOS_IRQ_RestoreUserRegs

	rgoto	IRQ_Handler

;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  Rx Hlp GetCMax: get number of expected MIDI bytes depending on MIDI status
;;  In:	Status byte in W
;; --------------------------------------------------------------------------
IRQ_UART_Rx_Hlp_GetCMax
	movwf	IRQ_TMP2
	btfss	IRQ_TMP2, 6	; 8n, 9n, An, Bn
	retlw	0x02

IRQ_UART_Rx_Hlp_GetCMax_CDEF
	BRA_IFSET IRQ_TMP2, 5, ACCESS, IRQ_UART_Rx_Hlp_GetCMax_EF
IRQ_UART_Rx_Hlp_GetCMax_CD
	retlw	0x01		; Cn, Dn

IRQ_UART_Rx_Hlp_GetCMax_EF
	btfss	IRQ_TMP2, 4	; En
	retlw	0x02

	;; Fx:
	movf	IRQ_TMP2, W
	andlw	0xfe		; 0xf0 and 0xf1: expected bytes = 1
	xorlw	0xf0
	skpnz
	retlw	0x01

	movf	IRQ_TMP2, W	; 0xf2:	expected bytes = 2
	xorlw	0xf2
	skpnz
	retlw	0x02

	retlw	0x00		; others: expected bytes = 0
