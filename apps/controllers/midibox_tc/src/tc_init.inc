; $Id$
;
; MIOS Application init routine
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; define number of shift registers: just use all 16 shift registers,
	;; although only 1 has been connected. It doesn't really matter ;-)
	movlw	16
	call	MIOS_SRIO_NumberSet

	;; define the update frequency (latency) of DIN/DOUTs in mS
	movlw	1 ; ms
	call	MIOS_SRIO_UpdateFrqSet

	;; initialize the user timer, we use it to flash the currently selected parameter
	;; (see tc_timer.inc)
	;; timer should be called every 100 ms == 1000000 ticks
	;; since this value is greater than 16 bit (== 65535 max) we just
	;; use a 1:8 prescaler and an additional software-implemented 1:2 postscaler
	;; so the init value must be: 1000000 / (8*2) == 62500
	;; (see also the MIOS_TIMER_Init documentation)
	movlw	62500 & 0xff		; low-byte
	movwf	MIOS_PARAMETER1
	movlw	62500 >> 8		; high-byte
	movlw	0x03			; use 1:8 prescaler
	call	MIOS_TIMER_Init
	
	;; initialize TC_MODE (we want to see the parameter page after startup)
	movlw	TC_MODE_PARA
	movwf	TC_MODE

	;; we begin with Bank #1 (-> TC_BANK == bank - 1)
	movlw	0
	movwf	TC_BANK

	;; we begin with Parameter #1 (-> TC_PARA == parameter number - 1)
	movlw	0
	movwf	TC_PARA

	;; clear the select counter
	clrf	TC_SELECT_CTR

	;; clear all values to zero
	lfsr	FSR0, TC_VALUES_BEGIN	; init pointer for the TC_VALUES array
TC_INIT_Values_Loop
	clrf	POSTINC0		; clear INDF0 and increment pointer
	movf	FSR0L, W		; load low-byte of incremented pointer
	bnz	TC_INIT_Values_Loop	; continue if low-byte not zero
					; (in fact we clear 0x100-0x1ff on this way)
	
	return
