; $Id$
;
; MIDIbox MF SysEx parser
;
; Supported SysEx commands:
;   a) F0 00 00 7E 45 <d>1 F7
;      Request a Dump
;      OBSOLETE!!! ONLY SUPPORTED FOR PIC16F compatibility!!!
;         <d> = device number (0-7)
;
;   b) F0 00 00 7E 45 <d>2 <dump> F7
;      Write a Dump
;      OBSOLETE!!! ONLY SUPPORTED FOR PIC16F compatibility!!!
;         <d> = device number (0-7)
;         <dump> = 4096 bytes dump data
;
;   c) F0 00 00 7E 45 <d>3 <b> F7
;      Request a block of 256 bytes
;         <d> = device number (0-7)
;         <b> = block number (00-0F)
;
;   d) F0 00 00 7E 45 <d>4 <b> <256 bytes> <checksum> F7
;      Write a block of 256 bytes
;         <d> = device number (0-7)
;         <b> = block number (00-0F)
;         <256 bytes> = the block data
;         <checksum> = the checksum of <b> and <256 bytes>
;
;   e) F0 00 00 7E 45 <d>5 F7
;      Request all 16 blocks
;         <d> = device number (0-7)
;
;   f) F0 00 00 7E 45 <d>6 <p> F7
;      Request a Patch (Snapshot)
;         <d> = device number (0-7)
;         <p> = patch number (0-127)
;
;   g) F0 00 00 7E 45 <d>7 <p> <160 bytes> <checksum> F7
;      Store a Patch (Snapshot)
;         <d> = device number (0-7)
;         <p> = patch number (0-127)
;         <checksum> = the checksum of <160 bytes>
;
;   h) F0 00 00 7E 45 <d>8 <nn> F7
;      Request Bank change
;         <d> = device number (0-7)
;         <nn> = bank number (0-64)
;
;      Sends an acknowledge when Bank has been changed:
;         F0 00 00 7E 45 <d>F F7
;
;      or a disacknowledge if BankStick not connected:
;         F0 00 00 7E 45 <d>E F7
;
;   i) F0 00 00 7E 45 <d>9 00 <fader> <value> F7
;      Remote Control of a fader
;         <d>     = device number (0-7)
;         <fader> = fader number (0-7)
;         <value> = fader value (0-127)

;   j) F0 00 00 7E 45 <d>9 01 <button> <value> F7
;      Remote Control of a button
;         <d>      = device number (0-7)
;         <button> = button number (0-63)
;         <value>  = button value (0=pressed, 1=depressed)

;   k) F0 00 00 7E 45 <d>9 02 <sfbn> <sfbp> <value> F7
;      Remote Control of a special button
;         <d>     = device number (0-7)
;         <sfbn>  = special button number (0-127)
;         <sfbp>  = special button parameter (0-127)
;         <value> = button value (0=pressed, 1=depressed)

;   l) F0 00 00 7E 45 <d>F F7
;      Ping (just sends back the same SysEx string)
;         <d>     = device number (0-7)
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MBMF_SYSEX_STATE_MYSYSEX	EQU	7
MBMF_SYSEX_STATE_ACTION	EQU	6
MBMF_SYSEX_STATE_BLOCK_RECEIVED EQU	5
MBMF_SYSEX_STATE_DATA_RECEIVED	EQU	4
MBMF_SYSEX_STATE_CS_RECEIVED	EQU	3

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc on a timeout
;;  it will used internally when an internal sequence has been parsed or 
;;  at the end of a sysex sequence
;; --------------------------------------------------------------------------
MBMF_SYSEX_ActionFinished
MBMF_SYSEX_ActionInvalid
	SET_BSR	MBMF_BASE
	clrf	MBMF_SYSEX_STATE, BANKED
	clrf	MBMF_SYSEX_ACTION, BANKED

	goto	MIOS_MPROC_MergerEnable

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MBMF_SYSEX_Parser
	;; store received byte in MBMF_SYSEX_IN
	SET_BSR	MBMF_BASE
	movwf	MBMF_SYSEX_IN, BANKED

	;; check sysex state
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_MYSYSEX, BANKED, MBMF_SYSEX_Handler

	;; parse incoming bytes
	movf	MBMF_SYSEX_STATE, W, BANKED
	rcall	MBMF_SYSEX_SysExHeaderGet
	cpfseq	MBMF_SYSEX_IN, BANKED
	rgoto MBMF_SYSEX_ActionInvalid
	incf	MBMF_SYSEX_STATE, F, BANKED
	movf	MBMF_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x05	; wait for 5 bytes (f0 00 00 7E 45)
	bnz	MBMF_SYSEX_Parser_End

	;; SysEx ID received, lets go
	movlw	(1 << MBMF_SYSEX_STATE_MYSYSEX)
	movwf	MBMF_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable

MBMF_SYSEX_Parser_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
MBMF_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x45		; MIDIbox MF ID (45 - the ultimative number + 3)
	retlw	0xff
	retlw	0xff
	retlw	0xff

;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
MBMF_SYSEX_Handler
	SET_BSR	MBMF_BASE
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_ACTION, BANKED, MBMF_SYSEX_Handler_DoAction

	;; 6th byte: check device ID, embedded in bit 6-4 of the command byte
	swapf	MBMF_SYSEX_IN, W, BANKED
	andlw	0x07
	xorwf	MBMF_DEVICE_ID, W, BANKED
	bz	MBMF_SYSEX_Handler_ID_Ok
MBMF_SYSEX_Handler_ID_NotOk
	;; if MIDI merger enabled: send 5th byte and command
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MBMF_SYSEX_ActionInvalid
	movlw	0x04
	call	MBMF_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	movff	MBMF_SYSEX_IN, WREG
	call	MIOS_MIDI_TxBufferPut
	rgoto	MBMF_SYSEX_ActionInvalid
	
MBMF_SYSEX_Handler_ID_Ok
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_ACTION, BANKED

	movf	MBMF_SYSEX_IN, W, BANKED		; store action ID
	andlw	0x0f
	movwf	MBMF_SYSEX_ACTION	
	rgoto	MBMF_SYSEX_Handler_InitAction		; initialize the action and exit

	;; ---

MBMF_SYSEX_Handler_DoAction
	SET_BSR	MBMF_BASE
	;; branch to end if status byte (i.e. F7)
	BRA_IFSET MBMF_SYSEX_IN, 7, BANKED, MBMF_SYSEX_Handler_EndAction

	;; branch depending on current action ID
	movf	MBMF_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_Action_DumpRead
	rgoto	MBMF_SYSEX_Action_DumpWrite
	rgoto	MBMF_SYSEX_Action_BlockRead
	rgoto	MBMF_SYSEX_Action_BlockWrite
	rgoto	MBMF_SYSEX_Action_AllRead
	rgoto	MBMF_SYSEX_Action_PatchRead
	rgoto	MBMF_SYSEX_Action_PatchWrite
	rgoto	MBMF_SYSEX_Action_BankChange
	rgoto	MBMF_SYSEX_Action_Remote
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_Action_Ping


;; --------------------------------------------------------------------------
;;  Initialize an action depending on MBMF_SYSEX_ACTION
;; --------------------------------------------------------------------------
MBMF_SYSEX_Handler_InitAction
	SET_BSR	MBMF_BASE
	movf	MBMF_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_Init_DumpRead
	rgoto	MBMF_SYSEX_Init_DumpWrite
	rgoto	MBMF_SYSEX_Init_BlockRead
	rgoto	MBMF_SYSEX_Init_BlockWrite
	rgoto	MBMF_SYSEX_Init_AllRead
	rgoto	MBMF_SYSEX_Init_PatchRead
	rgoto	MBMF_SYSEX_Init_PatchWrite
	rgoto	MBMF_SYSEX_Init_BankChange
	rgoto	MBMF_SYSEX_Init_Remote
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_Init_Ping

	
;; --------------------------------------------------------------------------
;;  Finish an action depending on MBMF_SYSEX_ACTION
;; --------------------------------------------------------------------------
MBMF_SYSEX_Handler_EndAction
	SET_BSR	MBMF_BASE
	;; if sysex footer (F7) has not been received here, action is invalid!
	movlw	0xf7
	cpfseq	MBMF_SYSEX_IN, BANKED
	rgoto MBMF_SYSEX_ActionInvalid

	;; else finish action
	movf	MBMF_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_End_DumpRead
	rgoto	MBMF_SYSEX_End_DumpWrite
	rgoto	MBMF_SYSEX_End_BlockRead
	rgoto	MBMF_SYSEX_End_BlockWrite
	rgoto	MBMF_SYSEX_End_AllRead
	rgoto	MBMF_SYSEX_End_PatchRead
	rgoto	MBMF_SYSEX_End_PatchWrite
	rgoto	MBMF_SYSEX_End_BankChange
	rgoto	MBMF_SYSEX_End_Remote
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_ActionInvalid
	rgoto	MBMF_SYSEX_End_Ping

	
;; --------------------------------------------------------------------------
;;  MIDI Action: Dump Read
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_DumpRead
	return

MBMF_SYSEX_Action_DumpRead
	;; do nothing until sysex footer (F7) has been received
	return

MBMF_SYSEX_End_DumpRead
	;; send dump
	rcall	MBMF_SYSEX_SendDump

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Dump Write
;; --------------------------------------------------------------------------
MBMF_SYSEX_STR_R_0	STRING 16, 0x00, "Receiving SysEx "
MBMF_SYSEX_STR_R_1	STRING 16, 0x40, "   Dump $000    "
MBMF_SYSEX_STR_RI_0	STRING 16, 0x00, " Received SysEx "
MBMF_SYSEX_STR_RI_1	STRING 16, 0x40, " Dump INVALID!! "
MBMF_SYSEX_STR_RV_0	STRING 16, 0x00, " Received SysEx "
MBMF_SYSEX_STR_RV_1	STRING 16, 0x40, " Dump complete! "

MBMF_SYSEX_Init_DumpWrite
	;; print message
	TABLE_ADDR MBMF_SYSEX_STR_R_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; clear address
	clrf	MB_ADDRL
	clrf	MB_ADDRH
	return

MBMF_SYSEX_Action_DumpWrite
	;; write immediately into EEPROM (since it doesn't support page writes)
	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MBMF_SYSEX_Action_DumpWrite_NEE
	movf	MB_ADDRH, W
	xorlw	0x06
	bnz	MBMF_SYSEX_Action_DumpWrite_NEE
	movff	MBMF_SYSEX_IN, WREG
	call	MBMF_BANK_Write
	rgoto	MBMF_SYSEX_Action_DumpWriteCont

MBMF_SYSEX_Action_DumpWrite_NEE
	;; transfer data into buffer
	lfsr	FSR1, MBMF_DATA_BUFFER
	movf	MB_ADDRL, W
	andlw	0x3f
	movff	MBMF_SYSEX_IN, PLUSW1
	
	;; if buffer of 0x40 bytes is full, store data
	incf	MB_ADDRL, W
	andlw	0x3f
	bz	MBMF_SYSEX_Action_DumpWriteStre

	;; don't store yet, just increment address...
	incf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	rgoto	MBMF_SYSEX_Action_DumpWriteCont

MBMF_SYSEX_Action_DumpWriteStre
	;; align address to 0x40
	movlw	0xc0
	andwf	MB_ADDRL, F
	;; call write function (pointer to transfer buffer already in FSR1)
	call	MBMF_BANK_WritePage
	;; address has been incremented by the WritePage routine
	;; 	rgoto	MBMF_SYSEX_Action_DumpWriteCont
MBMF_SYSEX_Action_DumpWriteCont	
	;; print address
	movlw	0x49
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRH, W
	call	MIOS_LCD_PrintHex1
	movf	MB_ADDRL, W
	call	MIOS_LCD_PrintHex2

	;; re-trigger message and exit
	movlw	0xff
	goto	MIOS_LCD_MessageStart


MBMF_SYSEX_End_DumpWrite
	;; check end address, print invalid message if not equal to 0x1000
	movf	MB_ADDRL, W
	bnz	MBMF_SYSEX_End_DumpWriteInvalid
	movf	MB_ADDRH, W
	xorlw	0x10
	bnz	MBMF_SYSEX_End_DumpWriteInvalid
MBMF_SYSEX_End_DumpWriteValid
	TABLE_ADDR MBMF_SYSEX_STR_RV_0
	rgoto	MBMF_SYSEX_End_DumpWriteCont

MBMF_SYSEX_End_DumpWriteInvalid
	TABLE_ADDR MBMF_SYSEX_STR_RI_0
	;; 	rgoto	MBMF_SYSEX_End_DumpWriteCont

MBMF_SYSEX_End_DumpWriteCont
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; transfer internal settings from BankStick into internal EEPROM
	call	MBMF_DUMP_TransferInternals
	;; restore the whole dump
	call	MBMF_DUMP_RestoreAll
	;; send acknowlegde
	call	MBMF_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Block Read
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_BlockRead
	;; clear block address
	clrf	MB_ADDRH
	return

MBMF_SYSEX_Action_BlockRead
	;; copy received byte to high address until F7 has been received
	movff	MBMF_SYSEX_IN, MB_ADDRH
	return

MBMF_SYSEX_End_BlockRead
	;; check if MB_ADDRH < 0x10
	movlw	0x10-1
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MBMF_SYSEX_End_BlockRead_Ok
MBMF_SYSEX_End_BlockRead_Fail
	;; send disacknowledge
	rcall	MBMF_SYSEX_Send_DisAcknowledge
	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished

MBMF_SYSEX_End_BlockRead_Ok
	;; send block
	rcall	MBMF_SYSEX_SendBlock
	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Block Write
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_BlockWrite
	;; re-use the Dump Write init routine
	rcall	MBMF_SYSEX_Init_DumpWrite
	;; clear checksum
	SET_BSR	MBMF_BASE
	clrf	MBMF_SYSEX_CHECKSUM, BANKED
	return

MBMF_SYSEX_Action_BlockWrite
	;; determine state
	SET_BSR	MBMF_BASE
	;; block and data received -> this byte is the checksum
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_DATA_RECEIVED, BANKED, MBMF_SYSEX_Action_BlockWrite_C
	;; else: add received byte to checksum
	movf	MBMF_SYSEX_IN, W, BANKED
	addwf	MBMF_SYSEX_CHECKSUM, F, BANKED

	;; first byte? then we received the block address - else we are receiving the block data	
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_BLOCK_RECEIVED, BANKED, MBMF_SYSEX_Action_BlockWrite_D
MBMF_SYSEX_Action_BlockWrite_A
	;; upload failed if block address >= 0x10
	movlw	0x10
	cpfslt	MBMF_SYSEX_IN, BANKED
	rgoto MBMF_SYSEX_Action_BlockWrite_F
	;; set block address
	movff	MBMF_SYSEX_IN, MB_ADDRH
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_BLOCK_RECEIVED, BANKED
	;; print high address
	movlw	0x49
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRH, W
	goto	MIOS_LCD_PrintHex1

MBMF_SYSEX_Action_BlockWrite_D
	;; store data in upload buffer
	lfsr	FSR0, MBMF_UPLOAD_BUFFER
	movf	MB_ADDRL, W
	addwf	FSR0L, F
	movff	MBMF_SYSEX_IN, INDF0
#if 0
	;; print low address
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRL, W
	call	MIOS_LCD_PrintHex2
#endif
	;; increment low byte of address, wait for checksum once 256 bytes have been received
	SET_BSR	MBMF_BASE
	incf	MB_ADDRL, F
	skpnz
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_DATA_RECEIVED, BANKED
	return

MBMF_SYSEX_Action_BlockWrite_C
	;; upload failed if checksum has already been received
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Action_BlockWrite_F
	;; compare received byte with checksum
	movf	MBMF_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	xorwf	MBMF_SYSEX_IN, W, BANKED
	;; failed upload if not equal
	bnz	MBMF_SYSEX_Action_BlockWrite_F
	;; else set flag which notifies that the whole block + checksum has been received
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED
	return

MBMF_SYSEX_End_BlockWrite
	;; upload failed if checksum has not been received
	BRA_IFCLR MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Action_BlockWrite_F
	;; restore data structure (which has been overwritten by the UPLOAD_BUFFER)
	rgoto	MBMF_SYSEX_Action_BlockWrite_R

MBMF_SYSEX_Action_BlockWrite_F
	;; disable CS flag (so that no acknowledge will be sent)
	SET_BSR	MBMF_BASE
	bcf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED
	;; print "dump invalid" message
	TABLE_ADDR MBMF_SYSEX_STR_RI_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

MBMF_SYSEX_Action_BlockWrite_R
	;; write 4 pages
	;; MB_ADDRH still set to block address)
	clrf	MB_ADDRL
	lfsr	FSR1, MBMF_UPLOAD_BUFFER
MBMF_SYSEX_Action_BlockWrite_L
	call	MBMF_BANK_WritePage
	;; increment FSR1 pointer by 0x40
	movlw	0x40
	addwf	FSR1L, F
	;; the write page function increments MB_ADDRL by 0x40
	;; repeat until MB_ADDRL reaches 0x00 again
	movf	MB_ADDRL, W
	bnz	MBMF_SYSEX_Action_BlockWrite_L

	;; workaround: if the merger mode changes here, the acknowledge message will be
	;; corrupted. This can be avoided by saving the merger, setting it to the old value
	;; after RestoreAll, and setting it to the new value after the acknowledge message
	;; merger state will be saved in MBMF_SYSEX_IN
	call	MIOS_MIDI_MergerGet
	movff	WREG, MBMF_SYSEX_IN

	;; transfer internal settings from BankStick into internal EEPROM
	call	MBMF_DUMP_TransferInternals
	;; restore the whole dump
	call	MBMF_DUMP_RestoreAll

	;; save new merger mode, switch to old merger mode
	call	MIOS_MIDI_MergerGet
	movff	MBMF_SYSEX_IN, WREG
	movff	MIOS_PARAMETER1, MBMF_SYSEX_IN
	call	MIOS_MIDI_MergerSet

	;; send acknowledge/disacknowledge depending on CS flag
	SET_BSR	MBMF_BASE
	RCALL_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Send_Acknowledge
	SET_BSR	MBMF_BASE
	RCALL_IFCLR MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Send_DisAcknowledge

	;; finally restore new merger mode
	movff	MBMF_SYSEX_IN, WREG
	call	MIOS_MIDI_MergerSet

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Read all blocks
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_AllRead
	return

MBMF_SYSEX_Action_AllRead
	;; do nothing until sysex footer (F7) has been received
	return

MBMF_SYSEX_End_AllRead
	;; send dump
	rcall	MBMF_SYSEX_SendAll

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Read
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_PatchRead
	return

MBMF_SYSEX_Action_PatchRead
	;; copy received byte to MB_PATCH until F7 has been received
	movff	MBMF_SYSEX_IN, MB_PATCH
	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

MBMF_SYSEX_End_PatchRead
	;; send patch
	rcall	MBMF_SYSEX_SendPatch
	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Write
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_PatchWrite
	return

MBMF_SYSEX_Action_PatchWrite
	;; determine state
	SET_BSR	MBMF_BASE
	;; patch and data received -> this byte is the checksum
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_DATA_RECEIVED, BANKED, MBMF_SYSEX_Action_PatchWrite_C
	;; else: add received byte to checksum
	movf	MBMF_SYSEX_IN, W, BANKED
	addwf	MBMF_SYSEX_CHECKSUM, F, BANKED

	;; first byte? then we received the patch number - else we are receiving the patch data	
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_BLOCK_RECEIVED, BANKED, MBMF_SYSEX_Action_PatchWrite_D
MBMF_SYSEX_Action_PatchWrite_A
	;; set patch number and clear low-byte of address
	movff	MBMF_SYSEX_IN, MB_PATCH
	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; clear low-byte of address
	clrf	MB_ADDRL
	;; clear checksum
	SET_BSR	MBMF_BASE
	clrf	MBMF_SYSEX_CHECKSUM, BANKED
	;; change state
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_BLOCK_RECEIVED, BANKED
	return

MBMF_SYSEX_Action_PatchWrite_D
	;; store data in patch memory
	movf	MB_ADDRL, W
	BRA_IFSET MB_ADDRL, 7, ACCESS, MBMF_SYSEX_Action_PatchWriteDBN
MBMF_SYSEX_Action_PatchWriteDK		; knob value
	lfsr	FSR0, MBMF_FADER_VALUES_00
	andlw	0x7f			; just to ensure that we never overwrite other data
	movff	MBMF_SYSEX_IN, PLUSW0
	rgoto	MBMF_SYSEX_Action_PatchWrite_DC

MBMF_SYSEX_Action_PatchWriteDBN	; button/name value
	BRA_IFSET MB_ADDRL, 4, ACCESS, MBMF_SYSEX_Action_PatchWriteDN
MBMF_SYSEX_Action_PatchWriteDB		; button value
	lfsr	FSR0, MBMF_BUTTON_VALUES_SR0
	rrf	MB_ADDRL, W
	andlw	0x07			; just to ensure that we never overwrite other data
	addwf	FSR0L, F
	BRA_IFCLR MB_ADDRL, 0, ACCESS, MBMF_SYSEX_Action_PatchWriteDBL
MBMF_SYSEX_Action_PatchWriteDBH	; write high byte
	;; (it's a read-modify-write operation)
	SET_BSR	MBMF_BASE
	swapf	MBMF_SYSEX_IN, W
	andlw	0xf0
	iorwf	INDF0, F
	rgoto	MBMF_SYSEX_Action_PatchWrite_DC
MBMF_SYSEX_Action_PatchWriteDBL	; write low byte
	movff	MBMF_SYSEX_IN, INDF0
 	rgoto	MBMF_SYSEX_Action_PatchWrite_DC

MBMF_SYSEX_Action_PatchWriteDN		; name value
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
	andlw	0x0f			; just to ensure that we never overwrite other data
	movff	MBMF_SYSEX_IN, PLUSW0
	;; 	rgoto	MBMF_SYSEX_Action_PatchWrite_DC

MBMF_SYSEX_Action_PatchWrite_DC
	;; increment low byte of address, wait for checksum once 160 bytes have been received
	SET_BSR	MBMF_BASE
	incf	MB_ADDRL, F
	movlw	160
	cpfsgt	MB_ADDRL, ACCESS
	return
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_DATA_RECEIVED, BANKED
	return

MBMF_SYSEX_Action_PatchWrite_C
	;; upload failed if checksum has already been received
	BRA_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Action_PatchWrite_F
	;; compare received byte with checksum
	movf	MBMF_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	xorwf	MBMF_SYSEX_IN, W, BANKED
	;; failed upload if not equal
	bnz	MBMF_SYSEX_Action_PatchWrite_F
	;; else set flag which notifies that the whole block + checksum has been received
	bsf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED
	return

MBMF_SYSEX_End_PatchWrite
	;; upload failed if checksum has not been received
	BRA_IFCLR MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Action_PatchWrite_F
	;; write patch to memory
	call	MBMF_DUMP_StorePatch
	rgoto	MBMF_SYSEX_Action_PatchWrite_R

MBMF_SYSEX_Action_PatchWrite_F
	;; disable CS flag (so that no acknowledge will be sent)
	SET_BSR	MBMF_BASE
	bcf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED

MBMF_SYSEX_Action_PatchWrite_R

	;; send acknowledge/disacknowledge depending on CS flag
	SET_BSR	MBMF_BASE
	RCALL_IFSET MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Send_Acknowledge
	SET_BSR	MBMF_BASE
	RCALL_IFCLR MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_CS_RECEIVED, BANKED, MBMF_SYSEX_Send_DisAcknowledge

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Bank Change
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_BankChange
	return

MBMF_SYSEX_Action_BankChange
	;; memorize incoming byte in buffer (the last byte will be taken)
	movff	MBMF_SYSEX_IN, MBMF_DATA_BUFFER
	return

MBMF_SYSEX_End_BankChange
	;; send acknowledge if successfull
	RCALL_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MBMF_SYSEX_Send_Acknowledge
	;; else send disacknowledge
	RCALL_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MBMF_SYSEX_Send_DisAcknowledge

	;; change bank
	;; TK: although the acknowledge message would be more interesting after
	;; the bank has been changed (since an uninitialized bank could be formatted,
	;; which wouldn't take longer than the common switching procedure), we are
	;; doing it thereafter, so that the complete SysEx message is sent out, before
	;; new MIDI events (Auto-Snapshot, Program Change) will be sent by BANK_Change
	;; this avoids a MIDI protocol violation
	movff	MBMF_DATA_BUFFER, WREG
	call	MBMF_BANK_Change

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Remote Control
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_Remote
	;; clear address to buffer
	clrf	MB_ADDRL
	;; clear the three used buffer bytes
	SET_BSR	MBMF_BASE
	clrf	MBMF_DATA_BUFFER+0, BANKED
	clrf	MBMF_DATA_BUFFER+1, BANKED
	clrf	MBMF_DATA_BUFFER+2, BANKED
	clrf	MBMF_DATA_BUFFER+3, BANKED
	return

MBMF_SYSEX_Action_Remote
	;; memorize incoming byte in buffer
	lfsr	FSR0, MBMF_DATA_BUFFER
	movf	MB_ADDRL, W
	movff	MBMF_SYSEX_IN, PLUSW0
	;; increment address
	incf	MB_ADDRL, W	; wrap at 0x40
	andlw	0x3f
	movwf	MB_ADDRL
	return

MBMF_SYSEX_End_Remote
	;; send F7 if merger enabled to ensure a proper MIDI protocol
	movlw	0x00
	rcall	MBMF_SYSEX_Send_SysExFooter

	;; execute remote action
	movff	MBMF_DATA_BUFFER, WREG
	JUMPTABLE_2BYTES 3	; 3 entries
	rgoto	MBMF_SYSEX_Remote_Fader
	rgoto	MBMF_SYSEX_Remote_Button
	rgoto	MBMF_SYSEX_Remote_SFB

	;; remote control a fader event
MBMF_SYSEX_Remote_Fader
	;; copy 7-bit value to MIOS_PARAMETER2
	movff	MBMF_DATA_BUFFER+2, MIOS_PARAMETER2
	;; copy fader number to MIOS_PARAMETER1 and WREG
	SET_BSR	MBMF_BASE
	movf	MBMF_DATA_BUFFER+1, W
	movwf	MIOS_PARAMETER1
	;; call fader handler
	call	MBMF_FADER_Handler
	rgoto	MBMF_SYSEX_Remote_End
	

	;; remote control a button event
MBMF_SYSEX_Remote_Button
	;; copy 7-bit value to MIOS_PARAMETER2
	movff	MBMF_DATA_BUFFER+2, MIOS_PARAMETER2
	;; copy button number to MIOS_PARAMETER1 and WREG
	SET_BSR	MBMF_BASE
	movf	MBMF_DATA_BUFFER+1, W
	movwf	MIOS_PARAMETER1
	;; call button handler
	call	MBMF_BUTTON_Handler
	rgoto	MBMF_SYSEX_Remote_End
	
	;; remote control a special button event
MBMF_SYSEX_Remote_SFB
	;; copy SFB number, parameter and value to MIDI_EVNT*
	movff	MBMF_DATA_BUFFER+1, MIDI_EVNT0
	movff	MBMF_DATA_BUFFER+2, MIDI_EVNT1
	movff	MBMF_DATA_BUFFER+3, MIDI_EVNT_VALUE
	;; call SFB handler
	call	MBMF_SFB_Handler
	rgoto	MBMF_SYSEX_Remote_End

MBMF_SYSEX_Remote_End
	;; exit
	rgoto	MBMF_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
MBMF_SYSEX_Init_Ping
	return

MBMF_SYSEX_Action_Ping
	;; do nothing until sysex footer (F7) has been received
	return

MBMF_SYSEX_End_Ping
	;; send Acknowledge
	rcall	MBMF_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MBMF_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MBMF_SYSEX_Send_DisAcknowledge
	rcall	MBMF_SYSEX_Send_SysExHeader

	SET_BSR	MBMF_BASE
	swapf	MBMF_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x0e		; (disacknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send SysEx footer
	movlw	0x01		; (independent from merger state)
	rgoto	MBMF_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MBMF_SYSEX_Send_Acknowledge
	rcall	MBMF_SYSEX_Send_SysExHeader

	SET_BSR	MBMF_BASE
	swapf	MBMF_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send SysEx footer
	movlw	0x01		; (independent from merger state)
	rgoto	MBMF_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send MBMF SysEx Header (Util function)
;; --------------------------------------------------------------------------
MBMF_SYSEX_Send_SysExHeader
	SET_BSR	MBMF_BASE
	;; if MBMF_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_MYSYSEX, BANKED, MBMF_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MBMF_SYSEX_Send_SysExHeader_Skp
	movlw	0x04		; send only command
	movwf	TMP1
	rgoto	MBMF_SYSEX_Send_SysExHeaderLoop
MBMF_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
MBMF_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	MBMF_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x05
	cpfseq	TMP1, ACCESS
	rgoto MBMF_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
MBMF_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, MBMF_SYSEX_Send_SysExFooterNow
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

MBMF_SYSEX_Send_SysExFooterNow
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Send a SysEx dump of MIDIbox MF
;; --------------------------------------------------------------------------
MBMF_SYSEX_SendDump
	;; send header
	rcall	MBMF_SYSEX_Send_SysExHeader

	;; indicate a write action
	SET_BSR	MBMF_BASE
	swapf	MBMF_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send 0x1000 bytes
	clrf	MB_ADDRL
	clrf	MB_ADDRH
MBMF_SYSEX_SendDump_Loop
	clrwdt		; feed watchdog
	call	MBMF_BANK_Read
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	0x10-1
	cpfsgt	MB_ADDRH, BANKED
	rgoto MBMF_SYSEX_SendDump_Loop

	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MBMF_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Send a SysEx block of MIDIbox MF
;; --------------------------------------------------------------------------
MBMF_SYSEX_SendBlock
	;; send header
	rcall	MBMF_SYSEX_Send_SysExHeader

	;; clear checksum
	SET_BSR	MBMF_BASE
	clrf	MBMF_SYSEX_CHECKSUM, BANKED

	;; indicate a block write action
	swapf	MBMF_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x04
	call	MIOS_MIDI_TxBufferPut

	;; add high address (the block number) to checksum and send it
	SET_BSR	MBMF_BASE
	movf	MB_ADDRH, W
	addwf	MBMF_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	
	;; send 256 bytes
	clrf	MB_ADDRL
MBMF_SYSEX_SendBlock_Loop
	clrwdt		; feed watchdog
	call	MBMF_BANK_Read
	SET_BSR	MBMF_BASE
	addwf	MBMF_SYSEX_CHECKSUM, F, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MB_ADDRL, W
	bnz	MBMF_SYSEX_SendBlock_Loop

	;; send checksum
	SET_BSR	MBMF_BASE
	movf	MBMF_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MBMF_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Sends all SysEx blocks of MIDIbox MF
;; --------------------------------------------------------------------------
MBMF_SYSEX_SendAll
	;; 16 blocks, using MB_ADDRH as block counter
	clrf	MB_ADDRH
MBMF_SYSEX_SendAllLoop
	;; send block
	rcall	MBMF_SYSEX_SendBlock
	movlw	0x10
	cpfslt	MB_ADDRH, ACCESS
	rgoto MBMF_SYSEX_SendAll_End

	;; workaround for merger issue: clear MYSYSEX flag, so that the
	;; next header will be sent regardless of merger state
	SET_BSR	MBMF_BASE
	bcf	MBMF_SYSEX_STATE, MBMF_SYSEX_STATE_MYSYSEX, BANKED

	;; insert delay of 750 mS
	movlw	0x03
	movwf	TMP1
MBMF_SYSEX_SendAllDlyLoop
	movlw	250
	call	MIOS_Delay
	decfsz	TMP1, F
	rgoto	MBMF_SYSEX_SendAllDlyLoop
	rgoto	MBMF_SYSEX_SendAllLoop

MBMF_SYSEX_SendAll_End
	return

;; --------------------------------------------------------------------------
;;  Send a Patch of MIDIbox MF
;; --------------------------------------------------------------------------
MBMF_SYSEX_SendPatch
	;; send header
	rcall	MBMF_SYSEX_Send_SysExHeader

	;; indicate a patch store action
	SET_BSR	MBMF_BASE
	swapf	MBMF_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x07
	call	MIOS_MIDI_TxBufferPut

	;; add patch number
	SET_BSR	MBMF_BASE
	movf	MB_PATCH, W
	call	MIOS_MIDI_TxBufferPut

	;; clear checksum
	SET_BSR	MBMF_BASE
	clrf	MBMF_SYSEX_CHECKSUM, BANKED
	
	;; send 160 bytes
	clrf	MB_ADDRL
MBMF_SYSEX_SendPatch_Loop
	clrwdt		; feed watchdog
	call	MBMF_BANK_PatchRead
	SET_BSR	MBMF_BASE
	addwf	MBMF_SYSEX_CHECKSUM, F, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	160
	cpfsgt	MB_ADDRL, ACCESS
	rgoto MBMF_SYSEX_SendPatch_Loop

	;; send checksum
	SET_BSR	MBMF_BASE
	movf	MBMF_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MBMF_SYSEX_Send_SysExFooter


