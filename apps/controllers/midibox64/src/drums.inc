; $Id$
;
; driver module for drum triggers
; 
; *** EXPERIMENTAL --- NOT FINISHED YET ***
; 
; 
; This include file provides following functions:
;    o DRUMS_Init: initializes the drums driver
;    o DRUMS_Timer: called by USER_Timer or USER_SR_Service_Finish periodically to update the DRUM_STATE
;    o DRUMS_AIN_NotifyChange: called from USER_AIN_NotifyChange on AIN events
;    o DRUMS_Tick: called by USER_Tick to handle the drum trigger events
;
; number of analog inputs which are assigned to drum triggers
#define DRUMS_NUMBER_AIN	8
;
; threshold value for rising signal - when will an "On" event be regognized?
#define DRUMS_THRESHOLD_RISING	64
;
; threshold value for falling signal - when will an "Off" event be regognized?
#define DRUMS_THRESHOLD_FALLING	16
;
; measuring time - defines, how long an analog value should be measured before 
; the maximum value will be sent out) - absolute time depends on the period time 
; of DRUMS_Timer. If DRUMS_Timer is called from USER_SR_Service_Finish,
; the time is: 1 mS * DRUMS_MEASURING_TIME
#define DRUMS_MEASURING_TIME	10
;
; using the MB64_POT handler to send an event (allows menu configuration
; of the MIDI events) or just send trigger the notes directly?
#define DRUMS_USE_MB64_POT_HANDLER 0
;
;
; Some comments to the concept: this driver consists of a finite state 
; machine which measures the maximum peak value of an analog drum trigger
; signal and sends an "On" event after a certain delay defined by MEASURING_TIME.
; An "Off" event will be sent once the analog value falls below the THRESHOLD
; value.
; 
; a) STATE == MEASURING_TIME+3
;    set by DRUMS_AIN_NotifyChange once the drum output passes the
;    THRESHOLD_RISING value or once the new measuring result is greater 
;    than the last recorded MAX_VALUE
;    Exception: don't record MAX_VALUE if STATE == 0x01
;    (On event has already been sent)
; 
; b) STATE >= 0x03
;    DRUMS_Timer decrements the STATE periodically until it is <= 0x02
;  
; c) STATE == 0x02
;    DRUMS_Tick sends an "On" event with the maximum value which has
;    been recorded by DRUMS_AIN_NotifyChange during STATE was >= 0x03
;    Thereafter it sets the STATE to 0x01
;
; d) STATE == 0x01
;    DRUMS_AIN_NotifyChange waits until the drum trigger falls below
;    the THRESHOLD_FALLING value and sends the "Off" event
;    Thereafter it sets the STATE to 0x00
;
; e) STATE == 0x00
;    Idle state - DRUMS_AIN_NotifyChange waits until the drum trigger
;    passes the THRESHOLD_RISING value again.
;    If so, the MAX_VALUE will be updated and the STATE will be
;    set to MEASURING_TIME+3 --- continue at a)
;
; Exception: 
;    if the analog signal falls below the THRESHOLD_FALLING value 
;    during STATE > 1, force state to 0x02, so that an On and Off
;    event will be sent
;
;
; Following variables have to be added to app_defines.inc
; --- locate it to free addresses!
;DRUMS_STATE_BEGIN	EQU	0x050	; for 8 drum triggers
;DRUMS_STATE_END	EQU	0x057
;DRUMS_MAX_VALUE_BEGIN	EQU	0x058	; for 8 drum triggers
;DRUMS_MAX_VALUE_END	EQU	0x05f
;DRUMS_CTR		EQU	0x060	; used as loop counter
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: DRUMS_Init
;;  DESCRIPTION: This function initializes the drum driver module, it should
;;  be called from USER_Init
;;  IN:   -
;;  OUT:  -
;;  USES: FSR0, FSR1, BSR
;; --------------------------------------------------------------------------
DRUMS_Init
	SET_BSR	DRUMS_CTR
	movlw	DRUMS_NUMBER_AIN
	movwf	DRUMS_CTR, BANKED	; init loop counter
	lfsr	FSR0, DRUMS_STATE_BEGIN	; address DRUMS_STATE and DRUMS_MAX_VALUE indirectly
	lfsr	FSR1, DRUMS_MAX_VALUE_BEGIN
DRUMS_Init_Loop
	clrf	POSTINC0		; clear DRUMS_STATE_x
	clrf	POSTINC1		; clear DRUMS_MAX_VALUE_x
	decfsz	DRUMS_CTR, F, BANKED	; loop until counter reaches zero
	rgoto	DRUMS_Init_Loop

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: DRUMS_Timer
;;  DESCRIPTION: This function has to be called periodically from USER_Timer
;;  or USER_SR_Service_Finish to update the STATE counter
;;  IN:   -
;;  OUT:  -
;;  USES: IRQ_TMP1, FSR2
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
DRUMS_Timer
	;; a) decrement the STATE values until they are <= 0x02
	movlw	DRUMS_NUMBER_AIN	; IRQ_TMP1 used as counter
	movwf	IRQ_TMP1
	lfsr	FSR2, DRUMS_STATE_BEGIN	; address DRUMS_STATE and DRUMS_MAX_VALUE indirectly
DRUMS_Timer_Loop
	movlw	0x03
        cpfslt  INDF2, ACCESS		; skip next instruction if STATE < 3
	decf	INDF2, F

	movf	POSTINC2, W		; dummy read which increments pointer

	decfsz	IRQ_TMP1, F		; loop until IRQ_TMP1 is zero
	rgoto	DRUMS_Timer_Loop

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: DRUMS_AIN_NotifyChange
;;  DESCRIPTION: This function has to be called from USER_AIN_NotifyChange
;;  on AIN events.
;;  IN:   Drum trigger number (0x00-DRUMS_NUMBER_AIN) in MIOS_PARAMETER1
;;        7-bit value in MIOS_PARAMETER2
;;  OUT:  -
;;  USES: BSR, FSR0, FSR1
;;  EXAMPLE: for MB64 application:
;;USER_AIN_NotifyChange
;;	;; convert 10-bit value to 7-bit value
;;	rrf	MIOS_PARAMETER3, F	; value / 2
;;	rrf	MIOS_PARAMETER2, F
;;	rrf	MIOS_PARAMETER3, F	; value / 2
;;	rrf	MIOS_PARAMETER2, F
;;	rrf	MIOS_PARAMETER3, F	; value / 2
;;	rrf	MIOS_PARAMETER2, F
;;	bcf	MIOS_PARAMETER2, 7	; clear 8th bit
;;
;;	;; now: pot number in MIOS_PARAMETER1
;;	;; 7-bit value in MIOS_PARAMETER2
;;
;;	;; if drum trigger (MIOS_PARAMETER1 < 8):
;;	;; branch to the DRUMS module
;;	movlw	8-1
;;	IFLEQ	MIOS_PARAMETER1, ACCESS, goto DRUMS_AIN_NotifyChange
;;
;;	;; else continue with MB64 pot handler
;;	;; (expects number of pot in WREG)
;;	movf	MIOS_PARAMETER1, W
;;	goto	MB64_POT_Handler
;;
;; --------------------------------------------------------------------------
DRUMS_AIN_NotifyChange
	;; store trigger number in DRUMS_CTR
	SET_BSR	DRUMS_CTR
	movf	MIOS_PARAMETER1, W
	movwf	DRUMS_CTR, BANKED

	;; prepare pointers
	;; FSR0 points to STATE_x
	lfsr	FSR0, DRUMS_STATE_BEGIN
	addwf	FSR0L, F

	;; FSR1 points to MAX_VALUE_x
	lfsr	FSR1, DRUMS_MAX_VALUE_BEGIN
	addwf	FSR1L, F

	;; branch depending on current state:
	;; if STATE == 0: wait until the THRESHOLD_RISING value has been passed
	movf	INDF0, W		; (INDF0 contains the STATE)
	bz	DRUMS_AIN_NotifyChange_Idle

DRUMS_AIN_NotifyChange_NotIdle
	;; STATE > 0x00

	;; if STATE == 0x01, wait for falling signal
	decf	INDF0, W		; (INDF0 contains the STATE)
	bnz	DRUMS_AIN_NotifyChange_NotState1
DRUMS_AIN_NotifyChange_State1
	;; nothing else to do so long as value < THRESHOLD_FALLING
	movlw	DRUMS_THRESHOLD_FALLING
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto DRUMS_AIN_NotifyChange_End

	;; send "Off" event
	rcall	DRUMS_Hlp_SendOffEvent

	;; set STATE and MAX_VALUE to 0x00 and exit
	;; (note: SendOffEvent can overwrite FSR0!)
	lfsr	FSR0, DRUMS_STATE_BEGIN
	SET_BSR	DRUMS_CTR
	movf	DRUMS_CTR, W, BANKED
	clrf	PLUSW0
	lfsr	FSR1, DRUMS_MAX_VALUE_BEGIN
	clrf	PLUSW1
	rgoto	DRUMS_AIN_NotifyChange_End

DRUMS_AIN_NotifyChange_NotState1
	;; STATE > 0x01

	;; if THRESHOLD_FALLING already reached, force state to 0x02,
	;; so that a Note On event will be sent
	movlw	DRUMS_THRESHOLD_FALLING
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto DRUMS_AIN_NotifyChange_NoFall
	movlw	0x02
	movwf	INDF0		; (FSR0 points to STATE)
	rgoto	DRUMS_AIN_NotifyChange_End

DRUMS_AIN_NotifyChange_NoFall
	;; if value greater than max value: record it and set the STATE to
	;; MEASURING_TIME+3 again
	movf	INDF1, W	; (FSR1 points to MAX_VALUE)
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto DRUMS_AIN_NotifyChange_NewMax
	rgoto	DRUMS_AIN_NotifyChange_End


DRUMS_AIN_NotifyChange_Idle
	;; idle state: if value passes THRESHOLD_RISING, record new
	;; value and set STATE to MEASURING_TIME+3
	movlw	DRUMS_THRESHOLD_RISING
	cpfsgt	MIOS_PARAMETER2, ACCESS
	rgoto DRUMS_AIN_NotifyChange_NoNewMax
DRUMS_AIN_NotifyChange_NewMax
	;; new max value: record it
	movff	MIOS_PARAMETER2, INDF1	; (FSR1 points to MAX_VALUE)

	;;  and set the STATE to MEASURING_TIME+3 again
	movlw	DRUMS_MEASURING_TIME+3
	movwf	INDF0			; (FSR0 points to STATE)
DRUMS_AIN_NotifyChange_NoNewMax

DRUMS_AIN_NotifyChange_End
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: DRUMS_Tick
;;  DESCRIPTION: This function should be called periodically from USER_Tick
;;  IN:   -
;;  OUT:  -
;;  USES: FSR0, FSR1
;; --------------------------------------------------------------------------
DRUMS_Tick
	SET_BSR	DRUMS_CTR
	clrf	DRUMS_CTR, BANKED	; init loop counter
DRUMS_Tick_Loop

	;; if STATE == 0x02, send an "On" event and set STATE to 0x01
	lfsr	FSR0, DRUMS_STATE_BEGIN
	movf	DRUMS_CTR, W, BANKED
	addwf	FSR0L, F
	movlw	0x02
	cpfseq	INDF0, ACCESS
	rgoto DRUMS_Tick_Loop_Next

	;; expects drum trigger number in MIOS_PARAMETER1
	movf	DRUMS_CTR, W, BANKED
	movwf	MIOS_PARAMETER1
	rcall	DRUMS_Hlp_SendOnEvent
	;; correct the BSR to DRUMS_CTR (BSR changed by DRUMS_Hlp_SendOnEvent)
	SET_BSR	DRUMS_CTR

	;; set state to 0x01
	;; (since SendOnEvent can modify FSR0, we have to calculate the pointer to STATE_x again)
	lfsr	FSR0, DRUMS_STATE_BEGIN
	movf	DRUMS_CTR, W, BANKED
	addwf	FSR0L, F
	movlw	0x01
	movwf	INDF0

DRUMS_Tick_Loop_Next
	movf	POSTINC0, W		; dummy read which increments the STATE pointer
	incf	DRUMS_CTR, F, BANKED	; increment counter
	movlw	DRUMS_NUMBER_AIN	; loop until counter reaches DRUMS_NUMBER_AIN
	cpfsgt	DRUMS_CTR, BANKED
	rgoto DRUMS_Tick_Loop

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: DRUMS_Hlp_SendOnEvent
;;  DESCRIPTION: This function sends an "On" event
;;  IN:   Drum trigger number (0x00-DRUMS_NUMBER_AIN) in MIOS_PARAMETER1
;;        7-bit value in MAX_VALUE_x
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
DRUMS_Hlp_SendOnEvent
#if DRUMS_USE_MB64_POT_HANDLER
	
	lfsr	FSR1, DRUMS_MAX_VALUE_BEGIN	; copy Velocity Value to MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W		; (pot number expected in WREG)
	movff	PLUSW1, MIOS_PARAMETER2
	goto	MB64_POT_Handler		; branch to POT handler of MB64
	
#else
	
	movlw	0x90				; Note Event Channel #1
	movwf	MIDI_EVNT0
	movf	MIOS_PARAMETER1, W		; Note number
	addlw	0x30				; base note: C-3
	movwf	MIDI_EVNT1
	lfsr	FSR1, DRUMS_MAX_VALUE_BEGIN	; Velocity Value
	movf	MIOS_PARAMETER1, W
	movf	PLUSW1, W
	movwf	MIDI_EVNT_VALUE
	goto	MIDI_EVNT_Send			; send MIDI event and exit
	
#endif

;; --------------------------------------------------------------------------
;;  FUNCTION: DRUMS_Hlp_SendOnEvent
;;  DESCRIPTION: This function sends an "On" event
;;  IN:   Drum trigger number (0x00-DRUMS_NUMBER_AIN) in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
DRUMS_Hlp_SendOffEvent
#if DRUMS_USE_MB64_POT_HANDLER

	clrf	MIOS_PARAMETER2			; write Velocity == 0x00 to MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W		; pot number expected in WREG
	goto	MB64_POT_Handler		; branch to POT handler of MB64
	
#else
	movlw	0x90				; Note Event Channel #1
	movwf	MIDI_EVNT0
	movf	MIOS_PARAMETER1, W		; Note number
	addlw	0x30				; base note: C-3
	movwf	MIDI_EVNT1
	clrf	MIDI_EVNT_VALUE			; Velocity Value == 0x00
	goto	MIDI_EVNT_Send			; send MIDI event and exit
#endif
