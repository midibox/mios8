; $Id$
;
; MIDIbox CV SysEx parser
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

CV_SYSEX_STATE_MYSYSEX		EQU	7
CV_SYSEX_STATE_ACTION		EQU	6

CV_SYSEX_STATE_BANK_RECEIVED	EQU	5	; used by Action BANK_[Read/Write]
CV_SYSEX_STATE_WAIT_CHECKSUM	EQU	4	; used by Action BANK_Write

CV_SYSEX_STATE_AH_RECEIVED	EQU	5	; used by Action PAR_[Read/Write]
CV_SYSEX_STATE_AL_RECEIVED	EQU	4	; used by Action PAR_[Read/Write]

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc on a timeout
;;  it will used internally when an internal sequence has been parsed or 
;;  at the end of a sysex sequence
;; --------------------------------------------------------------------------
CV_SYSEX_ActionInvalid
CV_SYSEX_ActionFinished
	SET_BSR	CV_BASE
	clrf	CV_SYSEX_STATE, BANKED
	clrf	CV_SYSEX_ACTION, BANKED

	call	MIOS_MPROC_MergerEnable
	rgoto	CV_SYSEX_SysExCheck_End

;; --------------------------------------------------------------------------
;;  This sysex parser waits for the MBCV Header
;;  IN: received byte in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CV_SYSEX_Parser
	;; store received byte in CV_SYSEX_IN
	SET_BSR	CV_BASE
	movf	MIOS_PARAMETER1, W
	movwf	CV_SYSEX_IN, BANKED

	;; exit if realtime event has been received
	movlw	0xf8
	cpfslt	CV_SYSEX_IN, BANKED
	return

	;; check sysex state
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_MYSYSEX, BANKED, CV_SYSEX_Handler

	movf	CV_SYSEX_STATE, W, BANKED
	rcall	CV_SYSEX_SysExHeaderGet
	cpfseq	CV_SYSEX_IN, BANKED
	rgoto CV_SYSEX_SysExCheckFailed
	incf	CV_SYSEX_STATE, F, BANKED
	movf	CV_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 47 <device-id>)
	bnz	CV_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	movlw	(1 << CV_SYSEX_STATE_MYSYSEX)
	movwf	CV_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable
	rgoto	CV_SYSEX_SysExCheckOk

CV_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rcall	CV_SYSEX_ActionInvalid
CV_SYSEX_SysExCheckOk

CV_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
CV_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x48		; MIDIbox CV ID (48 - the ultimative number + 6)
	movf	MIDI_DEVICE_ID, W
	return


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
CV_SYSEX_Handler
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_ACTION, BANKED, CV_SYSEX_Handler_DoAction

	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_ACTION, BANKED

	movff	CV_SYSEX_IN, CV_SYSEX_ACTION	; store action ID
	rcall	CV_SYSEX_Handler_InitAction	; initialize the action
	rgoto	CV_SYSEX_SysExCheck_End	; branch to the end

	;; ---

CV_SYSEX_Handler_DoAction
	SET_BSR	CV_BASE
	;; branch to end if status byte (i.e. F7)
	BRA_IFSET CV_SYSEX_IN, 7, BANKED, CV_SYSEX_Handler_EndAction

	;; branch depending on current action ID
	movf	CV_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_Action_BANK_Read
	rgoto	CV_SYSEX_Action_BANK_Write
	rgoto	CV_SYSEX_Action_BANKS_Read
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_Action_PAR_Read
	rgoto	CV_SYSEX_Action_PAR_Write
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_Action_Ping


;; --------------------------------------------------------------------------
;;  Initialize an action depending on CV_SYSEX_ACTION
;; --------------------------------------------------------------------------
CV_SYSEX_Handler_InitAction
	SET_BSR	CV_BASE
	movf	CV_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_Init_BANK_Read
	rgoto	CV_SYSEX_Init_BANK_Write
	rgoto	CV_SYSEX_Init_BANKS_Read
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_Init_PAR_Read
	rgoto	CV_SYSEX_Init_PAR_Write
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_Init_Ping

	
;; --------------------------------------------------------------------------
;;  Finish an action depending on CV_SYSEX_ACTION
;; --------------------------------------------------------------------------
CV_SYSEX_Handler_EndAction
	SET_BSR	CV_BASE
	;; if sysex footer (F7) has not been received here, action is invalid!
	movlw	0xf7
	xorwf	CV_SYSEX_IN, W, BANKED
	bnz	CV_SYSEX_ActionInvalid

	;; else finish action
	movf	CV_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_End_BANK_Read
	rgoto	CV_SYSEX_End_BANK_Write
	rgoto	CV_SYSEX_End_BANKS_Read
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_End_PAR_Read
	rgoto	CV_SYSEX_End_PAR_Write
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_ActionInvalid
	rgoto	CV_SYSEX_End_Ping

	

;; --------------------------------------------------------------------------
;;  MIDI Action: Bank Read
;; --------------------------------------------------------------------------
CV_SYSEX_Init_BANK_Read
	return

	;; receive <patch> F7
CV_SYSEX_Action_BANK_Read
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_BANK_RECEIVED, BANKED, CV_SYSEX_Action_BANK_ReadStall
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_BANK_RECEIVED, BANKED
	movff	CV_SYSEX_IN, CV_SYSEX_PATCH	; copy patch number to CV_SYSEX_PATCH
	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte
	
CV_SYSEX_Action_BANK_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	CV_SYSEX_SysExCheck_End

CV_SYSEX_End_BANK_Read
	;; action invalid if bank number has not been received
	BRA_IFCLR CV_SYSEX_STATE, CV_SYSEX_STATE_BANK_RECEIVED, BANKED, CV_SYSEX_ActionInvalid

	;; send SysEx dump
	movff	CV_PATCH, CV_SYSEX_IN		; patch number temporary stored in CV_SYSEX_IN
	movff	CV_SYSEX_PATCH, CV_PATCH
	call	CV_SYSEX_SendDump_EE
	movff	CV_SYSEX_IN, CV_PATCH		; restore patch number

	;; request display re-init
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; finish Action
	rgoto	CV_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Bank Write
;; --------------------------------------------------------------------------
CV_SYSEX_Init_BANK_Write
	;; request display update (will be done after the complete SysEx dump has been received)
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

	;; receive <bank> <256 bytes> F7
CV_SYSEX_Action_BANK_Write
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_BANK_RECEIVED, BANKED, CV_SYSEX_Action_BANK_WriteC
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_BANK_RECEIVED, BANKED
	movff	CV_SYSEX_IN, CV_SYSEX_PATCH	; copy patch number to CV_SYSEX_PATCH
	clrf	CV_SYSEX_CHECKSUM, BANKED	; clear checksum
	clrf	CV_SYSEX_ADDRESS, BANKED	; clear address
	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte

CV_SYSEX_Action_BANK_WriteC
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_WAIT_CHECKSUM, BANKED, CV_SYSEX_Action_BANK_WriteChk

	;; add received byte to checksum
	movf	CV_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	CV_SYSEX_CHECKSUM, F, BANKED

	;; store received byte directly in pattern depending on address memory
	;; expects: address in CV_SYSEX_ADDRESS
	;; parameter value in CV_SYSEX_IN
	rcall	CV_SYSEX_WriteP
	SET_BSR	CV_BASE

	;; increment address
	incf	CV_SYSEX_ADDRESS, F, BANKED

	;; change to WAIT_CHECKSUM state when 0x100 reached
	movf	CV_SYSEX_ADDRESS, W, BANKED
	skpnz
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	CV_SYSEX_SysExCheck_End

CV_SYSEX_Action_BANK_WriteChk
	;; store received byte in checksum (using CV_SYSEX_ADDRESS as store register)
	movff	CV_SYSEX_IN, CV_SYSEX_ADDRESS

	;; wait for next byte
	rgoto	CV_SYSEX_SysExCheck_End

CV_SYSEX_End_BANK_Write
	;; action invalid if checksum has not been received
	BRA_IFCLR CV_SYSEX_STATE, CV_SYSEX_STATE_WAIT_CHECKSUM, BANKED, CV_SYSEX_ActionInvalid

	;; calc final checksum
	movf	CV_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f

	;; compare with received checksum
	xorwf	CV_SYSEX_ADDRESS, W, BANKED

	;; if not equal jump to ActionInvalid
	skpz
	rgoto	CV_SYSEX_ActionInvalid

	;; write dump to EEPROM
	movff	CV_SYSEX_PATCH, CV_PATCH
	call	CV_DUMP_StoreAll

	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	
	;; send acknowledge
	rcall	CV_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	CV_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: All Bank Read
;; --------------------------------------------------------------------------
CV_SYSEX_Init_BANKS_Read
	return

CV_SYSEX_Action_BANKS_Read
	;; do nothing until sysex footer (F7) has been received
	rgoto	CV_SYSEX_SysExCheck_End

CV_SYSEX_End_BANKS_Read
	;; send 128 patches, CV_PATCH used as counter
	movff	CV_PATCH, CV_SYSEX_IN		; patch number temporary stored in CV_SYSEX_IN
	clrf	CV_PATCH
CV_SYSEX_End_BANKS_ReadLoop
	clrwdt
	call	CV_SYSEX_SendDump_EE
	incf	CV_PATCH, F
	BRA_IFCLR CV_PATCH, 7, ACCESS, CV_SYSEX_End_BANKS_ReadLoop

	movff	CV_SYSEX_IN, CV_PATCH		; restore patch number

	;; send checksum
	movf	CV_SYSEX_CHECKSUM, W
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; finish Action
	rgoto	CV_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Read
;; --------------------------------------------------------------------------
CV_SYSEX_Init_PAR_Read
	return

CV_SYSEX_Action_PAR_Read
	;; receive <AH> <AL> F7
CV_SYSEX_Action_PAR_ReadAH
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_AH_RECEIVED, BANKED, CV_SYSEX_Action_PAR_ReadAL
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_AH_RECEIVED, BANKED
	rcall	CV_SYSEX_Action_PAR_Hlp_SetH	; set the high-byte of address
	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte
	
CV_SYSEX_Action_PAR_ReadAL
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_AL_RECEIVED, BANKED, CV_SYSEX_Action_PAR_ReadStall
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_AL_RECEIVED, BANKED
	movf	CV_SYSEX_IN, W, BANKED		; OR CV_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	CV_SYSEX_ADDRESS, F, BANKED
	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte

CV_SYSEX_Action_PAR_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	CV_SYSEX_SysExCheck_End

CV_SYSEX_End_PAR_Read
	;; action invalid if low-byte of address has not been received
	BRA_IFCLR CV_SYSEX_STATE, CV_SYSEX_STATE_AL_RECEIVED, BANKED, CV_SYSEX_ActionInvalid

	;; send SysEx header
	rcall	CV_SYSEX_Send_SysExHeader

	;; send PAR_Write ID
	movlw	0x06
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	CV_BASE

	;; send AH
	movlw	0x00
	btfsc	CV_SYSEX_ADDRESS, 7, BANKED
	iorlw 0x01
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	CV_BASE

	;; send AL
	movf	CV_SYSEX_ADDRESS, W, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	CV_BASE

	;; get parameter value from selected address
	rcall	CV_SYSEX_ReadP
	SET_BSR	CV_BASE
	
	;; send value
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	CV_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	CV_SYSEX_ActionFinished

;; ---
	;; help function used by CV_SYSEX_Action_PAR_[Read/Write]
CV_SYSEX_Action_PAR_Hlp_SetH
	clrf	CV_SYSEX_ADDRESS, BANKED	; clear register and set CV_SYSEX_ADDRESS[7] if IN[0] is set

	btfsc	CV_SYSEX_IN, 0, BANKED
	bsf	CV_SYSEX_ADDRESS, 7, BANKED
	return

;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
CV_SYSEX_Init_PAR_Write
	;; request display init (will be done after the complete SysEx dump has been received)
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

CV_SYSEX_Action_PAR_Write
	;; receive <AH> <AL> <value> F7
CV_SYSEX_Action_PAR_WriteAH
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_AH_RECEIVED, BANKED, CV_SYSEX_Action_PAR_WriteAL
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_AH_RECEIVED, BANKED
	rcall	CV_SYSEX_Action_PAR_Hlp_SetH	; set the high-byte of address
	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte
	
CV_SYSEX_Action_PAR_WriteAL
	BRA_IFSET CV_SYSEX_STATE, CV_SYSEX_STATE_AL_RECEIVED, BANKED, CV_SYSEX_Action_PAR_WriteD
	bsf	CV_SYSEX_STATE, CV_SYSEX_STATE_AL_RECEIVED, BANKED
	movf	CV_SYSEX_IN, W, BANKED		; OR CV_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	CV_SYSEX_ADDRESS, F, BANKED
	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte

CV_SYSEX_Action_PAR_WriteD
CV_SYSEX_Action_PAR_WriteStall
	;; save parameter
	rcall	CV_SYSEX_WriteP
	SET_BSR	CV_BASE

	;; increment address
	;; (no overrun check here - but uncritical for MBCV since there are additional
	;; checks in CV_SYSEX_WriteP)
	incf	CV_SYSEX_ADDRESS, F, BANKED

	rgoto	CV_SYSEX_SysExCheck_End	; wait for next byte


CV_SYSEX_End_PAR_Write
	;; action invalid if address has not been received
	BRA_IFCLR CV_SYSEX_STATE, CV_SYSEX_STATE_AL_RECEIVED, BANKED, CV_SYSEX_ActionInvalid

	;; send acknowledge
	call	CV_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	CV_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
CV_SYSEX_Init_Ping
	return

CV_SYSEX_Action_Ping
	;; do nothing until sysex footer (F7) has been received
	rgoto	CV_SYSEX_SysExCheck_End

CV_SYSEX_End_Ping
	;; send Acknowledge
	rcall	CV_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	CV_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
CV_SYSEX_Send_Acknowledge
	rcall	CV_SYSEX_Send_SysExHeader

	movlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rgoto	CV_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send MBCV SysEx Header (Util function)
;; --------------------------------------------------------------------------
CV_SYSEX_Send_SysExHeader
	;; if CV_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR CV_SYSEX_STATE, CV_SYSEX_STATE_MYSYSEX, BANKED, CV_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	CV_SYSEX_Send_SysExHeader_Skp
	movlw	0x05		; send only DEVICE_ID
	movwf	TMP1
	rgoto	CV_SYSEX_Send_SysExHeaderLoop
CV_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
CV_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	CV_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto CV_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
CV_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, CV_SYSEX_Send_SysExFooter_Force
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

CV_SYSEX_Send_SysExFooter_Force
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Sends the SysEx Dump header
;; --------------------------------------------------------------------------
CV_SYSEX_SendDump_Header
	;; send SysEx header
	rcall	CV_SYSEX_Send_SysExHeader

	;; send BANK_Write ID
	movlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send requested patch number
	movf	CV_PATCH, W
	call	MIOS_MIDI_TxBufferPut

	;; clear checksum
	SET_BSR	CV_BASE
	clrf	CV_SYSEX_CHECKSUM, BANKED
	return

;; --------------------------------------------------------------------------
;;  Sends the SysEx Dump footer
;; --------------------------------------------------------------------------
CV_SYSEX_SendDump_Footer
	;; send checksum
	SET_BSR	CV_BASE
	movf	CV_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; send 0xf7
	movlw	0x01		; (independend from merger state)
	rgoto	CV_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Send a SysEx dump of a single pattern from EEPROM
;; --------------------------------------------------------------------------
CV_SYSEX_SendDump_EE
	;; send header
	rcall	CV_SYSEX_SendDump_Header

	;; send dump of 0x100 (256) bytes
	clrf	EEADR

CV_SYSEX_SendDump_EE_Loop
	call	CV_BANK_Read
	andlw	0x7f		; add to checksum
	SET_BSR	CV_BASE
	addwf	CV_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	movf	EEADR, W
	bnz	CV_SYSEX_SendDump_EE_Loop

	;; send footer
	rgoto	CV_SYSEX_SendDump_Footer


;; --------------------------------------------------------------------------
;;  Sends a SysEx dump of a single pattern from RAM
;;  USES: EEADR and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CV_SYSEX_SendDump_RAM
	;; send header
	rcall	CV_SYSEX_SendDump_Header

	;; send dump of 0x100 (256) bytes
	lfsr	FSR0, CV_CFG0
	clrf	EEADR
CV_SYSEX_SendDump_RAM_Loop
	movf	POSTINC0, W
	andlw	0x7f		; add to checksum
	SET_BSR	CV_BASE
	addwf	CV_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	incf	EEADR, F
	movf	EEADR, W
	bnz	CV_SYSEX_SendDump_RAM_Loop

	;; send footer
	rgoto	CV_SYSEX_SendDump_Footer


;; --------------------------------------------------------------------------
;;  This function transfers a parameter into pattern SRAM
;;  IN: low-byte of address in CV_SYSEX_ADDRESS
;;      MSB of address in CV_SYSEX_STATE
;;      parameter value in CV_SYSEX_IN
;; --------------------------------------------------------------------------
CV_SYSEX_WriteP
	SET_BSR	CV_BASE
	
	lfsr	FSR0, CV_CFG0
	movf	CV_SYSEX_ADDRESS, W, BANKED
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F

	movff	CV_SYSEX_IN, INDF0
	return


;; --------------------------------------------------------------------------
;;  This function returns a parameter depending on SysEx address
;;  IN: low-byte of address in CV_SYSEX_ADDRESS
;;      MSB of address in CV_SYSEX_STATE, CV_SYSEX_STATE_MSB
;;  OUT: parameter value in WREG
;; --------------------------------------------------------------------------
CV_SYSEX_ReadP
	SET_BSR	CV_BASE
	
	lfsr	FSR0, CV_CFG0
	movf	CV_SYSEX_ADDRESS, W, BANKED
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F

	movf	INDF0, W
	return
