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

SEQ_SYSEX_STATE2_BNK_RECEIVED	EQU	0	; used by Action PAT_[Read/Write]
SEQ_SYSEX_STATE2_PAT_RECEIVED	EQU	1	; used by Action PAT_[Read/Write]
SEQ_SYSEX_STATE2_WAIT_CHECKSUM	EQU	2	; used by Action PAT_Write

SEQ_SYSEX_STATE2_GRP_RECEIVED	EQU	0	; used by Action PAR_[Read/Write]
SEQ_SYSEX_STATE2_AH_RECEIVED	EQU	1	; used by Action PAR_[Read/Write]
SEQ_SYSEX_STATE2_AL_RECEIVED	EQU	2	; used by Action PAR_[Read/Write]

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
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 4A <device-id>)
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
	retlw	0x4a		; MIDIboxSEQ V3 ID (4A - the ultimative number + 8)
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
	BRA_IFSET STATUS, Z, ACCESS, SEQ_SYSEX_Action_Ping	; 15
	rgoto	SEQ_SYSEX_ActionInvalid
	;; (note: target address to far away for "bz", therefore I'm using "IFSET STATUS, Z"
	

;; --------------------------------------------------------------------------
;;  MIDI Action: Pattern Read
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAT_Read
	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAT_Read_End

	;; receive <bank> <pattern> F7
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_BNK_RECEIVED, BANKED, SEQ_SYSEX_Action_PAT_ReadPat
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_BNK_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_PATTERN_BANK	; copy pattern number to SEQ_PATTERN_BANK
	movff	SEQ_SYSEX_IN, SEQ_PATTERN_BANK0	; copy pattern number to SEQ_PATTERN_BANK0
	call	SEQ_DUMP_FillCache		; fill the whole cache
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAT_ReadPat
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED, SEQ_SYSEX_Action_PAT_ReadStall
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_PATTERN	; copy pattern number to SEQ_PATTERN
	movff	SEQ_SYSEX_IN, SEQ_PATTERN0	; copy pattern number to SEQ_PATTERN0
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAT_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PAT_Read_End
	;; action invalid if pattern number has not been received
	BRA_IFCLR SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED, SEQ_SYSEX_ActionInvalid

	;; send SysEx dump
	;; (bank in SEQ_PATTERN_BANK, pattern number in SEQ_PATTERN)
	call	SEQ_SYSEX_SendDump

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

	;; receive <bank> <pattern> <640 bytes> F7
SEQ_SYSEX_Action_PAT_WriteBnk
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_BNK_RECEIVED, BANKED, SEQ_SYSEX_Action_PAT_WritePat
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_BNK_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_SAVE_PATTERN_BANK	; copy pattern number to SEQ_SAVE_PATTERN_BANK
	movff	SEQ_SYSEX_IN, SEQ_PATTERN_BANK0	; copy pattern number to SEQ_PATTERN_BANK0
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAT_WritePat
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED, SEQ_SYSEX_Action_PAT_WriteC
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_PAT_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_SAVE_PATTERN	; copy pattern number to SEQ_SAVE_PATTERN
	movff	SEQ_SYSEX_IN, SEQ_PATTERN0	; copy pattern number to SEQ_PATTERN0
	clrf	SEQ_SYSEX_CHECKSUM, BANKED	; clear checksum
	clrf	SEQ_SYSEX_ADDRESS_L, BANKED	; clear address
	clrf	SEQ_SYSEX_ADDRESS_H, BANKED
	clrf	SEQ_GROUP, BANKED		; select first pattern group P1
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAT_WriteC
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, SEQ_SYSEX_Action_PAT_WriteChk

	;; add received byte to checksum
	movf	SEQ_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED

	;; store received byte directly in pattern depending on address memory
	;; expects: address in SEQ_SYSEX_ADDRESS_[LH]
	;; parameter value in SEQ_SYSEX_IN
	rcall	SEQ_SYSEX_WriteP
	SET_BSR	SEQ_BASE

	;; increment address
	incf	SEQ_SYSEX_ADDRESS_L, F, BANKED
	skpnz
	incf	SEQ_SYSEX_ADDRESS_H, F, BANKED

	;; change to WAIT_CHECKSUM state when 0x280 reached
	movf	SEQ_SYSEX_ADDRESS_H, W, BANKED
	xorlw	0x2
	bnz	SEQ_SYSEX_SysExCheck_End
	movf	SEQ_SYSEX_ADDRESS_L, W, BANKED
	xorlw	0x80
	bnz	SEQ_SYSEX_SysExCheck_End
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PAT_WriteChk
	;; store received byte in checksum (using SEQ_SYSEX_ADDRESS_L as store register)
	movff	SEQ_SYSEX_IN, SEQ_SYSEX_ADDRESS_L

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
	xorwf	SEQ_SYSEX_ADDRESS_L, W, BANKED

	;; if not equal jump to ActionInvalid
	skpz
	rgoto	SEQ_SYSEX_ActionInvalid

	;; write track and pot values to EEPROM (don't overwrite the global settings!)
	call	SEQ_DUMP_StoreTrk
	call	SEQ_DUMP_StorePots

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

	;; receive <bank> F7
SEQ_SYSEX_Action_PATS_ReadBnk
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_BNK_RECEIVED, BANKED, SEQ_SYSEX_Action_PATS_ReadStall
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_BNK_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_PATTERN_BANK	; copy pattern number to SEQ_PATTERN_BANK
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PATS_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SEQ_SYSEX_SysExCheck_End

SEQ_SYSEX_Action_PATS_Read_End

	;; send the SysEx footer if merger active
	movlw	0x00		; (depending on merger state)
	rcall	SEQ_SYSEX_Send_SysExFooter

	;; 128 patterns to send
	clrf	SEQ_PATTERN, BANKED
SEQ_SYSEX_Action_PATS_ReadLoop
	rcall	SEQ_SYSEX_SendDump
	
	SET_BSR	SEQ_BASE
	incf	SEQ_PATTERN, F, BANKED
	BRA_IFCLR SEQ_PATTERN, 7, BANKED, SEQ_SYSEX_Action_PATS_ReadLoop

	;; finish Action
	rgoto	SEQ_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Read
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAR_Read
	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAR_Read_End

	;; receive <group> <AH> <AL> F7
SEQ_SYSEX_Action_PAR_ReadGrp
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_GRP_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_ReadAH
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_GRP_RECEIVED, BANKED
	movf	SEQ_SYSEX_IN, W, BANKED		; set group
	andlw	0x03
	movwf	SEQ_GROUP, BANKED
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte
	
SEQ_SYSEX_Action_PAR_ReadAH
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_ReadAL
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_SYSEX_ADDRESS_H	; set the high-byte of address
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte
	
SEQ_SYSEX_Action_PAR_ReadAL
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_ReadStall
	;; convert 2*7bit address to 16bit address
	movf	SEQ_SYSEX_IN, W, BANKED		; OR SEQ_SYSEX_ADDRESS with low-byte
	movwf	SEQ_SYSEX_ADDRESS_L, BANKED
	btfsc	SEQ_SYSEX_ADDRESS_H, 0, BANKED
	bsf	SEQ_SYSEX_ADDRESS_L, 7, BANKED
	clrc
	rrf	SEQ_SYSEX_ADDRESS_H, W, BANKED
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

	;; send group
	movf	SEQ_GROUP, W, BANKED
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; send AH
	clrc
	rrf	SEQ_SYSEX_ADDRESS_H, W, BANKED
	btfsc	SEQ_SYSEX_ADDRESS_L, 7, BANKED
	iorlw 0x01
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; send AL
	movf	SEQ_SYSEX_ADDRESS_L, W, BANKED
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

;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
SEQ_SYSEX_Action_PAR_Write
	;; request display init (will be done after the complete SysEx dump has been received)
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; end of stream?
	BRA_IFSET SEQ_SYSEX_STATE, SEQ_SYSEX_STATE_END, BANKED, SEQ_SYSEX_Action_PAR_Write_End

	;; receive <groups> <AH> <AL> <value> F7
SEQ_SYSEX_Action_PAR_WriteGrp
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_GRP_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_WriteAH
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_GRP_RECEIVED, BANKED
	movf	SEQ_SYSEX_IN, W, BANKED		; set group
	andlw	0x03
	movwf	SEQ_GROUP, BANKED
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAR_WriteAH
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_WriteAL
	bsf	SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AH_RECEIVED, BANKED
	movff	SEQ_SYSEX_IN, SEQ_SYSEX_ADDRESS_H	; set the high-byte of address
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte
	
SEQ_SYSEX_Action_PAR_WriteAL
	BRA_IFSET SEQ_SYSEX_STATE2, SEQ_SYSEX_STATE2_AL_RECEIVED, BANKED, SEQ_SYSEX_Action_PAR_WriteD
	;; convert 2*7bit address to 16bit address
	movf	SEQ_SYSEX_IN, W, BANKED		; OR SEQ_SYSEX_ADDRESS with low-byte
	movwf	SEQ_SYSEX_ADDRESS_L, BANKED
	btfsc	SEQ_SYSEX_ADDRESS_H, 0, BANKED
	bsf	SEQ_SYSEX_ADDRESS_L, 7, BANKED
	clrc
	rrf	SEQ_SYSEX_ADDRESS_H, W, BANKED
	rgoto	SEQ_SYSEX_SysExCheck_End	; wait for next byte

SEQ_SYSEX_Action_PAR_WriteD
SEQ_SYSEX_Action_PAR_WriteStall
	;; save parameter
	rcall	SEQ_SYSEX_WriteP
	SET_BSR	SEQ_BASE

	;; increment address
	;; (no overrun check here - but uncritical for MBSEQ since there are additional
	;; checks in SEQ_SYSEX_WriteP)
	incf	SEQ_SYSEX_ADDRESS_L, F, BANKED
	skpnz
	incf	SEQ_SYSEX_ADDRESS_H, F, BANKED

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
	;; send SysEx header
	rcall	SEQ_SYSEX_Send_SysExHeader

	;; send 0F 1A
	movlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movlw	0x1a
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SEQ_SYSEX_Send_SysExFooter

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
;;  IN: source pattern in SEQ_PATTERN/SEQ_PATTERN_BANK
;;      if SEQ_PATTERN == 0x80..0x83, send RAM dump of P1..P4 instead
;;      pattern number/bank of the sent dump in SEQ_SYSEX_TARGET_PATTERN/SEQ_SYSEX_TARGET_BANK
;; --------------------------------------------------------------------------
SEQ_SYSEX_SendDump
	;; send SysEx header
	rcall	SEQ_SYSEX_Send_SysExHeader

	;; send PAT_Write ID
	movlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send requested pattern bank and number
	SET_BSR	SEQ_BASE
	movf	SEQ_SYSEX_TARGET_BANK, W, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	SET_BSR	SEQ_BASE
	movf	SEQ_SYSEX_TARGET_PATTERN, W, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; clear checksum
	SET_BSR	SEQ_BASE
	clrf	SEQ_SYSEX_CHECKSUM, BANKED

	;; send dump of 0x280 (640) bytes
	rcall	SEQ_SYSEX_SendPattern
	
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
;;  IN: source pattern in SEQ_PATTERN/SEQ_PATTERN_BANK
;;      if SEQ_PATTERN == 0x80..0x83, send RAM dump of P1..P4 instead
;;  USES: SEQ_GROUP, EEADR and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_SYSEX_SendPattern
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PATTERN, 7, BANKED, SEQ_SYSEX_SendPattern_RAM

SEQ_SYSEX_SendPattern_EE
	clrf	EEADR
	clrf	EEADRH

	;; Layer A/B/C (0x180 bytes)
SEQ_SYSEX_SendPattern_EE_Loop1
	call	SEQ_BANK_Read
	andlw	0x7f		; add to checksum
	SET_BSR	SEQ_BASE
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	movf	EEADRH, W
	bz	SEQ_SYSEX_SendPattern_EE_Loop1
	BRA_IFCLR EEADR, 7, ACCESS, SEQ_SYSEX_SendPattern_EE_Loop1

	;; CFG data (0x80 bytes)
SEQ_SYSEX_SendPattern_EE_Loop2
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
	bnz	SEQ_SYSEX_SendPattern_EE_Loop2

	return

;; ---
SEQ_SYSEX_SendPattern_RAM
	;; copy SEQ_PATTERN[1:0] into SEQ_GROUP
	movf	SEQ_PATTERN, W, BANKED
	andlw	0x03
	movwf	SEQ_GROUP

	;; fill the whole cache
	call	SEQ_DUMP_FillCache

	;; send all 0x280 values, using SEQ_SYSEX_ADDRESS_[LH] as counter
	clrf	SEQ_SYSEX_ADDRESS_L, BANKED
	clrf	SEQ_SYSEX_ADDRESS_H, BANKED
SEQ_SYSEX_SendPattern_RAM_Loop
	clrwdt
	rcall	SEQ_SYSEX_ReadP
	andlw	0x7f		; add to checksum
	SET_BSR	SEQ_BASE
	addwf	SEQ_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	incf	SEQ_SYSEX_ADDRESS_L, F, BANKED
	skpnz
	incf	SEQ_SYSEX_ADDRESS_H, F, BANKED

	;; loop until 0x280 reached
	movlw	0x02
	cpfseq	SEQ_SYSEX_ADDRESS_H, BANKED
	rgoto SEQ_SYSEX_SendPattern_RAM_Loop
	movlw	0x80
	cpfseq	SEQ_SYSEX_ADDRESS_L, BANKED
	rgoto SEQ_SYSEX_SendPattern_RAM_Loop

	return


;; --------------------------------------------------------------------------
;;  This function transfers a parameter into pattern SRAM
;;  IN: group in SEQ_GROUP
;;      address in SEQ_SYSEX_ADDRESS_[LH]
;;      parameter value in SEQ_SYSEX_IN
;; --------------------------------------------------------------------------
SEQ_SYSEX_WriteP
	;; store value in MIOS_PARAMETER1
	movff	SEQ_SYSEX_IN, MIOS_PARAMETER1

	;; branch depending on address

	;; 0x000..0x07f: Layer A
	;; 0x080..0x0ff: Layer B
	;; 0x100..0x17f: Layer C
	;; 0x180..0x27f: Config (2 Nibbles = 1 byte)

	rlf	SEQ_SYSEX_ADDRESS_L, W, BANKED	; (makes branches easier)
	rlf	SEQ_SYSEX_ADDRESS_H, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_SYSEX_WriteP_LayerA
	rgoto	SEQ_SYSEX_WriteP_LayerB
	rgoto	SEQ_SYSEX_WriteP_LayerC
	rgoto	SEQ_SYSEX_WriteP_CFG
	rgoto	SEQ_SYSEX_WriteP_CFG
	return
	return
	return

SEQ_SYSEX_WriteP_LayerA
	movlw	0
	rgoto	SEQ_SYSEX_WriteP_Layer_Cont
SEQ_SYSEX_WriteP_LayerB
	movlw	1
	rgoto	SEQ_SYSEX_WriteP_Layer_Cont
SEQ_SYSEX_WriteP_LayerC
	movlw	2
	;; 	rgoto	SEQ_SYSEX_WriteP_Layer_Cont
SEQ_SYSEX_WriteP_Layer_Cont
	call	SEQ_DUMP_Hlp_FSR1LayerOffset
	movff	MIOS_PARAMETER1, POSTINC1
	return

SEQ_SYSEX_WriteP_CFG
	;; ensure that MIOS_PARAMETER1 holds only 4 bits
	movlw	0x0f
	andwf	MIOS_PARAMETER1, F

	;; get pointer to address
	rcall	SEQ_SYSEX_ReadWriteP_GetCfgPtr

	BRA_IFSET SEQ_SYSEX_ADDRESS_L, 0, BANKED, SEQ_SYSEX_WriteP_CFG_H
SEQ_SYSEX_WriteP_CFG_L
	movlw	0xf0		; write low nibble
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	iorwf	INDF1, F
	return
	
SEQ_SYSEX_WriteP_CFG_H
	movlw	0x0f		; write high nibble
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	iorwf	INDF1, F
	return


;; help function for SEQ_SYSEX_WriteP and ..ReadP
SEQ_SYSEX_ReadWriteP_GetCfgPtr
	;; determine pointer to SEQ_TRKx depending on group
	lfsr	FSR1, SEQ_TRK0
	movf	SEQ_GROUP, W, BANKED
	mullw	4*SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	;; add offset ((address - 0x180) >> 1) & 0x7f  -> (address>>1) ^ 0x40
	rrf	SEQ_SYSEX_ADDRESS_L, W
	andlw	0x7f
	xorlw	0x40
	addwf	FSR1L, F
	movlw	0
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a parameter depending on SysEx address
;;  IN: group in SEQ_GROUP
;;      address in SEQ_SYSEX_ADDRESS_[LH]
;;  OUT: parameter value in WREG
;; --------------------------------------------------------------------------
SEQ_SYSEX_ReadP
	;; branch depending on address

	;; 0x000..0x07f: Layer A
	;; 0x080..0x0ff: Layer B
	;; 0x100..0x17f: Layer C
	;; 0x180..0x27f: Config (2 Nibbles = 1 byte)

	rlf	SEQ_SYSEX_ADDRESS_L, W, BANKED	; (makes branches easier)
	rlf	SEQ_SYSEX_ADDRESS_H, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_SYSEX_ReadP_LayerA
	rgoto	SEQ_SYSEX_ReadP_LayerB
	rgoto	SEQ_SYSEX_ReadP_LayerC
	rgoto	SEQ_SYSEX_ReadP_CFG
	rgoto	SEQ_SYSEX_ReadP_CFG
	return
	return
	return

SEQ_SYSEX_ReadP_LayerA
	movlw	0
	rgoto	SEQ_SYSEX_ReadP_Layer_Cont
SEQ_SYSEX_ReadP_LayerB
	movlw	1
	rgoto	SEQ_SYSEX_ReadP_Layer_Cont
SEQ_SYSEX_ReadP_LayerC
	movlw	2
	;; 	rgoto	SEQ_SYSEX_ReadP_Layer_Cont
SEQ_SYSEX_ReadP_Layer_Cont
	call	SEQ_DUMP_Hlp_FSR1LayerOffset
	movf	POSTINC1, W
	andlw	0x7f
	return

SEQ_SYSEX_ReadP_CFG
	;; get pointer to address
	rcall	SEQ_SYSEX_ReadWriteP_GetCfgPtr

	;; low or high nibble?
	movf	INDF1, W
	btfsc	SEQ_SYSEX_ADDRESS_L, 0, BANKED
	swapf	INDF1, W
	andlw	0x0f
	return
