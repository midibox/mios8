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
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut
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
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut
	movf	MIDI_EVNT_VALUE, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIDI_EVNT_Send_Int_End

	;; sending three bytes (7bit converted to 14bit)
MIDI_EVNT_Send_Int_Ex	; Pitch Bend
	movf	MIDI_EVNT0, W
	call	MIOS_MIDI_TxBufferPut

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
	;; Branch depending on CC/Note
	movf	MIDI_EVNT0, W
	andlw	0xf0
	xorlw	0xb0
	bz	MIDI_EVNT_Send_AOUT_CC
	xorlw	0x80 ^ 0xb0
	bz	MIDI_EVNT_Send_AOUT_NoteOff
	xorlw	0x90 ^ 0x80
	bz	MIDI_EVNT_Send_AOUT_Note
	rgoto	MIDI_EVNT_Send_AOUT_End
MIDI_EVNT_Send_AOUT_NoteOff
	clrf	MIDI_EVNT_VALUE	; ensure that velocity zero
MIDI_EVNT_Send_AOUT_Note
	;; if channel 1..8: set only note value on CV OUT #1..8, triggers Gate #1..8
	;; if channel 9..12: set note/velocity on each pin pair (CV OUT #1/2, #3/4, #5/6, #7/8), triggers Gate #1+2, #3+4, #5+6, #7+8
	;; if channel 13..15: set velocity/note on each pin pair (CV OUT #1/2, #3/4, #5/6), triggers Gate #1+2, #3+4, #5+6
	;; if channel 16: trigger the extension pins (DOUT)
	movf	MIDI_EVNT0, W
	andlw	0x0f
	xorlw	0x0f
	bz	MIDI_EVNT_Send_AOUT_ExtGate

	BRA_IFSET MIDI_EVNT0, 3, ACCESS, MIDI_EVNT_Send_AOUT_Note_NV
MIDI_EVNT_Send_AOUT_Note_N
	;; set Note CV if velocity > 0
	movf	MIDI_EVNT_VALUE, W
	bz	MIDI_EVNT_Send_AOUT_Note_NOff
MIDI_EVNT_Send_AOUT_Note_NOn
	movff	MIDI_EVNT1, MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x07
	call	AOUT_Pin7bitSet
MIDI_EVNT_Send_AOUT_Note_NOff

	;; set gate if velocity > 0
	movf	MIDI_EVNT_VALUE, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x07
	rcall	MIDI_EVNT_AOUT_GatePinSet

	rgoto	MIDI_EVNT_Send_AOUT_End
MIDI_EVNT_Send_AOUT_Note_NV
	BRA_IFSET MIDI_EVNT0, 2, ACCESS, MIDI_EVNT_Send_AOUT_Note_VN

	;; don't update Note and Velocity if Velocity is 0
	movf	MIDI_EVNT_VALUE, W
	bz	MIDI_EVNT_Send_AOUT_Note_NV_V0
MIDI_EVNT_Send_AOUT_Note_NV_V1
	;; Note
	movff	MIDI_EVNT1, MIOS_PARAMETER1
	rlf	MIDI_EVNT0, W
	andlw	0x06
	call	AOUT_Pin7bitSet

	;; Velocity
	movwf	MIOS_PARAMETER1
	rlf	MIDI_EVNT0, W
	andlw	0x06
	addlw	1
	call	AOUT_Pin7bitSet
MIDI_EVNT_Send_AOUT_Note_NV_V0

MIDI_EVNT_Send_AOUT_Note_VN_Gate
	;; set gates if velocity > 0
	movf	MIDI_EVNT_VALUE, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1

	rlf	MIDI_EVNT0, W
	andlw	0x06
	rcall	MIDI_EVNT_AOUT_GatePinSet

	rlf	MIDI_EVNT0, W
	andlw	0x06
	addlw	1
	rcall	MIDI_EVNT_AOUT_GatePinSet

	rgoto	MIDI_EVNT_Send_AOUT_End

MIDI_EVNT_Send_AOUT_Note_VN
	;; don't update Note and Velocity if Velocity is 0
	movf	MIDI_EVNT_VALUE, W
	bz	MIDI_EVNT_Send_AOUT_Note_VN_V0
MIDI_EVNT_Send_AOUT_Note_VN_V1
	;; Velocity
	movwf	MIOS_PARAMETER1
	rlf	MIDI_EVNT0, W
	andlw	0x06
	call	AOUT_Pin7bitSet

	;; Note
	movff	MIDI_EVNT1, MIOS_PARAMETER1
	rlf	MIDI_EVNT0, W
	andlw	0x06
	addlw	1
	call	AOUT_Pin7bitSet

MIDI_EVNT_Send_AOUT_Note_VN_V0
	;; set/clear gates
	rgoto	MIDI_EVNT_Send_AOUT_Note_VN_Gate

	;; if channel 1..8: sets CV Out #1..8 depending on CC number (16 = #1, 17 = #2, ...) - channel has no effect
	;; if channel 9..16: sets CV Out #1..8 depending on channel
	;; gate is always set
MIDI_EVNT_Send_AOUT_CC
	BRA_IFCLR MIDI_EVNT0, 3, ACCESS, MIDI_EVNT_Send_AOUT_CC_N
MIDI_EVNT_Send_AOUT_CC_C
	;; derive CV Out Number from Channel Number
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x07
	call	AOUT_Pin7bitSet

	;; gate always set
	setf	MIOS_PARAMETER1
	movf	MIDI_EVNT0, W
	andlw	0x07
	rcall	MIDI_EVNT_AOUT_GatePinSet
 	rgoto	MIDI_EVNT_Send_AOUT_End

MIDI_EVNT_Send_AOUT_CC_N
	;; derive CV Out Number from CC Number
	movff	MIDI_EVNT_VALUE, MIOS_PARAMETER1
	movf	MIDI_EVNT1, W
	andlw	0x07
	call	AOUT_Pin7bitSet

	;; gate always set
	setf	MIOS_PARAMETER1
	movf	MIDI_EVNT1, W
	andlw	0x07
	rcall	MIDI_EVNT_AOUT_GatePinSet
	rgoto	MIDI_EVNT_Send_AOUT_End

MIDI_EVNT_Send_AOUT_ExtGate
	;; exit if note number < 0x24 (C-1)
	movlw	0x24-1
	cpfsgt	MIDI_EVNT1, ACCESS
	rgoto MIDI_EVNT_Send_AOUT_ExtGate_Skip
	;; exit if note number >= 0x54 (0x24 + 0x30)
	movlw	0x54
	cpfslt	MIDI_EVNT1, ACCESS
	rgoto MIDI_EVNT_Send_AOUT_ExtGate_Skip
	;; set/clear trigger depending on velocity value
	movf	MIDI_EVNT_VALUE, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1
	movf	MIDI_EVNT1, W
	addlw	-0x24
	rcall	MIDI_EVNT_EXT_GatePinSet

MIDI_EVNT_Send_AOUT_ExtGate_Skip
	rgoto	MIDI_EVNT_Send_AOUT_End

MIDI_EVNT_Send_AOUT_End
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


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_AOUT_GatePinSet
;;  DESCRIPTION: Help Function which sets a Gate Pin independent from the interface
;;  IN: 
;;     o pin number in WREG
;;     o value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MIDI_EVNT_AOUT_GatePinSet
	SET_BSR	GATES
	movf	MIOS_PARAMETER1, F
	bz	MIDI_EVNT_AOUT_PinSet0
MIDI_EVNT_AOUT_PinSet1
#if 1
	;; extension for legato handling
	;; if gate was already set, save this state in GATES_LEGATO
	movwf	PRODL
	call	MIOS_HLP_GetBitANDMask
	andwf	GATES, W, BANKED
	iorwf	GATES_LEGATO, F, BANKED
	movf	PRODL, W
#endif
	call	MIOS_HLP_GetBitORMask
	iorwf	GATES, F, BANKED
	return

MIDI_EVNT_AOUT_PinSet0
	call	MIOS_HLP_GetBitANDMask
#if 1
	;; extension for legato handling
	;; if GATE_LEGATO flag is set, clear this flag and exit
	;; otherwise clear the flag in GATES (so that the pin is set to 0)
	movwf	PRODL

	andwf	GATES_LEGATO, W, BANKED
	bnz	MIDI_EVNT_AOUT_PinSet0_Legato
MIDI_EVNT_AOUT_PinSet0_NoLegato
	movf	PRODL, W
	andwf	GATES, F, BANKED
	return

MIDI_EVNT_AOUT_PinSet0_Legato
	movf	PRODL, W
	andwf	GATES_LEGATO, F, BANKED
	return
#else	
	andwf	GATES, F, BANKED
	return
#endif


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_EVNT_DOUT_GatePinSet
;;  DESCRIPTION: Help Function which sets a Gate Pin at the DOUT register chain
;;  IN: 
;;     o pin number in WREG
;;     o value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MIDI_EVNT_EXT_GATE_PINS
	;; for easier mapping, we put the #define values into a table
	db	DEFAULT_ENABLE_DOUT_GATE_09_16, DEFAULT_ENABLE_DOUT_GATE_17_24
	db	DEFAULT_ENABLE_DOUT_GATE_25_31, DEFAULT_ENABLE_DOUT_GATE_32_39
	db	DEFAULT_ENABLE_DOUT_GATE_40_47, DEFAULT_ENABLE_DOUT_GATE_48_55

MIDI_EVNT_EXT_GatePinSet
	movwf	PRODL		; temporary store pin number in PRODL

	;; get SR number
	rrf	WREG, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f		; (ensure that no invalid DOUT will be selected)
	addlw	LOW(MIDI_EVNT_EXT_GATE_PINS)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(MIDI_EVNT_EXT_GATE_PINS)
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	bz	MIDI_EVNT_EXT_GatePinSet_End	; exit if SR is 0
	decf	TABLAT, F	; -1 - we counting from zero (0, 1, 2, ... 15)

	;; determine pin number: SR*8 + PRODL
	rlf	TABLAT, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x78
	movwf	TABLAT

	movf	PRODL, W
	andlw	0x07
	iorwf	TABLAT, W

	;; now: pin number in WREG, value in MIOS_PARAMETER1
	call	MIOS_DOUT_PinSet
	
MIDI_EVNT_EXT_GatePinSet_End
	return

