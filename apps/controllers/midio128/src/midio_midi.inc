; $Id$
;
; MIDIO128
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
MIDIO_MIDI_NotifyReceivedEvent
	;; store last received event in MIDIO_LAST_RECEIVED_EVNT*
	movff	MIOS_PARAMETER1, MIDIO_LAST_RECEIVED_EVNT0
	movff	MIOS_PARAMETER2, MIDIO_LAST_RECEIVED_EVNT1
	movff	MIOS_PARAMETER3, MIDIO_LAST_RECEIVED_EVNT2

	;; request display update
 	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

	;; branch to MIDI event scanner
	rgoto	MIDIO_MIDI_Scan

;; --------------------------------------------------------------------------
;;  This function searches for matching pot/DIN events and copies the
;;  received parameter value into the appr. registers
;;
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
MIDIO_MIDI_Scan
	;; if note Off event has been received, convert it to Note On with velocity == 0
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	TMP2
	xorlw	0x80
	bnz	MIDIO_MIDI_Scan_NoNoteOff
MIDIO_MIDI_Scan_NoteOff
	movlw	0x90
	iorwf	MIOS_PARAMETER1, F
	clrf	MIOS_PARAMETER3
MIDIO_MIDI_Scan_NoNoteOff

	;; set TMP5[0] if the second byte shouldn't be checked; this is the
	;; case for:
	;;    o Channel Pressure
	;;    o Pitch Bend
	bcf	TMP5, 0
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

	;; if program change branch depending on MIDIO_CFG0_ALT_PROGCHNG
	bcf	TMP5, 2		; controls TMP5[2] and TMP5[3]
	bcf	TMP5, 3

	movf	TMP2, W
	xorlw	0xc0
	bnz	MIDIO_MIDI_Scan_NoAltChng
	;; branch depending on this flag
	SET_BSR	MIDIO_BASE
	BRA_IFSET MIDIO_CFG0, MIDIO_CFG0_ALT_PROGCHNG, BANKED, MIDIO_MIDI_Scan_AltChng1
MIDIO_MIDI_Scan_AltChng0
	;; set TMP5[2], so that the DOUT will be toggled
	bsf	TMP5, 2
	rgoto	MIDIO_MIDI_Scan_AltChngCont
MIDIO_MIDI_Scan_AltChng1
	;; set TMP5[3], so that all DOUTs assigned to the Program Change event will be
	;; cleared - execept for DOUT which matches with the Program Change number
	bsf	TMP5, 3
	bsf	TMP5, 0		; ignore second byte - will be checked in a special branch
	;; 	rgoto	MIDIO_MIDI_Scan_NoAltChng
MIDIO_MIDI_Scan_AltChngCont
MIDIO_MIDI_Scan_NoAltChng

	;; compare value for first event byte -> TMP4
	;; bit #7 of the first event byte is forced to zero in bank
	;; so we have to fix MIOS_PARAMETER1 here
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	movwf	TMP4

	;; now scan the DOUT events
	;; stored in internal flash (!) only (!!)
	TABLE_ADDR MIDIO_Presets_OffsetIn
	clrf	TMP1		; (128 events)
MIDIO_MIDI_ScanDOUTsLoop
MIDIO_MIDI_ScanDOUTsLoop_ChkE0
	tblrd*+			; compare Evnt0 with first byte
	movf	TABLAT, W
	xorwf	TMP4, W
	bz	MIDIO_MIDI_ScanDOUTsLoop_ChkE1
	tblrd*+			; (dummy read to increment pointer of Evnt1)
	rgoto	MIDIO_MIDI_ScanDOUTsLoopNext

MIDIO_MIDI_ScanDOUTsLoop_ChkE1
	tblrd*+			; compare Evnt1 with second byte
	BRA_IFSET TMP5, 0, ACCESS, MIDIO_MIDI_ScanDOUTsLoop_Found; (don't compare when TMP5[0] set)
	movf	TABLAT, W
	xorwf	MIOS_PARAMETER2, W
	bnz	MIDIO_MIDI_ScanDOUTsLoopNext

	;; Evnt0 and Evnt1 are matching - turn on/off the DOUT
MIDIO_MIDI_ScanDOUTsLoop_Found
	movf	MIOS_PARAMETER3, W
	btfsc	TMP5, 1		; TMP5[1] selects the value
	movf	MIOS_PARAMETER2, W
	movwf	TMP3		; TMP3 could be used for later analysis,
	;; NOTE: TMP3 can be used for further analysis, for example
	;; to set different DOUTs depending on the received value

	;; TMP5[2] set, if DOUT should be toggled
	BRA_IFCLR TMP5, 2, ACCESS, MIDIO_MIDI_ScanDOUTsLoopNoTog
MIDIO_MIDI_ScanDOUTsLoopTog
	;; get current value of DOUT and branch to Found0 or Found1 (toggle value)
	movf	TMP1, W
	call	MIDIO_DOUT_Get
	bz	MIDIO_MIDI_ScanDOUTsLoopFound1
	rgoto	MIDIO_MIDI_ScanDOUTsLoopFound0
MIDIO_MIDI_ScanDOUTsLoopNoTog

	;; TMP5[3] cleared: normal behaviour - if 0x00 received: clear pin, else set pin
	;; TMP5[3] set: all DOUTs assigned to the Program Change event will be
	;; cleared - execept for DOUT which matches with the Program Change number
	BRA_IFCLR TMP5, 3, ACCESS, MIDIO_MIDI_ScanDOUTsLoopNoPC
MIDIO_MIDI_ScanDOUTsLoopPC
	movf	TMP3, W
	xorwf	TABLAT, W
	bz	MIDIO_MIDI_ScanDOUTsLoopFound1
	rgoto	MIDIO_MIDI_ScanDOUTsLoopFound0
MIDIO_MIDI_ScanDOUTsLoopNoPC

	movf	TMP3, W
	bnz	MIDIO_MIDI_ScanDOUTsLoopFound1
MIDIO_MIDI_ScanDOUTsLoopFound0
	;; clear DOUT
	movf	TMP1, W
	call	MIDIO_DOUT_Clear; (this function takes the polarity into account)
	rgoto	MIDIO_MIDI_ScanDOUTsLoopFoundC
	
MIDIO_MIDI_ScanDOUTsLoopFound1
	;; set DOUT
	movf	TMP1, W
	call	MIDIO_DOUT_Set; (this function takes the polarity into account)
	;; 	rgoto	MIDIO_MIDI_ScanDOUTsLoopFoundC
	
MIDIO_MIDI_ScanDOUTsLoopFoundC
	;; request display update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	
MIDIO_MIDI_ScanDOUTsLoopNext
	incf	TMP1, F
	BRA_IFCLR TMP1, 7, ACCESS, MIDIO_MIDI_ScanDOUTsLoop

	return


;; --------------------------------------------------------------------------
;;  This function puts a DIN event into MIDI_EVNT[01] and MIDI_EVNT_VALUE
;;  used by MIDI and LCD functions
;;  Input: number of pot in MIDIO_CURRENT_DIN
;; --------------------------------------------------------------------------
MIDIO_MIDI_PrepareDINEvent
	;; copy first byte to MIDI_EVNT0
	;; copy second byte to MIDI_EVNT1
	;; copy value to MIDI_EVNT_VALUE

	;; get BANK address to DIN entry
	call	MIDIO_ADDR_DINEntry
	
	;; select On/Off entry depending on DIN status
	call	MIDIO_FSR_DINValue
	movff	MIDIO_CURRENT_DIN, WREG
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	bz	MIDIO_MIDI_PrepareDINEventOn
MIDIO_MIDI_PrepareDINEventOff
	;; off event: add 3 to address
	movlw	0x03
	addwf	MB_ADDRL, F
	movlw	0x00
	addwfc	MB_ADDRH, F
MIDIO_MIDI_PrepareDINEventOn

	;; copy the three bytes to MIDI_EVNT*
	call	MIDIO_BANK_Read
	iorlw	0x80		; force status bit
	movff	WREG, MIDI_EVNT0
	call	MIDIO_BANK_Read
	movff	WREG, MIDI_EVNT1
	call	MIDIO_BANK_Read
	movff	WREG, MIDI_EVNT_VALUE

	;; on "program change" or "channel preassure": copy 2nd byte to 3rd byte
	SET_BSR	MIDI_EVNT0
	movf	MIDI_EVNT0, W, BANKED
	andlw	0xf0		; mask out upper bits
	xorlw	0xc0		; compare with 0xc0
	bz	MIDIO_MIDI_PrepareDINEvent_Cx
	xorlw	0xc0		; (undo the previous xor)
	xorlw	0xd0		; compare with 0xd0
	bz	MIDIO_MIDI_PrepareDINEvent_Dx
	rgoto	MIDIO_MIDI_PrepareDINEvent_End
MIDIO_MIDI_PrepareDINEvent_Cx
MIDIO_MIDI_PrepareDINEvent_Dx
	movff	MIDI_EVNT1, MIDI_EVNT_VALUE
MIDIO_MIDI_PrepareDINEvent_End
	return

;; --------------------------------------------------------------------------
;;  This function sends a DIN event
;;  Input: number of DIN in MIDIO_CURRENT_DIN
;; --------------------------------------------------------------------------
MIDIO_MIDI_SendDINEvent	
	rcall	MIDIO_MIDI_PrepareDINEvent

	;; if meta event: branch to meta handler, else to MIDI sender
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xf0
	skpnz
	goto	MIDIO_META_Handler_DIN
	goto	MIDI_EVNT_Send
