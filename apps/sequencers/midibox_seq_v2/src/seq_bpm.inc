; $Id$
;
; MIDIboxSEQ
; BPM Timer function
;
; Some notes to the way how MIDIboxSEQ generates the internal 96ppqn clock
;
; MASTER MODE
; ~~~~~~~~~~~
;
; The period of USER_Timer will be configured based on bpm_table.inc which
; is a generated file (utils\gen_bpm_table.pl). The values are calculated 
; based on the formula: delay = (60 / (bpm * 24)) / prescaler_delay
; 
; For a common MIDI clock (24ppqn), 8 should be used as prescaler delay to 
; realize BPM rates from 48 to 255
; But MBSEQ works with 4 times resolution (96ppqn), therefore a prescaler
; delay of 2 is used here
;
; The USER_Timer hook increments the SEQ_CLK_REQ_CTR with every invokation
; which triggers the SEQ_CORE_Clk function. Using a counter instead of a
; request flag ensures that a clock event never get lost, even though the
; sequencer handler won't be called for a certain time - e.g. during a
; EEPROM write transfer
;
;
; SLAVE MODE
; ~~~~~~~~~~
;
; Could be realized by incrementing the SEQ_CLK_REQ_CTR by 4 with every
; received MIDI clock. But for the current implementation I spent some
; effort to realize 3 interpolated clock events between the received
; MIDI clocks in order to increase the resolution.
;
; This mechanism works that way: USER_Timer is used as reference
; counter which will be invoked every 500 uS. It measures the delay
; between two F8 events by incrementing a counter SEQ_INCOMING_CLK_CTR
; which will be registered in SEQ_INCOMING_CLK_DELAY on the next F8
; event.
;
; The delay value will be divided by 4, this is the preload value of
; SEQ_SENT_CLK_DELAY which defines the delay between interpolated clock
; events. The SEQ_SENT_CLK_DELAY will be decremented by USER_Timer
; every 500 uS. Once it reaches zero, a clock event will be triggered,
; and SEQ_SENT_CLK_DELAY will be reloaded by SEQ_INCOMING_CLK_DELAY / 4
;
; The SEQ_SENT_CLK_CTR ensures that never more than 3 internal clock
; events will be triggered by USER_Timer. This is to avoid timing 
; failures on BPM sweeps or jittering MIDI interfaces.
;
; And now the trick for stable timings: SEQ_MIDI_NotifyRx_Clk 
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
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns the BPM setting
;; --------------------------------------------------------------------------
SEQ_BPM_Get
	SET_BSR	SEQ_BPM
	movf	SEQ_BPM, W, BANKED
	return
	

;; --------------------------------------------------------------------------
;;  This function sets the timer to a given BPM period length
;; --------------------------------------------------------------------------
SEQ_BPM_Set
	;; disable all IRQs
	IRQ_DISABLE

	;; set BPM rate
	SET_BSR	SEQ_BPM
	movwf	SEQ_BPM, BANKED

	;; branch depending on master/slave mode
	movf	SEQ_BPM, W, BANKED
	bnz	SEQ_BPM_Master
SEQ_BPM_Slave
	;; if BPM is zero, the timer will be used to measure the delay between
	;; one clock, in order to produce 4 internal clocks on every F8 event
	;; using 500 uS as reference
	movlw	5000 & 0xff
	movwf	MIOS_PARAMETER1
	movlw	5000 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x00
	call	MIOS_TIMER_Init
	rgoto	SEQ_BPM_End

	
SEQ_BPM_Master
	;; reset NOECHO flag
	bcf	SEQ_REQ, SEQ_REQ_NOECHO, BANKED

	;; calc address to timer value:
	movlw	(3125000 >> 16) & 0xff
	movwf	MATH_AARGB0
	movlw	(3125000 >> 8) & 0xff
	movwf	MATH_AARGB1
	movlw	(3125000) & 0xff
	movwf	MATH_AARGB2

	clrf	MATH_BARGB0
	movf	SEQ_BPM, W, BANKED
	movwf	MATH_BARGB1

	call	MATH_DIV24_16

	;; result in MATH_AARGB1:MATH_AARGB2
	;; copy to MIOS_PARAMETER[12]
	movff	MATH_AARGB2, MIOS_PARAMETER1
	movff	MATH_AARGB1, MIOS_PARAMETER2

	;; init timer; prescaler 1:2
	movlw	0x01
	call	MIOS_TIMER_ReInit

SEQ_BPM_End
	;; enable IRQs again and exit
	IRQ_ENABLE
	return


SEQ_BPM_Stop
	goto	MIOS_TIMER_Stop


;; --------------------------------------------------------------------------
;;  This function is called by mb_timer.inc to generate a MIDI clock event
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
SEQ_BPM_Tick
	SET_BSR	SEQ_BASE
	movf	SEQ_BPM, W, BANKED
	bnz	SEQ_BPM_Tick_Master

SEQ_BPM_Tick_Slave
	;; increment the clock counter, used to measure the delay between two F8 events
	;; see also SEQ_MIDI_NotifyRx
	incf	SEQ_INCOMING_CLK_CTR, F, BANKED

	;; decrement sent clock delay, send interpolated clock events 3 times
	;; (more infos: see header of seq_bpm.inc)
	decf	SEQ_SENT_CLK_DELAY, F, BANKED
	bnz	SEQ_BPM_Tick_Slave_NoTrigger
SEQ_BPM_Tick_Slave_Trigger
	movf	SEQ_SENT_CLK_CTR, W, BANKED
	xorlw	0x03
	bz	SEQ_BPM_Tick_Slave_NoTrigger
	incf	SEQ_SENT_CLK_CTR, F, BANKED
	incf	SEQ_CLK_REQ_CTR, F, BANKED
	rrf	SEQ_INCOMING_CLK_DELAY, W
	rrf	WREG, W
	andlw	0x3f
	movwf	SEQ_SENT_CLK_DELAY, BANKED
SEQ_BPM_Tick_Slave_NoTrigger
	return


SEQ_BPM_Tick_Master
	;; increment clock request counter which is used by SEQ_CORE
	incf	SEQ_CLK_REQ_CTR, F, BANKED

	return
