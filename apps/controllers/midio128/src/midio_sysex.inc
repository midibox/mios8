; $Id$
;
; MIDIO128 SysEx parser
;
; Supported SysEx commands:
;   a) F0 00 00 7E 44 <d>1 F7
;      Request a Dump
;      OBSOLETE!!! ONLY SUPPORTED FOR PIC16F compatibility!!!
;         <d> = device number (0-7)
;
;   b) F0 00 00 7E 44 <d>2 <dump> F7
;      Write a Dump
;      OBSOLETE!!! ONLY SUPPORTED FOR PIC16F compatibility!!!
;         <d> = device number (0-7)
;         <dump> = 1536 bytes dump data
;
;   c) F0 00 00 7E 44 <d>3 <b> F7
;      Request a block of 256 bytes
;         <d> = device number (0-7)
;         <b> = block number (00-05)
;
;   d) F0 00 00 7E 44 <d>4 <b> <256 bytes> <checksum> F7
;      Write a block of 256 bytes
;         <d> = device number (0-7)
;         <b> = block number (00-05)
;         <256 bytes> = the block data
;         <checksum> = the checksum of <b> and <256 bytes>
;
;   e) F0 00 00 7E 44 <d>5 F7
;      Request all 6 blocks
;         <d> = device number (0-7)
;
;   f) F0 00 00 7E 44 <d>9 01 <din> <value> F7
;      Remote Control of a DIN
;         <d>      = device number (0-7)
;         <button> = DIN number (0-127)
;         <value>  = DIN value (for buttons, low-active: 0=pressed, 1=depressed)

;   g) F0 00 00 7E 44 <d>F F7
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

MIDIO_SYSEX_STATE_MYSYSEX	EQU	7
MIDIO_SYSEX_STATE_ACTION	EQU	6
MIDIO_SYSEX_STATE_BLOCK_RECEIVED EQU	5
MIDIO_SYSEX_STATE_DATA_RECEIVED	EQU	4
MIDIO_SYSEX_STATE_CS_RECEIVED	EQU	3

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc on a timeout
;;  it will used internally when an internal sequence has been parsed or 
;;  at the end of a sysex sequence
;; --------------------------------------------------------------------------
MIDIO_SYSEX_ActionFinished
MIDIO_SYSEX_ActionInvalid
	SET_BSR	MIDIO_BASE
	clrf	MIDIO_SYSEX_STATE, BANKED
	clrf	MIDIO_SYSEX_ACTION, BANKED

	goto	MIOS_MPROC_MergerEnable

;; --------------------------------------------------------------------------
;;  This function is called from mb_mproc.inc when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Parser
	;; store received byte in MIDIO_SYSEX_IN
	SET_BSR	MIDIO_BASE
	movwf	MIDIO_SYSEX_IN, BANKED

	;; check sysex state
	BRA_IFSET MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_MYSYSEX, BANKED, MIDIO_SYSEX_Handler

	;; parse incoming bytes
	movf	MIDIO_SYSEX_STATE, W, BANKED
	rcall	MIDIO_SYSEX_SysExHeaderGet
	cpfseq	MIDIO_SYSEX_IN, BANKED
	rgoto MIDIO_SYSEX_ActionInvalid
	incf	MIDIO_SYSEX_STATE, F, BANKED
	movf	MIDIO_SYSEX_STATE, W, BANKED
	andlw	0x07
	xorlw	0x05	; wait for 5 bytes (f0 00 00 7E 44)
	bnz	MIDIO_SYSEX_Parser_End

	;; SysEx ID received, lets go
	movlw	(1 << MIDIO_SYSEX_STATE_MYSYSEX)
	movwf	MIDIO_SYSEX_STATE, BANKED
	call	MIOS_MPROC_MergerDisable

MIDIO_SYSEX_Parser_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
MIDIO_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x44		; MIDIO128 ID (44 - the ultimative number + 2)
	retlw	0xff
	retlw	0xff
	retlw	0xff

;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Handler
	SET_BSR	MIDIO_BASE
	BRA_IFSET MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_ACTION, BANKED, MIDIO_SYSEX_Handler_DoAction

	;; 6th byte: check device ID, embedded in bit 6-4 of the command byte
	swapf	MIDIO_SYSEX_IN, W, BANKED
	andlw	0x07
	xorwf	MIDIO_DEVICE_ID, W, BANKED
	bz	MIDIO_SYSEX_Handler_ID_Ok
MIDIO_SYSEX_Handler_ID_NotOk
	;; if MIDI merger enabled: send 5th byte and command
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MIDIO_SYSEX_ActionInvalid
	movlw	0x04
	call	MIDIO_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	movff	MIDIO_SYSEX_IN, WREG
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDIO_SYSEX_ActionInvalid
	
MIDIO_SYSEX_Handler_ID_Ok
	bsf	MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_ACTION, BANKED

	movf	MIDIO_SYSEX_IN, W, BANKED		; store action ID
	andlw	0x0f
	movwf	MIDIO_SYSEX_ACTION	
	rgoto	MIDIO_SYSEX_Handler_InitAction		; initialize the action and exit

	;; ---

MIDIO_SYSEX_Handler_DoAction
	SET_BSR	MIDIO_BASE
	;; branch to end if status byte (i.e. F7)
	BRA_IFSET MIDIO_SYSEX_IN, 7, BANKED, MIDIO_SYSEX_Handler_EndAction

	;; branch depending on current action ID
	movf	MIDIO_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_Action_DumpRead
	rgoto	MIDIO_SYSEX_Action_DumpWrite
	rgoto	MIDIO_SYSEX_Action_BlockRead
	rgoto	MIDIO_SYSEX_Action_BlockWrite
	rgoto	MIDIO_SYSEX_Action_AllRead
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_Action_BankChange
	rgoto	MIDIO_SYSEX_Action_Remote
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_Action_Ping


;; --------------------------------------------------------------------------
;;  Initialize an action depending on MIDIO_SYSEX_ACTION
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Handler_InitAction
	SET_BSR	MIDIO_BASE
	movf	MIDIO_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_Init_DumpRead
	rgoto	MIDIO_SYSEX_Init_DumpWrite
	rgoto	MIDIO_SYSEX_Init_BlockRead
	rgoto	MIDIO_SYSEX_Init_BlockWrite
	rgoto	MIDIO_SYSEX_Init_AllRead
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_Init_BankChange
	rgoto	MIDIO_SYSEX_Init_Remote
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_Init_Ping

	
;; --------------------------------------------------------------------------
;;  Finish an action depending on MIDIO_SYSEX_ACTION
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Handler_EndAction
	SET_BSR	MIDIO_BASE
	;; if sysex footer (F7) has not been received here, action is invalid!
	movlw	0xf7
	cpfseq	MIDIO_SYSEX_IN, BANKED
	rgoto MIDIO_SYSEX_ActionInvalid

	;; else finish action
	movf	MIDIO_SYSEX_ACTION, W, BANKED
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_End_DumpRead
	rgoto	MIDIO_SYSEX_End_DumpWrite
	rgoto	MIDIO_SYSEX_End_BlockRead
	rgoto	MIDIO_SYSEX_End_BlockWrite
	rgoto	MIDIO_SYSEX_End_AllRead
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_End_BankChange
	rgoto	MIDIO_SYSEX_End_Remote
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_ActionInvalid
	rgoto	MIDIO_SYSEX_End_Ping

	
;; --------------------------------------------------------------------------
;;  MIDI Action: Dump Read
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_DumpRead
	return

MIDIO_SYSEX_Action_DumpRead
	;; do nothing until sysex footer (F7) has been received
	return

MIDIO_SYSEX_End_DumpRead
	;; send dump
	rcall	MIDIO_SYSEX_SendDump

	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Dump Write
;; --------------------------------------------------------------------------
MIDIO_SYSEX_STR_R_0	STRING 16, 0x00, "Receiving SysEx "
MIDIO_SYSEX_STR_R_1	STRING 16, 0x40, "   Dump $000    "
MIDIO_SYSEX_STR_RI_0	STRING 16, 0x00, " Received SysEx "
MIDIO_SYSEX_STR_RI_1	STRING 16, 0x40, " Dump INVALID!! "
MIDIO_SYSEX_STR_RV_0	STRING 16, 0x00, " Received SysEx "
MIDIO_SYSEX_STR_RV_1	STRING 16, 0x40, " Dump complete! "

MIDIO_SYSEX_Init_DumpWrite
	;; print message
	TABLE_ADDR MIDIO_SYSEX_STR_R_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; clear address
	clrf	MB_ADDRL
	clrf	MB_ADDRH
	return

MIDIO_SYSEX_Action_DumpWrite
	;; write immediately into EEPROM (since it doesn't support page writes)
	movf	MB_ADDRH, W
	xorlw	0x04
	bnz	MIDIO_SYSEX_Action_DumpWrite_NEE
	movff	MIDIO_SYSEX_IN, WREG
	call	MIDIO_BANK_Write
	rgoto	MIDIO_SYSEX_Action_DumpWriteCont

MIDIO_SYSEX_Action_DumpWrite_NEE
	;; transfer data into buffer
	lfsr	FSR1, MIDIO_DATA_BUFFER
	movf	MB_ADDRL, W
	andlw	0x3f
	movff	MIDIO_SYSEX_IN, PLUSW1
	
	;; if buffer of 0x40 bytes is full, store data
	incf	MB_ADDRL, W
	andlw	0x3f
	bz	MIDIO_SYSEX_Action_DumpWriteSt

	;; don't store yet, just increment address...
	incf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	rgoto	MIDIO_SYSEX_Action_DumpWriteCont

MIDIO_SYSEX_Action_DumpWriteSt
	;; align address to 0x40
	movlw	0xc0
	andwf	MB_ADDRL, F
	;; call write function (pointer to transfer buffer already in FSR1)
	call	MIDIO_BANK_WritePage
	;; address has been incremented by the WritePage routine
	;; 	rgoto	MIDIO_SYSEX_Action_DumpWriteCont
MIDIO_SYSEX_Action_DumpWriteCont	
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


MIDIO_SYSEX_End_DumpWrite
	;; check end address, print invalid message if not equal to 0x0600
	movf	MB_ADDRL, W
	bnz	MIDIO_SYSEX_End_DumpWriteInvalid
	movf	MB_ADDRH, W
	xorlw	0x06
	bnz	MIDIO_SYSEX_End_DumpWriteInvalid
MIDIO_SYSEX_End_DumpWrite_Valid
	TABLE_ADDR MIDIO_SYSEX_STR_RV_0
	rgoto	MIDIO_SYSEX_End_DumpWriteCont

MIDIO_SYSEX_End_DumpWriteInvalid
	TABLE_ADDR MIDIO_SYSEX_STR_RI_0
	;; 	rgoto	MIDIO_SYSEX_End_DumpWriteCont

MIDIO_SYSEX_End_DumpWriteCont
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; transfer internal settings from BankStick into internal EEPROM
	call	MIDIO_DUMP_TransferInternals
	;; restore the whole dump
	call	MIDIO_DUMP_RestoreAll
	;; send acknowlegde
	call	MIDIO_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Block Read
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_BlockRead
	;; clear block address
	clrf	MB_ADDRH
	return

MIDIO_SYSEX_Action_BlockRead
	;; copy received byte to high address until F7 has been received
	movff	MIDIO_SYSEX_IN, MB_ADDRH
	return

MIDIO_SYSEX_End_BlockRead
	;; check if MB_ADDRH < 0x06
	movlw	0x06-1
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_SYSEX_End_BlockRead_Ok
MIDIO_SYSEX_End_BlockRead_Fail
	;; send disacknowledge
	rcall	MIDIO_SYSEX_Send_DisAcknowledge
	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished

MIDIO_SYSEX_End_BlockRead_Ok
	;; send block
	rcall	MIDIO_SYSEX_SendBlock
	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Block Write
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_BlockWrite
	;; re-use the Dump Write init routine
	rcall	MIDIO_SYSEX_Init_DumpWrite
	;; clear checksum
	SET_BSR	MIDIO_BASE
	clrf	MIDIO_SYSEX_CHECKSUM, BANKED
	return

MIDIO_SYSEX_Action_BlockWrite
	;; determine state
	SET_BSR	MIDIO_BASE
	;; block and data received -> this byte is the checksum
	BRA_IFSET MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_DATA_RECEIVED, BANKED, MIDIO_SYSEX_Action_BlockWrite_C
	;; else: add received byte to checksum
	movf	MIDIO_SYSEX_IN, W, BANKED
	addwf	MIDIO_SYSEX_CHECKSUM, F, BANKED

	;; first byte? then we received the block address - else we are receiving the block data	
	BRA_IFSET MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_BLOCK_RECEIVED, BANKED, MIDIO_SYSEX_Action_BlockWrite_D
MIDIO_SYSEX_Action_BlockWrite_A
	;; upload failed if block address >= 0x06
	movlw	0x06
	cpfslt	MIDIO_SYSEX_IN, BANKED
	rgoto MIDIO_SYSEX_Action_BlockWrite_F
	;; set block address
	movff	MIDIO_SYSEX_IN, MB_ADDRH
	bsf	MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_BLOCK_RECEIVED, BANKED
	;; print high address
	movlw	0x49
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRH, W
	goto	MIOS_LCD_PrintHex1

MIDIO_SYSEX_Action_BlockWrite_D
	;; store data in upload buffer
	lfsr	FSR0, MIDIO_UPLOAD_BUFFER
	movf	MB_ADDRL, W
	addwf	FSR0L, F
	movff	MIDIO_SYSEX_IN, INDF0
#if 0
	;; print low address
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	movf	MB_ADDRL, W
	call	MIOS_LCD_PrintHex2
#endif
	;; increment low byte of address, wait for checksum once 256 bytes have been received
	SET_BSR	MIDIO_BASE
	incf	MB_ADDRL, F
	skpnz
	bsf	MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_DATA_RECEIVED, BANKED
	return

MIDIO_SYSEX_Action_BlockWrite_C
	;; upload failed if checksum has already been received
	BRA_IFSET MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_CS_RECEIVED, BANKED, MIDIO_SYSEX_Action_BlockWrite_F
	;; compare received byte with checksum
	movf	MIDIO_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	xorwf	MIDIO_SYSEX_IN, W, BANKED
	;; failed upload if not equal
	bnz	MIDIO_SYSEX_Action_BlockWrite_F
	;; else set flag which notifies that the whole block + checksum has been received
	bsf	MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_CS_RECEIVED, BANKED
	return

MIDIO_SYSEX_End_BlockWrite
	;; upload failed if checksum has not been received
	BRA_IFCLR MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_CS_RECEIVED, BANKED, MIDIO_SYSEX_Action_BlockWrite_F
	;; restore data structure (which has been overwritten by the UPLOAD_BUFFER)
	rgoto	MIDIO_SYSEX_Action_BlockWrite_R

MIDIO_SYSEX_Action_BlockWrite_F
	;; disable CS flag (so that no acknowledge will be sent)
	SET_BSR	MIDIO_BASE
	bcf	MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_CS_RECEIVED, BANKED
	;; print "dump invalid" message
	TABLE_ADDR MIDIO_SYSEX_STR_RI_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

MIDIO_SYSEX_Action_BlockWrite_R
	;; write 4 pages
	;; MB_ADDRH still set to block address)
	clrf	MB_ADDRL
	lfsr	FSR1, MIDIO_UPLOAD_BUFFER
MIDIO_SYSEX_Action_BlockWrite_L
	call	MIDIO_BANK_WritePage
	;; increment FSR1 pointer by 0x40
	movlw	0x40
	addwf	FSR1L, F
	;; the write page function increments MB_ADDRL by 0x40
	;; repeat until MB_ADDRL reaches 0x00 again
	movf	MB_ADDRL, W
	bnz	MIDIO_SYSEX_Action_BlockWrite_L

	;; transfer internal settings from BankStick into internal EEPROM
	call	MIDIO_DUMP_TransferInternals
	;; restore the whole dump
	call	MIDIO_DUMP_RestoreAll

	;; send acknowledge/disacknowledge depending on CS flag
	SET_BSR	MIDIO_BASE
	RCALL_IFSET MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_CS_RECEIVED, BANKED, MIDIO_SYSEX_Send_Acknowledge
	SET_BSR	MIDIO_BASE
	RCALL_IFCLR MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_CS_RECEIVED, BANKED, MIDIO_SYSEX_Send_DisAcknowledge

	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Read all blocks
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_AllRead
	return

MIDIO_SYSEX_Action_AllRead
	;; do nothing until sysex footer (F7) has been received
	return

MIDIO_SYSEX_End_AllRead
	;; send dump
	rcall	MIDIO_SYSEX_SendAll

	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Bank Change
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_BankChange
	return

MIDIO_SYSEX_Action_BankChange
	;; memorize incoming byte in buffer (the last byte will be taken)
	movff	MIDIO_SYSEX_IN, MIDIO_DATA_BUFFER
	return

MIDIO_SYSEX_End_BankChange
	;; not supported by MIDIO128
	rcall	MIDIO_SYSEX_Send_DisAcknowledge

	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Remote Control
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_Remote
	;; clear address to buffer
	clrf	MB_ADDRL
	;; clear the three used buffer bytes
	SET_BSR	MIDIO_BASE
	clrf	MIDIO_DATA_BUFFER+0, BANKED
	clrf	MIDIO_DATA_BUFFER+1, BANKED
	clrf	MIDIO_DATA_BUFFER+2, BANKED
	clrf	MIDIO_DATA_BUFFER+3, BANKED
	return

MIDIO_SYSEX_Action_Remote
	;; memorize incoming byte in buffer
	lfsr	FSR0, MIDIO_DATA_BUFFER
	movf	MB_ADDRL, W
	movff	MIDIO_SYSEX_IN, PLUSW0
	;; increment address
	incf	MB_ADDRL, W	; wrap at 0x40
	andlw	0x3f
	movwf	MB_ADDRL
	return

MIDIO_SYSEX_End_Remote
	;; send F7 if merger enabled to ensure a proper MIDI protocol
	movlw	0x00
	rcall	MIDIO_SYSEX_Send_SysExFooter

	;; execute remote action
	movff	MIDIO_DATA_BUFFER, WREG
	JUMPTABLE_2BYTES 3	; 3 entries
	rgoto	MIDIO_SYSEX_Remote_Pot
	rgoto	MIDIO_SYSEX_Remote_DIN
	rgoto	MIDIO_SYSEX_Remote_SFB

	;; remote control a pot event
MIDIO_SYSEX_Remote_Pot
	;; not supported by MIDIO128
	rgoto	MIDIO_SYSEX_Remote_End
	

	;; remote control a DIN event
MIDIO_SYSEX_Remote_DIN
	;; copy 7-bit value to MIOS_PARAMETER2
	movff	MIDIO_DATA_BUFFER+2, MIOS_PARAMETER2
	;; copy din number to MIOS_PARAMETER1 and WREG
	SET_BSR	MIDIO_BASE
	movf	MIDIO_DATA_BUFFER+1, W
	movwf	MIOS_PARAMETER1
	;; call din handler
	call	MIDIO_DIN_Handler
	rgoto	MIDIO_SYSEX_Remote_End
	
	;; remote control a special button event
MIDIO_SYSEX_Remote_SFB
	;; not supported by MIDIO128
	rgoto	MIDIO_SYSEX_Remote_End

MIDIO_SYSEX_Remote_End
	;; exit
	rgoto	MIDIO_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Init_Ping
	return

MIDIO_SYSEX_Action_Ping
	;; do nothing until sysex footer (F7) has been received
	return

MIDIO_SYSEX_End_Ping
	;; send Acknowledge
	rcall	MIDIO_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	MIDIO_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Send_DisAcknowledge
	rcall	MIDIO_SYSEX_Send_SysExHeader

	SET_BSR	MIDIO_BASE
	swapf	MIDIO_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x0e		; (disacknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send SysEx footer
	movlw	0x01		; (independent from merger state)
	rgoto	MIDIO_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Send_Acknowledge
	rcall	MIDIO_SYSEX_Send_SysExHeader

	SET_BSR	MIDIO_BASE
	swapf	MIDIO_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send SysEx footer
	movlw	0x01		; (independent from merger state)
	rgoto	MIDIO_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send MIDIO SysEx Header (Util function)
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Send_SysExHeader
	SET_BSR	MIDIO_BASE
	;; if MIDIO_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_MYSYSEX, BANKED, MIDIO_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	MIDIO_SYSEX_Send_SysExHeader_Skp
	movlw	0x04		; send only command
	movwf	TMP1
	rgoto	MIDIO_SYSEX_Send_SysExHeaderLoop
MIDIO_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
MIDIO_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	MIDIO_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x05
	cpfseq	TMP1, ACCESS
	rgoto MIDIO_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
MIDIO_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, MIDIO_SYSEX_Send_SysExFooterNow
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

MIDIO_SYSEX_Send_SysExFooterNow
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Send a SysEx dump of MIDIO128
;; --------------------------------------------------------------------------
MIDIO_SYSEX_SendDump
	;; send header
	rcall	MIDIO_SYSEX_Send_SysExHeader

	;; indicate a write action
	SET_BSR	MIDIO_BASE
	swapf	MIDIO_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send 0x600 bytes
	clrf	MB_ADDRL
	clrf	MB_ADDRH
MIDIO_SYSEX_SendDump_Loop
	clrwdt		; feed watchdog
	call	MIDIO_BANK_Read
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	0x06-1
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_SYSEX_SendDump_Loop
	
	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MIDIO_SYSEX_Send_SysExFooter




;; --------------------------------------------------------------------------
;;  Send a SysEx block of MIDIO128
;; --------------------------------------------------------------------------
MIDIO_SYSEX_SendBlock
	;; send header
	rcall	MIDIO_SYSEX_Send_SysExHeader

	;; clear checksum
	SET_BSR	MIDIO_BASE
	clrf	MIDIO_SYSEX_CHECKSUM, BANKED

	;; indicate a block write action
	swapf	MIDIO_DEVICE_ID, W, BANKED
	andlw	0x70
	iorlw	0x04
	call	MIOS_MIDI_TxBufferPut

	;; add high address (the block number) to checksum and send it
	SET_BSR	MIDIO_BASE
	movf	MB_ADDRH, W
	addwf	MIDIO_SYSEX_CHECKSUM, F, BANKED
	call	MIOS_MIDI_TxBufferPut
	
	;; send 256 bytes
	clrf	MB_ADDRL
MIDIO_SYSEX_SendBlock_Loop
	clrwdt		; feed watchdog
	call	MIDIO_BANK_Read
	SET_BSR	MIDIO_BASE
	addwf	MIDIO_SYSEX_CHECKSUM, F, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MB_ADDRL, W
	bnz	MIDIO_SYSEX_SendBlock_Loop

	;; send checksum
	SET_BSR	MIDIO_BASE
	movf	MIDIO_SYSEX_CHECKSUM, W, BANKED
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send SysEx footer and exit
	movlw	0x01		; (independent from merger state)
	rgoto	MIDIO_SYSEX_Send_SysExFooter


;; --------------------------------------------------------------------------
;;  Sends all SysEx blocks of MIDIO128
;; --------------------------------------------------------------------------
MIDIO_SYSEX_SendAll
	;; 18 blocks, using MB_ADDRH as block counter
	clrf	MB_ADDRH
MIDIO_SYSEX_SendAllLoop
	;; send block
	rcall	MIDIO_SYSEX_SendBlock
	movlw	0x06
	cpfslt	MB_ADDRH, ACCESS
	rgoto MIDIO_SYSEX_SendAll_End

	;; workaround for merger issue: clear MYSYSEX flag, so that the
	;; next header will be sent regardless of merger state
	SET_BSR	MIDIO_BASE
	bcf	MIDIO_SYSEX_STATE, MIDIO_SYSEX_STATE_MYSYSEX, BANKED

	;; insert delay of 750 mS
	movlw	0x03
	movwf	TMP1
MIDIO_SYSEX_SendAllDlyLoop
	movlw	250
	call	MIOS_Delay
	decfsz	TMP1, F
	rgoto	MIDIO_SYSEX_SendAllDlyLoop
	rgoto	MIDIO_SYSEX_SendAllLoop

MIDIO_SYSEX_SendAll_End
	return
