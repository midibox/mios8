; $Id$
;
; MIDIbox LC MIDI Processor
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
LC_MPROC_NotifyReceivedEvent
	;; branch to LC_MPROC_Received_Ex if pitch bender event (for motorfaders) has been received
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0xe0
	bz	LC_MPROC_Received_Ex

	;; branch to LC_MPROC_Received_90 if note event has been received
	movf	MIOS_PARAMETER1, W
	xorlw	0x90
	bz	LC_MPROC_Received_90

	;; branch to LC_MPROC_Received_D0 if aftertouch event has been received
	movf	MIOS_PARAMETER1, W
	xorlw	0xd0
	bz	LC_MPROC_Received_D0

#if 0
	;; disabled since Mackie Control receives MTC messages over channel 16!

	;; only continue of B0 (controller event, channel 0) has been received
	movlw	0xb0
	cpfseq	MIOS_PARAMETER1, ACCESS
	return
#endif

	;; branch if B0 3[0-9] has been received
	movf	MIOS_PARAMETER2, W
	andlw	0xf0
	xorlw	0x30
	bz	LC_MPROC_Received_B03x

	;; branch if B0 4[0-F] has been received
	movf	MIOS_PARAMETER2, W
	andlw	0xf0
	xorlw	0x40
	bz	LC_MPROC_Received_B04x

	;; else do nothing
	return

;; --------------------------------------------------------------------------
;;  Ex has been received: move motorfader
;; --------------------------------------------------------------------------
LC_MPROC_Received_Ex
	;; mask out MIDI channel (== fader number)
	movlw	0x0f
	andwf	MIOS_PARAMETER1, F
#if MOTORFADER0_IS_MASTERFADER == 0
	;; do nothing if fader number >= 8
	movlw	0x08
	cpfslt	MIOS_PARAMETER1, ACCESS
	return
#else
	;; exit if fader number == 0
	movf	MIOS_PARAMETER1, W
	skpnz
	return
	;; do nothing if fader number >= 9
	movlw	0x09
	cpfslt	MIOS_PARAMETER1, ACCESS
	return
	;; master fader 8 -> 0
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
#endif
	;; move fader
	movf	MIOS_PARAMETER1, W
	goto	LC_MF_FaderMove	; (located in lc_mf.inc)

;; --------------------------------------------------------------------------
;;  90 has been received: process LED event
;; --------------------------------------------------------------------------
LC_MPROC_Received_90
	;; save LED status in LED_STATUS array
	lfsr	FSR0, LED_STATUS_BEGIN
	movf	MIOS_PARAMETER2, W
	andlw	0x7f		; prevent overrun
	addwf	FSR0L, F
	movff	MIOS_PARAMETER3, INDF0

	;; set LED
	call	LC_LED_Set
	
	;; call the display status update routine
	movf	FSR0L, W
	andlw	0x7f
	call	LC_DISPLAY_LEDStatusUpdate

	return
	
;; --------------------------------------------------------------------------
;;  D0 xx has been received: set meter
;; --------------------------------------------------------------------------
LC_MPROC_Received_D0
	;; value in MIOS_PARAMETER2
	;; coding:
	;; 0hhhllll
	;; hhh:  channel to be addressed (0 thru 7)
	;; llll: level meter:
	;;       0 thru C: level meter 0%..100% (overload not cleared!)
	;;       E       : set overload
	;;       F       : clear overload

	;; our own coding in METER_LEVEL[meter_number] array:
	;; O000llll
	;; O: overload flag
	;; llll: 0 thru C - lever meter 0%..100%

	;; calc address to meter level register
	lfsr	FSR0, METER_LEVEL_BEGIN
	swapf	MIOS_PARAMETER2, W
	andlw	0x07		; mask out meter number
	addwf	FSR0L, F	; add to base address

	;; store masked out llll in TMP1
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	TMP1

	IRQ_DISABLE

	;; if llll == 0xe: set overload
	xorlw	0x0e
	bz	LC_MPROC_Received_D0_SetO
	;; if llll == 0xf: clear overload
	movf	TMP1, W
	xorlw	0x0f
	bz	LC_MPROC_Received_D0_ClrO
LC_MPROC_Received_D0_llll
	;; else set meter level (don't overwrite overload flag)
	movlw	0x80
	andwf	INDF0, F	; clear old llll content
	incf	TMP1, W		; (increment llll by one so that we also see the "00" value)
	iorwf	INDF0, F	; insert new llll content
	rgoto	LC_MPROC_Received_D0_Cont

LC_MPROC_Received_D0_SetO
#if 0
	;; set overload flag
 	bsf	INDF0, 7
#else
	;; set overload flag and set level to highest value (seems to be better with Logic Audio)
	movlw	0x8d
	movwf	INDF0
#endif
	rgoto	LC_MPROC_Received_D0_Cont

LC_MPROC_Received_D0_ClrO
	;; clear overload flag
	bcf	INDF0, 7
	;; 	rgoto	LC_MPROC_Received_D0_Cont

LC_MPROC_Received_D0_Cont
	;; set meter delay counter
	lfsr	FSR0, METER_CTR_BEGIN
	swapf	MIOS_PARAMETER2, W
	andlw	0x07		; mask out meter number
	addwf	FSR0L, F	; add to base address
#if 0
	movlw	15		; according to spec
#else
	movlw	127		; slower (and nicer)
#endif
	movwf	INDF0
	
	;; request meter update
	swapf	MIOS_PARAMETER2, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	iorwf	LC_METER_UPDATE_REQ, F
	btfsc	LC_STAT, LC_STAT_LEDMETERS	; update ledring as well if "meter ledring" option enabled
	iorwf	LC_LEDRING_UPDATE_REQ, F

	IRQ_ENABLE

	return

;; --------------------------------------------------------------------------
;;  B0 3x has been received: set LED ring
;; --------------------------------------------------------------------------
LC_MPROC_Received_B03x
	;; save LED ring position in LEDRING_POS[ledring_number]
	lfsr	FSR0, LEDRING_POS_BEGIN
	movf	MIOS_PARAMETER2, W
	andlw	0x07		; mask out ledring number
	addwf	FSR0L, F	; add to base address
	movff	MIOS_PARAMETER3, INDF0

	;; set appr. update request flag
	movf	MIOS_PARAMETER2, W
	andlw	0x07		; mask out ledring number
	call	MIOS_HLP_GetBitORMask
	iorwf	LC_LEDRING_UPDATE_REQ, F

	return

;; --------------------------------------------------------------------------
;;  B0 4x has been received: set MTC or STATUS digit
;; --------------------------------------------------------------------------
LC_MPROC_Received_B04x
	;; mask out digit position
	movlw	0x0f
	andwf	MIOS_PARAMETER2, F

	;; branch depending on digit position
	movlw	0x0a
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto LC_MPROC_Received_B04x_STATUS
LC_MPROC_Received_B04x_MTC
	goto	LC_DISPLAY_Print_MTC_Digit
LC_MPROC_Received_B04x_STATUS
	;; status digit 0 or 1
	movlw	-0x0a
	addwf	MIOS_PARAMETER2, F
	goto	LC_DISPLAY_Print_STATUS_Digit

