; $Id$
;
; MIOS Timer Routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_TIMER_Init
;;  C_DECLARATION: void MIOS_TIMER_Init(unsigned char mode, unsigned int period)
;;  DESCRIPTION: initializes the timer which calls USER_Timer periodically
;;  and starts it. The resolution of the timer is 100nS with Prescaler 1:1,
;;  200nS with 1:2, 400nS with 1:4, 800nS with 1:8. The period (number of
;;  clocks) is specified as 16 bit value.
;;  IN:	  prescaler value in WREG:
;;           0x00: 1:1
;;           0x01: 1:2
;;           0x02: 1:4
;;           0x03: 1:8
;;        number of ticks (low-byte) in MIOS_PARAMETER1
;;        number of ticks (high-byte) in MIOS_PARAMETER2
;;  C_IN: prescaler value in <mode>:
;;           0x00: 1:1
;;           0x01: 1:2
;;           0x02: 1:4
;;           0x03: 1:8
;;        number of ticks (0..65535) in <period>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;;  EXAMPLE:
;;	;; we want to setup the timer with a frequency of 500 Hz = 2 mS
;;	;; prescaler 1:1 should be used
;;	;; calculate the required number of clocks for this period:
;;	;; clocks = period / 100 nS = 2 mS / 100 nS = 20000
;;	;; calculate low and high byte:
;;	;;    low byte  = period & 0xff
;;	;;    high byte = period >> 8
;;	;; therefore:
;;	movlw	20000 & 0xff
;;	movwf	MIOS_PARAMETER1
;;	movlw	20000 >> 8
;;	movwf	MIOS_PARAMETER2
;;	movlw	0x00
;;	call	MIOS_TIMER_Init
;;
;;	;; now the USER_Timer function is called every 2 mS!
;;  C_EXAMPLE:
;;	// we want to setup the timer with a frequency of 500 Hz = 2 mS
;;	// prescaler 1:1 should be used
;;	// calculate the required number of clocks for this period:
;;	// clocks = period / 100 nS = 2 mS / 100 nS = 20000
;;	// therefore:
;;	MIOS_TIMER_Init(0x00, 20000);
;;
;;	// now the Timer() function is called every 2 mS!
;; --------------------------------------------------------------------------
MIOS_TIMER_Init
	;; load prescaler value to T3CON and stop timer
	swapf	WREG, W
	andlw	0x30
	movwf	T3CON

	;; copy reload values
	SET_BSR	MIOS_TIMER3_RELOAD_H
	comf	MIOS_PARAMETER2, W
	movwf	MIOS_TIMER3_RELOAD_H, BANKED
 	movwf	TMR3H
	
	comf	MIOS_PARAMETER1, W
	movwf	MIOS_TIMER3_RELOAD_L, BANKED
 	movwf	TMR3L

	;; start timer
	bsf	T3CON, TMR3ON	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_TIMER_ReInit
;;  C_DECLARATION: void MIOS_TIMER_ReInit(unsigned char mode, unsigned int period)
;;  DESCRIPTION: same like MIOS_TIMER_Init, but the timer won't be reset to
;;  allow a smooth re-initialization
;;  IN:	  prescaler value in WREG:
;;           0x00: 1:1
;;           0x01: 1:2
;;           0x02: 1:4
;;           0x03: 1:8
;;        number of ticks (low-byte) in MIOS_PARAMETER1
;;        number of ticks (high-byte) in MIOS_PARAMETER2
;;  C_IN: prescaler value in <mode>:
;;           0x00: 1:1
;;           0x01: 1:2
;;           0x02: 1:4
;;           0x03: 1:8
;;        number of ticks (0..65535) in <period>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_TIMER_ReInit
	;; load prescaler value to T3CON and stop timer
	swapf	WREG, W
	andlw	0x30
	movwf	T3CON

	;; copy reload values
	SET_BSR	MIOS_TIMER3_RELOAD_H
	comf	MIOS_PARAMETER2, W
	movwf	MIOS_TIMER3_RELOAD_H, BANKED
	;; 	movwf	TMR3H
	
	comf	MIOS_PARAMETER1, W
	movwf	MIOS_TIMER3_RELOAD_L, BANKED
	;; 	movwf	TMR3L

	;; start timer
	bsf	T3CON, TMR3ON	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_TIMER_Start
;;  C_DECLARATION: void MIOS_TIMER_Start(void)
;;  DESCRIPTION: (re)starts the timer if it has been stopped before
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_TIMER_Start
	bsf	T3CON, TMR3ON
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_TIMER_Stop
;;  C_DECLARATION: void MIOS_TIMER_Stop(void)
;;  DESCRIPTION: stops the timer
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_TIMER_Stop
	bcf	T3CON, TMR3ON
	return
