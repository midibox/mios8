; $Id$
;
; MIDIboxCV, Clock Output functions
;
; Some notes to the way how MIDIbox CV generates the internal 96ppqn clock
; (Code taken from MIDIbox SEQ)
;
; SLAVE MODE
; ~~~~~~~~~~
;
; Could be realized by incrementing the CV_CLK_REQ_CTR by 4 with every
; received MIDI clock. But for the current implementation I spent some
; effort to realize 3 interpolated clock events between the received
; MIDI clocks in order to increase the resolution.
;
; This mechanism works that way: USER_Timer is used as reference
; counter which will be invoked every 500 uS. It measures the delay
; between two F8 events by incrementing a counter CV_INCOMING_CLK_CTR
; which will be registered in CV_INCOMING_CLK_DELAY on the next F8
; event.
;
; The delay value will be divided by 4, this is the preload value of
; CV_SENT_CLK_DELAY which defines the delay between interpolated clock
; events. The CV_SENT_CLK_DELAY will be decremented by USER_Timer
; every 500 uS. Once it reaches zero, a clock event will be triggered,
; and CV_SENT_CLK_DELAY will be reloaded by CV_INCOMING_CLK_DELAY / 4
;
; The CV_SENT_CLK_CTR ensures that never more than 3 internal clock
; events will be triggered by USER_Timer. This is to avoid timing 
; failures on BPM sweeps or jittering MIDI interfaces.
;
; And now the trick for stable timings: CV_MIDI_NotifyRx_Clk 
; (which receives the F8 events) increments SEQ_SENT_CLK_CTR by
; 4 - SEQ_SENT_CLK_CTR. 
;
; This ensures that the sequencer will always run in sync with
; the external MIDI clock, and that a 96ppqn clock event never
; get lost, regardless of jitter or BPM sweeps
;
; Copyright (C) 22th January 2004 by Thorsten Klose
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function inits the user timer for clock slave mode
;; --------------------------------------------------------------------------
CV_CLK_Init
	;; the timer will be used to measure the delay between
	;; one clock, in order to produce 4 internal clocks on every F8 event
	;; using 500 uS as reference
	movlw	5000 & 0xff
	movwf	MIOS_PARAMETER1
	movlw	5000 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x00
	call	MIOS_TIMER_Init
	return


;; --------------------------------------------------------------------------
;;  This function is called by USER_Timer to generate clock events
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
CV_CLK_Timer
	SET_BSR	CV_BASE


	;; if clk divider < 3, we are using interpolated clock
	;; otherwise the clock will be directly forwarded and the timer is not required
	movlw	3
	cpfslt	CV_CLK_DIVIDER_PRELOAD, BANKED
	rgoto CV_CLK_Timer_Slave_NoTrigger

CV_CLK_Timer_Slave
	;; increment the clock counter, used to measure the delay between two F8 events
	;; see also CV_MIDI_NotifyRx
	incf	CV_INCOMING_CLK_CTR, F, BANKED

	;; decrement sent clock delay, send interpolated clock events 3 times
	;; (more infos: see header of seq_bpm.inc)
	decf	CV_SENT_CLK_DELAY, F, BANKED
	bnz	CV_CLK_Timer_Slave_NoTrigger
CV_CLK_Timer_Slave_Trigger
	movf	CV_SENT_CLK_CTR, W, BANKED
	xorlw	0x03
	bz	CV_CLK_Timer_Slave_NoTrigger
	incf	CV_SENT_CLK_CTR, F, BANKED
	incf	CV_CLK_REQ_CTR, F, BANKED
	rrf	CV_INCOMING_CLK_DELAY, W
	rrf	WREG, W
	andlw	0x3f
	movwf	CV_SENT_CLK_DELAY, BANKED
CV_CLK_Timer_Slave_NoTrigger


#if DEFAULT_EXT_CLK_LAT
	;; branch to pin clear routine if pin is set
	BRA_IFSET DEFAULT_EXT_CLK_LAT, DEFAULT_EXT_CLK_PIN, ACCESS, CV_CLK_Timer_ClkClr

	;; else check if clocks are requested
	movf	CV_CLK_REQ_CTR, W, BANKED
	bz	CV_CLK_Timer_NoClk

	;; decrement request counter
	decf	CV_CLK_REQ_CTR, F, BANKED

	;; preload pulsewidth counter
	movlw	DEFAULT_EXT_CLK_PULSEWIDTH
	movwf	CV_CLK_PULSEWIDTH_CTR, BANKED

	;; clock divider handling
	movf	CV_CLK_DIVIDER, W, BANKED
	bnz	CV_CLK_Timer_ClkClrDv
	bsf	DEFAULT_EXT_CLK_LAT, DEFAULT_EXT_CLK_PIN
CV_CLK_Timer_ClkClrDv
	decf	CV_CLK_DIVIDER, F, BANKED
	incf	CV_CLK_DIVIDER, W, BANKED
	bnz	CV_CLK_Timer_ClkCont
	;; if clk divider < 3, we are using interpolated clock
	;; otherwise the clock will be directly forwarded
	movlw	3-1
	cpfsgt	CV_CLK_DIVIDER_PRELOAD, BANKED
	rgoto CV_CLK_Timer_ClkIP
CV_CLK_Timer_ClkNoIP
	movf	CV_CLK_DIVIDER_PRELOAD, W, BANKED
	addlw	-3
	movwf	CV_CLK_DIVIDER, BANKED
	rgoto	CV_CLK_Timer_ClkCont
CV_CLK_Timer_ClkIP
	movff	CV_CLK_DIVIDER_PRELOAD, CV_CLK_DIVIDER
	rgoto	CV_CLK_Timer_ClkCont

CV_CLK_Timer_ClkClr
CV_CLK_Timer_NoClk
	;; ensure pulsewidth specified in DEFAULT_EXT_CLK_PULSEWIDTH
	movf	CV_CLK_PULSEWIDTH_CTR, W, BANKED
	bz	CV_CLK_Timer_ClkClr_Ok
	decf	CV_CLK_PULSEWIDTH_CTR, F, BANKED
	rgoto	CV_CLK_Timer_ClkClr_Wait
CV_CLK_Timer_ClkClr_Ok
	bcf	DEFAULT_EXT_CLK_LAT, DEFAULT_EXT_CLK_PIN
CV_CLK_Timer_ClkClr_Wait

CV_CLK_Timer_ClkCont
#endif

	return

;; --------------------------------------------------------------------------
;;  This function is called from USER_MIDI_NotifyRx when a MIDI byte has 
;;  been received. It's used to check for MIDI clock events
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o received byte in WREG
;; --------------------------------------------------------------------------
CV_CLK_NotifyRx
	movwf	IRQ_TMP1

	SET_BSR	CV_BASE

	;; notify clock event if received
	movlw	0xf8
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CV_CLK_NotifyRx_NoClk
CV_CLK_NotifyRx_Clk
	;; if clk divider >= 3, clock will be directly forwarded
	movlw	3-1
	cpfsgt	CV_CLK_DIVIDER_PRELOAD, BANKED
	rgoto CV_CLK_NotifyRx_Clk_IP
CV_CLK_NotifyRx_Clk_NoIP
	incf	CV_CLK_REQ_CTR, F, BANKED
	return

CV_CLK_NotifyRx_Clk_IP
	;; we've measured a new delay between two F8 events
	movf	CV_INCOMING_CLK_CTR, W, BANKED
	movwf	CV_INCOMING_CLK_DELAY, BANKED
	clrf	CV_INCOMING_CLK_CTR, BANKED

	;; increment clock counter by 4 - CV_SENT_CLK_CTR and ensure that the clock won't be echoed
	movf	CV_SENT_CLK_CTR, W, BANKED
	sublw	4
	addwf	CV_CLK_REQ_CTR, F, BANKED

	;; clear interpolation clock counter and get new SENT_CLK delay
	clrf	CV_SENT_CLK_CTR, BANKED
	rrf	CV_INCOMING_CLK_DELAY, W, BANKED
	rrf	WREG, W
	andlw	0x3f
	movwf	CV_SENT_CLK_DELAY, BANKED
	return

CV_CLK_NotifyRx_NoClk

	movlw	0xfa
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CV_CLK_NotifyRx_NoStart
CV_CLK_NotifyRx_Start
	clrf	CV_CLK_DIVIDER, BANKED	; clear clock divider (!)
	rgoto	CV_CLK_NotifyRx_Start_Cont
CV_CLK_NotifyRx_NoStart

	movlw	0xfb
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CV_CLK_NotifyRx_NoCont
CV_CLK_NotifyRx_Cont
CV_CLK_NotifyRx_Start_Cont

	;; cancel all requested clocks
	clrf	CV_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	CV_SENT_CLK_CTR, BANKED

	;; notify that clock is running
	bsf	CV_CLK_STAT, CV_CLK_STAT_RUNNING, BANKED

	;; set start pin if defined
#if DEFAULT_EXT_START_LAT
	bsf	DEFAULT_EXT_START_LAT, DEFAULT_EXT_START_PIN
#endif

	return
CV_CLK_NotifyRx_NoCont

	movlw	0xfc
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CV_CLK_NotifyRx_NoStop
CV_SYSEX_SysExCheck_Stop
	;; notify that clock is not running anymore
	bcf	CV_CLK_STAT, CV_CLK_STAT_RUNNING, BANKED
	;; clear start pin if defined
#if DEFAULT_EXT_START_LAT
	bcf	DEFAULT_EXT_START_LAT, DEFAULT_EXT_START_PIN
#endif
	return
CV_CLK_NotifyRx_NoStop

CV_CLK_NotifyRx_End
	return

