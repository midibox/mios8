; $Id$
;
; MIOS Application Timer
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; since this is an interrupt service routine where we shouldn't
	;; send MIDI events (in order to retain the MIDI protocol consistency)
	;; we just request a MIDI clock for the Main program (-> USER_Tick, mmd_tick.inc)
	bsf	MMD_MIDI_CLOCK_REQUEST, 0
	return

;; --------------------------------------------------------------------------
;;  This function sets the timer to a given BPM period length
;; --------------------------------------------------------------------------
MMD_Timer_BPMSet
	movwf	MMD_BPM

	;; if BPM is zero, the delay will act as MIDI slave. Stop timer in this case
	movf	MMD_BPM, W
	bz	MMD_Timer_BPM_Stop
	
MMD_Timer_BPM_Reinit
	;; calc address to timer value: BPM_TABLE + (BPM << 1)
	clrf	TMP2				; clear help register
	clrc					; clear carry bit
	rlf	MMD_BPM, W			; shift BPM value to the left (== BPM * 2)
	movwf	TMP1				; save result in TMP1
	rlf	TMP2, F				; shift TMP2 to the left (it's zero, but just for info)
	
	TABLE_ADDR BPM_TABLE			; this macro sets the base address	
	movf	TMP1, W				; add BPM offset (number *2) to TABLPTR
	addwf	TBLPTRL, F			; low-byte
	movf	TMP2, W
	addwfc	TBLPTRH, F			; high-byte (+carry bit on overrun)

	;; read value from table and copy to MIOS_PARAMETER[12]
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2

	;; finally init timer; prescaler 1:8
	movlw	0x03
	goto	MIOS_TIMER_Init


	;; if zero: stop timer
MMD_Timer_BPM_Stop
	goto	MIOS_TIMER_Stop
