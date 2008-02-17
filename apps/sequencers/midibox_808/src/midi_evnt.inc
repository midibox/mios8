; $Id$
;
; This file includes helpful functions to handle with common MIDI events
;
;    o MIDI_EVNT_Send:       sends a simple MIDI event
;    o MIDI_EVNT_Print:      prints a simple MIDI event
;    o MIDI_EVNT_PrintShort: prints the hex bytes of the MIDI event
;    o MIDI_EVNT_PrintNote:  prints a Note
;
; See the function headers for further details
;
; Following registers have to be located to free addresses in app_defines.h:
;
;MIDI_EVNT0		EQU	0x017
;MIDI_EVNT1		EQU	0x018
;MIDI_EVNT_VALUE	EQU	0x019
;
; ==========================================================================
; NOTE that this is a modified version for MIDIboxSEQ which handles IIC
; and AOUT interface as well!
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_CheckPort
;;  DESCRIPTION: checks if a given port is available
;;  IN: 
;;     o WREG == 0: Default port (depends in MIOS Interface Definition)
;;     o WREG == 1: Internal port
;;     o WREG == 2..5: IIC0..IIC3
;;     o WREG == 6: Loopback to Sequencer MIDI Engine
;;     o WREG == 7: AOUT
;;  OUT:
;;     o WREG == 0: port not available
;;     o WREG == 1: port available
;;     o WREG == 2: port enabled/always available (no check)
;; --------------------------------------------------------------------------
MIDI_EVNT_CheckPort
	;; check if selected interface is available
	andlw	0xff				; (fix ZERO flag)
	bz	MIDI_EVNT_CheckPort_Def		; special check for default port
	addlw	-1
	bz	MIDI_EVNT_CheckPort_Avail	; internal port always available
	addlw	-1
	bz	MIDI_EVNT_CheckPort_ChkIIC0	; check IIC0
	addlw	-1
	bz	MIDI_EVNT_CheckPort_ChkIIC1	; check IIC1
	addlw	-1
	bz	MIDI_EVNT_CheckPort_ChkIIC2	; check IIC2
	addlw	-1
	bz	MIDI_EVNT_CheckPort_ChkIIC3	; check IIC3
	rgoto	MIDI_EVNT_CheckPort_Enabled	; Loopback, AOUT (and undefined ports) are just handled as "enabled"...

MIDI_EVNT_CheckPort_Def
	;; branch depending on interface
	call	MIOS_MIDI_InterfaceGet
	xorlw	0x10
	bz	MIDI_EVNT_CheckPort_ChkIIC0
	xorlw	0x12 ^ 0x10
	bz	MIDI_EVNT_CheckPort_ChkIIC1
	xorlw	0x14 ^ 0x12
	bz	MIDI_EVNT_CheckPort_ChkIIC2
	xorlw	0x16 ^ 0x14
	bz	MIDI_EVNT_CheckPort_ChkIIC3
	rgoto	MIDI_EVNT_CheckPort_Avail

MIDI_EVNT_CheckPort_ChkIIC0
	movlw	0x01		; compare mask
	rgoto	MIDI_EVNT_CheckPort_ChkIIC_Cont
MIDI_EVNT_CheckPort_ChkIIC1
	movlw	0x02		; compare mask
	rgoto	MIDI_EVNT_CheckPort_ChkIIC_Cont
MIDI_EVNT_CheckPort_ChkIIC2
	movlw	0x04		; compare mask
	rgoto	MIDI_EVNT_CheckPort_ChkIIC_Cont
MIDI_EVNT_CheckPort_ChkIIC3
	movlw	0x08		; compare mask
	rgoto	MIDI_EVNT_CheckPort_ChkIIC_Cont
MIDI_EVNT_CheckPort_ChkIIC_Cont
	andwf	_iic_midi_available_slaves, W
	bz	MIDI_EVNT_CheckPort_NotAvail
	rgoto	MIDI_EVNT_CheckPort_Avail

MIDI_EVNT_CheckPort_Avail
	movlw	0x01
	rgoto	MIDI_EVNT_CheckPort_End

MIDI_EVNT_CheckPort_Enabled
	movlw	0x02
	rgoto	MIDI_EVNT_CheckPort_End

MIDI_EVNT_CheckPort_NotAvail
	movlw	0x00
	;; 	rgoto	MIDI_EVNT_CheckPort_End

MIDI_EVNT_CheckPort_End
	andlw	0xff		; fix ZERO flag
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_Send
;;  DESCRIPTION: sends a simple MIDI event
;;  IN: 
;;     o output port in MIDI_EVNT_PORT
;;     o first  MIDI event byte in MIDI_EVNT0
;;     o second MIDI event byte (if applicable) in MIDI_EVNT1
;;     o value in MIDI_EVNT_VALUE
;; --------------------------------------------------------------------------
MIDI_EVNT_Send
	;; branch depending on selected port
	movf	MIDI_EVNT_PORT, W
	bz	MIDI_EVNT_Send_Def		; special check for default port
	addlw	-1
	bz	MIDI_EVNT_Send_Int
	addlw	-1
	bz	MIDI_EVNT_Send_IIC0
	addlw	-1
	bz	MIDI_EVNT_Send_IIC1
	addlw	-1
	bz	MIDI_EVNT_Send_IIC2
	addlw	-1
	bz	MIDI_EVNT_Send_IIC3
	addlw	-1
	skpnz
	rgoto	MIDI_EVNT_Send_Loop
	addlw	-1
	skpnz
	rgoto	MIDI_EVNT_Send_AOUT

MIDI_EVNT_Send_Def
	;; branch depending on interface
	call	MIOS_MIDI_InterfaceGet
	xorlw	0x10
	bz	MIDI_EVNT_Send_IIC0
	xorlw	0x12 ^ 0x10
	bz	MIDI_EVNT_Send_IIC1
	xorlw	0x14 ^ 0x12
	bz	MIDI_EVNT_Send_IIC2
	xorlw	0x16 ^ 0x14
	bz	MIDI_EVNT_Send_IIC3
	;; 	rgoto	MIDI_EVNT_Send_Int

MIDI_EVNT_Send_Int
#if 1
	movlw	MIOS_MIDI_INTERFACE_COMMON
	call	MIOS_MIDI_InterfaceSet
#else
	movlw	MIOS_MIDI_INTERFACE_TO_HOST	; very excotic case, therefore no official "default" option in main.asm
	call	MIOS_MIDI_InterfaceSet
#endif

	call	MIOS_MIDI_BeginStream	; begin with stream

	;; branch depending on MIDI status
	swapf	MIDI_EVNT0, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE	; 8 entries
	rgoto	MIDI_EVNT_Send_Int_8x	; Note Off
	rgoto	MIDI_EVNT_Send_Int_9x	; Note On
	rgoto	MIDI_EVNT_Send_Int_Ax	; Aftertouch
	rgoto	MIDI_EVNT_Send_Int_Bx	; Controller
	rgoto	MIDI_EVNT_Send_Int_Cx	; Program Change
	rgoto	MIDI_EVNT_Send_Int_Dx	; Channel Pressure
	rgoto	MIDI_EVNT_Send_Int_Ex	; Pitch Bend
	rgoto	MIDI_EVNT_Send_Int_Fx	; System Exclusive


	;; sending three bytes:	
MIDI_EVNT_Send_Int_8x	; Note Off
MIDI_EVNT_Send_Int_9x	; Note On
MIDI_EVNT_Send_Int_Ax	; Aftertouch
MIDI_EVNT_Send_Int_Bx	; Controller
	movf	MIDI_EVNT0, W		; running status optimisation
	xorwf	MIDI_EVNT_PORT0_RS, W
	bz	MIDI_EVNT_Send_Int_89ABx_RS
MIDI_EVNT_Send_Int_89ABx_NoRS
	movf	MIDI_EVNT0, W
	movwf	MIDI_EVNT_PORT0_RS
	call	MIOS_MIDI_TxBufferPut
MIDI_EVNT_Send_Int_89ABx_RS
	movf	MIDI_EVNT1, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_Int_End

	;; sending two bytes
MIDI_EVNT_Send_Int_Cx	; Program Change
MIDI_EVNT_Send_Int_Dx	; Channel Pressure
	movf	MIDI_EVNT0, W		; running status optimisation
	xorwf	MIDI_EVNT_PORT0_RS, W
	bz	MIDI_EVNT_Send_Int_CDx_RS
MIDI_EVNT_Send_Int_CDx_NoRS
	movf	MIDI_EVNT0, W
	movwf	MIDI_EVNT_PORT0_RS
	call	MIOS_MIDI_TxBufferPut
MIDI_EVNT_Send_Int_CDx_RS
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_Int_End

	;; sending three bytes (7bit converted to 14bit)
MIDI_EVNT_Send_Int_Ex	; Pitch Bend
	movf	MIDI_EVNT0, W		; running status optimisation
	xorwf	MIDI_EVNT_PORT0_RS, W
	bz	MIDI_EVNT_Send_Int_Ex_RS
MIDI_EVNT_Send_Int_Ex_NoRS
	movf	MIDI_EVNT0, W
	movwf	MIDI_EVNT_PORT0_RS
	call	MIOS_MIDI_TxBufferPut
MIDI_EVNT_Send_Int_Ex_RS

	movlw	0x00
	BRA_IFCLR MIDI_EVNT_VALUE, 6, ACCESS, MIDI_EVNT_Send_Int_ExLow
	rlf	MIDI_EVNT_VALUE, W
	andlw	0x7e
MIDI_EVNT_Send_Int_ExLow
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_Int_End

MIDI_EVNT_Send_Int_Fx
	;; only send Events >= 0xf8 (MIDI Clock, etc...)
	BRA_IFCLR MIDI_EVNT0, 3, ACCESS, MIDI_EVNT_Send_Int_End
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut

MIDI_EVNT_Send_Int_End
	call	MIOS_MIDI_EndStream		; stream finished
 	goto	MIOS_MIDI_InterfaceAutoSet	; set default interface

	;; ---------------------------------------------------------------------
	;; using IIC handler
	;; ---------------------------------------------------------------------
MIDI_EVNT_Send_IIC0
	movlw	0x00
	rgoto	MIDI_EVNT_Send_IIC_Cont
MIDI_EVNT_Send_IIC1
	movlw	0x01
	rgoto	MIDI_EVNT_Send_IIC_Cont
MIDI_EVNT_Send_IIC2
	movlw	0x02
	rgoto	MIDI_EVNT_Send_IIC_Cont
MIDI_EVNT_Send_IIC3
	movlw	0x03
	rgoto	MIDI_EVNT_Send_IIC_Cont
MIDI_EVNT_Send_IIC_Cont
	movwf	_iic_midi_slave

	;; send package
	swapf	MIDI_EVNT0, W
	andlw	0x0f
	movwf	_iic_midi_tx_package+0
	movff	MIDI_EVNT0, _iic_midi_tx_package+1
	movff	MIDI_EVNT1, _iic_midi_tx_package+2
	movff	MIDI_EVNT_VALUE, _iic_midi_tx_package+3
	goto	IIC_MIDI_Transmit
	
	;; ---------------------------------------------------------------------
	;; using AOUT interface
	;; ---------------------------------------------------------------------
MIDI_EVNT_Send_AOUT
	;; TODO
	return


	;; ---------------------------------------------------------------------
	;; Loopback
	;; ---------------------------------------------------------------------
MIDI_EVNT_Send_Loop
	SET_BSR	SEQ_BASE

	;; store variables which could be changed by the MIDI handlers
	movff	FSR0L, SEQ_LOOPBACK_STORED_FSR0L
	movff	FSR0H, SEQ_LOOPBACK_STORED_FSR0H
	movff	FSR2L, SEQ_LOOPBACK_STORED_FSR2L
	movff	FSR2H, SEQ_LOOPBACK_STORED_FSR2H
	movff	SEQ_EVNTT, SEQ_LOOPBACK_STORED_EVNTT

	;; transfer MIDI event to MIOS_PARAMETER[123]
	movff	MIDI_EVNT0, MIOS_PARAMETER1
	movff	MIDI_EVNT1, MIOS_PARAMETER2
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER3

	;; call MIDI handler depending on MIDI event type
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0x90
	bnz	MIDI_EVNT_Send_Loop_NoNote
MIDI_EVNT_Send_Loop_Note
	movf	MIDI_EVNT_VALUE, W
	bz	MIDI_EVNT_Send_Loop_NoteOff
MIDI_EVNT_Send_Loop_NoteOn
	call	SEQ_MIDI_NoteOn_FromLoopback
	rgoto	MIDI_EVNT_Send_Loop_Cont
MIDI_EVNT_Send_Loop_NoteOff
	call	SEQ_MIDI_NoteOff_FromLoopback
	rgoto	MIDI_EVNT_Send_Loop_Cont
MIDI_EVNT_Send_Loop_NoNote	

	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xb0
	bnz	MIDI_EVNT_Send_Loop_NoCC
MIDI_EVNT_Send_Loop_CC
	call	SEQ_MIDI_CC_FromLoopback
	rgoto	MIDI_EVNT_Send_Loop_Cont
MIDI_EVNT_Send_Loop_NoCC

MIDI_EVNT_Send_Loop_Cont
	;; restore variables which have been changed by the MIDI handlers
	movff	SEQ_LOOPBACK_STORED_FSR0L, FSR0L
	movff	SEQ_LOOPBACK_STORED_FSR0H, FSR0H
	movff	SEQ_LOOPBACK_STORED_FSR2L, FSR2L
	movff	SEQ_LOOPBACK_STORED_FSR2H, FSR2H
	movff	SEQ_LOOPBACK_STORED_EVNTT, SEQ_EVNTT
	return
