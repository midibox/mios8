; $Id$
;
; MIDIbox64E
; MIDI Interface part
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called by MB_MPROC when a complete MIDI event has been
;;  received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MB64E_MIDI_NotifyReceivedEvent
	;; branch to MIDI learn handler if enabled
	GOTO_IFSET MB_STAT, MB_STAT_LEARN_MODE, ACCESS, CS_M_LEARN_NotifyReceivedEvent

	;; if RECEIVE_PC_FOR_BCHNGE flag is set, check if channel matches and switch bank in this case
	SET_BSR	MB64E_BASE
	BRA_IFCLR MB64E_CFG1, MB64E_CFG1_RECEIVE_PC_FOR_BCHNGE, BANKED, MB64E_MIDI_NotifyReceivedEventNP
MB64E_MIDI_NotifyReceivedEventPC
	;; compare with expected program change for bank changes
	movlw	0xc0 | ((DEFAULT_BANK_PRG_CHNG_CHANNEL-1) & 0xf)
	xorwf	MIOS_PARAMETER1, W
	bnz	MB64E_MIDI_NotifyReceivedEventNB
	;; change bank
	movf	MIOS_PARAMETER2, W
	call	MB64E_BANK_Change
MB64E_MIDI_NotifyReceivedEventNB
	;; compare with expected program change for patch changes
	movlw	0xc0 | ((DEFAULT_PATCH_PRG_CHNG_CHANNEL-1) & 0xf)
	xorwf	MIOS_PARAMETER1, W
	bnz	MB64E_MIDI_NotifyReceivedEventNP
	;; change patch
	movf	MIOS_PARAMETER2, W
	call	MB64E_PATCH_Change
MB64E_MIDI_NotifyReceivedEventNP
MB64E_MIDI_NotifyReceivedEvntNPC

	;; branch to MIDI event scanner
	rgoto	MB64E_MIDI_Scan

;; --------------------------------------------------------------------------
;;  This function searches for matching enc/button events and copies the
;;  received parameter value into the appr. registers
;;
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MB64E_MIDI_Scan
	;; if note Off event has been received, convert it to Note On with velocity == 0
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	TMP2
	xorlw	0x80
	bnz	MB64E_MIDI_Scan_NoNoteOff
MB64E_MIDI_Scan_NoteOff
	movlw	0x90
	iorwf	MIOS_PARAMETER1, F
	clrf	MIOS_PARAMETER3
MB64E_MIDI_Scan_NoNoteOff

	;; set TMP5[0] if the second byte shouldn't be checked; this is the
	;; case for:
	;;    o Program Change
	;;    o Channel Pressure
	;;    o Pitch Bend
	bcf	TMP5, 0
	movf	TMP2, W		; (TMP2 contains masked status byte (without channel))
	xorlw	0xc0		; Program Change
	skpnz
	bsf	TMP5, 0
	movf	TMP2, W
	xorlw	0xd0		; Channel Pressure
	skpnz
	bsf	TMP5, 0
	movf	TMP2, W
	xorlw	0xe0		; Pitch Bend
	skpnz
	bsf	TMP5, 0

	;; set TMP5[1] if the value is located in second, and not in third byte
	;; this is the case for:
	;;    o Program Change
	;;    o Channel Pressure
	bcf	TMP5, 1
	movf	TMP2, W
	xorlw	0xc0		; Program Change
	skpnz
	bsf	TMP5, 1
	movf	TMP2, W
	xorlw	0xd0		; Channel Pressure
	skpnz
	bsf	TMP5, 1


	;; now scan the enc events
	lfsr	FSR0, MB64E_ENC_EVNT0_00
	lfsr	FSR1, MB64E_ENC_EVNT1_00
	clrf	TMP1		; (64 events)
MB64E_MIDI_ScanEncsLoop
MB64E_MIDI_ScanEncsLoop_ChkEvnt0
	movf	POSTINC0, W	; compare Evnt0 with first byte
	xorwf	MIOS_PARAMETER1, W
	bz	MB64E_MIDI_ScanEncsLoop_ChkEvnt1
	movf	POSTINC1, W	; (dummy read to increment pointer of Evnt1)
	rgoto	MB64E_MIDI_ScanEncsLoopNext

MB64E_MIDI_ScanEncsLoop_ChkEvnt1
	movf	POSTINC1, W	; compare Evnt1 with second byte
	BRA_IFSET TMP5, 0, ACCESS, MB64E_MIDI_ScanEncsLoop_Found; (don't compare when TMP5[0] set)
	xorwf	MIOS_PARAMETER2, W
	bnz	MB64E_MIDI_ScanEncsLoopNext

	;; Evnt0 and Evnt1 are matching - copy received value into register
MB64E_MIDI_ScanEncsLoop_Found
	lfsr	FSR0, MB64E_ENC_VALUES_00
	movf	TMP1, W

	movff	PLUSW0, TMP2	; save last value in TMP2

	btfss	TMP5, 1		; TMP5[1] selects the value
	movff	MIOS_PARAMETER3, PLUSW0
	btfsc	TMP5, 1
	movff	MIOS_PARAMETER2, PLUSW0

	;; save value and set snap bit
	movf	TMP1, W
	bsf	PLUSW0, 7

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; request "soft" fader pos update
	bsf	MB_STAT3, MB_STAT3_FADER_SOFT_UPDATE_REQ
	bsf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ

	;; restore FSR0
	lfsr	FSR0, MB64E_ENC_EVNT0_00
	incf	TMP1, W
	addwf	FSR0L, F
	
MB64E_MIDI_ScanEncsLoopNext
	incf	TMP1, F
	BRA_IFCLR TMP1, 7, ACCESS, MB64E_MIDI_ScanEncsLoop


	;; now scan the button events
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_BUTTON_EVNT0_00
	lfsr	FSR1, MB64E_BUTTON_EVNT1_00
	clrf	TMP1		; (64 events)
MB64E_MIDI_ScanButtonsLoop
MB64E_MIDI_ScanButtonsLoop_ChkE0
	movf	POSTINC0, W	; compare Evnt0 with first byte
	xorwf	MIOS_PARAMETER1, W
	bz	MB64E_MIDI_ScanButtonsLoop_ChkE1
	movf	POSTINC1, W	; (dummy read to increment pointer of Evnt1)
	rgoto	MB64E_MIDI_ScanButtonsLoopNext

MB64E_MIDI_ScanButtonsLoop_ChkE1
	movf	POSTINC1, W	; compare Evnt1 with second byte
	BRA_IFSET TMP5, 0, ACCESS, MB64E_MIDI_ScanButtonsLoop_Found; (don't compare when TMP5[0] set)
	xorwf	MIOS_PARAMETER2, W
	bnz	MB64E_MIDI_ScanButtonsLoopNext

	;; Evnt0 and Evnt1 are matching - copy received value into register
MB64E_MIDI_ScanButtonsLoop_Found
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
	lfsr	FSR1, MB64E_MBUTTON_VALUES_SR0

	movf	MIOS_PARAMETER3, W
	btfsc	TMP5, 1		; TMP5[1] selects the value
	movf	MIOS_PARAMETER2, W
	movwf	TMP3		; TMP3 could be used for later analysis,
	;; NOTE: TMP3 can be used for further analysis, for example
	;; to set different LEDs depending on the received value

	;; here we just set/clear the button flag
	;; select shift register (TMP1 >> 3)
	rrf	TMP1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x07
	addwf	FSR0L, F
	addwf	FSR1L, F

	;; note: button flags are saved low-active. means: 0=pressed, 1=depressed
	movf	TMP3, W
	bz	MB64E_MIDI_ScanButtonsLoop_Fnd1
MB64E_MIDI_ScanButtonsLoop_Fnd0
	;; clear (== button pressed)
	movf	TMP1, W
	call	MIOS_HLP_GetBitANDMask
	movwf	TMP2

#if 0	
	andwf	INDF0, F	; (MB64E_BUTTON_VALUES_SR0)
#endif

	andwf	INDF1, F	; (MB64E_MBUTTON_VALUES_SR0)
	rgoto	MB64E_MIDI_ScanButtonsLoop_FndC
	
MB64E_MIDI_ScanButtonsLoop_Fnd1
	;; set (== button depressed):	
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask

#if 0
	iorwf	INDF0, F	; (MB64E_BUTTON_VALUES_SR0)
#endif

	iorwf	INDF1, F	; (MB64E_MBUTTON_VALUES_SR0)
	;; 	rgoto	MB64E_MIDI_ScanButtonsLoop_FndC
	
MB64E_MIDI_ScanButtonsLoop_FndC
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; restore FSR0
	lfsr	FSR0, MB64E_BUTTON_EVNT0_00
	incf	TMP1, W
	addwf	FSR0L, F
	
MB64E_MIDI_ScanButtonsLoopNext
	incf	TMP1, F
	BRA_IFCLR TMP1, 6, ACCESS, MB64E_MIDI_ScanButtonsLoop

	return

;; --------------------------------------------------------------------------
;;  This function is used to feedback a sent value
;;  Required if several bank entries are assigned to the same event
;;
;;  Input:
;;     o first  MIDI event byte in MIDI_EVNT0
;;     o second MIDI event byte in MIDI_EVNT1
;;     o third  MIDI event byte in MIDI_EVNT_VALUE
;; --------------------------------------------------------------------------
MB64E_MIDI_Scan_Feedback
	movff	MIDI_EVNT0, MIOS_PARAMETER1
	movff	MIDI_EVNT1, MIOS_PARAMETER2
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER3
	rgoto	MB64E_MIDI_Scan


;; --------------------------------------------------------------------------
;;  This function puts a enc/fader event into MIDI_EVNT[01] and MIDI_EVNT_VALUE
;;  used by MIDI and LCD functions
;;  Input: number of enc in MB64E_CURRENT_ENC
;; --------------------------------------------------------------------------
MB64E_MIDI_PrepareEncEvent
MB64E_MIDI_PrepareFaderEvent
	;; copy first byte to MIDI_EVNT0
	;; copy second byte to MIDI_EVNT1
	;; copy value to MIDI_EVNT_VALUE
	call	MB64E_FSR_EncEvnt0
	movff	INDF0, MIDI_EVNT0
	call	MB64E_FSR_EncEvnt1
	movff	INDF0, MIDI_EVNT1
	call	MB64E_FSR_EncValue
	movf	INDF0, W
	andlw	0x7f
	movwf	MIDI_EVNT_VALUE
	return

;; --------------------------------------------------------------------------
;;  This function sends an enc event with absolute value
;;  Input: number of enc in MB64E_CURRENT_ENC
;;         encoder mode in MB64E_CURRENT_ENC_MODE
;; --------------------------------------------------------------------------
MB64E_MIDI_SendEncEvent
	;; prepare event
	rcall	MB64E_MIDI_PrepareEncEvent
	;; if meta event: branch to meta handler, else to MIDI sender
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xf0
	skpnz
	goto	MB64E_META_Handler_Enc

	;; now branch depending on encoder mode stored in MB64E_CURRENT_ENC_MODE
	SET_BSR	MB64E_BASE
	movf	MB64E_CURRENT_ENC_MODE, W, BANKED
	JUMPTABLE_2BYTES 6 ; entries
	rgoto	MB64E_MIDI_SendEncEvent_M0	; == ENC_MODE_ABSOLUTE
	rgoto	MB64E_MIDI_SendEncEvent_M1	; == ENC_MODE_40SPEED
	rgoto	MB64E_MIDI_SendEncEvent_M2	; == ENC_MODE_00SPEED
	rgoto	MB64E_MIDI_SendEncEvent_M3	; == ENC_MODE_40_1
	rgoto	MB64E_MIDI_SendEncEvent_M4	; == ENC_MODE_00_1
	rgoto	MB64E_MIDI_SendEncEvent_M5	; == ENC_MODE_INCDEC


MB64E_MIDI_SendEncEvent_M0	; == ENC_MODE_ABSOLUTE
	rgoto	MB64E_MIDI_SendEncEvent_Send

MB64E_MIDI_SendEncEvent_M1	; == ENC_MODE_40SPEED
	movf	MIDI_EVNT_VALUE, W
	addlw	0x40
	andlw	0x7f
	movwf	MIDI_EVNT_VALUE
	rgoto	MB64E_MIDI_SendEncEvent_Send

MB64E_MIDI_SendEncEvent_M2	; == ENC_MODE_00SPEED
	;; (incrementer already stored in MIDI_EVNT_VALUE)
	rgoto	MB64E_MIDI_SendEncEvent_Send

MB64E_MIDI_SendEncEvent_M3	; == ENC_MODE_40_1
	movlw	0x41
	btfsc	MIDI_EVNT_VALUE, 6
	movlw 0x3f
	movwf	MIDI_EVNT_VALUE
	rgoto	MB64E_MIDI_SendEncEvent_Send

MB64E_MIDI_SendEncEvent_M4	; == ENC_MODE_00_1
	movlw	0x01
	btfsc	MIDI_EVNT_VALUE, 6
	movlw 0x7f
	movwf	MIDI_EVNT_VALUE
	rgoto	MB64E_MIDI_SendEncEvent_Send
	
MB64E_MIDI_SendEncEvent_M5	; == ENC_MODE_INCDEC
	;; Inc/Dec according to MIDI spec
	;; Inc: CC 0x60 <number>
	;; Dec: CC 0x61 <number>
	;; the second MIDI byte will be taken as number, so up to 128 controllers are addressible in this way
	movlw	0x60
	btfsc	MIDI_EVNT_VALUE, 6
	movlw 0x61
	movff	MIDI_EVNT1, MIDI_EVNT_VALUE
	movwf	MIDI_EVNT1
	;; 	rgoto	MB64E_MIDI_SendEncEvent_Send

MB64E_MIDI_SendEncEvent_Send
	rgoto	MB64E_MIDI_Send

;; --------------------------------------------------------------------------
;;  This function sends an fader event with absolute value
;;  Input: number of fader in MB64E_CURRENT_FADER
;; --------------------------------------------------------------------------
MB64E_MIDI_SendFaderEvent
	;; prepare event
	rcall	MB64E_MIDI_PrepareFaderEvent

	;; if meta event: branch to meta handler, else to MIDI sender
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xf0
	skpnz
	goto	MB64E_META_Handler_Fader
	rgoto	MB64E_MIDI_Send

;; --------------------------------------------------------------------------
;;  This function puts a button event into MIDI_EVNT[01] and MIDI_EVNT_VALUE
;;  used by MIDI and LCD functions
;;  Input: number of enc in MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_MIDI_PrepareButtonEvent
	;; copy first byte to MIDI_EVNT0
	;; copy second byte to MIDI_EVNT1
	;; copy value to MIDI_EVNT_VALUE
	call	MB64E_FSR_ButtonEvnt0
	movff	INDF0, MIDI_EVNT0
	call	MB64E_FSR_ButtonEvnt1
	movff	INDF0, MIDI_EVNT1
	;; get address to button entry
	call	MB64E_ADDR_ButtonEntry
	;; +2
	movlw	2
	addwf	MB_ADDRL, F
	;; get third event byte (used when button pressed)
	call	MB64E_BANK_Read
	movff	WREG, MIDI_EVNT_VALUE

	;; get button state, clear third byte if button depressed
	call	MB64E_FSR_ButtonValue
	movff	MB64E_CURRENT_BUTTON, WREG
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	SET_BSR	MIDI_EVNT_VALUE
	skpz
	clrf	MIDI_EVNT_VALUE, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function sends a button event
;;  Input: number of button in MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_MIDI_SendButtonEvent	
	rcall	MB64E_MIDI_PrepareButtonEvent

	;; don't send special function (0xff)
	incf	MIDI_EVNT0, W
	skpnz
	return

	;; if meta event: branch to meta handler, else to MIDI sender
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xf0
	skpnz
	goto	MB64E_META_Handler_Button
	rgoto	MB64E_MIDI_Send


;; --------------------------------------------------------------------------
;;  This function sends a MIDI event
;;  Input: MIDI event in MIDI_EVNT[01] and MIDI_EVNT_VALUE
;; --------------------------------------------------------------------------
MB64E_MIDI_Send
	;; don't send if disabled event (status == 0x80)
	movf	MIDI_EVNT0, W
	andlw	0x70
	skpnz
	return

	;; else branch to standard MIDI_EVNT function
	goto	MIDI_EVNT_Send

