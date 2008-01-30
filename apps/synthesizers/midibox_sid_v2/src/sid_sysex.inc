; $Id$
;
; MIDIbox SID SysEx Parser
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

SID_SYSEX_STATE_MYSYSEX		EQU	7
SID_SYSEX_STATE_ACTION		EQU	6
SID_SYSEX_STATE_END		EQU	5
SID_SYSEX_STATE_ERROR		EQU	4

SID_SYSEX_STATE2_TYPE_RECEIVED	EQU	0	; used by most actions

SID_SYSEX_STATE2_BANK_RECEIVED	EQU	1	; used by Action PATCH_[Read/Write]
SID_SYSEX_STATE2_PATCH_RECEIVED EQU	2	; used by Action PATCH_[Read/Write]
SID_SYSEX_STATE2_WAIT_CHECKSUM	EQU	3	; used by Action PATCH_Write
SID_SYSEX_STATE2_LNIBBLE_RECV	EQU	4	; used by Action PATCH_Write

SID_SYSEX_STATE2_AH_RECEIVED	EQU	1	; used by Action PAR_[Read/Write]
SID_SYSEX_STATE2_AL_RECEIVED	EQU	2	; used by Action PAR_[Read/Write]
SID_SYSEX_STATE2_DL_RECEIVED	EQU	3	; used by Action PAR_[Write]
SID_SYSEX_STATE2_DH_RECEIVED	EQU	4	; used by Action PAR_[Write]

SID_DISACK_LESS_BYTES_THAN_EXP	EQU     0x01
SID_DISACK_WRONG_CHECKSUM	EQU     0x03
SID_DISACK_BS_NOT_AVAILABLE	EQU	0x0a
SID_DISACK_PAR_NOT_AVAILABLE	EQU	0x0b
SID_DISACK_NO_RAM_ACCESS	EQU	0x10
SID_DISACK_BS_TOO_SMALL		EQU	0x11

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SID_SYSEX_Parser
	;; store received byte in SID_SYSEX_IN
	SET_BSR	SID_BASE
	movwf	SID_SYSEX_IN, BANKED

	;; ignore realtime messages
	movlw	0xf8
	cpfslt	SID_SYSEX_IN, BANKED
	return

	;; check sysex state
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_MYSYSEX, BANKED, SID_SYSEX_Handler

	movf	SID_SYSEX_STATE, W, BANKED
	rcall	SID_SYSEX_SysExHeaderGet
	cpfseq	SID_SYSEX_IN, BANKED
	rgoto SID_SYSEX_SysExCheckFailed
	incf	SID_SYSEX_STATE, F, BANKED
	movf	SID_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 4B <device-id>)
	bnz	SID_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	movlw	(1 << SID_SYSEX_STATE_MYSYSEX)
	movwf	SID_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable
	rgoto	SID_SYSEX_SysExCheckOk

SID_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rgoto	SID_SYSEX_ActionInvalid

SID_SYSEX_SysExCheckOk
SID_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
SID_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x4b		; MIDIboxSID V2 ID (4B - the ultimative number + 9)
	movf	SID_MIDI_DEVICE, W
	return

;; --------------------------------------------------------------------------
;;  Action Invalid will be invoked when receiving an invalid sequence
;; --------------------------------------------------------------------------
SID_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  Action finished will be invoked when midi action is done
;; --------------------------------------------------------------------------
SID_SYSEX_ActionFinished
	SET_BSR	SID_BASE
	clrf	SID_SYSEX_STATE, BANKED
	clrf	SID_SYSEX_STATE2, BANKED
	clrf	SID_SYSEX_ACTION, BANKED

	call	MIOS_MPROC_MergerEnable
	rgoto	SID_SYSEX_SysExCheck_End


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
SID_SYSEX_Handler
	SET_BSR	SID_BASE

	;; if first byte after header, store command ID
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_ACTION, BANKED, SID_SYSEX_Handler_NotBegin
SID_SYSEX_Handler_Begin
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_ACTION, BANKED
	movff	SID_SYSEX_IN, SID_SYSEX_ACTION
	rgoto	SID_SYSEX_SysExCheck_End
SID_SYSEX_Handler_NotBegin

	;; fetch data until next status byte (-> 0xf7)
	BRA_IFCLR SID_SYSEX_IN, 7, BANKED, SID_SYSEX_Handler_NotEnd
SID_SYSEX_Handler_End
	;; if sysex footer (F7) has not been received here, command is invalid!
	movlw	0xf7
	cpfseq	SID_SYSEX_IN, BANKED
	rgoto SID_SYSEX_ActionInvalid
	;; notify end
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_END, BANKED
SID_SYSEX_Handler_NotEnd

	;; branch depending on current action ID
	decf	SID_SYSEX_ACTION, W, BANKED
	BRA_IFSET STATUS, Z, ACCESS, SID_SYSEX_PatchRead	; 1
	addlw	-2+1
	BRA_IFSET STATUS, Z, ACCESS, SID_SYSEX_PatchWrite	; 2
	addlw	-3+2
	BRA_IFSET STATUS, Z, ACCESS, SID_SYSEX_ReadAll	; 3
	addlw	-6+3
	BRA_IFSET STATUS, Z, ACCESS, SID_SYSEX_ParWrite	; 6
	addlw	-15+6
	BRA_IFSET STATUS, Z, ACCESS, SID_SYSEX_Ping	; 15
	rgoto	SID_SYSEX_ActionInvalid
	;; (note: target address to far away for "bz", therefore I'm using "IFSET STATUS, Z"

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Read
;; --------------------------------------------------------------------------
SID_SYSEX_PatchRead
	;; end of stream?
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_END, BANKED, SID_SYSEX_PatchRead_End

	;; receive <type> <bank> <patch> F7
SID_SYSEX_PatchRead_Type
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED, SID_SYSEX_PatchRead_Bnk
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_SYSEX_TYPE	; copy type number to SID_SYSEX_TYPE
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_PatchRead_Bnk
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED, SID_SYSEX_PatchRead_Nr
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_BANK	; copy bank number to SID_BANK
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_PatchRead_Nr
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_PATCH_RECEIVED, BANKED, SID_SYSEX_PatchRead_Stl
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_PATCH_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_PATCH	; copy patch number to SID_PATCH
	rcall	SID_SYSEX_SetPatch		; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_ERROR, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_PatchRead_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End


	;; end of sysex command
SID_SYSEX_PatchRead_End

	;; action invalid if patch number has not been received
	BRA_IFCLR SID_SYSEX_STATE2, SID_SYSEX_STATE2_PATCH_RECEIVED, BANKED, SID_SYSEX_DisAck_NotComplete

	;; action invalid if patch/bank not available
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_ERROR, BANKED, SID_SYSEX_DisAck_BS_NotAvail

	;; action invalid if patch >= 64 should be read from a too small bankstick
	rcall	SID_SYSEX_Check_BS_Size
	skpz
	rgoto	SID_SYSEX_DisAck_BSTooSmall

	;; change to ensemble/patches (will be skipped if RAM flag set)
	rcall	SID_SYSEX_ChangePatch		; change patch

	;; send the dump
	rcall	SID_SYSEX_SendDump

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Write
;; --------------------------------------------------------------------------
SID_SYSEX_PatchWrite
	;; ensure that synth engine is disabled during transfer
	;; (the LEVEL0 flag will be cleared within USER_Tick - this ensures best performance
	;; on back-to-back transfers)
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE_LEVEL0

	;; end of stream?
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_END, BANKED, SID_SYSEX_PatchWrite_End

	;; receive <type> <bank> <patch> <2*512 bytes> <checksum> F7
SID_SYSEX_PatchWrite_Type
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED, SID_SYSEX_PatchWrite_Bnk
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_SYSEX_TYPE	; copy type number to SID_SYSEX_TYPE
	clrf	SID_SYSEX_CHECKSUM, BANKED	; clear checksum
	clrf	SID_SYSEX_ADDRESS_L, BANKED	; clear address
	clrf	SID_SYSEX_ADDRESS_H, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_PatchWrite_Bnk
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED, SID_SYSEX_PatchWrite_Nr
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_BANK		; copy bank number to SID_BANK
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_PatchWrite_Nr
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_PATCH_RECEIVED, BANKED, SID_SYSEX_PatchWrite_Data
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_PATCH_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_PATCH		; copy patch number to SID_PATCH
	rcall	SID_SYSEX_SetPatch		; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_ERROR, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_PatchWrite_Data
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, SID_SYSEX_PatchWrite_Chk

	;; low or high nibble?
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_LNIBBLE_RECV, BANKED, SID_SYSEX_PatchWrite_Data_U
SID_SYSEX_PatchWrite_Data_L
	;; lower nibble received
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_LNIBBLE_RECV, BANKED

	;; store low-nibble in SID_SYSEX_DATA
	movf	SID_SYSEX_IN, W, BANKED
	andlw	0x0f
	movwf	SID_SYSEX_DATA, BANKED

	;; add to checksum
	movf	SID_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	SID_SYSEX_CHECKSUM, F, BANKED

	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_PatchWrite_Data_U
	;; upper nibble received
	bcf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_LNIBBLE_RECV, BANKED

	;; OR high-nibble to SID_SYSEX_DATA
	swapf	SID_SYSEX_IN, W, BANKED
	andlw	0xf0
	iorwf	SID_SYSEX_DATA, F, BANKED

	;; add to checksum
	movf	SID_SYSEX_IN, W, BANKED
	andlw	0x7f
	addwf	SID_SYSEX_CHECKSUM, F, BANKED

	;; extra for environment: a direct transfer would be too slow (MIDI IN buffer overrun)
	;; therefore data is transfered to SID_EDIT_BUFFER first, before it will be written into EEPROM and transfered to slaves
	movf	SID_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; mask out "RAM" flag
	xorlw	0x70
	bz	SID_SYSEX_PatchWrite_Data_U_E
SID_SYSEX_PatchWrite_Data_U_I
	;; transfer parameter to synth engine
	rcall	SID_SYSEX_WritePar

	;; increment address
	incf	SID_SYSEX_ADDRESS_L, F, BANKED
	skpnz
	incf	SID_SYSEX_ADDRESS_H, F, BANKED

	;; once high byte is 2, go into WAIT_CHECKSUM state
	movf	SID_SYSEX_ADDRESS_H, W, BANKED
	xorlw	0x02
	skpnz
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_PatchWrite_Data_U_E
	;; temporary store ensemble data into SID_EDIT_BUFFER
	lfsr	FSR0, SID_EDIT_BUFFER
	movff	SID_SYSEX_ADDRESS_L, FSR0L
	movff	SID_SYSEX_DATA, INDF0

	;; increment address
	incf	SID_SYSEX_ADDRESS_L, F, BANKED
	;; on overrun, go into WAIT_CHECKSUM state
	skpnz
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_WAIT_CHECKSUM, BANKED

	;; wait for next byte
	rgoto	SID_SYSEX_SysExCheck_End


SID_SYSEX_PatchWrite_Chk
	;; store received byte in checksum (using SID_SYSEX_DATA as store register)
	movff	SID_SYSEX_IN, SID_SYSEX_DATA

	;; wait for next byte
	rgoto	SID_SYSEX_SysExCheck_End


	;; end of sysex command
SID_SYSEX_PatchWrite_End
	;; action invalid if checksum has not been received
	BRA_IFCLR SID_SYSEX_STATE2, SID_SYSEX_STATE2_WAIT_CHECKSUM, BANKED, SID_SYSEX_DisAck_NotComplete

	;; action invalid if patch/bank not available
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_ERROR, BANKED, SID_SYSEX_DisAck_BS_NotAvail

	;; action invalid if patch >= 64 should be read from a too small bankstick
	rcall	SID_SYSEX_Check_BS_Size
	skpz
	rgoto	SID_SYSEX_DisAck_BSTooSmall

	;; calc final checksum
	movf	SID_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f

	;; compare with received checksum
	xorwf	SID_SYSEX_DATA, W, BANKED

	;; if not equal send disacknowledge
	skpz
	rgoto	SID_SYSEX_DisAck_WrongChecksum

	;; branch depending on type
	movf	SID_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; mask out "RAM" flag
	bz	SID_SYSEX_PATCH_Write_I
	xorlw	0x70
	bz	SID_SYSEX_PATCH_Write_E
	rgoto	SID_SYSEX_PATCH_Write_End

SID_SYSEX_PATCH_Write_I
	;; (only write into RAM?)
	BRA_IFSET SID_SYSEX_TYPE, 3, BANKED, SID_SYSEX_PATCH_Write_I_RAM
SID_SYSEX_PATCH_Write_I_EE
	;; write buffer to EEPROM
	lfsr	FSR1, SID_PATCH_BUFFER	; init pointer to patch buffer
	clrf	EEADR		; use EEADR as loop counter
	clrf	EEADRH
SID_SYSEX_PATCH_Write_ILoop
	clrwdt			; feed watchdog
	call	SID_PBANK_WritePage ; write a 64 bytes page to EEPROM
	;; increment FSR1 by 0x40
	movlw	0x40
	addwf	FSR1L, F
	movlw	0
	addwfc	FSR1H, F
	movf	EEADR, W	; until all 512 bytes are written
	bnz	SID_SYSEX_PATCH_Write_ILoop
	movlw	2-1
	cpfsgt	EEADRH, ACCESS
	rgoto SID_SYSEX_PATCH_Write_ILoop

SID_SYSEX_PATCH_Write_I_RAM
	;; print message if CS enabled
	TABLE_ADDR SID_SYSEX_PATCH_Write_I_STR
	call	MIOS_LCD_PrintMessage
	
	movlw	0x00 + 6
	call	MIOS_LCD_CursorSet
	call	SID_LCD_PrintPatchNumber

	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, SID_PATCH_BUFFER	; init pointer to patch buffer (name located between 0x00-0x0f)
SID_SYSEX_PATCH_WritePrnLoop
	movf	POSTINC0, W
	call	MIOS_LCD_PrintChar
	movf	FSR0L, W
	andlw	0x0f
	bnz	SID_SYSEX_PATCH_WritePrnLoop
	movlw	4
	call	SID_LCD_PrintSpaces

	;; if CS: re-init patch here (will be skipped if RAM flag set)
	rcall	SID_SYSEX_ChangePatch
	;;  update edit buffer if required
	call	CS_MENU_MS_UpdateEditBufferMS
	;; ensure that patch won't be initialized again
	bcf	SID_STAT, SID_STAT_ENGINE_DISABLE

	rgoto	SID_SYSEX_PATCH_Write_Ack

SID_SYSEX_PATCH_Write_I_STR
	STRING	20, 0x00, "Patch A  1 uploaded "
SID_SYSEX_PATCH_Write_E_STR
	STRING	20, 0x00, "Ensemble xxxx upld. "

SID_SYSEX_PATCH_Write_E
	;; write into EEPROM or RAM?
	BRA_IFSET SID_SYSEX_TYPE, 3, BANKED, SID_SYSEX_PATCH_Write_E_RAM
SID_SYSEX_PATCH_Write_E_EE
	;; write buffer to EEPROM
	lfsr	FSR1, SID_EDIT_BUFFER
	clrf	EEADR		; use EEADR as loop counter
SID_SYSEX_PATCH_Write_E_EE_Loop
	clrwdt			; feed watchdog
	call	SID_EBANK_WritePage ; write a 64 bytes page to EEPROM
	;; increment FSR1 by 0x40
	movlw	0x40
	addwf	FSR1L, F
	movf	EEADR, W	; until all 256 bytes are written
	bnz	SID_SYSEX_PATCH_Write_E_EE_Loop
	rgoto	SID_SYSEX_PATCH_Write_E_EE_Cont

SID_SYSEX_PATCH_Write_E_RAM
	;; transfer SID_EDIT_BUFFER content into ENS slots
	;; use a very slow, but secure method here
	clrf	SID_SYSEX_ADDRESS_L, BANKED	; used as loop counter
SID_SYSEX_PATCH_Write_E_RAM_Loop
	clrwdt			; feed watchdog
	lfsr	FSR1, SID_EDIT_BUFFER
	movff	SID_SYSEX_ADDRESS_L, FSR1L
	movff	INDF1, SID_SYSEX_DATA
	rcall	SID_SYSEX_WritePar
	incf	SID_SYSEX_ADDRESS_L, F, BANKED
	bnz	SID_SYSEX_PATCH_Write_E_RAM_Loop


SID_SYSEX_PATCH_Write_E_EE_Cont
	;; print message if CS enabled
	TABLE_ADDR SID_SYSEX_PATCH_Write_E_STR
	call	MIOS_LCD_PrintMessage
	
	movlw	0x00 + 9
	call	MIOS_LCD_CursorSet
	call	SID_LCD_PrintEnsembleNumber

	;; if no direct RAM upload: re-init ensemble here
	SET_BSR	SID_BASE
	CALL_IFCLR SID_SYSEX_TYPE, 3, BANKED, SID_ENS_Init

	;; reload ensemble edit buffer with data of currently selected SID
	call	CS_MENU_MS_UpdateEnsBuffer
	;; reload patch edit buffer
	call	CS_MENU_MS_UpdateEditBufferNow
	;; ensure that patch won't be initialized again
	bcf	SID_STAT, SID_STAT_ENGINE_DISABLE

	;; 	rgoto	SID_SYSEX_PATCH_Write_Ack

SID_SYSEX_PATCH_Write_Ack
	;; send acknowledge
	rcall	SID_SYSEX_Send_Acknowledge

SID_SYSEX_PATCH_Write_End
	;; finish Action
	rgoto	SID_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Read All
;; --------------------------------------------------------------------------
SID_SYSEX_ReadAll
	;; end of stream?
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_END, BANKED, SID_SYSEX_ReadAll_End

	;; receive <type> <bank> F7
SID_SYSEX_ReadAll_Type
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED, SID_SYSEX_ReadAll_Bnk
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_SYSEX_TYPE	; copy type number to SID_SYSEX_TYPE
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_ReadAll_Bnk
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED, SID_SYSEX_ReadAll_Stl
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_BANK	; copy bank number to SID_BANK
	clrf	SID_PATCH		; start with patch #1
	rcall	SID_SYSEX_SetPatch	; change patch if required
	skpz					; set error flag if patch/bank not available
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_ERROR, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_ReadAll_Stl
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

	;; end of sysex command
SID_SYSEX_ReadAll_End

	;; action invalid if patch number has not been received
	BRA_IFCLR SID_SYSEX_STATE2, SID_SYSEX_STATE2_BANK_RECEIVED, BANKED, SID_SYSEX_DisAck_NotComplete
	;; action invalid if patch/bank not available
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_ERROR, BANKED, SID_SYSEX_DisAck_BS_NotAvail

	;; branch depending on patch/ensemble
	movf	SID_SYSEX_TYPE, W, BANKED
	bz	SID_SYSEX_ReadAll_I
	xorlw	0x70
	bz	SID_SYSEX_ReadAll_E
	rgoto	SID_SYSEX_DisAck_NoRAM	; (will also be sent if type != 00 and != 70, regardless of bit #3)

SID_SYSEX_ReadAll_I
	;; send the dump of all patches
	clrf	SID_PATCH
SID_SYSEX_ReadAll_I_Loop
	clrwdt					; feed watchdog
	rcall	SID_SYSEX_ChangePatch		; change patch
	rcall	SID_SYSEX_SendDump		; send the dump

	incf	SID_PATCH, F			; loop until last patch reached
	movf	SID_BANK, W			
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_SIZE, W
	movlw	128-1
	skpnz
	movlw	64-1
	cpfsgt	SID_PATCH, W
	rgoto SID_SYSEX_ReadAll_I_Loop

	;; back to first patch
	clrf	SID_PATCH
	rcall	SID_SYSEX_ChangePatch		; change patch

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished


SID_SYSEX_ReadAll_E
	;; store initial ensemble in TMP5
	movff	SID_ENSEMBLE, TMP5
	;; send the dump of all ensembles
	clrf	SID_ENSEMBLE
SID_SYSEX_ReadAll_E_Loop
	clrwdt					; feed watchdog
	call	SID_ENS_Init			; load ensemble
	movff	SID_ENSEMBLE, SID_PATCH		; (ensemble number must be located in SID_PATCH)
	rcall	SID_SYSEX_SendDump		; send the dump

	incf	SID_ENSEMBLE, F			; loop until last ensemble reached
	BRA_IFCLR SID_ENSEMBLE, 7, ACCESS, SID_SYSEX_ReadAll_E_Loop

	;; back to first ensemble
	clrf	SID_ENSEMBLE
	call	SID_ENS_Init

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
SID_SYSEX_ParWrite
	;; end of stream?
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_END, BANKED, SID_SYSEX_ParWrite_End

	;; receive <type> <AH> <AL> <DL> <DH> F7
SID_SYSEX_ParWrite_AT
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED, SID_SYSEX_ParWrite_AH
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_TYPE_RECEIVED, BANKED
	movff	SID_SYSEX_IN, SID_SYSEX_TYPE	; copy type number to SID_SYSEX_TYPE
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_ParWrite_AH
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_AH_RECEIVED, BANKED, SID_SYSEX_ParWrite_AL
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_AH_RECEIVED, BANKED
	rrf	SID_SYSEX_IN, W, BANKED		; store upper part of address
	andlw	0x03
	movwf	SID_SYSEX_ADDRESS_H, BANKED
	clrf	SID_SYSEX_ADDRESS_L, BANKED	; clear register and set SID_SYSEX_ADDRESS[7] if IN[0] is set
	btfsc	SID_SYSEX_IN, 0, BANKED
	bsf	SID_SYSEX_ADDRESS_L, 7, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_ParWrite_AL
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_AL_RECEIVED, BANKED, SID_SYSEX_ParWrite_DL
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_AL_RECEIVED, BANKED
	movf	SID_SYSEX_IN, W, BANKED			; OR SID_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	SID_SYSEX_ADDRESS_L, F, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_ParWrite_DL
	BRA_IFSET SID_SYSEX_STATE2, SID_SYSEX_STATE2_DL_RECEIVED, BANKED, SID_SYSEX_ParWrite_DH
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_DL_RECEIVED, BANKED
	
	;; store low-nibble in SID_SYSEX_DATA
	movf	SID_SYSEX_IN, W, BANKED
	andlw	0x0f
	movwf	SID_SYSEX_DATA, BANKED
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_ParWrite_DH
	;; switch back to DL received, this allows to send continuous data
	bcf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_DL_RECEIVED, BANKED
	;; notify that at least one DH has been received
	bsf	SID_SYSEX_STATE2, SID_SYSEX_STATE2_DH_RECEIVED, BANKED

	;; OR high-nibble to SID_SYSEX_DATA
	swapf	SID_SYSEX_IN, W, BANKED
	andlw	0xf0
	iorwf	SID_SYSEX_DATA, BANKED

	;; transfer parameter to synth engine
	rcall	SID_SYSEX_WritePar

	;; increment address
	incf	SID_SYSEX_ADDRESS_L, F, BANKED
	skpnz
	incf	SID_SYSEX_ADDRESS_H, F, BANKED

	;; if ensemble: update ens buffer
	CALL_IFSET SID_SYSEX_TYPE, 6, BANKED, CS_MENU_MS_UpdateEnsBuffer
	SET_BSR	SID_BASE

	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_ParWrite_End
	;; action invalid if data has not been received
	BRA_IFCLR SID_SYSEX_STATE2, SID_SYSEX_STATE2_DH_RECEIVED, BANKED, SID_SYSEX_DisAck_NotComplete

	;; send acknowledge
	rcall	SID_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

	
;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
SID_SYSEX_Ping
	;; do nothing until end of stream
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_END, BANKED, SID_SYSEX_SysExCheck_End

	;; send Acknowledge
	rcall	SID_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  Error handling (send disacknowledge)
;; --------------------------------------------------------------------------
SID_SYSEX_DisAck_NotComplete
	;; send disacknowledge "not enough bytes have been received"
	movlw	SID_DISACK_LESS_BYTES_THAN_EXP
	rgoto	SID_SYSEX_DisAck_Cont

SID_SYSEX_DisAck_WrongChecksum
	;; send disacknowledge "wrong checksum"
	movlw	SID_DISACK_WRONG_CHECKSUM
	rgoto	SID_SYSEX_DisAck_Cont

SID_SYSEX_DisAck_BS_NotAvail
	;; send disacknowledge "bank (or patch) not available"
	movlw	SID_DISACK_BS_NOT_AVAILABLE
	rgoto	SID_SYSEX_DisAck_Cont

SID_SYSEX_DisAck_PAR_NotAvail
	;; send disacknowledge "parameter not available"
	movlw	SID_DISACK_PAR_NOT_AVAILABLE
 	rgoto	SID_SYSEX_DisAck_Cont

SID_SYSEX_DisAck_NoRAM
	;; send disacknowledge "no RAM access"
	movlw	SID_DISACK_NO_RAM_ACCESS
	rgoto	SID_SYSEX_DisAck_Cont

SID_SYSEX_DisAck_BSTooSmall
	;; send disacknowledge "BankStick too small"
	movlw	SID_DISACK_BS_TOO_SMALL
	;; 	rgoto	SID_SYSEX_DisAck_Cont

SID_SYSEX_DisAck_Cont
	movwf	TMP2		; error code in WREG
	
	rcall	SID_SYSEX_Send_SysExHeader

	movlw	0x0e
	rcall	SID_SYSEX_TxBufferPut
	movf	TMP2, W
	rcall	SID_SYSEX_TxBufferPut

#if 0
	movff	SID_SYSEX_ADDRESS_H, WREG
	swapf	WREG, W
	andlw	0x0f
	call	SID_SYSEX_TxBufferPut
	movff	SID_SYSEX_ADDRESS_H, WREG
	andlw	0x0f
	call	SID_SYSEX_TxBufferPut
	movff	SID_SYSEX_ADDRESS_L, WREG
	swapf	WREG, W
	andlw	0x0f
	call	SID_SYSEX_TxBufferPut
	movff	SID_SYSEX_ADDRESS_L, WREG
	andlw	0x0f
	call	SID_SYSEX_TxBufferPut
#endif

	movlw	0x01		; (independend from merger state)
	rcall	SID_SYSEX_Send_SysExFooter

	rgoto SID_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
SID_SYSEX_Send_Acknowledge
	rcall	SID_SYSEX_Send_SysExHeader

	movlw	0x0f		; (acknowledge ID)
	rcall	SID_SYSEX_TxBufferPut

	movlw	0x01		; (independend from merger state)
	rgoto	SID_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send SID SysEx Header (Util function)
;; --------------------------------------------------------------------------
SID_SYSEX_Send_SysExHeader
	;; if SID_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	SET_BSR	SID_BASE
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_MYSYSEX, BANKED, SID_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	SID_SYSEX_Send_SysExHeader_Skp
	movlw	0x05		; send only DEVICE_ID
	movwf	TMP1
	rgoto	SID_SYSEX_Send_SysExHeaderLoop
SID_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
SID_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	SID_SYSEX_SysExHeaderGet
	rcall	SID_SYSEX_TxBufferPut
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto SID_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
SID_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, SID_SYSEX_Send_SysExFooterForce
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

SID_SYSEX_Send_SysExFooterForce
	movlw	0xf7
	rgoto	SID_SYSEX_TxBufferPut


;; --------------------------------------------------------------------------
;;  This function transfers a parameter to the synth engine
;;  IN: address in SID_SYSEX_ADDRESS_[LH]
;;      type in SID_SYSEX_TYPE
;;      parameter value in SID_SYSEX_DATA
;;  OUT: zero flag cleared if write failed!
;; --------------------------------------------------------------------------
SID_SYSEX_WritePar
	SET_BSR	SID_BASE

	;; branch depending on type
	movf	SID_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; mask out "RAM" flag
	bz	SID_SYSEX_WritePar_I
	xorlw	0x70
	bz	SID_SYSEX_WritePar_E
	rgoto	SID_SYSEX_WritePar_Fail

	;; write instrument parameter (patch)
SID_SYSEX_WritePar_I
	;; transfer value (stored in SID_SYSEX_CHECKSUM) into patch buffer
	lfsr	FSR1, SID_PATCH_BUFFER
	movf	SID_SYSEX_ADDRESS_L, W, BANKED
	addwf	FSR1L, F	; (PLUSW1 not working here due to address >= 0x80)
	movf	SID_SYSEX_ADDRESS_H, W, BANKED
	addwfc	FSR1H, F
	movff	SID_SYSEX_DATA, INDF1

	;; transfer to shadow buffer as well
	movlw	HIGH(SID_PATCH_BUFFER_SHADOW)-HIGH(SID_PATCH_BUFFER)
	addwf	FSR1H, F
	movff	SID_SYSEX_DATA, INDF1

	;; transfer value to edit buffer as well if master selected
	movf	CS_MENU_EDIT_BUFFER_SID, W
	bnz	SID_SYSEX_End_PAR_Write_NoMS
SID_SYSEX_End_PAR_Write_MS
	movlw	HIGH(SID_EDIT_BUFFER)-HIGH(SID_PATCH_BUFFER_SHADOW)
	addwf	FSR1H, F
	movff	SID_SYSEX_DATA, INDF1
SID_SYSEX_End_PAR_Write_NoMS
	rgoto	SID_SYSEX_WritePar_End


SID_SYSEX_WritePar_E
	;; select SID depending on address
	swapf	SID_SYSEX_ADDRESS_L, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	movwf	CS_MENU_SID

	;; branch depending on master/slave
	bnz	SID_SYSEX_WritePar_E_S
SID_SYSEX_WritePar_E_M
	call	SID_ENS_LocalSetupIntoBuffer
	rgoto	SID_SYSEX_WritePar_E_M_Cont

SID_SYSEX_WritePar_E_S
	;; load SID related part of ensemble into edit buffer
	call	CS_MENU_MBNET_Tx_GetEns
SID_SYSEX_WritePar_E_M_Cont
	;; change value
	lfsr	FSR0, SID_ENS_BUFFER
	movff	SID_SYSEX_ADDRESS_L, WREG
	andlw	0x3f
	addwf	FSR0L, F
	movff	SID_SYSEX_DATA, INDF0

	;; transfer byte to ensemble
	call	CS_MENU_MS_SendEnsParameter

	;; reload edit buffer with data of currently selected SID
	;; 	call	CS_MENU_MS_UpdateEnsBuffer
	;; (done externally after call)

	;; update CS Channel/Bank/Patch if required
	movff	SID_SYSEX_ADDRESS_L, WREG
	andlw	0x3f
	xorlw	SID_ENSx_I1_BASE + SID_ENSx_Ix_CHN
	bz	SID_SYSEX_WritePar_E_Chn
	xorlw	SID_ENSx_BANK ^ (SID_ENSx_I1_BASE + SID_ENSx_Ix_CHN)
	bz	SID_SYSEX_WritePar_E_Bank
	xorlw	SID_ENSx_PATCH ^ SID_ENSx_BANK
	bz	SID_SYSEX_WritePar_E_Patch
	rgoto	SID_SYSEX_WritePar_E_End

SID_SYSEX_WritePar_E_Chn
	;; update variable in CS
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	movff	SID_SYSEX_DATA, PLUSW0
	rgoto	SID_SYSEX_WritePar_E_End

SID_SYSEX_WritePar_E_Bank
	;; update variable in CS
	lfsr	FSR0, CS_MENU_SID_M_BANK
	movf	CS_MENU_SID, W
	movff	SID_SYSEX_DATA, PLUSW0
	rgoto	SID_SYSEX_WritePar_E_Bank_Cont

SID_SYSEX_WritePar_E_Patch
	;; update variable in CS
	lfsr	FSR0, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	movff	SID_SYSEX_DATA, PLUSW0
	;; 	rgoto	SID_SYSEX_WritePar_E_Bank_Cont

SID_SYSEX_WritePar_E_Bank_Cont
	;; request delayed Tx
	lfsr	FSR0, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	addwf	FSR0L, F
	movlw	0x81
	movwf	INDF0

	rgoto	SID_SYSEX_WritePar_E_End

SID_SYSEX_WritePar_E_End
	;; fix BSR
	SET_BSR	SID_BASE
	rgoto	SID_SYSEX_WritePar_End
	
SID_SYSEX_WritePar_End
	;; return ZERO flag set
	andlw	0x00
	return

SID_SYSEX_WritePar_Fail
	;; return ZERO flag cleared
	iorlw	0xff
	return


;; --------------------------------------------------------------------------
;;  This function sets the patch number if SID_SYSEX_TYPE == 0 and the
;;  ensemble if SID_SYSEX_TYPE == 0x70
;;  IN: type in SID_SYSEX_TYPE
;;      bank in SID_BANK
;;      patch in SID_PATCH
;;  OUT: zero flag cleared if patch/bank not available
;; --------------------------------------------------------------------------
SID_SYSEX_SetPatch
	SET_BSR	SID_BASE

	;; ensure that bank is within 0-7
	movlw	8
	cpfslt	SID_BANK, ACCESS
	rgoto SID_SYSEX_SetPatch_Fail

	;; check if bank available
	movf	SID_BANK, W
	bz	SID_SYSEX_SetPatch_Bank0	; bank #0 always available (EEPROM/BankStick)
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bz	SID_SYSEX_SetPatch_Fail
SID_SYSEX_SetPatch_Bank0

	;; branch depending on type
	movf	SID_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; mask out "RAM" flag
	bz	SID_SYSEX_SetPatch_I
	xorlw	0x70
	bz	SID_SYSEX_SetPatch_E
	rgoto	SID_SYSEX_SetPatch_Fail

SID_SYSEX_SetPatch_I

	movff	SID_PATCH, CS_MENU_SID_M_PATCH
	bsf	CS_MENU_SID_M_PATCH, 7
	movff	SID_BANK, CS_MENU_SID_M_BANK

	rgoto	SID_SYSEX_SetPatch_End

SID_SYSEX_SetPatch_E
	;; only one ensemble bank available
	movf	SID_BANK, W
	bnz	SID_SYSEX_SetPatch_Fail

	;; patch -> ensemble
	movff	SID_PATCH, SID_ENSEMBLE
	return
	;; 	rgoto	SID_SYSEX_SetPatch_End

SID_SYSEX_SetPatch_End
	;; return ZERO flag set
	andlw	0x00
	return

SID_SYSEX_SetPatch_Fail
	;; return ZERO flag cleared
	iorlw	0xff
	return


;; --------------------------------------------------------------------------
;;  This function changes to the selected patch
;;  IN: type in SID_SYSEX_TYPE
;; --------------------------------------------------------------------------
SID_SYSEX_ChangePatch
	SET_BSR	SID_BASE

	;; branch depending on patch type
	movf	SID_SYSEX_TYPE, W, BANKED
	andlw	0xf7		; mask out "RAM" flag
	bz	SID_SYSEX_ChangePatch_I
	xorlw	0x70
	bz	SID_SYSEX_ChangePatch_E
	rgoto	SID_SYSEX_ChangePatch_End

SID_SYSEX_ChangePatch_I
	;; skip if RAM flag is set
	BRA_IFSET SID_SYSEX_TYPE, 3, BANKED, SID_SYSEX_ChangePatch_I_RAM
SID_SYSEX_ChangePatch_I_EE
	;; load patch buffer and re-init patch
	call	CS_MENU_MS_GetSIDNumber
	call	SID_PATCH_LoadPatchBuffer
SID_SYSEX_ChangePatch_I_RAM
	;; request complete CS initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	rgoto	SID_SYSEX_ChangePatch_End

SID_SYSEX_ChangePatch_E
	;; skip if RAM flag is set
	BRA_IFSET SID_SYSEX_TYPE, 3, BANKED, SID_SYSEX_ChangePatch_E_RAM
SID_SYSEX_ChangePatch_E_EE
	;; load ensemble buffer and re-init
	call	SID_ENS_Init
SID_SYSEX_ChangePatch_E_RAM
	rgoto	SID_SYSEX_ChangePatch_End

SID_SYSEX_ChangePatch_End
	return

;; --------------------------------------------------------------------------
;;  This function checks the BankStick size
;;  IN: type in SID_SYSEX_TYPE
;;      bank in SID_BANK
;;      patch in SID_PATCH
;;  OUT: zero flag cleared if bankstick too small
;; --------------------------------------------------------------------------
SID_SYSEX_Check_BS_Size
	SET_BSR	SID_BASE

	movf	SID_SYSEX_TYPE, W, BANKED		; check SysEx type (only patch and non-RAM for interest)
	bnz	SID_SYSEX_Check_BS_Size_Ok
	movf	SID_PATCH, W				; check if patch >= 64
	andlw	0xc0
	bz	SID_SYSEX_Check_BS_Size_Ok
	movf	SID_BANK, W				; check bank size (32k = 0)
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_SIZE, W
	bnz	SID_SYSEX_Check_BS_Size_Ok

SID_SYSEX_Check_BS_Size_Fail
	;; return ZERO flag cleared
	iorlw	0xff
	return

SID_SYSEX_Check_BS_Size_Ok
	;; return ZERO flag set
	andlw	0x00
	return

;; --------------------------------------------------------------------------
;;  This function sends a parameter dump of 512 (patch) or 256 (ensemble) bytes
;;  IN: type in SID_SYSEX_TYPE
;;      bank number in SID_BANK
;;      patch/ensemble number in SID_PATCH
;;  Note: SID_SYSEX_SendDump_Edit sends the EDIT_BUFFER
;; --------------------------------------------------------------------------
SID_SYSEX_SendDump_Edit
	;; send edit buffer (allows to send patches from SID slaves)
 	lfsr	FSR0, SID_EDIT_BUFFER
	rgoto	SID_SYSEX_SendDump_Edit_Cont

SID_SYSEX_SendDump
	;; send local patch buffer
 	lfsr	FSR0, SID_PATCH_BUFFER

SID_SYSEX_SendDump_Edit_Cont

	;; send SysEx header
	rcall	SID_SYSEX_Send_SysExHeader

	;; send PATCH_Write ID
	movlw	0x02
	rcall	SID_SYSEX_TxBufferPut

	;; send requested type number
	movff	SID_SYSEX_TYPE, WREG
	rcall	SID_SYSEX_TxBufferPut

	;; send requested bank number
	movf	SID_BANK, W
	rcall	SID_SYSEX_TxBufferPut

	;; send requested patch/ensemble number
	movf	SID_PATCH, W
	rcall	SID_SYSEX_TxBufferPut

	;; clear checksum
	SET_BSR	SID_BASE
	clrf	SID_SYSEX_CHECKSUM, BANKED

	;; feed WDT
	clrwdt

	;; branch depending on SysEx type
	movf	SID_SYSEX_TYPE, W, BANKED
	andlw	0xf7
	xorlw	0x70
	bz	SID_SYSEX_SendDump_E
	;; 	rgoto	SID_SYSEX_SendDump_I
SID_SYSEX_SendDump_I
	clrf	EEADR			; 2*0x200 bytes to send, use EEADR as counter
	clrf	EEADRH
	;; 	lfsr	FSR0, SID_PATCH_BUFFER
	;; (already prepared above, so that we can differ between EDIT and PATCH buffer)
SID_SYSEX_SendDump_I_Loop
	;; store byte in TABLAT
	movff	POSTINC0, TABLAT

	movlw	0x10		; ensure that patch name doesn't contain characters < 0x20
	cpfslt	EEADR, ACCESS
	rgoto SID_SYSEX_SendDump_I_Loop_NoN
	movf	EEADRH, W
	bnz	SID_SYSEX_SendDump_I_Loop_NoN
SID_SYSEX_SendDump_I_Loop_N
	movlw	0x20
	cpfslt	TABLAT, ACCESS
	rgoto SID_SYSEX_SendDump_I_Loop_NoN
	movwf	TABLAT
SID_SYSEX_SendDump_I_Loop_NoN

	;; send low-nibble
	movf	TABLAT, W
	andlw	0x0f
	addwf	SID_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SID_BASE

	;; send high-nibble
	swapf	TABLAT, W
	andlw	0x0f
	addwf	SID_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SID_BASE

	;; increment offset
	incf	EEADR, F
	skpnz
	incf	EEADRH, F

	;; loop 512 times
	movf	EEADR, W
	bnz	SID_SYSEX_SendDump_I_Loop
	movlw	2-1
	cpfsgt	EEADRH, ACCESS
	rgoto SID_SYSEX_SendDump_I_Loop
	rgoto	SID_SYSEX_SendDump_I_Cont


SID_SYSEX_SendDump_E
	clrf	TMP1			; 2*0x100 bytes to send, use TMP1 as counter
SID_SYSEX_SendDump_E_Loop
	;; whenever 0x40 boundary is reached, load SID specific ensemble into edit buffer
	movf	TMP1, W
	andlw	0x3f
	bnz	SID_SYSEX_SendDump_E_Loop_NoLoad
	;; select SID
	swapf	TMP1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	movwf	CS_MENU_SID

	;; if master: load local setup into edit buffer
	bnz	SID_SYSEX_SendDump_E_Loop_Load_S
SID_SYSEX_SendDump_E_Loop_Load_M
	call	SID_ENS_LocalSetupIntoBuffer
	rgoto	SID_SYSEX_SendDump_E_Loop_Load_C
SID_SYSEX_SendDump_E_Loop_Load_S
	;; try to load from slave
	call	CS_MENU_MBNET_Tx_GetEns
	;; if node not available (anymore): load from EEPROM instead
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MBNET_BASE
	andwf	MBNET_NODE_AVAIL, W, BANKED
	SET_BSR	SID_BASE
	bnz	SID_SYSEX_SendDump_E_Loop_Load_C
	;; load from EEPROM
	movf	CS_MENU_SID, W
	call	SID_ENS_LoadSIDSetup
SID_SYSEX_SendDump_E_Loop_Load_C
SID_SYSEX_SendDump_E_Loop_NoLoad
	;; pointer to edit buffer
	lfsr	FSR0, SID_ENS_BUFFER
	movf	TMP1, W
	andlw	0x3f
	addwf	FSR0L, F

	;; store byte in TABLAT
	movff	POSTINC0, TABLAT

	;; send low-nibble
	movf	TABLAT, W
	andlw	0x0f
	addwf	SID_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SID_BASE

	;; send high-nibble
	swapf	TABLAT, W
	andlw	0x0f
	addwf	SID_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SID_BASE

	;; increment offset
	incf	TMP1, F

	;; loop 256 times
	movf	TMP1, W
	bnz	SID_SYSEX_SendDump_E_Loop

	;; reload edit buffer with data of currently selected SID
	call	CS_MENU_MS_UpdateEnsBuffer

SID_SYSEX_SendDump_I_Cont
	;; send checksum
	movff	SID_SYSEX_CHECKSUM, WREG
	sublw	0x80
	andlw	0x7f
	rcall	SID_SYSEX_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rgoto	SID_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Forward a MIDI byte to the MIOS function (allows the use of "rcall")
;; --------------------------------------------------------------------------
SID_SYSEX_TxBufferPut
	goto	MIOS_MIDI_TxBufferPut

