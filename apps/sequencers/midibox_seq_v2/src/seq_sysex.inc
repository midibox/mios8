; $Id$
;
; MIDIboxSEQ SysEx parser
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

SEQ_SYSEX_STATE_MYSYSEX		EQU	7
SEQ_SYSEX_STATE_ACTION		EQU	6
SEQ_SYSEX_STATE_END		EQU	5

SEQ_SYSEX_STATE2_PAT_RECEIVED	EQU	0	; used by Action PAT_[Read/Write]
SEQ_SYSEX_STATE2_WAIT_CHECKSUM	EQU	1	; used by Action PAT_Write

SEQ_SYSEX_STATE2_AH_RECEIVED	EQU	0	; used by Action PAR_[Read/Write]
SEQ_SYSEX_STATE2_AL_RECEIVED	EQU	1	; used by Action PAR_[Read/Write]

SEQ_SYSEX_STATE2_MSB		EQU	2	; used by Action PAT_[Read/Write] and PAR_[Read/Write]


;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc on a timeout
;;  it will used internally when an internal sequence has been parsed or 
;;  at the end of a sysex sequence
;; --------------------------------------------------------------------------
SEQ_SYSEX_ActionInvalid
SEQ_SYSEX_ActionFinished
	SET_BSR	SEQ_BASE
	clrf	SEQ_SYSEX_STATE, BANKED
	clrf	SEQ_SYSEX_STATE2, BANKED
	clrf	SEQ_SYSEX_ACTION, BANKED

	call	MIOS_MPROC_MergerEnable
	rgoto	SEQ_SYSEX_SysExCheck_End

;; --------------------------------------------------------------------------
;;  This sysex parser waits for the MBSEQ Header
;;  IN: received byte in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_SYSEX_Parser
	;; store received byte in SEQ_SYSEX_IN
	SET_BSR	SEQ_BASE
	movf	MIOS_PARAMETER1, W
	movwf	SEQ_SYSEX_IN, BANKED

	;; exit if realtime event has been received
	movlw	0xf8
	cpfslt	SEQ_SYSEX_IN, BANKED
	return

	;; check sysex state
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_MYSYSEX, BANKED, SEQ_SYSEX_Handler

	movf	SEQ_SYSEX_STATE, W, BANKED
	rcall	SEQ_SYSEX_SysExHeaderGet
	cpfseq	SEQ_SYSEX_IN, BANKED
	rgoto SEQ_SYSEX_SysExCheckFailed
	incf	SEQ_SYSEX_STATE, F, BANKED
	movf	SEQ_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 47 <device-id>)
	bnz	SEQ_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	movlw	(1 << SEQ_SYSEX_STATE_MYSYSEX)
	movwf	SEQ_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable
	rgoto	SEQ_SYSEX_SysExCheckOk

SEQ_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rcall	SEQ_SYSEX_ActionInvalid
SEQ_SYSEX_SysExCheckOk

SEQ_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
SEQ_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x47		; MIDIboxSEQ ID (47 - the ultimative number + 5)
	movf	MIDI_DEVICE_ID, W
	return


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
SEQ_SYSEX_Handler
	;; if first byte after header, store action ID
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_ACTION, BANKED, SEQ_SYSEX_Handler_NotBegin
SEQ_SYSEX_Handler_Begin
	bsf	SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_ACTION, BANKED
	movff	SEQ_SYSEX_IN, SEQ_SYSEX_ACTION	; store action ID
	rgoto	SEQ_SYSEX_SysExCheck_End	; branch to the end
SEQ_SYSEX_Handler_NotBegin

	;; fetch data until next command byte (-> 0xf7)
	BRA_IFCLR SEQ_SYSEX_IN, 7, BANKED, SEQ_SYSEX_Handler_NotEnd
SEQ_SYSEX_Handler_End
	;; if sysex footer (F7) has not been received here, action is invalid!
	movlw	0xf7
	cpfseq	SEQ_SYSEX_IN, BANKED
	rgoto SEQ_SYSEX_ActionInvalid
	;; notify end
	bsf	SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED
SEQ_SYSEX_Handler_NotEnd

	;; branch depending on current action ID
	decf	SEQ_SYSEX_ACTION, W, BANKED
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_PAT_Read	; 1
	addlw	-2+1
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_PAT_Write	; 2
	addlw	-3+2
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_PATS_Read	; 3
	addlw	-5+3
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_PAR_Read	; 5
	addlw	-6+5
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_PAR_Write	; 6
	addlw	-15+6
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_Ping		; 15
	rgoto	SEQ_SYSEX_ActionInvalid
	;; (note: target address to far away for "bz", therefore I'm using "IFSET STATUS, Z"
	

;; --------------------------------------------------------------------------
;;  MIDI Action: Pattern Read
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAT_Read
	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAT_Read_End

	;; receive <pattern> F7
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED, SEQ_SYSEX_Action_PAT_ReadStall
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_PATTERN	; copy pattern number to SEQ_PATTERN
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAT_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PAT_Read_End
	;; action invalid if pattern number has not been received
	BRA_IFCLR SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED, SEQ_SYSEX_ActionInvalid

	;; send SysEx dump
	call	SEQ_SYSEX_SendDump

	;; request display re-init
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Pattern Write
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAT_Write
	;; request display update (will be done after the complete SysEx dump has been received)
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAT_Write_End

	;; receive <pattern> <320 bytes> F7
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED, SEQ_SYSEX_Action_PAT_WriteC
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_PATTERN	; copy pattern number to SEQ_PATTERN
	movff	SEQ_SYSEX_IN, SEQ_PATTERN0	; copy pattern number to SEQ_PATTERN0
	clrf	SEQ_SYSEX_CHECKSUM, BANKED	; clear checksum
	clrf	SEQ_SYSEX_ADDRESS, BANKED	; clear address
	bcf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED; just to ensure
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAT_WriteC
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, SEQ_SYSEX_Action_PAT_WriteChk

	;; add received byte to checksum
	movf	SEQ_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED

	;; store received byte directly in pattern depending on address memory
	;; expects: address in SEQ_SYSEX_ADDRESS and SEQ_SYSEX_STATE2_MSB
	;; parameter value in SEQ_SYSEX_IN
	rcall	SEQ_SYSEX_WriteP
	SET_BSR	SEQ_BASE

	;; increment address
	incf	SEQ_SYSEX_ADDRESS, F, BANKED
	skpnz
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED

	;; change to WAIT_CHECKSUM state when 0x140 reached
	BRA_IFCLR SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED, SEQ_SYSEX_SysExCheck_End
	btfsc	SEQ_SYSEX_ADDRESS, 6, BANKED
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PAT_WriteChk
	;; store received byte in checksum (using SEQ_SYSEX_ADDRESS as store register)
	movff	SEQ_SYSEX_IN, SEQ_SYSEX_ADDRESS

	;; wait for next byte
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PAT_Write_End
	;; action invalid if checksum has not been received
	BRA_IFCLR SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, SEQ_SYSEX_ActionInvalid

	;; calc final checksum
	movf	SEQ_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f

	;; compare with received checksum
	xorwf	SEQ_SYSEX_ADDRESS, W, BANKED

	;; if not equal jump to ActionInvalid
	skpz
	rgoto	SEQ_SYSEX_ActionInvalid

	;; write track and pot values to EEPROM (don't overwrite the global settings!)
	call	SEQ_DUMP_StoreTrk
	call	SEQ_DUMP_StorePots

	;; update global settings
	call	SEQ_DUMP_RestoreGlobal

	;; send acknowledge
	rcall	SEQ_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: All Patterns Read
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PATS_Read
	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PATS_Read_End

	;; do nothing until sysex footer (F7) has been received
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PATS_Read_End
	;; send SysEx header
	rcall	SEQ_SYSEX_Send_SysExHeader

	;; send PATS_Write ID
	movlw	0x04
	call	MIOS_MIDI_TxBufferPut

	;; save current pattern in TMP1
	movff	SEQ_PATTERN, TMP1

	;; clear checksum
	SET_BSR	SEQ_BASE
	clrf	SEQ_SYSEX_CHECKSUM, BANKED

	;; 128 patterns to send, SEQ_PATTERN used as counter
	clrf	SEQ_PATTERN, BANKED
SEQ_SYSEX_Action_PATS_Read_OL	; outer loop
	clrwdt				; feed the watchdog
	rcall	SEQ_SYSEX_SendDump_EE	; send pattern content

	;; loop until last pattern reached
	SET_BSR	SEQ_BASE
	incf	SEQ_PATTERN, F, BANKED	; loop 128 times
	BRA_IFCLR SEQ_PATTERN, 7, BANKED, SEQ_SYSEX_Action_PATS_Read_OL

	;; send checksum
	movf	SEQ_SYSEX_CHECKSUM, W
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; restore pattern number which has been saved in TMP1
	movff	TMP1, SEQ_PATTERN

	;; send the SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SEQ_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Read
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAR_Read
	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAR_Read_End

	;; receive <AH> <AL> F7
SEQ_SYSEX_Action_PAR_ReadAH
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_ReadAL
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED
	rcall	SEQ_SYSEX_Action_PAR_Hlp_SetH	; set the high-byte of address
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte
	
SEQ_SYSEX_Action_PAR_ReadAL
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_ReadStall
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED
	movf	SEQ_SYSEX_IN, W, BANKED		; OR SEQ_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	SEQ_SYSEX_ADDRESS, F, BANKED
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAR_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PAR_Read_End
	;; action invalid if low-byte of address has not been received
	BRA_IFCLR SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED, SEQ_SYSEX_ActionInvalid

	;; send SysEx header
	rcall	SEQ_SYSEX_Send_SysExHeader

	;; send PAR_Write ID
	movlw	0x06
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; send AH
	movlw	0x00
	btfsc	SEQ_SYSEX_ADDRESS, 7, BANKED
	iorlw 0x01
	btfsc	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED
	movlw 0x02
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; send AL
	movf	SEQ_SYSEX_ADDRESS, W, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; get parameter value from selected address
	rcall	SEQ_SYSEX_ReadP
	SET_BSR	SEQ_BASE
	
	;; send value
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SEQ_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished

;; ---
	;; help function used by SEQ_SYSEX_Action_PAR_[Read/Write]
SEQ_SYSEX_Action_PAR_Hlp_SetH
	clrf	SEQ_SYSEX_ADDRESS, BANKED	; clear register and set SEQ_SYSEX_ADDRESS[7] if IN[0] is set
	bcf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED

	btfsc	SEQ_SYSEX_IN, 0, BANKED
	bsf	SEQ_SYSEX_ADDRESS, 7, BANKED
	btfsc	SEQ_SYSEX_IN, 1, BANKED
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED
	return

;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAR_Write
	;; request display init (will be done after the complete SysEx dump has been received)
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAR_Write_End

	;; receive <AH> <AL> <value> F7
SEQ_SYSEX_Action_PAR_WriteAH
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_WriteAL
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED
	rcall	SEQ_SYSEX_Action_PAR_Hlp_SetH	; set the high-byte of address
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte
	
SEQ_SYSEX_Action_PAR_WriteAL
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_WriteD
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED
	movf	SEQ_SYSEX_IN, W, BANKED		; OR SEQ_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	SEQ_SYSEX_ADDRESS, F, BANKED
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAR_WriteD
SEQ_SYSEX_Action_PAR_WriteStall
	;; save parameter
	rcall	SEQ_SYSEX_WriteP
	SET_BSR	SEQ_BASE

	;; increment address
	;; (no overrun check here - but uncritical for MBSEQ since there are additional
	;; checks in SEQ_SYSEX_WriteP)
	incf	SEQ_SYSEX_ADDRESS, F, BANKED
	skpnz
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED

	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte


SEQ_SYSEX_Action_PAR_Write_End
	;; action invalid if address has not been received
	BRA_IFCLR SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED, SEQ_SYSEX_ActionInvalid

	;; send acknowledge
	call	SEQ_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_Ping
	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_Ping_End

	;; do nothing until sysex footer (F7) has been received
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_Ping_End
	;; send Acknowledge
	rcall	SEQ_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
SEQ_SYSEX_Send_Acknowledge
	rcall	SEQ_SYSEX_Send_SysExHeader

	movlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rgoto	SEQ_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send MBSEQ SysEx Header (Util function)
;; --------------------------------------------------------------------------
SEQ_SYSEX_Send_SysExHeader
	;; if SEQ_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_MYSYSEX, BANKED, SEQ_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	SEQ_SYSEX_Send_SysExHeader_Skp
	movlw	0x05		; send only DEVICE_ID
	movwf	TMP1
	rgoto	SEQ_SYSEX_Send_SysExHeaderLoop
SEQ_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
SEQ_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	SEQ_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto SEQ_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
SEQ_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, SEQ_SYSEX_Send_SysExFooter_Force
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

SEQ_SYSEX_Send_SysExFooter_Force
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Send a SysEx dump of a single pattern
;; --------------------------------------------------------------------------
SEQ_SYSEX_SendDump
	;; send SysEx header
	rcall	SEQ_SYSEX_Send_SysExHeader

	;; send PAT_Write ID
	movlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send requested pattern number
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	call	MIOS_MIDI_TxBufferPut

	;; clear checksum
	SET_BSR	SEQ_BASE
	clrf	SEQ_SYSEX_CHECKSUM, BANKED

	;; send dump of 0x140 (320) bytes
	rcall	SEQ_SYSEX_SendDump_EE
	
	;; send checksum
	SET_BSR	SEQ_BASE
	movf	SEQ_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rgoto	SEQ_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send the pattern content
;;  USES: EEADR and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_SYSEX_SendDump_EE
	clrf	EEADR

	;; Layer A/B/C
SEQ_SYSEX_SendDump_EE_Loop1
	call	SEQ_BANK_Read
	andlw	0x7f		; add to checksum
	SET_BSR	SEQ_BASE
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	movlw	0xc0-1
	cpfsgt	EEADR, ACCESS
	rgoto SEQ_SYSEX_SendDump_EE_Loop1

	;; CFG data
SEQ_SYSEX_SendDump_EE_Loop2
	call	SEQ_BANK_Read
	movwf	MIOS_PARAMETER1
	andlw	0x0f		; add to checksum
	SET_BSR	SEQ_BASE
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f		; add to checksum
	SET_BSR	SEQ_BASE
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	movf	EEADR, W
	bnz	SEQ_SYSEX_SendDump_EE_Loop2

	return


;; --------------------------------------------------------------------------
;;  This function transfers a parameter into pattern SRAM
;;  IN: low-byte of address in SEQ_SYSEX_ADDRESS
;;      MSB of address in SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB
;;      parameter value in SEQ_SYSEX_IN
;; --------------------------------------------------------------------------
SEQ_SYSEX_WriteP
	;; store value in MIOS_PARAMETER1
	movff	SEQ_SYSEX_IN, MIOS_PARAMETER1

	;; get 5-bit address (CC#)
	movf	SEQ_SYSEX_ADDRESS, W, BANKED
	andlw	0x3f

	;; and branch depending on address
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED, SEQ_SYSEX_WriteP_CFG
	BRA_IFSET SEQ_SYSEX_ADDRESS, 7, BANKED, SEQ_SYSEX_WriteP_LC_CFG
	BRA_IFSET SEQ_SYSEX_ADDRESS, 6, BANKED, SEQ_SYSEX_WriteP_LB
SEQ_SYSEX_WriteP_LA
	;; 0x000-0x03f: store value in Layer A
	goto	SEQ_CCIN_SetLayerA

SEQ_SYSEX_WriteP_LB
	;; 0x040-0x07f: store value in Layer B
	goto	SEQ_CCIN_SetLayerB

SEQ_SYSEX_WriteP_LC_CFG
	BRA_IFSET SEQ_SYSEX_ADDRESS, 6, BANKED, SEQ_SYSEX_WriteP_CFG
SEQ_SYSEX_WriteP_LC
	;; 0x080-0x0bf: store value in Layer C
	goto	SEQ_CCIN_SetLayerC

SEQ_SYSEX_WriteP_CFG
	;; 0x0c0-0x13f: store value in CFG
	;; set 6th bit depending on MSB
	btfsc	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED
	bsf	WREG, 6
	goto	SEQ_CCIN_Set


;; --------------------------------------------------------------------------
;;  This function returns a parameter depending on SysEx address
;;  IN: low-byte of address in SEQ_SYSEX_ADDRESS
;;      MSB of address in SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB
;;  OUT: parameter value in WREG
;; --------------------------------------------------------------------------
SEQ_SYSEX_ReadP
	;; get 5-bit address (CC#)
	movf	SEQ_SYSEX_ADDRESS, W, BANKED
	andlw	0x3f

	;; and branch depending on address
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED, SEQ_SYSEX_ReadP_CFG
	BRA_IFSET SEQ_SYSEX_ADDRESS, 7, BANKED, SEQ_SYSEX_ReadP_LC_CFG
	BRA_IFSET SEQ_SYSEX_ADDRESS, 6, BANKED, SEQ_SYSEX_ReadP_LB
SEQ_SYSEX_ReadP_LA
	;; 0x000-0x03f: get value from Layer A
	goto	SEQ_CCOUT_GetLayerA

SEQ_SYSEX_ReadP_LB
	;; 0x040-0x07f: get value from Layer B
	goto	SEQ_CCOUT_GetLayerB

SEQ_SYSEX_ReadP_LC_CFG
	BRA_IFSET SEQ_SYSEX_ADDRESS, 6, BANKED, SEQ_SYSEX_ReadP_CFG
SEQ_SYSEX_ReadP_LC
	;; 0x080-0x0bf: get value from Layer C
	goto	SEQ_CCOUT_GetLayerC

SEQ_SYSEX_ReadP_CFG
	;; 0x0c0-0x13f: store value in CFG
	;; set 6th bit depending on MSB
	btfsc	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_MSB, BANKED
	bsf	WREG, 6
	goto	SEQ_CCOUT_Get
