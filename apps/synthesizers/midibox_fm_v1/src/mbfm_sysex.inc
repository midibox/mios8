; $Id$
;
; MIDIbox FM
; SysEx parser
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MBFM_SYSEX_STATE_MYSYSEX	EQU	7
MBFM_SYSEX_STATE_ACTION		EQU	6
MBFM_SYSEX_STATE_END		EQU	5
MBFM_SYSEX_STATE_ERROR		EQU	4

MBFM_SYSEX_STATE2_TYPE_RECEIVED	EQU	0	; used by most actions

MBFM_SYSEX_STATE2_BANK_RECEIVED	EQU	1	; used by Action PATCH_[Read/Write]
MBFM_SYSEX_STATE2_PATCH_RECEIVED EQU	2	; used by Action PATCH_[Read/Write]
MBFM_SYSEX_STATE2_WAIT_CHECKSUM	EQU	3	; used by Action PATCH_Write

MBFM_SYSEX_STATE2_AH_RECEIVED	EQU	1	; used by Action PAR_[Read/Write]
MBFM_SYSEX_STATE2_AL_RECEIVED	EQU	2	; used by Action PAR_[Read/Write]
MBFM_SYSEX_STATE2_D_RECEIVED	EQU	3	; used by Action PAR_[Write]

MBFM_SYSEX_STATE2_A_RECEIVED	EQU	1	; used by Action CFG_Write
MBFM_SYSEX_STATE2_DH_RECEIVED	EQU	2	; used by Action CFG_Write
MBFM_SYSEX_STATE2_DL_RECEIVED	EQU	3	; used by Action CFG_Write

MBFM_DISACK_LESS_BYTES_THAN_EXP EQU     0x01
MBFM_DISACK_WRONG_CHECKSUM      EQU     0x03
MBFM_DISACK_BS_NOT_AVAILABLE	EQU	0x0a
MBFM_DISACK_PAR_NOT_AVAILABLE	EQU	0x0b

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MBFM_SYSEX_Parser
	;; store received byte in MBFM_SYSEX_IN
	SET_BSR	MBFM_BASE
	movwf	MBFM_SYSEX_IN, BANKED

	;; ignore realtime messages
	movlw	0xf8
	cpfslt	MBFM_SYSEX_IN, BANKED
	return

	;; check sysex state
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_MYSYSEX, BANKED, MBFM_SYSEX_Handler

	movf	MBFM_SYSEX_STATE, W, BANKED
	rcall	MBFM_SYSEX_SysExHeaderGet
	cpfseq	MBFM_SYSEX_IN, BANKED
	rgoto MBFM_SYSEX_SysExCheckFailed
	incf	MBFM_SYSEX_STATE, F, BANKED
	movf	MBFM_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 49 <device-id>)
	bnz	MBFM_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	movlw	(1 << MBFM_SYSEX_STATE_MYSYSEX)
	movwf	MBFM_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable
	rgoto	MBFM_SYSEX_SysExCheckOk

MBFM_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rgoto	MBFM_SYSEX_ActionInvalid

MBFM_SYSEX_SysExCheckOk
MBFM_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
MBFM_SYSEX_SysExHeaderGet
	SET_BSR	MBFM_BASE
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x49		; MIDIbox FM ID (49 - the ultimative number + 7)
	movf	MBFM_MIDI_DEVICE, W, BANKED
	return

;; --------------------------------------------------------------------------
;;  Action Invalid will be invoked when receiving an invalid sequence
;; --------------------------------------------------------------------------
MBFM_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  Action finished will be invoked when midi action is done
;; --------------------------------------------------------------------------
MBFM_SYSEX_ActionFinished
	SET_BSR	MBFM_BASE
	clrf	MBFM_SYSEX_STATE, BANKED
	clrf	MBFM_SYSEX_STATE2, BANKED
	clrf	MBFM_SYSEX_ACTION, BANKED

	;; reinit voices if engine has been disabled during dump upload
	CALL_IFSET MBFM_STAT, MBFM_STAT_ENGINE_DISABLE, ACCESS, MBFM_VOICE_Init
	bcf	MBFM_STAT, MBFM_STAT_ENGINE_DISABLE
	
	call	MIOS_MPROC_MergerEnable
	rgoto	MBFM_SYSEX_SysExCheck_End


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
MBFM_SYSEX_Handler
	SET_BSR	MBFM_BASE

	;; if first byte after header, store command ID
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_ACTION, BANKED, MBFM_SYSEX_Handler_NotBegin
MBFM_SYSEX_Handler_Begin
	bsf	MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_ACTION, BANKED
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_ACTION
	rgoto	MBFM_SYSEX_SysExCheck_End
MBFM_SYSEX_Handler_NotBegin

	;; fetch data until next status byte (-> 0xf7)
	BRA_IFCLR MBFM_SYSEX_IN, 7, BANKED, MBFM_SYSEX_Handler_NotEnd
MBFM_SYSEX_Handler_End
	;; if sysex footer (F7) has not been received here, command is invalid!
	movlw	0xf7
	cpfseq	MBFM_SYSEX_IN, BANKED
	rgoto MBFM_SYSEX_ActionInvalid
	;; notify end
	bsf	MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_END, BANKED
MBFM_SYSEX_Handler_NotEnd

	;; branch depending on current action ID
	decf	MBFM_SYSEX_ACTION, W, BANKED
	BRA_IFSET STATUS, Z, ACCESS, MBFM_SYSEX_PatchRead	; 1
	addlw	-2+1
	BRA_IFSET STATUS, Z, ACCESS, MBFM_SYSEX_PatchWrite	; 2
	addlw	-6+2
	BRA_IFSET STATUS, Z, ACCESS, MBFM_SYSEX_ParWrite	; 6
	addlw	-10+6
	BRA_IFSET STATUS, Z, ACCESS, MBFM_SYSEX_PatchStore	; 10
	addlw	-15+10
	BRA_IFSET STATUS, Z, ACCESS, MBFM_SYSEX_Ping	; 15
	rgoto	MBFM_SYSEX_ActionInvalid
	;; (note: target address to far away for "bz", therefore I'm using "IFSET STATUS, Z"

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Read
;; --------------------------------------------------------------------------
MBFM_SYSEX_PatchRead
	;; end of stream?
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_END, BANKED, MBFM_SYSEX_PatchRead_End

	;; receive <type> <bank> <patch> F7
MBFM_SYSEX_PatchRead_Type
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED, MBFM_SYSEX_PatchRead_Bnk
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_TYPE	; copy type number to MBFM_SYSEX_TYPE
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte
	
MBFM_SYSEX_PatchRead_Bnk
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_BANK_RECEIVED, BANKED, MBFM_SYSEX_PatchRead_Nr
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_BANK_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_BANK	; copy bank number to MBFM_BANK
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte
	
MBFM_SYSEX_PatchRead_Nr
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_PATCH_RECEIVED, BANKED, MBFM_SYSEX_PatchRead_Stl
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_PATCH_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_PATCH	; copy patch number to MBFM_PATCH
	rcall	MBFM_SYSEX_SetPatch		; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_ERROR, BANKED
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte

MBFM_SYSEX_PatchRead_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	MBFM_SYSEX_SysExCheck_End


	;; end of sysex command
MBFM_SYSEX_PatchRead_End

	;; action invalid if patch number has not been received
	BRA_IFCLR MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_PATCH_RECEIVED, BANKED, MBFM_SYSEX_DisAck_NotComplete
	;; action invalid if patch/bank not available
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_ERROR, BANKED, MBFM_SYSEX_DisAck_BS_NotAvail

	;; change to ensemble/drumset/patches if no direct RAM access
	SET_BSR	MBFM_BASE
	BRA_IFSET MBFM_SYSEX_TYPE, 3, BANKED, MBFM_SYSEX_PatchRead_RAM
MBFM_SYSEX_PatchRead_EE
	movff	MBFM_PATCH, MBFM_SYSEX_IN	; we have to save MBFM_BANK and MBFM_PATCH
	movff	MBFM_BANK, MBFM_SYSEX_ADDRESS	; since they are touched when an ensemble is initialized
	rcall	MBFM_SYSEX_ChangePatch		; change patch
	movff	MBFM_SYSEX_IN, MBFM_PATCH	; restore MBFM_BANK and MBFM_PATCH
	movff	MBFM_SYSEX_ADDRESS, MBFM_BANK
MBFM_SYSEX_PatchRead_RAM

	;; send the dump
	rcall	MBFM_SYSEX_SendDump

	;; finish Action
	rgoto	MBFM_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Write
;; --------------------------------------------------------------------------
MBFM_SYSEX_PatchWrite
	;; ensure that synth engine is disabled during transfer
	bsf	MBFM_STAT, MBFM_STAT_ENGINE_DISABLE

	;; end of stream?
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_END, BANKED, MBFM_SYSEX_PatchWrite_End

	;; receive <type> <bank> <patch> <256 bytes> <checksum> F7
MBFM_SYSEX_PatchWrite_Type
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED, MBFM_SYSEX_PatchWrite_Bnk
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_TYPE	; copy type number to MBFM_SYSEX_TYPE
	clrf	MBFM_SYSEX_CHECKSUM, BANKED	; clear checksum
	clrf	MBFM_SYSEX_ADDRESS, BANKED	; clear address
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte

MBFM_SYSEX_PatchWrite_Bnk
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_BANK_RECEIVED, BANKED, MBFM_SYSEX_PatchWrite_Nr
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_BANK_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_BANK	; copy bank number to MBFM_BANK
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte

MBFM_SYSEX_PatchWrite_Nr
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_PATCH_RECEIVED, BANKED, MBFM_SYSEX_PatchWrite_Data
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_PATCH_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_PATCH	; copy patch number to MBFM_PATCH
	rcall	MBFM_SYSEX_SetPatch		; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_ERROR, BANKED
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte

MBFM_SYSEX_PatchWrite_Data
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, MBFM_SYSEX_PatchWrite_Chk

	;; transfer parameter to synth engine
	rcall	MBFM_SYSEX_WritePar

	;; add to checksum
	movf	MBFM_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	MBFM_SYSEX_CHECKSUM, F, BANKED
	
	;; increment address
	incf	MBFM_SYSEX_ADDRESS, F, BANKED

	;; if FSR0L is zero, go into WAIT_CHECKSUM state
	skpnz
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	MBFM_SYSEX_SysExCheck_End

MBFM_SYSEX_PatchWrite_Chk
	;; store received byte in checksum (using MBFM_SYSEX_ADDRESS as store register)
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_ADDRESS

	;; wait for next byte
	rgoto	MBFM_SYSEX_SysExCheck_End


	;; end of sysex command
MBFM_SYSEX_PatchWrite_End
	;; action invalid if checksum has not been received
	BRA_IFCLR MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, MBFM_SYSEX_DisAck_NotComplete
	;; action invalid if patch/bank not available
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_ERROR, BANKED, MBFM_SYSEX_DisAck_BS_NotAvail

	;; calc final checksum
	movf	MBFM_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f

	;; compare with received checksum
	xorwf	MBFM_SYSEX_ADDRESS, W, BANKED

	;; if not equal send disacknowledge
	skpz
	rgoto	MBFM_SYSEX_DisAck_WrongChecksum

	;; branch depending on type
	movlw	0x10-1
	cpfsgt	MBFM_SYSEX_TYPE, BANKED
	rgoto MBFM_SYSEX_PATCH_Write_I
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; allow 0x10/0x18
	xorlw	0x10
	bz	MBFM_SYSEX_PATCH_Write_D
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; allow 0x70/0x78
	xorlw	0x70
	bz	MBFM_SYSEX_PATCH_Write_E
	rgoto	MBFM_SYSEX_PATCH_Write_End

MBFM_SYSEX_PATCH_Write_I
	SET_BSR	MBFM_BASE
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED
	BRA_IFSET MBFM_SYSEX_TYPE, 3, BANKED, MBFM_SYSEX_PATCH_Write_I_RAM
MBFM_SYSEX_PATCH_Write_I_EE
	;; store patch and wavetable
	call	MBFM_BANK_StoreP
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetWTBase
#else
	lfsr	FSR1, MBFM_EE_BUFFER
#endif
  	call	MBFM_BANK_StoreW
	;; initialize instrument from EEPROM
	call	MBFM_INSTR_Init
	rgoto	MBFM_SYSEX_PATCH_Write_Ack

MBFM_SYSEX_PATCH_Write_I_RAM
	;; initialize instrument from RAM
	call	MBFM_INSTR_Init_RAM
	rgoto	MBFM_SYSEX_PATCH_Write_Ack

MBFM_SYSEX_PATCH_Write_D
	;; store drumset
	SET_BSR	MBFM_BASE
	BRA_IFSET MBFM_SYSEX_TYPE, 3, BANKED, MBFM_SYSEX_PATCH_Write_D_RAM
MBFM_SYSEX_PATCH_Write_D_EE
	;; store in EEPROM
	call MBFM_BANK_StoreD	
	;; initialize drum from EEPROM
	call	MBFM_DRUM_Init
	rgoto	MBFM_SYSEX_PATCH_Write_Ack

MBFM_SYSEX_PATCH_Write_D_RAM
	;; initialize drum from RAM
	call	MBFM_DRUM_Init_RAM
	rgoto	MBFM_SYSEX_PATCH_Write_Ack

MBFM_SYSEX_PATCH_Write_E
	;; store ensemble
	SET_BSR	MBFM_BASE
	BRA_IFSET MBFM_SYSEX_TYPE, 3, BANKED, MBFM_SYSEX_PATCH_Write_E_RAM
MBFM_SYSEX_PATCH_Write_E_EE
	;; store ensemble
	call	MBFM_BANK_StoreE
	;; initialize ensemble from EEPROM
	call	MBFM_ENS_Init
	rgoto	MBFM_SYSEX_PATCH_Write_Ack
MBFM_SYSEX_PATCH_Write_E_RAM
	;; initialize ensemble from RAM
	call	MBFM_ENS_Init_RAM
	rgoto	MBFM_SYSEX_PATCH_Write_Ack

MBFM_SYSEX_PATCH_Write_Ack
	;; send acknowledge
	rcall	MBFM_SYSEX_Send_Acknowledge

MBFM_SYSEX_PATCH_Write_End
	;; finish Action
	rgoto	MBFM_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
MBFM_SYSEX_ParWrite
	;; end of stream?
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_END, BANKED, MBFM_SYSEX_ParWrite_End

	;; receive <type> <AH> <AL> <value> F7
MBFM_SYSEX_ParWrite_AT
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED, MBFM_SYSEX_ParWrite_AH
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_TYPE	; copy type number to MBFM_SYSEX_TYPE
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte
	
MBFM_SYSEX_ParWrite_AH
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_AH_RECEIVED, BANKED, MBFM_SYSEX_ParWrite_AL
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_AH_RECEIVED, BANKED
	clrf	MBFM_SYSEX_ADDRESS, BANKED		; clear register and set MBFM_SYSEX_ADDRESS[7] if IN[0] is set
	btfsc	MBFM_SYSEX_IN, 0, BANKED
	bsf	MBFM_SYSEX_ADDRESS, 7, BANKED
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte
	
MBFM_SYSEX_ParWrite_AL
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_AL_RECEIVED, BANKED, MBFM_SYSEX_ParWrite_Data
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_AL_RECEIVED, BANKED
	movf	MBFM_SYSEX_IN, W, BANKED			; OR MBFM_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	MBFM_SYSEX_ADDRESS, F, BANKED
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte

MBFM_SYSEX_ParWrite_Data
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_D_RECEIVED, BANKED, MBFM_SYSEX_ParWrite_Stall
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_D_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_CHECKSUM; store byte in MBFM_SYSEX_CHECKSUM (used as write buffer here)
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte

MBFM_SYSEX_ParWrite_Stall
	;; do nothing until sysex footer (F7) has been received
	rgoto	MBFM_SYSEX_SysExCheck_End

MBFM_SYSEX_ParWrite_End
	;; action invalid if data has not been received
	BRA_IFCLR MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_D_RECEIVED, BANKED, MBFM_SYSEX_DisAck_NotComplete

	;; transfer parameter to synth engine
	movff	MBFM_SYSEX_CHECKSUM, MBFM_SYSEX_IN	; (parameter has been stored in MBFM_SYSEX_CHECKSUM)
	rcall	MBFM_SYSEX_WritePar

	;; send disacknowledge if parameter not available
	skpz
	rgoto	MBFM_SYSEX_DisAck_PAR_NotAvail

	;; send acknowledge
	rcall	MBFM_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MBFM_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Store
;; --------------------------------------------------------------------------
MBFM_SYSEX_PatchStore
	;; end of stream?
	BRA_IFSET MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_END, BANKED, MBFM_SYSEX_PatchStore_End

	;; receive <type> F7
MBFM_SYSEX_PatchStore_Type
	BRA_IFSET MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED, MBFM_SYSEX_PatchStore_Stl
	bsf	MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	MBFM_SYSEX_IN, MBFM_SYSEX_TYPE	; copy type number to MBFM_SYSEX_TYPE
	rgoto	MBFM_SYSEX_SysExCheck_End	; wait for next byte
	
MBFM_SYSEX_PatchStore_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	MBFM_SYSEX_SysExCheck_End


	;; end of sysex command
MBFM_SYSEX_PatchStore_End

	;; action invalid if type has not been received
	BRA_IFCLR MBFM_SYSEX_STATE2, MBFM_SYSEX_STATE2_TYPE_RECEIVED, BANKED, MBFM_SYSEX_DisAck_NotComplete

	;; branch depending on type
	movlw	0x10-1
	cpfsgt	MBFM_SYSEX_TYPE, BANKED
	rgoto MBFM_SYSEX_PATCH_Store_I
	movf	MBFM_SYSEX_TYPE, W, BANKED
	xorlw	0x10
	bz	MBFM_SYSEX_PATCH_Store_D
	movf	MBFM_SYSEX_TYPE, W, BANKED
	xorlw	0x70
	bz	MBFM_SYSEX_PATCH_Store_E
	rgoto	MBFM_SYSEX_PATCH_Store_End

MBFM_SYSEX_PATCH_Store_I
	SET_BSR	MBFM_BASE
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED
	call	MBFM_INSTR_GetBasePatchAndBank
	;; store patch
	call	MBFM_BANK_StoreP
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetWTBase
  	call	MBFM_BANK_StoreW
#else
	;; (don't store wavetable)
	lfsr	FSR1, MBFM_EE_BUFFER
  	call	MBFM_BANK_StoreW
#endif
	rgoto	MBFM_SYSEX_PATCH_Store_Ack

MBFM_SYSEX_PATCH_Store_D
	;; store drumset
	call MBFM_BANK_StoreD	
	rgoto	MBFM_SYSEX_PATCH_Store_Ack

MBFM_SYSEX_PATCH_Store_E
	;; store ensemble
	call	MBFM_BANK_StoreE
	;; 	rgoto	MBFM_SYSEX_PATCH_Store_Ack

MBFM_SYSEX_PATCH_Store_Ack
	;; send acknowledge
	rcall	MBFM_SYSEX_Send_Acknowledge

MBFM_SYSEX_PATCH_Store_End
	;; finish Action
	rgoto	MBFM_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
MBFM_SYSEX_Ping
	;; do nothing until end of stream
	BRA_IFCLR MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_END, BANKED, MBFM_SYSEX_SysExCheck_End

	;; send Acknowledge
	rcall	MBFM_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MBFM_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  Error handling (send disacknowledge)
;; --------------------------------------------------------------------------
MBFM_SYSEX_DisAck_NotComplete
	;; send disacknowledge "not enough bytes have been received"
	movlw	MBFM_DISACK_LESS_BYTES_THAN_EXP
	rgoto	MBFM_SYSEX_DisAck_Cont

MBFM_SYSEX_DisAck_WrongChecksum
	;; send disacknowledge "wrong checksum"
	movlw	MBFM_DISACK_WRONG_CHECKSUM
	rgoto	MBFM_SYSEX_DisAck_Cont

MBFM_SYSEX_DisAck_BS_NotAvail
	;; send disacknowledge "bank (or patch) not available"
	movlw	MBFM_DISACK_BS_NOT_AVAILABLE
	rgoto	MBFM_SYSEX_DisAck_Cont

MBFM_SYSEX_DisAck_PAR_NotAvail
	;; send disacknowledge "parameter not available"
	movlw	MBFM_DISACK_PAR_NOT_AVAILABLE
	;; 	rgoto	MBFM_SYSEX_DisAck_Cont

MBFM_SYSEX_DisAck_Cont
	movwf	TMP2		; error code in WREG
	
	rcall	MBFM_SYSEX_Send_SysExHeader

	movlw	0x0e
	rcall	MBFM_SYSEX_TxBufferPut
	movf	TMP2, W
	rcall	MBFM_SYSEX_TxBufferPut

	movlw	0x01		; (independend from merger state)
	rcall	MBFM_SYSEX_Send_SysExFooter

	rgoto MBFM_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MBFM_SYSEX_Send_Acknowledge
	rcall	MBFM_SYSEX_Send_SysExHeader

	movlw	0x0f		; (acknowledge ID)
	rcall	MBFM_SYSEX_TxBufferPut

	movlw	0x01		; (independend from merger state)
	rgoto	MBFM_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send MBFM SysEx Header (Util function)
;; --------------------------------------------------------------------------
MBFM_SYSEX_Send_SysExHeader
	;; if MBFM_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	SET_BSR	MBFM_BASE
	BRA_IFCLR MBFM_SYSEX_STATE, MBFM_SYSEX_STATE_MYSYSEX, BANKED, MBFM_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MBFM_SYSEX_Send_SysExHeader_Skp
	movlw	0x05		; send only DEVICE_ID
	movwf	TMP1
	rgoto	MBFM_SYSEX_Send_SysExHeaderLoop
MBFM_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
MBFM_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	MBFM_SYSEX_SysExHeaderGet
	rcall	MBFM_SYSEX_TxBufferPut
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto MBFM_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
MBFM_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, MBFM_SYSEX_Send_SysExFooterForce
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

MBFM_SYSEX_Send_SysExFooterForce
	movlw	0xf7
	rgoto	MBFM_SYSEX_TxBufferPut


;; --------------------------------------------------------------------------
;;  This function transfers a parameter to the synth engine
;;  IN: address in MBFM_SYSEX_ADDRESS
;;      type in MBFM_SYSEX_TYPE
;;      parameter value in MBFM_SYSEX_IN
;;      action in MBFM_SYSEX_ACTION (if ACTION == 6, WT values will be written directly into EEPROM)
;;  OUT: zero flag cleared if write failed!
;; --------------------------------------------------------------------------
MBFM_SYSEX_WritePar
	;; copy value to MIOS_PARAMETER1
	movff	MBFM_SYSEX_IN, MIOS_PARAMETER1

	;; branch depending on type
	movlw	0x10-1
	cpfsgt	MBFM_SYSEX_TYPE, BANKED
	rgoto MBFM_SYSEX_WritePar_I
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x10
	bz	MBFM_SYSEX_WritePar_D
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x70
	bz	MBFM_SYSEX_WritePar_E
	rgoto	MBFM_SYSEX_WritePar_Fail

	;; write instrument parameter (patch)
MBFM_SYSEX_WritePar_I
	;; the instrument number is type[1:0]
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED

	;; if 7th bit set, we have to write into EEPROM
	BRA_IFSET MBFM_SYSEX_ADDRESS, 7, BANKED, MBFM_SYSEX_WritePar_I_EE
MBFM_SYSEX_WritePar_I_RAM
	;; get index and call parameter set function
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	call	MBFM_PARIN_SysEx_Set
	rgoto	MBFM_SYSEX_WritePar_End
	
MBFM_SYSEX_WritePar_I_EE
	;; direct write?
	BRA_IFSET MBFM_SYSEX_TYPE, 3, BANKED, MBFM_SYSEX_WritePar_I_EE_Drct
	movlw	0x06
	cpfseq	MBFM_SYSEX_ACTION, BANKED
	rgoto MBFM_SYSEX_WritePar_I_EE_NDrct
MBFM_SYSEX_WritePar_I_EE_Drct
	;; write into EEPROM
	movff	MBFM_SYSEX_ADDRESS, EEADR
	bcf	EEADR, 7
	movf	MBFM_SYSEX_IN, W, BANKED
	call	MBFM_BANK_WriteWByte
	rgoto	MBFM_SYSEX_WritePar_End
MBFM_SYSEX_WritePar_I_EE_NDrct
	;; write into RAM buffer
#if WAVETABLES_IN_RAM
	call	MBFM_INSTR_GetWTBase
#else
	lfsr	FSR1, MBFM_EE_BUFFER
#endif
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	andlw	0x7f
	movff	MBFM_SYSEX_IN, PLUSW1
	rgoto	MBFM_SYSEX_WritePar_End


MBFM_SYSEX_WritePar_D
	;; don't write if address >= 0x70
	movlw	0x70
	cpfslt	MBFM_SYSEX_ADDRESS, BANKED
	rgoto MBFM_SYSEX_WritePar_End

	;; get index and call parameter set function
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	iorlw	0x80
	call	MBFM_PARIN_SysEx_Set
	rgoto	MBFM_SYSEX_WritePar_End
	
MBFM_SYSEX_WritePar_E
	;; don't write if address >= 0x40
	movlw	0x40
	cpfslt	MBFM_SYSEX_ADDRESS, BANKED
	rgoto MBFM_SYSEX_WritePar_End

	;; get index and call parameter set function
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	call	MBFM_PAR_TABLE_ENS_Map
	call	MBFM_PARIN_SysEx_Set
	;; 	rgoto	MBFM_SYSEX_WritePar_End
	
MBFM_SYSEX_WritePar_End
	;; return ZERO flag set
	andlw	0x00
	return

MBFM_SYSEX_WritePar_Fail
	;; return ZERO flag cleared
	iorlw	0xff
	return


;; --------------------------------------------------------------------------
;;  This function returns a parameter from the synth engine
;;  IN: address in MBFM_SYSEX_ADDRESS
;;      type in MBFM_SYSEX_TYPE
;;  OUT: read byte
;; --------------------------------------------------------------------------
MBFM_SYSEX_ReadPar
	;; branch depending on type
	movlw	0x10-1
	cpfsgt	MBFM_SYSEX_TYPE, BANKED
	rgoto MBFM_SYSEX_ReadPar_I
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x10
	bz	MBFM_SYSEX_ReadPar_D
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x70
	bz	MBFM_SYSEX_ReadPar_E
	rgoto	MBFM_SYSEX_ReadPar_End00

	;; read instrument parameter (patch)
MBFM_SYSEX_ReadPar_I
	;; the instrument number is type[1:0]
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED

	;; if 7th bit set, we have to read from EEPROM
	BRA_IFSET MBFM_SYSEX_ADDRESS, 7, BANKED, MBFM_SYSEX_ReadPar_I_EE
MBFM_SYSEX_ReadPar_I_RAM
	;; get index and call parameter get function
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	call	MBFM_PAROUT_SysEx_Get
	rgoto	MBFM_SYSEX_ReadPar_End
	
MBFM_SYSEX_ReadPar_I_EE
	;; read from EEPROM (PIC18F4685: from RAM)
	movff	MBFM_SYSEX_ADDRESS, EEADR
	bcf	EEADR, 7
	call	MBFM_WT_ReadByte
	rgoto	MBFM_SYSEX_ReadPar_End

MBFM_SYSEX_ReadPar_D
	;; read 00 if address >= 0x70
	movlw	0x70
	cpfslt	MBFM_SYSEX_ADDRESS, BANKED
	rgoto MBFM_SYSEX_ReadPar_End00

	;; get index and call parameter get function
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	iorlw	0x80
	call	MBFM_PAROUT_SysEx_Get
	rgoto	MBFM_SYSEX_ReadPar_End

MBFM_SYSEX_ReadPar_E
	;; read 00 if address >= 0x40
	movlw	0x40
	cpfslt	MBFM_SYSEX_ADDRESS, BANKED
	rgoto MBFM_SYSEX_ReadPar_End00

	;; get index and call parameter get function
	movf	MBFM_SYSEX_ADDRESS, W, BANKED
	call	MBFM_PAR_TABLE_ENS_Map
	call	MBFM_PAROUT_SysEx_Get
	rgoto	MBFM_SYSEX_ReadPar_End
	
MBFM_SYSEX_ReadPar_End00
	movlw	0x00

MBFM_SYSEX_ReadPar_End
	return


;; --------------------------------------------------------------------------
;;  This function sets the patch number if MBFM_SYSEX_TYPE < 4 and the
;;  drumset if MBFM_SYSEX_TYPE == 0x10
;;  ensemble if MBFM_SYSEX_TYPE == 0x70
;;  IN: type in MBFM_SYSEX_TYPE
;;      bank in MBFM_BANK
;;      patch in MBFM_PATCH
;;  OUT: zero flag cleared if patch/bank not available
;; --------------------------------------------------------------------------
MBFM_SYSEX_SetPatch
	;; ensure that bank is within 0-7
	movlw	8
	cpfslt	MBFM_BANK, ACCESS
	rgoto MBFM_SYSEX_SetPatch_Fail

	;; check if bank available
	movf	MBFM_BANK, W
	bz	MBFM_SYSEX_SetPatch_Bank0	; bank #0 always available (EEPROM/BankStick)
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	bz	MBFM_SYSEX_SetPatch_Fail
MBFM_SYSEX_SetPatch_Bank0

	;; branch depending on type
	movlw	0x10-1
	cpfsgt	MBFM_SYSEX_TYPE, BANKED
	rgoto MBFM_SYSEX_SetPatch_I
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x10
	bz	MBFM_SYSEX_SetPatch_D
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x70
	bz	MBFM_SYSEX_SetPatch_E
	rgoto	MBFM_SYSEX_SetPatch_Fail

MBFM_SYSEX_SetPatch_I
	;; the instrument number is type[1:0]
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED

	;; get base address to instrument
	call	MBFM_INSTR_GetBase

	;; copy bank to Ix_BANK
	movlw	MBFM_Ix_BANK
	movff	MBFM_BANK, PLUSW1

	;; copy patch to Ix_PATCH
	movlw	MBFM_Ix_PATCH
	movff	MBFM_PATCH, PLUSW1

	rgoto	MBFM_SYSEX_SetPatch_End

MBFM_SYSEX_SetPatch_D
	;; check drumset number
	movlw	16
	cpfslt	MBFM_PATCH, ACCESS
	rgoto MBFM_SYSEX_SetPatch_Fail

	;; copy bank number to MBFM_DBANK
	movff	MBFM_BANK, MBFM_DBANK

	;; copy patch number to MBFM_DRUMSET
	movff	MBFM_PATCH, MBFM_DRUMSET
	rgoto	MBFM_SYSEX_SetPatch_End

MBFM_SYSEX_SetPatch_E
	;; check ensemble number
	movlw	32
	cpfslt	MBFM_PATCH, ACCESS
	rgoto MBFM_SYSEX_SetPatch_Fail

	;; copy bank number to MBFM_EBANK
	movff	MBFM_BANK, MBFM_EBANK

	;; copy patch number to MBFM_ENSEMBLE
	movff	MBFM_PATCH, MBFM_ENSEMBLE
	;; 	rgoto	MBFM_SYSEX_SetPatch_End

MBFM_SYSEX_SetPatch_End
	;; return ZERO flag set
	andlw	0x00
	return

MBFM_SYSEX_SetPatch_Fail
	;; return ZERO flag cleared
	iorlw	0xff
	return


;; --------------------------------------------------------------------------
;;  This function changes to the selected patch
;;  IN: type in MBFM_SYSEX_TYPE
;; --------------------------------------------------------------------------
MBFM_SYSEX_ChangePatch
	;; check if bank is available
	movlw	0x10-1
	cpfsgt	MBFM_SYSEX_TYPE, BANKED
	rgoto MBFM_SYSEX_ChangePatch_I
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x10
	bz	MBFM_SYSEX_ChangePatch_D
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x70
	bz	MBFM_SYSEX_ChangePatch_E
	rgoto	MBFM_SYSEX_ChangePatch_End

MBFM_SYSEX_ChangePatch_I
	;; the instrument number is type[1:0]
	movf	MBFM_SYSEX_TYPE, W, BANKED
	andlw	0x03
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED
	call	MBFM_INSTR_Init
	rgoto	MBFM_SYSEX_ChangePatch_End

MBFM_SYSEX_ChangePatch_D
	call	MBFM_DRUM_Init
	rgoto	MBFM_SYSEX_ChangePatch_End

MBFM_SYSEX_ChangePatch_E
	call	MBFM_ENS_Init
	rgoto	MBFM_SYSEX_ChangePatch_End

MBFM_SYSEX_ChangePatch_End
	return


;; --------------------------------------------------------------------------
;;  This function sends a parameter dump of 256 bytes
;;  IN: type in MBFM_SYSEX_TYPE
;; --------------------------------------------------------------------------
MBFM_SYSEX_SendDump
	;; send SysEx header
	rcall	MBFM_SYSEX_Send_SysExHeader

	;; send PATCH_Write ID
	movlw	0x02
	rcall	MBFM_SYSEX_TxBufferPut

	;; send requested type number
	movff	MBFM_SYSEX_TYPE, WREG
	rcall	MBFM_SYSEX_TxBufferPut

	;; send requested bank number
	movf	MBFM_BANK, W
	rcall	MBFM_SYSEX_TxBufferPut

	;; send requested patch number
	movf	MBFM_PATCH, W
	rcall	MBFM_SYSEX_TxBufferPut

	;; clear checksum
	SET_BSR	MBFM_BASE
	clrf	MBFM_SYSEX_CHECKSUM, BANKED

	;; send a dump of 256 bytes
	clrf	MBFM_SYSEX_ADDRESS
MBFM_SYSEX_SendDump_Loop
	;; get parameter
	call	MBFM_SYSEX_ReadPar
	;; add byte to checksum
	andlw	0x7f
	addwf	MBFM_SYSEX_CHECKSUM, F, BANKED
	;; and send byte
	rcall	MBFM_SYSEX_TxBufferPut
	SET_BSR	MBFM_BASE

	;; increment loop counter (256 times)
	incf	MBFM_SYSEX_ADDRESS, F
	bnz	MBFM_SYSEX_SendDump_Loop

	;; send checksum
	movff	MBFM_SYSEX_CHECKSUM, WREG
	sublw	0x80
	andlw	0x7f
	rcall	MBFM_SYSEX_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rgoto	MBFM_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Forward a MIDI byte to the MIOS function (allows the use of "rcall")
;; --------------------------------------------------------------------------
MBFM_SYSEX_TxBufferPut
	goto	MIOS_MIDI_TxBufferPut
