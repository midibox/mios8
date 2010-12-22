; $Id$
;
; MBHP_MF SysEx parser
;
; ==========================================================================
;
;  Copyright 2010 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MF_SYSEX_STATE_MYSYSEX		EQU	7
MF_SYSEX_STATE_ACTION		EQU	6
MF_SYSEX_STATE_END		EQU	5
MF_SYSEX_STATE_CMD3		EQU	3	; part of sysex command
MF_SYSEX_STATE_CMD2		EQU	2	; part of sysex command
MF_SYSEX_STATE_CMD1		EQU	1	; part of sysex command
MF_SYSEX_STATE_CMD0		EQU	0	; part of sysex command

MF_SYSEX_STATE2_ERROR		EQU	0
MF_SYSEX_STATE2_PATCH_RECEIVED EQU	1	; used by Action PATCH_[Read/Write]
MF_SYSEX_STATE2_WAIT_CHECKSUM	EQU	2	; used by Action PATCH_Write
MF_SYSEX_STATE2_LNIBBLE_RECV	EQU	3	; used by Action PATCH_Write

MF_SYSEX_STATE2_AH_RECEIVED	EQU	4	; used by Action PAR_[Read/Write]
MF_SYSEX_STATE2_AL_RECEIVED	EQU	5	; used by Action PAR_[Read/Write]
MF_SYSEX_STATE2_DL_RECEIVED	EQU	6	; used by Action PAR_[Write]
MF_SYSEX_STATE2_DH_RECEIVED	EQU	7	; used by Action PAR_[Write]

MF_DISACK_LESS_BYTES_THAN_EXP	EQU     0x01
MF_DISACK_WRONG_CHECKSUM	EQU     0x03
MF_DISACK_BS_NOT_AVAILABLE	EQU	0x0a
MF_DISACK_PAR_NOT_AVAILABLE	EQU	0x0b
MF_DISACK_INVALID_COMMAND	EQU	0x0c
MF_DISACK_NO_RAM_ACCESS		EQU	0x10

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc on a timeout
;;  it will used internally when an internal sequence has been parsed or 
;;  at the end of a sysex sequence
;; --------------------------------------------------------------------------
MF_SYSEX_ActionInvalid
MF_SYSEX_ActionFinished
	SET_BSR	MF_BASE
	clrf	MF_SYSEX_STATE, BANKED
	clrf	MF_SYSEX_STATE2, BANKED

	call	MIOS_MPROC_MergerEnable
	rgoto	MF_SYSEX_SysExCheck_End

;; --------------------------------------------------------------------------
;;  This sysex parser waits for the MBCV Header
;;  IN: received byte in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MF_SYSEX_Parser
	;; store received byte in MF_SYSEX_IN
	SET_BSR	MF_BASE
	movf	MIOS_PARAMETER1, W
	movwf	MF_SYSEX_IN, BANKED

	;; exit if realtime event has been received
	movlw	0xf8
	cpfslt	MF_SYSEX_IN, BANKED
	return

	;; check sysex state
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_MYSYSEX, BANKED, MF_SYSEX_Handler

	movf	MF_SYSEX_STATE, W, BANKED
	rcall	MF_SYSEX_SysExHeaderGet
	cpfseq	MF_SYSEX_IN, BANKED
	rgoto MF_SYSEX_SysExCheckFailed
	incf	MF_SYSEX_STATE, F, BANKED
	movf	MF_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 4F <device-id>)
	bnz	MF_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	movlw	(1 << MF_SYSEX_STATE_MYSYSEX)
	movwf	MF_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable

	rgoto	MF_SYSEX_SysExCheckOk

MF_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rcall	MF_SYSEX_ActionInvalid
MF_SYSEX_SysExCheckOk

MF_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
MF_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x4f		; MBHP_MF V3 ID
	call	MIOS_MIDI_DeviceIDGet
	SET_BSR	MF_BASE		; (just to improve portability with old sysex handlers which are not prepared for BSR change caused by MIOS_MIDI_DeviceIDGet)
	return


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
MF_SYSEX_Handler
	SET_BSR	MF_BASE

	;; if first byte after header, store command ID
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_ACTION, BANKED, MF_SYSEX_Handler_NotBegin
MF_SYSEX_Handler_Begin
	bsf	MF_SYSEX_STATE, MF_SYSEX_STATE_ACTION, BANKED
	movf	MF_SYSEX_IN, W, BANKED
	andlw	0x0f
	iorwf	MF_SYSEX_STATE, F, BANKED
	rgoto	MF_SYSEX_SysExCheck_End
MF_SYSEX_Handler_NotBegin

	;; fetch data until next status byte (-> 0xf7)
	BRA_IFCLR MF_SYSEX_IN, 7, BANKED, MF_SYSEX_Handler_NotEnd
MF_SYSEX_Handler_End
	;; if sysex footer (F7) has not been received here, command is invalid!
	movlw	0xf7
	cpfseq	MF_SYSEX_IN, BANKED
	rgoto	MF_SYSEX_ActionInvalid
	;; notify end
	bsf	MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED
MF_SYSEX_Handler_NotEnd

	;; branch depending on current action ID
	movf	MF_SYSEX_STATE, W, BANKED
	andlw	0x0f
	addlw	-1
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_PatchRead	; 1
	addlw	-2+1
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_PatchWrite; 2
	addlw	-6+2
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_ParWrite	; 6
	addlw	-9+6
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_FadersGet	; 9
	addlw	-10+9
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_FadersSet	; 10
	addlw	-11+10
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_TraceReq	; 11
	addlw	-15+10
	BRA_IFSET STATUS, Z, ACCESS, MF_SYSEX_Ping	; 15
	rgoto	MF_SYSEX_ActionInvalid
	;; (note: target address to far away for "bz", therefore I'm using "IFSET STATUS, Z"

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Read
;; --------------------------------------------------------------------------
MF_SYSEX_PatchRead
	;; end of stream?
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_PatchRead_End

	;; receive <patch> F7
MF_SYSEX_PatchRead_Nr
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_PATCH_RECEIVED, BANKED, MF_SYSEX_PatchRead_Stl
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_PATCH_RECEIVED, BANKED
#if 0
	movff	MF_SYSEX_IN, MF_PATCH	; copy patch number to MF_PATCH
	rcall	MF_SYSEX_SetPatch		; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_ERROR, BANKED
#endif
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_PatchRead_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	MF_SYSEX_SysExCheck_End


	;; end of sysex command
MF_SYSEX_PatchRead_End

	;; action invalid if patch number has not been received
	BRA_IFCLR MF_SYSEX_STATE2, MF_SYSEX_STATE2_PATCH_RECEIVED, BANKED, MF_SYSEX_DisAck_NotComplete

	;; action invalid if patch/bank not available
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_ERROR, BANKED, MF_SYSEX_DisAck_BS_NotAvail

	;; send the dump
	rcall	MF_SYSEX_SendDump_EE

	;; finish Action
	rgoto	MF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Write
;; --------------------------------------------------------------------------
MF_SYSEX_PatchWrite
	;; end of stream?
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_PatchWrite_End

	;; receive <patch> <2*256 bytes> <checksum> F7
MF_SYSEX_PatchWrite_Nr
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_PATCH_RECEIVED, BANKED, MF_SYSEX_PatchWrite_Data
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_PATCH_RECEIVED, BANKED
	clrf	MF_SYSEX_ADDRESS, BANKED
	clrf	MF_SYSEX_CHECKSUM, BANKED
#if 0
	movff	MF_SYSEX_IN, MF_PATCH		; copy patch number to MF_PATCH
	rcall	MF_SYSEX_SetPatch		; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_ERROR, BANKED
#endif
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_PatchWrite_Data
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, MF_SYSEX_PatchWrite_Chk

	;; low or high nibble?
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_LNIBBLE_RECV, BANKED, MF_SYSEX_PatchWrite_Data_U
MF_SYSEX_PatchWrite_Data_L
	;; lower nibble received
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_LNIBBLE_RECV, BANKED

	;; store low-nibble in MF_SYSEX_DATA
	movf	MF_SYSEX_IN, W, BANKED
	andlw	0x0f
	movwf	MF_SYSEX_DATA, BANKED

	;; add to checksum
	movf	MF_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	MF_SYSEX_CHECKSUM, F, BANKED

	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte
	
MF_SYSEX_PatchWrite_Data_U
	;; upper nibble received
	bcf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_LNIBBLE_RECV, BANKED

	;; OR high-nibble to MF_SYSEX_DATA
	swapf	MF_SYSEX_IN, W, BANKED
	andlw	0xf0
	iorwf	MF_SYSEX_DATA, F, BANKED

	;; add to checksum
	movf	MF_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	MF_SYSEX_CHECKSUM, F, BANKED

	;; write directly into RAM
	movff	MF_SYSEX_ADDRESS, MIOS_PARAMETER1
	movf	MF_SYSEX_DATA, W, BANKED
	call	MF_DUMP_WriteData
	SET_BSR	MF_BASE

	;; increment address
	incf	MF_SYSEX_ADDRESS, F, BANKED

	;; on overrun (256 bytes received) go into WAIT_CHECKSUM state
	skpnz
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	MF_SYSEX_SysExCheck_End

MF_SYSEX_PatchWrite_Chk
	;; store received byte in checksum (using MF_SYSEX_DATA as store register)
	movff	MF_SYSEX_IN, MF_SYSEX_DATA

	;; wait for next byte
	rgoto	MF_SYSEX_SysExCheck_End


	;; end of sysex command
MF_SYSEX_PatchWrite_End
	;; action invalid if checksum has not been received
	BRA_IFCLR MF_SYSEX_STATE2, MF_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, MF_SYSEX_DisAck_NotComplete

	;; calc final checksum
	movf	MF_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f

	;; compare with received checksum
	xorwf	MF_SYSEX_DATA, W, BANKED

	;; if not equal send disacknowledge
	skpz
	rgoto	MF_SYSEX_DisAck_WrongChecksum

	;; store data in EEPROM
	call	MF_DUMP_StoreAll

	;; send acknowledge
	rcall	MF_SYSEX_Send_Acknowledge

MF_SYSEX_PATCH_Write_End
	;; finish Action
	rgoto	MF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
MF_SYSEX_ParWrite
	;; end of stream?
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_ParWrite_End

	;; receive <AH> <AL> <DL> <DH> F7
MF_SYSEX_ParWrite_AH
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED, MF_SYSEX_ParWrite_AL
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED
	clrf	MF_SYSEX_ADDRESS, BANKED	; clear register and set MF_SYSEX_ADDRESS[7] if IN[0] is set
	btfsc	MF_SYSEX_IN, 0, BANKED
	bsf	MF_SYSEX_ADDRESS, 7, BANKED
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte
	
MF_SYSEX_ParWrite_AL
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED, MF_SYSEX_ParWrite_DL
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED
	movf	MF_SYSEX_IN, W, BANKED			; OR MF_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	MF_SYSEX_ADDRESS, F, BANKED
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_ParWrite_DL
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_DL_RECEIVED, BANKED, MF_SYSEX_ParWrite_DH
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_DL_RECEIVED, BANKED
	
	;; store low-nibble in MF_SYSEX_DATA
	movf	MF_SYSEX_IN, W, BANKED
	andlw	0x0f
	movwf	MF_SYSEX_DATA, BANKED
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_ParWrite_DH
	;; switch back to DL received, this allows to send continuous data
	bcf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_DL_RECEIVED, BANKED
	;; notify that at least one DH has been received
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_DH_RECEIVED, BANKED

	;; OR high-nibble to MF_SYSEX_DATA
	swapf	MF_SYSEX_IN, W, BANKED
	andlw	0xf0
	iorwf	MF_SYSEX_DATA, BANKED

	;; transfer parameter into RAM
	movff	MF_SYSEX_ADDRESS, MIOS_PARAMETER1
	movf	MF_SYSEX_DATA, W, BANKED
	call	MF_DUMP_WriteData
	SET_BSR	MF_BASE

	;; increment address
	incf	MF_SYSEX_ADDRESS, F, BANKED

	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_ParWrite_End
	;; action invalid if data has not been received
	BRA_IFCLR MF_SYSEX_STATE2, MF_SYSEX_STATE2_DH_RECEIVED, BANKED, MF_SYSEX_DisAck_NotComplete

	;; send acknowledge
	rcall	MF_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Faders Get
;; --------------------------------------------------------------------------
MF_SYSEX_FadersGet
	;; end of stream?
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_FadersGet_End

	;; receive <first fader> F7
MF_SYSEX_FadersGet_Nr
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED, MF_SYSEX_FadersGet_Stl
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED
	movff	MF_SYSEX_IN, MF_SYSEX_ADDRESS
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_FadersGet_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	MF_SYSEX_SysExCheck_End


	;; end of sysex command
MF_SYSEX_FadersGet_End

	;; action invalid if fader number has not been received
	BRA_IFCLR MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED, MF_SYSEX_DisAck_NotComplete

	;; send fader values
	rcall	MF_SYSEX_Send_SysExHeader
	movlw	0x0a		; same action like FadersSet! (handle it like a dump)
	rcall	MF_SYSEX_TxBufferPut
	movff	MF_SYSEX_ADDRESS, WREG ;start fader
	rcall	MF_SYSEX_TxBufferPut

	;; send position of all faders, starting from MF_SYSEX_ADDRESS
MF_SYSEX_FadersGet_Loop
	SET_BSR	MF_BASE
	movf	AIN_NUMBER, W, BANKED
	cpfslt	MF_SYSEX_ADDRESS, BANKED
	rgoto	MF_SYSEX_FadersGet_LoopEnd
MF_SYSEX_FadersGet_LoopSend
	movf	MF_SYSEX_ADDRESS, W, BANKED
	call	AIN_PinGet
	SET_BSR	MF_BASE
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	rcall	MF_SYSEX_TxBufferPut
	rlf	MIOS_PARAMETER1, W
	rlf	MIOS_PARAMETER2, W
	andlw	0x7f
	rcall	MF_SYSEX_TxBufferPut

	SET_BSR	MF_BASE
	incf	MF_SYSEX_ADDRESS, F, BANKED
	rgoto	MF_SYSEX_FadersGet_Loop
MF_SYSEX_FadersGet_LoopEnd

	movlw	0x01		; (independend from merger state)
	rcall	MF_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	MF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Faders Set
;; --------------------------------------------------------------------------
MF_SYSEX_FadersSet
	;; end of stream?
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_FadersSet_End

	;; receive <first fader> <fader-values> F7
MF_SYSEX_FadersSet_Nr
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED, MF_SYSEX_FadersSet_Data
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED
	movff	MF_SYSEX_IN, MF_SYSEX_ADDRESS
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_FadersSet_Data
	;; low or high byte?
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_LNIBBLE_RECV, BANKED, MF_SYSEX_FadersSet_Data_U
MF_SYSEX_FadersSet_Data_L
	;; lowbyte received
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_LNIBBLE_RECV, BANKED

	;; store it in MF_SYSEX_DATA
	movff	MF_SYSEX_IN, MF_SYSEX_DATA

	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte
	
MF_SYSEX_FadersSet_Data_U
	;; highbyte received
	bcf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_LNIBBLE_RECV, BANKED

	;; construct 10bit value -> MIOS_PARAMETER[12]
	movff	MF_SYSEX_DATA, MIOS_PARAMETER1
	btfsc	MF_SYSEX_IN, 0, BANKED
	bsf	MIOS_PARAMETER1, 7
	clrc
	rrf	MF_SYSEX_IN, W, BANKED
	movwf	MIOS_PARAMETER2

	;; reset touch detection
	movf	MF_SYSEX_ADDRESS, W, BANKED
	call	MF_TouchDetectionReset

	;; move fader
	SET_BSR	MF_BASE
	movf	MF_SYSEX_ADDRESS, W, BANKED
	call	MF_FaderMove
	SET_BSR	MF_BASE

	;; increment address (next fader)
	incf	MF_SYSEX_ADDRESS, F, BANKED

	;; wait for next fader
	rgoto	MF_SYSEX_SysExCheck_End


	;; end of sysex command
MF_SYSEX_FadersSet_End
	;; send acknowledge
	rcall	MF_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MF_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Action: Trace Request
;; --------------------------------------------------------------------------
MF_SYSEX_TraceReq
	;; end of stream?
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_TraceReq_End

	;; receive <fader> <scale> F7
MF_SYSEX_TraceReq_Nr
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED, MF_SYSEX_TraceReq_Scale
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_AL_RECEIVED, BANKED
	movff	MF_SYSEX_IN, MF_TRACE_FADER
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_TraceReq_Scale
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED, MF_SYSEX_TraceReq_Stl
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED
	movff	MF_SYSEX_IN, MF_TRACE_SCALE
	rgoto	MF_SYSEX_SysExCheck_End	; wait for next byte

MF_SYSEX_TraceReq_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	MF_SYSEX_SysExCheck_End

	;; end of sysex command
MF_SYSEX_TraceReq_End

	;; action invalid if trace scale has not been received
	BRA_IFCLR MF_SYSEX_STATE2, MF_SYSEX_STATE2_AH_RECEIVED, BANKED, MF_SYSEX_DisAck_NotComplete

	;; request trace
	movlw	1 << MF_TRACE_STATE_FLAG_ARMED
	movwf	MF_TRACE_STATE, BANKED

	;; send acknowledge
	rcall	MF_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MF_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
MF_SYSEX_Ping
	;; do nothing until end of stream
	BRA_IFSET MF_SYSEX_STATE, MF_SYSEX_STATE_END, BANKED, MF_SYSEX_Ping_End
	;; remember that we received at least one byte (for feedback detection)
	bsf	MF_SYSEX_STATE2, MF_SYSEX_STATE2_WAIT_CHECKSUM, BANKED
	rgoto	MF_SYSEX_SysExCheck_End

MF_SYSEX_Ping_End
	;; send Acknowledge
	;; feedback detection: but only if no additional byte has been received
	BRA_IFSET MF_SYSEX_STATE2, MF_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, MF_SYSEX_Ping_End_NoAck
	rcall	MF_SYSEX_Send_Acknowledge

MF_SYSEX_Ping_End_NoAck
	;; finish Action
	rgoto	MF_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  Error handling (send disacknowledge)
;; --------------------------------------------------------------------------
MF_SYSEX_DisAck_NotComplete
	;; send disacknowledge "not enough bytes have been received"
	movlw	MF_DISACK_LESS_BYTES_THAN_EXP
	rgoto	MF_SYSEX_DisAck_Cont

MF_SYSEX_DisAck_WrongChecksum
	;; send disacknowledge "wrong checksum"
	movlw	MF_DISACK_WRONG_CHECKSUM
	rgoto	MF_SYSEX_DisAck_Cont

MF_SYSEX_DisAck_BS_NotAvail
	;; send disacknowledge "bank (or patch) not available"
	movlw	MF_DISACK_BS_NOT_AVAILABLE
	rgoto	MF_SYSEX_DisAck_Cont

MF_SYSEX_DisAck_PAR_NotAvail
	;; send disacknowledge "parameter not available"
	movlw	MF_DISACK_PAR_NOT_AVAILABLE
 	rgoto	MF_SYSEX_DisAck_Cont

MF_SYSEX_DisAck_InvCmd
	;; send disacknowledge "invalid command"
	movlw	MF_DISACK_INVALID_COMMAND
 	rgoto	MF_SYSEX_DisAck_Cont

MF_SYSEX_DisAck_NoRAM
	;; send disacknowledge "no RAM access"
	movlw	MF_DISACK_NO_RAM_ACCESS
	;; 	rgoto	MF_SYSEX_DisAck_Cont

MF_SYSEX_DisAck_Cont
	movwf	TMP2		; error code in WREG
	
	rcall	MF_SYSEX_Send_SysExHeader

	movlw	0x0e
	rcall	MF_SYSEX_TxBufferPut
	movf	TMP2, W
	rcall	MF_SYSEX_TxBufferPut

	movlw	0x01		; (independend from merger state)
	rcall	MF_SYSEX_Send_SysExFooter

	rgoto MF_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MF_SYSEX_Send_Acknowledge
	rcall	MF_SYSEX_Send_SysExHeader

	;; acknowledge ID
	movlw	0x0f
	rcall	MF_SYSEX_TxBufferPut

	;; for feedback detection:
	;; send 0x00 in addition
	movlw	0x00
	rcall	MF_SYSEX_TxBufferPut

	;; send 0xf7
	movlw	0x01		; (independend from merger state)
	rgoto	MF_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send SID SysEx Header (Util function)
;; --------------------------------------------------------------------------
MF_SYSEX_Send_SysExHeader
	;; if MF_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	SET_BSR	MF_BASE
	BRA_IFCLR MF_SYSEX_STATE, MF_SYSEX_STATE_MYSYSEX, BANKED, MF_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MF_SYSEX_Send_SysExHeader_Skp
	movlw	0x05		; send only DEVICE_ID
	movwf	TMP1
	rgoto	MF_SYSEX_Send_SysExHeaderLoop
MF_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
MF_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	MF_SYSEX_SysExHeaderGet
	rcall	MF_SYSEX_TxBufferPut
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto MF_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
MF_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, MF_SYSEX_Send_SysExFooterForce
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

MF_SYSEX_Send_SysExFooterForce
	movlw	0xf7
	rgoto	MF_SYSEX_TxBufferPut


;; --------------------------------------------------------------------------
;;  Sends the SysEx Dump header
;; --------------------------------------------------------------------------
MF_SYSEX_SendDump_Header
	;; send SysEx header
	rcall	MF_SYSEX_Send_SysExHeader

	;; send BANK_Write ID
	movlw	0x02
	rcall	MF_SYSEX_TxBufferPut

	;; send requested patch number
	;; 	movf	MF_PATCH, W
	movlw	0x00		; always send patch 0
	rcall	MF_SYSEX_TxBufferPut

	;; clear checksum
	SET_BSR	MF_BASE
	clrf	MF_SYSEX_CHECKSUM, BANKED
	return

;; --------------------------------------------------------------------------
;;  Sends the SysEx Dump footer
;; --------------------------------------------------------------------------
MF_SYSEX_SendDump_Footer
	;; send checksum
	SET_BSR	MF_BASE
	movf	MF_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	rcall	MF_SYSEX_TxBufferPut

	;; send 0xf7
	movlw	0x01		; (independend from merger state)
	rgoto	MF_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Send a SysEx dump of a single pattern from EEPROM
;; --------------------------------------------------------------------------
MF_SYSEX_SendDump_EE
	;; send header
	rcall	MF_SYSEX_SendDump_Header

	;; send dump of 0x100 (256) bytes
	clrf	EEADR
MF_SYSEX_SendDump_EE_Loop
	call	MF_BANK_Read

	movwf	PRODL
	andlw	0x0f		; low nibble
	SET_BSR	MF_BASE
	addwf	MF_SYSEX_CHECKSUM, F, BANKED
	rcall	MF_SYSEX_TxBufferPut
	swapf	PRODL, W	; high nibble
	andlw	0x0f
	SET_BSR	MF_BASE
	addwf	MF_SYSEX_CHECKSUM, F, BANKED
	rcall	MF_SYSEX_TxBufferPut

	movf	EEADR, W
	bnz	MF_SYSEX_SendDump_EE_Loop

	;; send footer
	rgoto	MF_SYSEX_SendDump_Footer


;; --------------------------------------------------------------------------
;;  Sends a SysEx dump of a single pattern from RAM
;;  USES: EEADR, PRODL and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MF_SYSEX_SendDump_RAM
	;; send header
	rcall	MF_SYSEX_SendDump_Header

	;; send dump of 0x100 bytes
	clrf	EEADR		; using EEADR as loop counter
MF_SYSEX_SendDump_RAM_Loop
	movff	EEADR, MIOS_PARAMETER1
	call	MF_DUMP_ReadData

	movwf	PRODL
	andlw	0x0f		; low nibble
	SET_BSR	MF_BASE
	addwf	MF_SYSEX_CHECKSUM, F, BANKED
	rcall	MF_SYSEX_TxBufferPut
	swapf	PRODL, W	; high nibble
	andlw	0x0f
	SET_BSR	MF_BASE
	addwf	MF_SYSEX_CHECKSUM, F, BANKED
	rcall	MF_SYSEX_TxBufferPut

	incf	EEADR, F
	movf	EEADR, W
	bnz	MF_SYSEX_SendDump_RAM_Loop

	;; send footer
	rgoto	MF_SYSEX_SendDump_Footer


;; --------------------------------------------------------------------------
;;  send trace dump
;; --------------------------------------------------------------------------
MF_SYSEX_SendTraceDump
	rcall	MF_SYSEX_Send_SysExHeader

	movlw	0x0c		; same action like (unimplemented) TraceSet! (handle it like a dump)
	rcall	MF_SYSEX_TxBufferPut

	;; send dump
	lfsr	FSR0, TRACE_MEM_START
MF_SYSEX_SendTraceDump_Loop
	movf	INDF0, W
	andlw	0x7f
	rcall	MF_SYSEX_TxBufferPut
	rlf	POSTINC0, W
	rlf	POSTINC0, W
	andlw	0x7f
	rcall	MF_SYSEX_TxBufferPut

	movf	FSR0L, W
	xorlw	LOW(TRACE_MEM_END+1)
	bnz	MF_SYSEX_SendTraceDump_Loop
	movf	FSR0H, W
	xorlw	HIGH(TRACE_MEM_END+1)
	bnz	MF_SYSEX_SendTraceDump_Loop

	movlw	0x01		; (independend from merger state)
	rcall	MF_SYSEX_Send_SysExFooter

	return

;; --------------------------------------------------------------------------
;;  shortcut to save memory
;; --------------------------------------------------------------------------
MF_SYSEX_TxBufferPut
	goto	MIOS_MIDI_TxBufferPut
