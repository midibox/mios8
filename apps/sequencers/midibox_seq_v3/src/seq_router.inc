; $Id$
;
; MIDIboxSEQ
; MIDI Router
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; available target ports
;; 0: Int.
;; 1: IIC1
;; 2: IIC2
;; 3: IIC3
;; 4: IIC4
;; 5: AOUT
;; 6: reserved
;; 7: reserved

SEQ_ROUTER_MAP_TABLE
	;; maps these target ports to the port numbers used in "midi_evnt.inc"
	db	2, 3, 4, 5, 7, 1, 0, 0

;; number of available output ports
SEQ_ROUTER_AVAILABLE_PORTS	EQU	6

;; --------------------------------------------------------------------------
;;  Prints the router input name
;;  IN: router input in WREG
;;  OUT: 4 characters
;;  USES: BSR, FSR1, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_ROUTER_PrnIn
	TABLE_ADDR_MUL_W SEQ_ROUTER_PrnIn_TABLE, 4

	;; print 4 characters
	movlw	4
	goto	MIOS_LCD_PrintPreconfString

SEQ_ROUTER_PrnIn_TABLE
	db	"Int.", "IIC1"

;; --------------------------------------------------------------------------
;;  Prints the router output name
;;  IN: router output in WREG
;;  OUT: 4 characters
;;  USES: BSR, FSR1, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_ROUTER_PrnOut
	TABLE_ADDR_MUL_W SEQ_ROUTER_PrnOut_TABLE, 4

	;; print 4 characters
	movlw	4
	goto	MIOS_LCD_PrintPreconfString

SEQ_ROUTER_PrnOut_TABLE
	db	"IIC1", "IIC2", "IIC3", "IIC4", "AOUT", "Int.", "Res", "Res"


;; --------------------------------------------------------------------------
;;  Prints the router source channel
;;  IN: router output in WREG[3:0], router input in WREG[7:4]
;;  OUT: 3 characters
;;  USES: BSR, FSR1, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_ROUTER_PrnSrcChn
	lfsr	FSR1, ROUTER_IN0_SRC0_CHN
	btfsc	WREG, 4
	lfsr	FSR1, ROUTER_IN1_SRC0_CHN
	andlw	0x0f
	addwf	FSR1L, F
	;; 	rgoto	SEQ_ROUTER_PrnSrcDst_Cont

SEQ_ROUTER_PrnDst_Cont
	;; 0: print "---"
	;; 1..16: print #<chn>
	;; 17: print "All"
	movf	INDF1, W
	bz	SEQ_ROUTER_PrnSrcDst_Dis
	addlw	-17
	bz	SEQ_ROUTER_PrnSrcDst_All
SEQ_ROUTER_PrnSrcDst_Single
	movff	INDF1, TABLAT

	movlw	'#'
	call	MIOS_LCD_PrintChar
	movf	TABLAT, W
	goto	MIOS_LCD_PrintBCD2

SEQ_ROUTER_PrnSrcDst_All
	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	'l'
	call	MIOS_LCD_PrintChar
	movlw	'l'
	goto	MIOS_LCD_PrintChar

SEQ_ROUTER_PrnSrcDst_Dis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  Prints the router destination channel
;;  IN: router output in WREG[3:0], router input in WREG[7:4]
;;  OUT: 3 characters
;;  USES: BSR, FSR1, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_ROUTER_PrnDstChn
	lfsr	FSR1, ROUTER_IN0_DST0_CHN
	btfsc	WREG, 4
	lfsr	FSR1, ROUTER_IN1_DST0_CHN
	andlw	0x0f
	addwf	FSR1L, F
	rgoto	SEQ_ROUTER_PrnDst_Cont


;; --------------------------------------------------------------------------
;;  This function is called from USER_Tick to periodically check if a new
;;  MIDI event has been received on the IIC1 port
;;  The event will be forwarded to the router and to the sequencer MIDI handler
;; --------------------------------------------------------------------------
SEQ_ROUTER_CheckIIC1
	;; check if a new event has been received via IIC1
	;; this is done via port polling
#if DEFAULT_IIC1_RI_N_PORT
	BRA_IFSET DEFAULT_IIC1_RI_N_PORT, DEFAULT_IIC1_RI_N_PIN, ACCESS, SEQ_ROUTER_CheckIIC1_End

	clrf	IIC_MIDI_SLAVE	; (select slave - always 0, we only use one MIDI input to save performance)
	call	IIC_MIDI_Receive
	bz	SEQ_ROUTER_CheckIIC1_End

	;; only forward normal MIDI events of type 8-e (no system events)
	movf	IIC_MIDI_RX_PACKAGE+0, W
	andlw	0x0f
	xorlw	0x0f
	bz	SEQ_ROUTER_CheckIIC1_End
	xorlw	0x0f
	andlw	0x08
	bz	SEQ_ROUTER_CheckIIC1_End

	;; copy event to MIOS_PARAMETER[123]
	movff	IIC_MIDI_RX_PACKAGE+1, MIOS_PARAMETER1
	movff	IIC_MIDI_RX_PACKAGE+2, MIOS_PARAMETER2
	movff	IIC_MIDI_RX_PACKAGE+3, MIOS_PARAMETER3

	;; forward via router
	movlw	1		; (IIC1 port)
	rcall	SEQ_ROUTER_In

	;; continue at sequencer MIDI handler
	call	SEQ_MIDI_NotifyReceivedEvent

SEQ_ROUTER_CheckIIC1_End
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is called from USER_MPROC_NotifyReceivedEvent when a
;;  new MIDI event has been received
;;  Input:
;;     o MIDI Input number in WREG (0=Internal or 1=IIC1)
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;;  Note: it must be ensured, that these variables won't be touched, other-
;;  wise other MIDI event handlers of the application could fail!
;; --------------------------------------------------------------------------
SEQ_ROUTER_In
	movff	MIOS_PARAMETER1, SEQ_DATA_BUFFER+0	; we have to ensure that MIOS_PARAMETER[123] won't be overwritten!
	movff	MIOS_PARAMETER2, SEQ_DATA_BUFFER+1	; therefore save it in SEQ_DATA_BUFFER (dirty!!!) 
	movff	MIOS_PARAMETER3, SEQ_DATA_BUFFER+2	; sometimes I whished I would have good stack method...

	movff	WREG, SEQ_DATA_BUFFER+3			; store MIDI Input number in SEQ_DATA_BUFFER+3

	;; in record mode: forward event to CS_M_RECORD
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, SEQ_ROUTER_In_NoRec
SEQ_ROUTER_In_Rec
	call	CS_M_RECORD_NotifyEvent			; call record hook
	movff	SEQ_DATA_BUFFER+0, MIOS_PARAMETER1	; restore MIOS_PARAMETER[123]
	movff	SEQ_DATA_BUFFER+1, MIOS_PARAMETER2
	movff	SEQ_DATA_BUFFER+2, MIOS_PARAMETER3
SEQ_ROUTER_In_NoRec

	;; go through all available output ports
	SET_BSR	SEQ_BASE
	clrf	SEQ_ROUTER_PORT, BANKED
SEQ_ROUTER_In_Loop
	;; select router source/destination items
	movff	SEQ_DATA_BUFFER+3, PRODL		; MIDI Input number stored in SEQ_DATA_BUFFER+3 -> PRODL
	movf	SEQ_ROUTER_PORT, W, BANKED
	lfsr	FSR0, ROUTER_IN0_SRC0_CHN
	btfsc	PRODL, 0
	lfsr	FSR0, ROUTER_IN1_SRC0_CHN

	addwf	FSR0L, F
	lfsr	FSR2, ROUTER_IN0_DST0_CHN
	btfsc	PRODL, 0
	lfsr	FSR2, ROUTER_IN1_DST0_CHN
	addwf	FSR2L, F

	;; check if forwarding enabled
	movf	INDF0, W
	bz	SEQ_ROUTER_In_Loop_Next
	movf	INDF2, W
	bz	SEQ_ROUTER_In_Loop_Next

	;; check MIDI channel
	movf	INDF0, W
	addlw	-17
	bz	SEQ_ROUTER_In_Loop_SrcOk

	decf	INDF0, W
	xorwf	MIOS_PARAMETER1, W
	andlw	0x0f
	bnz	SEQ_ROUTER_In_Loop_Next
SEQ_ROUTER_In_Loop_SrcOk

	;; forward to destination channel
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	MIDI_EVNT0

	movf	INDF2, W
	addlw	-17
	bz	SEQ_ROUTER_In_Loop_DstAll
SEQ_ROUTER_In_Loop_DstFix
	;; select fixed channel
	decf	INDF2, W
	rgoto	SEQ_ROUTER_In_Loop_DstCont

SEQ_ROUTER_In_Loop_DstAll
	;; use original channel
	movf	MIOS_PARAMETER1, W
	;; 	rgoto	SEQ_ROUTER_In_Loop_DstCont
SEQ_ROUTER_In_Loop_DstCont
	andlw	0x0f
	iorwf	MIDI_EVNT0, F

	;; second and third byte are the same
	movff	MIOS_PARAMETER2, MIDI_EVNT1
	movff	MIOS_PARAMETER3, MIDI_EVNT_VALUE

	;; send event
	rcall	SEQ_ROUTER_SendPort

SEQ_ROUTER_In_Loop_Next
	SET_BSR	SEQ_BASE
	incf	SEQ_ROUTER_PORT, F, BANKED
	movlw	SEQ_ROUTER_AVAILABLE_PORTS - 1
	cpfsgt	SEQ_ROUTER_PORT, BANKED
	rgoto SEQ_ROUTER_In_Loop


	movff	SEQ_DATA_BUFFER+0, MIOS_PARAMETER1	; restore MIOS_PARAMETER[123]
	movff	SEQ_DATA_BUFFER+1, MIOS_PARAMETER2
	movff	SEQ_DATA_BUFFER+2, MIOS_PARAMETER3

	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This function sends a MIDI package to the given router port
;;  IN: port in SEQ_ROUTER_PORT
;;      MIDI package in SEQ_EVNT[01] and _VALUE
;; --------------------------------------------------------------------------
SEQ_ROUTER_SendPort
	;; select port
	SET_BSR	SEQ_BASE
	movf	SEQ_ROUTER_PORT, W, BANKED
	addlw	LOW(SEQ_ROUTER_MAP_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_ROUTER_MAP_TABLE)
	addwfc	TBLPTRH, F
	tblrd*+
	movff	TABLAT, MIDI_EVNT_PORT

	;; send event
	goto	MIDI_EVNT_Send


;; --------------------------------------------------------------------------
;;  This function sends a MIDI clock sync (0xf8, 0xfa, 0xfb, 0xfc) to all
;;  enabled Router Outputs
;; --------------------------------------------------------------------------
SEQ_ROUTER_SendSync
	;; store MIDI clock event
	movwf	MIDI_EVNT0

	;; go through all available output ports
	SET_BSR	SEQ_BASE
	clrf	SEQ_ROUTER_PORT, BANKED
SEQ_ROUTER_SendSync_Loop
	;; check if MIDI clock is enabled
	movf	SEQ_ROUTER_PORT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_ROUTER_MCLK_ENABLED, W, BANKED
	bz	SEQ_ROUTER_SendSync_Loop_Next

	;; forward MIDI_EVNT0 to port
	rcall	SEQ_ROUTER_SendPort

SEQ_ROUTER_SendSync_Loop_Next
	SET_BSR	SEQ_BASE
	incf	SEQ_ROUTER_PORT, F, BANKED
	movlw	SEQ_ROUTER_AVAILABLE_PORTS - 1
	cpfsgt	SEQ_ROUTER_PORT, BANKED
	rgoto SEQ_ROUTER_SendSync_Loop

	return
