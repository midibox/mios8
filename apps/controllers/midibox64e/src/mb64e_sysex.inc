; $Id$
;
; MIDIbox64E SysEx parser
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
;   i) F0 00 00 7E 45 <d>9 00 <enc> <value> F7
;      Remote Control of a enc
;         <d>     = device number (0-7)
;         <enc>   = enc number (0-63)
;         <value> = enc value (0-127)

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

MB64E_SYSEX_STATE_MYSYSEX	EQU	7
MB64E_SYSEX_STATE_ACTION	EQU	6
MB64E_SYSEX_STATE_BLOCK_RECEIVED EQU	5
MB64E_SYSEX_STATE_DATA_RECEIVED	EQU	4
MB64E_SYSEX_STATE_CS_RECEIVED	EQU	3

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc on a timeout
;;  it will used internally when an internal sequence has been parsed or 
;;  at the end of a sysex sequence
;; --------------------------------------------------------------------------
MB64E_SYSEX_ActionFinished
MB64E_SYSEX_ActionInvalid
	SET_BSR	MB64E_BASE
	clrf	MB64E_SYSEX_STATE, BANKED
	clrf	MB64E_SYSEX_ACTION, BANKED

	goto	MIOS_MPROC_MergerEnable

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MB64E_SYSEX_Parser
	;; store received byte in MB64E_SYSEX_IN
	SET_BSR	MB64E_BASE
	movwf	MB64E_SYSEX_IN, BANKED

	;; check sysex state
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_MYSYSEX, BANKED, MB64E_SYSEX_Handler

	;; parse incoming bytes
	movf	MB64E_SYSEX_STATE, W, BANKED
	rcall	MB64E_SYSEX_SysExHeaderGet
	cpfseq	MB64E_SYSEX_IN, BANKED
	rgoto MB64E_SYSEX_ActionInvalid
	incf	MB64E_SYSEX_STATE, F, BANKED
	movf	MB64E_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x05	; wait for 5 bytes (f0 00 00 7E 45)
	bnz	MB64E_SYSEX_Parser_End

	;; SysEx ID received, lets go
	movlw	(1 << MB64E_SYSEX_STATE_MYSYSEX)
	movwf	MB64E_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable

MB64E_SYSEX_Parser_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
MB64E_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x45		; MIDIbox64E ID (45 - the ultimative number + 3)
	retlw	0xff
	retlw	0xff
	retlw	0xff

;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
MB64E_SYSEX_Handler
	SET_BSR	MB64E_BASE
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_ACTION, BANKED, MB64E_SYSEX_Handler_DoAction

	;; 6th byte: check device ID, embedded in bit 6-4 of the command byte
	swapf	MB64E_SYSEX_IN, W, BANKED
	andlw	0x07
	xorwf	MB64E_DEVICE_ID, W, BANKED
	bz	MB64E_SYSEX_Handler_ID_Ok
MB64E_SYSEX_Handler_ID_NotOk
	;; if MIDI merger enabled: send 5th byte and command
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MB64E_SYSEX_ActionInvalid
	movlw	0x04
	call	MB64E_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	movff	MB64E_SYSEX_IN, WREG
	call	MIOS_MIDI_TxBufferPut
	rgoto	MB64E_SYSEX_ActionInvalid
	
MB64E_SYSEX_Handler_ID_Ok
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_ACTION, BANKED

	movf	MB64E_SYSEX_IN, W, BANKED		; store action ID
	andlw	0x0f
	movwf	MB64E_SYSEX_ACTION	
	rgoto	MB64E_SYSEX_Handler_InitAction		; initialize the action and exit

	;; ---

MB64E_SYSEX_Handler_DoAction
	SET_BSR	MB64E_BASE
	;; branch to end if status byte (i.e. F7)
	BRA_IFSET MB64E_SYSEX_IN, 7, BANKED, MB64E_SYSEX_Handler_EndAction

	;; branch depending on current action ID
	movf	MB64E_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_Action_DumpRead
	rgoto	MB64E_SYSEX_Action_DumpWrite
	rgoto	MB64E_SYSEX_Action_BlockRead
	rgoto	MB64E_SYSEX_Action_BlockWrite
	rgoto	MB64E_SYSEX_Action_AllRead
	rgoto	MB64E_SYSEX_Action_PatchRead
	rgoto	MB64E_SYSEX_Action_PatchWrite
	rgoto	MB64E_SYSEX_Action_BankChange
	rgoto	MB64E_SYSEX_Action_Remote
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_Action_Ping


;; --------------------------------------------------------------------------
;;  Initialize an action depending on MB64E_SYSEX_ACTION
;; --------------------------------------------------------------------------
MB64E_SYSEX_Handler_InitAction
	SET_BSR	MB64E_BASE
	movf	MB64E_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_Init_DumpRead
	rgoto	MB64E_SYSEX_Init_DumpWrite
	rgoto	MB64E_SYSEX_Init_BlockRead
	rgoto	MB64E_SYSEX_Init_BlockWrite
	rgoto	MB64E_SYSEX_Init_AllRead
	rgoto	MB64E_SYSEX_Init_PatchRead
	rgoto	MB64E_SYSEX_Init_PatchWrite
	rgoto	MB64E_SYSEX_Init_BankChange
	rgoto	MB64E_SYSEX_Init_Remote
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_Init_Ping

	
;; --------------------------------------------------------------------------
;;  Finish an action depending on MB64E_SYSEX_ACTION
;; --------------------------------------------------------------------------
MB64E_SYSEX_Handler_EndAction
	SET_BSR	MB64E_BASE
	;; if sysex footer (F7) has not been received here, action is invalid!
	movlw	0xf7
	cpfseq	MB64E_SYSEX_IN, BANKED
	rgoto MB64E_SYSEX_ActionInvalid

	;; else finish action
	movf	MB64E_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_End_DumpRead
	rgoto	MB64E_SYSEX_End_DumpWrite
	rgoto	MB64E_SYSEX_End_BlockRead
	rgoto	MB64E_SYSEX_End_BlockWrite
	rgoto	MB64E_SYSEX_End_AllRead
	rgoto	MB64E_SYSEX_End_PatchRead
	rgoto	MB64E_SYSEX_End_PatchWrite
	rgoto	MB64E_SYSEX_End_BankChange
	rgoto	MB64E_SYSEX_End_Remote
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_ActionInvalid
	rgoto	MB64E_SYSEX_End_Ping

	
;; --------------------------------------------------------------------------
;;  MIDI Action: Dump Read
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_DumpRead
	return

MB64E_SYSEX_Action_DumpRead
	;; do nothing until sysex footer (F7) has been received
	return

MB64E_SYSEX_End_DumpRead
	;; send dump
	rcall	MB64E_SYSEX_SendDump

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Dump Write
;; --------------------------------------------------------------------------
MB64E_SYSEX_STR_R_0	STRING 16, 0x00, "Receiving SysEx "
MB64E_SYSEX_STR_R_1	STRING 16, 0x40, "   Dump $000    "
MB64E_SYSEX_STR_RI_0	STRING 16, 0x00, " Received SysEx "
MB64E_SYSEX_STR_RI_1	STRING 16, 0x40, " Dump INVALID!! "
MB64E_SYSEX_STR_RV_0	STRING 16, 0x00, " Received SysEx "
MB64E_SYSEX_STR_RV_1	STRING 16, 0x40, " Dump complete! "

MB64E_SYSEX_Init_DumpWrite
	;; print message
	TABLE_ADDR MB64E_SYSEX_STR_R_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; clear address
	clrf	MB_ADDRL
	clrf	MB_ADDRH
	return

MB64E_SYSEX_Action_DumpWrite
	;; write immediately into EEPROM (since it doesn't support page writes)
	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64E_SYSEX_Action_DumpWrite_NEE
	movf	MB_ADDRH, W
	xorlw	0x06
	bnz	MB64E_SYSEX_Action_DumpWrite_NEE
	movff	MB64E_SYSEX_IN, WREG
	call	MB64E_BANK_Write
	rgoto	MB64E_SYSEX_Action_DumpWriteCont

MB64E_SYSEX_Action_DumpWrite_NEE
	;; transfer data into buffer
	lfsr	FSR1, MB64E_DATA_BUFFER
	movf	MB_ADDRL, W
	andlw	0x3f
	movff	MB64E_SYSEX_IN, PLUSW1
	
	;; if buffer of 0x40 bytes is full, store data
	incf	MB_ADDRL, W
	andlw	0x3f
	bz	MB64E_SYSEX_Action_DumpWriteStre

	;; don't store yet, just increment address...
	incf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	rgoto	MB64E_SYSEX_Action_DumpWriteCont

MB64E_SYSEX_Action_DumpWriteStre
	;; align address to 0x40
	movlw	0xc0
	andwf	MB_ADDRL, F
	;; call write function (pointer to transfer buffer already in FSR1)
	call	MB64E_BANK_WritePage
	;; address has been incremented by the WritePage routine
	;; 	rgoto	MB64E_SYSEX_Action_DumpWriteCont
MB64E_SYSEX_Action_DumpWriteCont	
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


MB64E_SYSEX_End_DumpWrite
	;; check end address, print invalid message if not equal to 0x1000
	movf	MB_ADDRL, W
	bnz	MB64E_SYSEX_End_DumpWriteInvalid
	movf	MB_ADDRH, W
	xorlw	0x10
	bnz	MB64E_SYSEX_End_DumpWriteInvalid
MB64E_SYSEX_End_DumpWriteValid
	TABLE_ADDR MB64E_SYSEX_STR_RV_0
	rgoto	MB64E_SYSEX_End_DumpWriteCont

MB64E_SYSEX_End_DumpWriteInvalid
	TABLE_ADDR MB64E_SYSEX_STR_RI_0
	;; 	rgoto	MB64E_SYSEX_End_DumpWriteCont

MB64E_SYSEX_End_DumpWriteCont
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; transfer internal settings from BankStick into internal EEPROM
	call	MB64E_DUMP_TransferInternals
	;; restore the whole dump
	call	MB64E_DUMP_RestoreAll
	;; send acknowlegde
	call	MB64E_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Block Read
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_BlockRead
	;; clear block address
	clrf	MB_ADDRH
	return

MB64E_SYSEX_Action_BlockRead
	;; copy received byte to high address until F7 has been received
	movff	MB64E_SYSEX_IN, MB_ADDRH
	return

MB64E_SYSEX_End_BlockRead
	;; check if MB_ADDRH < 0x10
	movlw	0x10-1
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MB64E_SYSEX_End_BlockRead_Ok
MB64E_SYSEX_End_BlockRead_Fail
	;; send disacknowledge
	rcall	MB64E_SYSEX_Send_DisAcknowledge
	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished

MB64E_SYSEX_End_BlockRead_Ok
	;; send block
	rcall	MB64E_SYSEX_SendBlock
	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Block Write
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_BlockWrite
	;; re-use the Dump Write init routine
	rcall	MB64E_SYSEX_Init_DumpWrite
	;; clear checksum
	SET_BSR	MB64E_BASE
	clrf	MB64E_SYSEX_CHECKSUM, BANKED
	return

MB64E_SYSEX_Action_BlockWrite
	;; determine state
	SET_BSR	MB64E_BASE
	;; block and data received -> this byte is the checksum
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_DATA_RECEIVED, BANKED, MB64E_SYSEX_Action_BlockWrite_C
	;; else: add received byte to checksum
	movf	MB64E_SYSEX_IN, W, BANKED
	addwf	MB64E_SYSEX_CHECKSUM, F, BANKED

	;; first byte? then we received the block address - else we are receiving the block data	
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_BLOCK_RECEIVED, BANKED, MB64E_SYSEX_Action_BlockWrite_D
MB64E_SYSEX_Action_BlockWrite_A
	;; upload failed if block address >= 0x10
	movlw	0x10
	cpfslt	MB64E_SYSEX_IN, BANKED
	rgoto MB64E_SYSEX_Action_BlockWrite_F
	;; set block address
	movff	MB64E_SYSEX_IN, MB_ADDRH
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_BLOCK_RECEIVED, BANKED
	;; print high address
	movlw	0x49
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRH, W
	goto	MIOS_LCD_PrintHex1

MB64E_SYSEX_Action_BlockWrite_D
	;; store data in upload buffer
	lfsr	FSR0, MB64E_UPLOAD_BUFFER
	movf	MB_ADDRL, W
	addwf	FSR0L, F
	movff	MB64E_SYSEX_IN, INDF0
#if 0
	;; print low address
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRL, W
	call	MIOS_LCD_PrintHex2
#endif
	;; increment low byte of address, wait for checksum once 256 bytes have been received
	SET_BSR	MB64E_BASE
	incf	MB_ADDRL, F
	skpnz
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_DATA_RECEIVED, BANKED
	return

MB64E_SYSEX_Action_BlockWrite_C
	;; upload failed if checksum has already been received
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Action_BlockWrite_F
	;; compare received byte with checksum
	movf	MB64E_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	xorwf	MB64E_SYSEX_IN, W, BANKED
	;; failed upload if not equal
	bnz	MB64E_SYSEX_Action_BlockWrite_F
	;; else set flag which notifies that the whole block + checksum has been received
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED
	return

MB64E_SYSEX_End_BlockWrite
	;; upload failed if checksum has not been received
	BRA_IFCLR MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Action_BlockWrite_F
	;; restore data structure (which has been overwritten by the UPLOAD_BUFFER)
	rgoto	MB64E_SYSEX_Action_BlockWrite_R

MB64E_SYSEX_Action_BlockWrite_F
	;; disable CS flag (so that no acknowledge will be sent)
	SET_BSR	MB64E_BASE
	bcf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED
	;; print "dump invalid" message
	TABLE_ADDR MB64E_SYSEX_STR_RI_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

MB64E_SYSEX_Action_BlockWrite_R
	;; write 4 pages
	;; MB_ADDRH still set to block address)
	clrf	MB_ADDRL
	lfsr	FSR1, MB64E_UPLOAD_BUFFER
MB64E_SYSEX_Action_BlockWrite_L
	call	MB64E_BANK_WritePage
	;; increment FSR1 pointer by 0x40
	movlw	0x40
	addwf	FSR1L, F
	;; the write page function increments MB_ADDRL by 0x40
	;; repeat until MB_ADDRL reaches 0x00 again
	movf	MB_ADDRL, W
	bnz	MB64E_SYSEX_Action_BlockWrite_L

	;; workaround: if the merger mode changes here, the acknowledge message will be
	;; corrupted. This can be avoided by saving the merger, setting it to the old value
	;; after RestoreAll, and setting it to the new value after the acknowledge message
	;; merger state will be saved in MB64E_SYSEX_IN
	call	MIOS_MIDI_MergerGet
	movff	WREG, MB64E_SYSEX_IN

	;; transfer internal settings from BankStick into internal EEPROM
	call	MB64E_DUMP_TransferInternals
	;; restore the whole dump
	call	MB64E_DUMP_RestoreAll

	;; save new merger mode, switch to old merger mode
	call	MIOS_MIDI_MergerGet
	movff	MB64E_SYSEX_IN, WREG
	movff	MIOS_PARAMETER1, MB64E_SYSEX_IN
	call	MIOS_MIDI_MergerSet

	;; send acknowledge/disacknowledge depending on CS flag
	SET_BSR	MB64E_BASE
	RCALL_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Send_Acknowledge
	SET_BSR	MB64E_BASE
	RCALL_IFCLR MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Send_DisAcknowledge

	;; finally restore new merger mode
	movff	MB64E_SYSEX_IN, WREG
	call	MIOS_MIDI_MergerSet

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Read all blocks
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_AllRead
	return

MB64E_SYSEX_Action_AllRead
	;; do nothing until sysex footer (F7) has been received
	return

MB64E_SYSEX_End_AllRead
	;; send dump
	rcall	MB64E_SYSEX_SendAll

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Read
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_PatchRead
	return

MB64E_SYSEX_Action_PatchRead
	;; copy received byte to MB_PATCH until F7 has been received
	movff	MB64E_SYSEX_IN, MB_PATCH
	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

MB64E_SYSEX_End_PatchRead
	;; send patch
	rcall	MB64E_SYSEX_SendPatch
	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Write
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_PatchWrite
	return

MB64E_SYSEX_Action_PatchWrite
	;; determine state
	SET_BSR	MB64E_BASE
	;; patch and data received -> this byte is the checksum
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_DATA_RECEIVED, BANKED, MB64E_SYSEX_Action_PatchWrite_C
	;; else: add received byte to checksum
	movf	MB64E_SYSEX_IN, W, BANKED
	addwf	MB64E_SYSEX_CHECKSUM, F, BANKED

	;; first byte? then we received the patch number - else we are receiving the patch data	
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_BLOCK_RECEIVED, BANKED, MB64E_SYSEX_Action_PatchWrite_D
MB64E_SYSEX_Action_PatchWrite_A
	;; set patch number and clear low-byte of address
	movff	MB64E_SYSEX_IN, MB_PATCH
	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; clear low-byte of address
	clrf	MB_ADDRL
	;; clear checksum
	SET_BSR	MB64E_BASE
	clrf	MB64E_SYSEX_CHECKSUM, BANKED
	;; change state
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_BLOCK_RECEIVED, BANKED
	return

MB64E_SYSEX_Action_PatchWrite_D
	;; store data in patch memory
	movf	MB_ADDRL, W
	BRA_IFSET MB_ADDRL, 7, ACCESS, MB64E_SYSEX_Action_PatchWriteDBN
MB64E_SYSEX_Action_PatchWriteDK		; knob value
	lfsr	FSR0, MB64E_ENC_VALUES_00
	andlw	0x7f			; just to ensure that we never overwrite other data
	movff	MB64E_SYSEX_IN, PLUSW0
	rgoto	MB64E_SYSEX_Action_PatchWrite_DC

MB64E_SYSEX_Action_PatchWriteDBN	; button/name value
	BRA_IFSET MB_ADDRL, 4, ACCESS, MB64E_SYSEX_Action_PatchWriteDN
MB64E_SYSEX_Action_PatchWriteDB		; button value
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
	rrf	MB_ADDRL, W
	andlw	0x07			; just to ensure that we never overwrite other data
	addwf	FSR0L, F
	BRA_IFCLR MB_ADDRL, 0, ACCESS, MB64E_SYSEX_Action_PatchWriteDBL
MB64E_SYSEX_Action_PatchWriteDBH	; write high byte
	;; (it's a read-modify-write operation)
	SET_BSR	MB64E_BASE
	swapf	MB64E_SYSEX_IN, W
	andlw	0xf0
	iorwf	INDF0, F
	rgoto	MB64E_SYSEX_Action_PatchWrite_DC
MB64E_SYSEX_Action_PatchWriteDBL	; write low byte
	movff	MB64E_SYSEX_IN, INDF0
 	rgoto	MB64E_SYSEX_Action_PatchWrite_DC

MB64E_SYSEX_Action_PatchWriteDN		; name value
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
	andlw	0x0f			; just to ensure that we never overwrite other data
	movff	MB64E_SYSEX_IN, PLUSW0
	;; 	rgoto	MB64E_SYSEX_Action_PatchWrite_DC

MB64E_SYSEX_Action_PatchWrite_DC
	;; increment low byte of address, wait for checksum once 160 bytes have been received
	SET_BSR	MB64E_BASE
	incf	MB_ADDRL, F
	movlw	160
	cpfsgt	MB_ADDRL, ACCESS
	return
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_DATA_RECEIVED, BANKED
	return

MB64E_SYSEX_Action_PatchWrite_C
	;; upload failed if checksum has already been received
	BRA_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Action_PatchWrite_F
	;; compare received byte with checksum
	movf	MB64E_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	xorwf	MB64E_SYSEX_IN, W, BANKED
	;; failed upload if not equal
	bnz	MB64E_SYSEX_Action_PatchWrite_F
	;; else set flag which notifies that the whole block + checksum has been received
	bsf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED
	return

MB64E_SYSEX_End_PatchWrite
	;; upload failed if checksum has not been received
	BRA_IFCLR MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Action_PatchWrite_F
	;; write patch to memory
	call	MB64E_DUMP_StorePatch
	rgoto	MB64E_SYSEX_Action_PatchWrite_R

MB64E_SYSEX_Action_PatchWrite_F
	;; disable CS flag (so that no acknowledge will be sent)
	SET_BSR	MB64E_BASE
	bcf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED

MB64E_SYSEX_Action_PatchWrite_R

	;; send acknowledge/disacknowledge depending on CS flag
	SET_BSR	MB64E_BASE
	RCALL_IFSET MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Send_Acknowledge
	SET_BSR	MB64E_BASE
	RCALL_IFCLR MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_CS_RECEIVED, BANKED, MB64E_SYSEX_Send_DisAcknowledge

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Bank Change
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_BankChange
	return

MB64E_SYSEX_Action_BankChange
	;; memorize incoming byte in buffer (the last byte will be taken)
	movff	MB64E_SYSEX_IN, MB64E_DATA_BUFFER
	return

MB64E_SYSEX_End_BankChange
	;; send acknowledge if successfull
	RCALL_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64E_SYSEX_Send_Acknowledge
	;; else send disacknowledge
	RCALL_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64E_SYSEX_Send_DisAcknowledge

	;; change bank
	;; TK: although the acknowledge message would be more interesting after
	;; the bank has been changed (since an uninitialized bank could be formatted,
	;; which wouldn't take longer than the common switching procedure), we are
	;; doing it thereafter, so that the complete SysEx message is sent out, before
	;; new MIDI events (Auto-Snapshot, Program Change) will be sent by BANK_Change
	;; this avoids a MIDI protocol violation
	movff	MB64E_DATA_BUFFER, WREG
	call	MB64E_BANK_Change

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Remote Control
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_Remote
	;; clear address to buffer
	clrf	MB_ADDRL
	;; clear the three used buffer bytes
	SET_BSR	MB64E_BASE
	clrf	MB64E_DATA_BUFFER+0, BANKED
	clrf	MB64E_DATA_BUFFER+1, BANKED
	clrf	MB64E_DATA_BUFFER+2, BANKED
	clrf	MB64E_DATA_BUFFER+3, BANKED
	return

MB64E_SYSEX_Action_Remote
	;; memorize incoming byte in buffer
	lfsr	FSR0, MB64E_DATA_BUFFER
	movf	MB_ADDRL, W
	movff	MB64E_SYSEX_IN, PLUSW0
	;; increment address
	incf	MB_ADDRL, W	; wrap at 0x40
	andlw	0x3f
	movwf	MB_ADDRL
	return

MB64E_SYSEX_End_Remote
	;; send F7 if merger enabled to ensure a proper MIDI protocol
	movlw	0x00
	rcall	MB64E_SYSEX_Send_SysExFooter

	;; execute remote action
	movff	MB64E_DATA_BUFFER, WREG
	JUMPTABLE_2BYTES 3	; 3 entries
	rgoto	MB64E_SYSEX_Remote_Enc
	rgoto	MB64E_SYSEX_Remote_Button
	rgoto	MB64E_SYSEX_Remote_SFB

	;; remote control a enc event
MB64E_SYSEX_Remote_Enc
	;; copy 7-bit value to MIOS_PARAMETER2
	movff	MB64E_DATA_BUFFER+2, MIOS_PARAMETER2
	;; copy enc number to MIOS_PARAMETER1 and WREG
	SET_BSR	MB64E_BASE
	movf	MB64E_DATA_BUFFER+1, W
	movwf	MIOS_PARAMETER1
	;; call enc handler
	call	MB64E_ENC_Handler
	rgoto	MB64E_SYSEX_Remote_End
	

	;; remote control a button event
MB64E_SYSEX_Remote_Button
	;; copy 7-bit value to MIOS_PARAMETER2
	movff	MB64E_DATA_BUFFER+2, MIOS_PARAMETER2
	;; copy button number to MIOS_PARAMETER1 and WREG
	SET_BSR	MB64E_BASE
	movf	MB64E_DATA_BUFFER+1, W
	movwf	MIOS_PARAMETER1
	;; call button handler
	call	MB64E_BUTTON_Handler
	rgoto	MB64E_SYSEX_Remote_End
	
	;; remote control a special button event
MB64E_SYSEX_Remote_SFB
	;; copy SFB number, parameter and value to MIDI_EVNT*
	movff	MB64E_DATA_BUFFER+1, MIDI_EVNT0
	movff	MB64E_DATA_BUFFER+2, MIDI_EVNT1
	movff	MB64E_DATA_BUFFER+3, MIDI_EVNT_VALUE
	;; call SFB handler
	call	MB64E_SFB_Handler
	rgoto	MB64E_SYSEX_Remote_End

MB64E_SYSEX_Remote_End
	;; exit
	rgoto	MB64E_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
MB64E_SYSEX_Init_Ping
	return

MB64E_SYSEX_Action_Ping
	;; do nothing until sysex footer (F7) has been received
	return

MB64E_SYSEX_End_Ping
	;; send Acknowledge
	rcall	MB64E_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MB64E_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MB64E_SYSEX_Send_DisAcknowledge
	rcall	MB64E_SYSEX_Send_SysExHeader

	SET_BSR	MB64E_BASE
	swapf	MB64E_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x0e		; (disacknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send SysEx footer
	movlw	0x01		; (independent from merger state)
	rgoto	MB64E_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MB64E_SYSEX_Send_Acknowledge
	rcall	MB64E_SYSEX_Send_SysExHeader

	SET_BSR	MB64E_BASE
	swapf	MB64E_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send SysEx footer
	movlw	0x01		; (independent from merger state)
	rgoto	MB64E_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send MB64E SysEx Header (Util function)
;; --------------------------------------------------------------------------
MB64E_SYSEX_Send_SysExHeader
	SET_BSR	MB64E_BASE
	;; if MB64E_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_MYSYSEX, BANKED, MB64E_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MB64E_SYSEX_Send_SysExHeader_Skp
	movlw	0x04		; send only command
	movwf	TMP1
	rgoto	MB64E_SYSEX_Send_SysExHeaderLoop
MB64E_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
MB64E_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	MB64E_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x05
	cpfseq	TMP1, ACCESS
	rgoto MB64E_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
MB64E_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, MB64E_SYSEX_Send_SysExFooterNow
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

MB64E_SYSEX_Send_SysExFooterNow
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Send a SysEx dump of MIDIbox64E
;; --------------------------------------------------------------------------
MB64E_SYSEX_SendDump
	;; send header
	rcall	MB64E_SYSEX_Send_SysExHeader

	;; indicate a write action
	SET_BSR	MB64E_BASE
	swapf	MB64E_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send 0x1000 bytes
	clrf	MB_ADDRL
	clrf	MB_ADDRH
MB64E_SYSEX_SendDump_Loop
	clrwdt		; feed watchdog
	call	MB64E_BANK_Read
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	0x10-1
	cpfsgt	MB_ADDRH, BANKED
	rgoto MB64E_SYSEX_SendDump_Loop

	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MB64E_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Send a SysEx block of MIDIbox64E
;; --------------------------------------------------------------------------
MB64E_SYSEX_SendBlock
	;; send header
	rcall	MB64E_SYSEX_Send_SysExHeader

	;; clear checksum
	SET_BSR	MB64E_BASE
	clrf	MB64E_SYSEX_CHECKSUM, BANKED

	;; indicate a block write action
	swapf	MB64E_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x04
	call	MIOS_MIDI_TxBufferPut

	;; add high address (the block number) to checksum and send it
	SET_BSR	MB64E_BASE
	movf	MB_ADDRH, W
	addwf	MB64E_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	
	;; send 256 bytes
	clrf	MB_ADDRL
MB64E_SYSEX_SendBlock_Loop
	clrwdt		; feed watchdog
	call	MB64E_BANK_Read
	SET_BSR	MB64E_BASE
	addwf	MB64E_SYSEX_CHECKSUM, F, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MB_ADDRL, W
	bnz	MB64E_SYSEX_SendBlock_Loop

	;; send checksum
	SET_BSR	MB64E_BASE
	movf	MB64E_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MB64E_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Sends all SysEx blocks of MIDIbox64E
;; --------------------------------------------------------------------------
MB64E_SYSEX_SendAll
	;; 16 blocks, using MB_ADDRH as block counter
	clrf	MB_ADDRH
MB64E_SYSEX_SendAllLoop
	;; send block
	rcall	MB64E_SYSEX_SendBlock
	movlw	0x10
	cpfslt	MB_ADDRH, ACCESS
	rgoto MB64E_SYSEX_SendAll_End

	;; workaround for merger issue: clear MYSYSEX flag, so that the
	;; next header will be sent regardless of merger state
	SET_BSR	MB64E_BASE
	bcf	MB64E_SYSEX_STATE, MB64E_SYSEX_STATE_MYSYSEX, BANKED

	;; insert delay of 750 mS
	movlw	0x03
	movwf	TMP1
MB64E_SYSEX_SendAllDlyLoop
	movlw	250
	call	MIOS_Delay
	decfsz	TMP1, F
	rgoto	MB64E_SYSEX_SendAllDlyLoop
	rgoto	MB64E_SYSEX_SendAllLoop

MB64E_SYSEX_SendAll_End
	return

;; --------------------------------------------------------------------------
;;  Send a Patch of MIDIbox64E
;; --------------------------------------------------------------------------
MB64E_SYSEX_SendPatch
	;; send header
	rcall	MB64E_SYSEX_Send_SysExHeader

	;; indicate a patch store action
	SET_BSR	MB64E_BASE
	swapf	MB64E_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x07
	call	MIOS_MIDI_TxBufferPut

	;; add patch number
	SET_BSR	MB64E_BASE
	movf	MB_PATCH, W
	call	MIOS_MIDI_TxBufferPut

	;; clear checksum
	SET_BSR	MB64E_BASE
	clrf	MB64E_SYSEX_CHECKSUM, BANKED
	
	;; send 160 bytes
	clrf	MB_ADDRL
MB64E_SYSEX_SendPatch_Loop
	clrwdt		; feed watchdog
	call	MB64E_BANK_PatchRead
	SET_BSR	MB64E_BASE
	addwf	MB64E_SYSEX_CHECKSUM, F, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	160
	cpfsgt	MB_ADDRL, ACCESS
	rgoto MB64E_SYSEX_SendPatch_Loop

	;; send checksum
	SET_BSR	MB64E_BASE
	movf	MB64E_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MB64E_SYSEX_Send_SysExFooter


