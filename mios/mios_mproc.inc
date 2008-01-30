; $Id$
;
; MIDI Processor of MIOS
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MIOS_MPROC_SYXSTATE_SKIP_MERGER		EQU	7
MIOS_MPROC_SYXSTATE_SKIP_CLRREQ		EQU	6
MIOS_MPROC_SYXSTATE_MYSYSEX		EQU	5
MIOS_MPROC_SYXSTATE_ACTION		EQU	4
MIOS_MPROC_SYXSTATE_RESET		EQU	3

MIOS_MIDI_MPROC_READREQ		EQU	0x01
MIOS_MIDI_MPROC_WRITEREQ	EQU	0x02
MIOS_MIDI_MPROC_SELBS		EQU	0x03
MIOS_MIDI_MPROC_LCD		EQU	0x08
MIOS_MIDI_MPROC_DEBUG		EQU	0x0d
MIOS_MIDI_MPROC_DISACK		EQU	0x0e
MIOS_MIDI_MPROC_ACK		EQU	0x0f

MIOS_DISACK_LESS_BYTES_THAN_EXP EQU     0x01
MIOS_DISACK_MORE_BYTES_THAN_EXP EQU     0x02
MIOS_DISACK_WRONG_CHECKSUM      EQU     0x03
MIOS_DISACK_WRITE_FAILED        EQU     0x04
MIOS_DISACK_WRITE_ACCESS        EQU     0x05
MIOS_DISACK_MIDI_TIMEOUT        EQU     0x06
MIOS_DISACK_WRONG_DEBUG_CMD	EQU	0x07
MIOS_DISACK_WRONG_ADDR_RANGE	EQU	0x08
MIOS_DISACK_ADDR_NOT_ALIGNED    EQU	0x09
MIOS_DISACK_BS_NOT_AVAILABLE	EQU	0x0a
MIOS_DISACK_OVERRUN		EQU	0x0b
MIOS_DISACK_FRAME_ERROR		EQU	0x0c

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MPROC_NotifyReceivedEvent
;;  C_DECLARATION: void MPROC_NotifyReceivedEvent(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2)
;;  DESCRIPTION: This function is called by MIOS when a complete MIDI event
;;  has been received
;;  IN: first MIDI event byte in MIOS_PARAMETER1
;;      second MIDI event byte in MIOS_PARAMETER2
;;      third MIDI event byte in MIOS_PARAMETER3
;;  C_IN: first MIDI event byte in <evnt0>
;;      second MIDI event byte in <evnt1>
;;      third MIDI event byte in <evnt2>
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MPROC_NotifyFoundEvent
;;  C_DECLARATION: void MPROC_NotifyFoundEvent(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2)
;;  DESCRIPTION: This function is called by MIOS when a MIDI event has
;;  been received which has been specified in the CONFIG_MIDI_IN table
;;  IN: number of entry in WREG
;;      first MIDI event byte in MIOS_PARAMETER1
;;      second MIDI event byte in MIOS_PARAMETER2
;;      third MIDI event byte in MIOS_PARAMETER3
;;  C_IN: first MIDI event byte in <evnt0>
;;      second MIDI event byte in <evnt1>
;;      third MIDI event byte in <evnt2>
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MPROC_NotifyTimeout
;;  C_DECLARATION: void MPROC_NotifyTimeout(void)
;;  DESCRIPTION: This function is called by MIOS when a MIDI event has
;;  not been completly received within 2 seconds
;;  IN: -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MPROC_NotifyReceivedByte
;;  C_DECLARATION: void MPROC_NotifyReceivedByte(unsigned char byte)
;;  DESCRIPTION: This function is called by MIOS when a MIDI byte has been
;;  received
;;  IN:  received MIDI byte in WREG and MIOS_PARAMETER1
;;  C_IN:  received MIDI byte in <byte>
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MPROC_DebugTrigger
;;  C_DECLARATION: not available in C - code has to be added to mios_wrapper
;;  DESCRIPTION: This function is called by MIOS when a debug command has
;;  been received via SysEx
;;  IN:  WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3 like
;;       specified in the debug command
;;  OUT: return values WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MPROC_MergerEnable
;;  C_DECLARATION: void MIOS_MPROC_MergerEnable(void)
;;  DESCRIPTION: enables MIDI merger processing like specified with MIOS_MIDI_MergerSet
;;  IN:	  -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MPROC_MergerEnable
	SET_BSR	MIOS_MPROC_SYXSTATE
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_CLRREQ, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MPROC_MergerDisable
;;  C_DECLARATION: void MIOS_MPROC_MergerDisable(void)
;;  DESCRIPTION: this function allows you to temporary disable the MIDI merger
;;  processing during receiving a new event. It's used by SysEx parsers to 
;;  prevent the forwarding of SysEx strings, but can also used by the
;;  USER_MPROC_NotifyReceivedByte hook to filter other events
;;  The merger will be enabled again after a complete MIDI event has been
;;  received!
;;  IN:	  -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT: -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MPROC_MergerDisable
	SET_BSR	MIOS_MPROC_SYXSTATE
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_MERGER, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MPROC_MergerGet
;;  C_DECLARATION: unsigned char MIOS_MPROC_MergerGet(void)
;;  DESCRIPTION: returns 1 if merger processing is enabled, 0 if disabled
;;  IN:  -
;;  C_IN:  -
;;  OUT: status in WREG and MIOS_PARAMETER1
;;  C_OUT: status
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MPROC_MergerGet
	SET_BSR	MIOS_MPROC_SYXSTATE
	movlw	0x01
	btfsc	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_MERGER, BANKED
	movlw	0x00
	movwf	MIOS_PARAMETER1
	andlw	0xff		; update status
	return

;; --------------------------------------------------------------------------
;;  Handler: Called in the mainloop to check if new MIDI data has been
;;  received which needs to be processed
;; --------------------------------------------------------------------------
MIOS_MPROC_Handler
	SET_BSR MIOS_MPROC_TIMEOUT_L
	clrf	MIOS_MPROC_TIMEOUT_L, BANKED
	clrf	MIOS_MPROC_TIMEOUT_H, BANKED
	movlw	0xf6
	movwf	MIOS_MPROC_TIMEOUT_U, BANKED

_MIOS_MPROC_Handler
	clrwdt			; clear watchdog timer

	;; check for MIDI overrun
	BRA_IFSET RCSTA, OERR, ACCESS, MIOS_MPROC_Overrun

	;; check for frame error
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_FERR, ACCESS, MIOS_MPROC_FrameError

	SET_BSR MIOS_MPROC_TIMEOUT_L
	incf	MIOS_MPROC_TIMEOUT_L, F, BANKED
	skpnz
	incf	MIOS_MPROC_TIMEOUT_H, F, BANKED
	skpnz
	incf	MIOS_MPROC_TIMEOUT_U, F, BANKED

	;; check for MIDI timeout
	movf	MIOS_MPROC_TIMEOUT_L, W, BANKED
	iorwf	MIOS_MPROC_TIMEOUT_H, W, BANKED
	iorwf	MIOS_MPROC_TIMEOUT_U, W, BANKED
	bz	MIOS_MPROC_TimeOut	;; Time Out !!!

MIOS_MPROC_Handler_NoTimeOut
	rcall	MIOS_MIDI_RxBufferUsed
	bnz	MIOS_MPROC_ReceiveHandler

	SET_BSR	MIOS_MPROC_STATE
	movf	MIOS_MPROC_STATE, W, BANKED	; loop until complete MIDI event has been received
	iorwf	MIOS_MIDI_EXPBYTES, W, BANKED
	bnz	_MIOS_MPROC_Handler
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_MYSYSEX, BANKED, _MIOS_MPROC_Handler
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED, _MIOS_MPROC_Handler

	return	; no new value and not in Halt mode


	;; ---

MIOS_MPROC_ReceiveHandler
	rcall	MIOS_MIDI_RxBufferGet
	SET_BSR	MIOS_MPROC_IN
	movwf	MIOS_MPROC_IN, BANKED

	;; ignore realtime messages during parsing (trace and sysex)
	movlw	0xf8
	cpfslt	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_ReceiveRTSkip

	;; dont trace MIDI event if sysex stream is received
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_MYSYSEX, BANKED, MIOS_MPROC_BranchEvent_Return
	;; or reset has been requested
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED, MIOS_MPROC_BranchEvent_Return

	;; trace MIDI event
	BRA_IFSET MIOS_MPROC_STATE, 1, BANKED, MIOS_MPROC_BranchEvent2 ; MIDI event byte #3
	BRA_IFSET MIOS_MPROC_STATE, 0, BANKED, MIOS_MPROC_BranchEvent1 ; MIDI event byte #2
	rgoto	MIOS_MPROC_BranchEvent0	; MIDI event byte #1
MIOS_MPROC_BranchEvent_Return

MIOS_MPROC_SysExCheck
	;; check sysex state
	SET_BSR	MIOS_MPROC_SYXSTATE
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_MYSYSEX, BANKED, MIOS_MPROC_Handler_SyxEx

	movf	MIOS_MPROC_SYXSTATE, W, BANKED
	rcall	MIOS_MPROC_SysExHeaderGet
	cpfseq	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_SysExCheckFailed
	incf	MIOS_MPROC_SYXSTATE, F, BANKED
	movf	MIOS_MPROC_SYXSTATE, W, BANKED
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7e 40 <device number>)
	bnz	MIOS_MPROC_SysExCheckOk

	;; SysEx ID received, lets go
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_MYSYSEX, BANKED
	movlw	0xf8
	andwf	MIOS_MPROC_SYXSTATE, F, BANKED
	rgoto	MIOS_MPROC_Handler_SkipMerger

MIOS_MPROC_SysExCheckFailed
	rcall	MIOS_MPROC_HlpResetSyxState
MIOS_MPROC_SysExCheckOk
MIOS_MPROC_ReceiveRTSkip

	;; call user sysex handler
	movf	MIOS_MPROC_IN, W, BANKED
	movwf	MIOS_PARAMETER1
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_MPROC_NotifyReceivedByte
	SET_BSR	MIOS_MPROC_IN

	;; skip merger?
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_MERGER, BANKED, MIOS_MPROC_Handler_SkipMerger

	SET_BSR	MIOS_MPROC_IN
	GOTO_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_MBLINK, ACCESS, MIOS_MPROC_NoMBLMerger
MIOS_MPROC_MBLMerger
	GOTO_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER, ACCESS, MIOS_MPROC_MBLMergeIt
MIOS_MPROC_MBLMerger_1
	movlw	MIOS_MIDI_MBLINK_START
	cpfseq	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_MBLMerger_2
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_MBLINK_TUNNEL_PASS
	rgoto	MIOS_MPROC_NoMerger
MIOS_MPROC_MBLMerger_2
	movlw	MIOS_MIDI_MBLINK_STOP
	cpfseq	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_MBLMerger_3
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_MBLINK_TUNNEL_PASS
	rgoto	MIOS_MPROC_NoMerger
MIOS_MPROC_MBLMerger_3
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_MBLINK_TUNNEL_PASS, ACCESS, MIOS_MPROC_NoMerger
	rgoto	MIOS_MPROC_MBLMergeIt
MIOS_MPROC_NoMBLMerger

	;; check if MIDI Merger switched on
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER, ACCESS, MIOS_MPROC_NoMerger

MIOS_MPROC_MBLMergeIt
MIOS_MPROC_MergeIt
	;; forward byte to MIDI Out
	movf	MIOS_MPROC_IN, W, BANKED
	rcall	_MIOS_MPROC_SendByte

MIOS_MPROC_NoMerger
MIOS_MPROC_Handler_SkipMerger

	;; clear "skip merger" flag if requested
	BRA_IFCLR MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_CLRREQ, BANKED, MIOS_MPROC_Handler_SkipM_NoClr
	bcf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_CLRREQ, BANKED
	bcf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_SKIP_MERGER, BANKED
MIOS_MPROC_Handler_SkipM_NoClr

	;; if reset has been requested, don't reset timeout counter
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED, MIOS_MPROC_Handler_NoTimeOut

	;; on realtime messages don't reset timeout counter
	movlw	0xf8
	cpfslt	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_Handler_NoTimeOut

	rgoto	MIOS_MPROC_Handler


;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
MIOS_MPROC_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x40		; MIOS ID (40 - the ultimative number - 2)
	movf	MIOS_MIDI_DEVICE_ID, W, BANKED
	andlw	0x7f
	return

;; --------------------------------------------------------------------------
;;  Action Invalid will be invoked when receiving an invalid sequence
;; --------------------------------------------------------------------------
MIOS_MPROC_ActionInvalid

;; --------------------------------------------------------------------------
;;  Action finished will be invoked when midi action is done
;; --------------------------------------------------------------------------
MIOS_MPROC_ActionFinished
	rcall	MIOS_MPROC_HlpResetSyxState
	rgoto	MIOS_MPROC_Handler

;; --------------------------------------------------------------------------
;;  to reset the sysex state machine
;; --------------------------------------------------------------------------
MIOS_MPROC_HlpResetSyxState
	SET_BSR	MIOS_MPROC_SYXSTATE
	movlw	(1 << MIOS_MPROC_SYXSTATE_SKIP_MERGER) | (1 << MIOS_MPROC_SYXSTATE_SKIP_CLRREQ) | (1 << MIOS_MPROC_SYXSTATE_RESET)
	andwf	MIOS_MPROC_SYXSTATE, F, BANKED
	clrf	MIOS_MPROC_ACTION, BANKED	; clear action ID
	return

;; --------------------------------------------------------------------------
;;  Overrun: invoked on Rx receiver overruns
;; --------------------------------------------------------------------------
MIOS_MPROC_Overrun
	bcf	RCSTA, CREN		; re-enable receiver
	bsf	RCSTA, CREN

	;; always send overrun error response (for debugging w/o LCD)
	movlw	MIOS_DISACK_OVERRUN
	rcall	MIOS_MPROC_Send_DisAcknowledge

	rgoto	MIOS_MPROC_Overrun_Cont

;; --------------------------------------------------------------------------
;;  Frame Error: invoked on FERR during upload
;; --------------------------------------------------------------------------
MIOS_MPROC_FrameError
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_FERR	; clear flag

	;; always send frame error response (for debugging w/o LCD)
	movlw	MIOS_DISACK_FRAME_ERROR
	rcall	MIOS_MPROC_Send_DisAcknowledge

	rgoto	MIOS_MPROC_FrameError_Cont

;; --------------------------------------------------------------------------
;;  Time Out: invoked on time out in halt state
;; --------------------------------------------------------------------------
MIOS_MPROC_TimeOut
	;; send timeout error response on aborted sysex transfer
	BRA_IFCLR MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_MYSYSEX, BANKED, MIOS_MPROC_TimeOut_NoSysEx
	movlw	MIOS_DISACK_MIDI_TIMEOUT
	rcall	MIOS_MPROC_Send_DisAcknowledge
MIOS_MPROC_TimeOut_NoSysEx

	;; if jobs have been suspended due to MIOS upload, reset OS after Tx buffer has been flushed
	GOTO_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED, MIOS_Reset

MIOS_MPROC_Overrun_Cont
MIOS_MPROC_FrameError_Cont
	;; notify the timeout or error to the user
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_MPROC_NotifyTimeout

	SET_BSR	MIOS_MIDI_EXPBYTES
	clrf	MIOS_MIDI_EXPBYTES, BANKED
	clrf	MIOS_MPROC_STATE, BANKED
	rgoto	MIOS_MPROC_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI Check action: do a sysex action
;; --------------------------------------------------------------------------
MIOS_MPROC_Handler_SyxEx
	;; if first byte after header, clear checksum and store command ID
	BRA_IFSET MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_ACTION, BANKED, MIOS_MPROC_Handler_SyxEx_NotBgn
MIOS_MPROC_Handler_SyxEx_Begin
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_ACTION, BANKED

	;; (error case: status byte has been received)
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Handler_SyxEx_End

	movf	MIOS_MPROC_IN, W, BANKED		; copy command ID to MIOS_MPROC_ACTION
	andlw	0x0f
	movwf	MIOS_MPROC_ACTION, BANKED
	swapf	MIOS_MPROC_IN, W, BANKED		; store address extension
	andlw	0x07
	movwf	MIOS_MPROC_ADDRESS_EXT, BANKED
	clrf	MIOS_MPROC_CHECKSUM, BANKED		; clear checksum
	rgoto	MIOS_MPROC_Handler_SyxEx_Cont
MIOS_MPROC_Handler_SyxEx_NotBgn

	;; fetch data until next status byte
	BRA_IFCLR MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Handler_SyxEx_NotEnd
MIOS_MPROC_Handler_SyxEx_End
	;; if sysex footer (F7) has not been received here, command is invalid!
	movlw	0xf7
	cpfseq	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_ActionInvalid

	;; note: each sysex command handler has to check "MIOS_MPROC_IN, 7" to determine if the end of a stream has been received
	rgoto	MIOS_MPROC_Handler_SyxEx_Cont
MIOS_MPROC_Handler_SyxEx_NotEnd

	;; add received byte to checksum
	movf	MIOS_MPROC_IN, W, BANKED
	addwf	MIOS_MPROC_CHECKSUM, F, BANKED

	;; increment action counter
	movlw	0x10
	addwf	MIOS_MPROC_ACTION, F, BANKED

MIOS_MPROC_Handler_SyxEx_Cont
	;; branch to command handler
	decf	MIOS_MPROC_ACTION, W, BANKED
	andlw	0x0f
	BRA_IFSET STATUS, Z, ACCESS, MIOS_MPROC_Cmd_Read	; 1
	addlw	-2+1
	BRA_IFSET STATUS, Z, ACCESS, MIOS_MPROC_Cmd_Write	; 2
	addlw	-3+2
	BRA_IFSET STATUS, Z, ACCESS, MIOS_MPROC_Cmd_SelBS	; 3
	addlw	-8+3
	BRA_IFSET STATUS, Z, ACCESS, MIOS_MPROC_Cmd_LCD	; 8
	addlw	-13+8
	BRA_IFSET STATUS, Z, ACCESS, MIOS_MPROC_Cmd_Debug	; 13
	addlw	-15+13
	BRA_IFSET STATUS, Z, ACCESS, MIOS_MPROC_Cmd_Ping	; 15
	rgoto	MIOS_MPROC_ActionInvalid
	;; (note: target address to far away for "bz", therefore I'm using "IFSET STATUS, Z"


;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;;  MIDI Trace Functions
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------

	;; trace first byte, called when MIOS_MPROC_STATE == 0
MIOS_MPROC_BranchEvent0
	;; leave routine if MSB of received byte not set (i.e. on SysEx streams)
	BRA_IFCLR MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_BranchEvent_Return

	;; leave routine if received byte is 0xf. (i.e. SysEx streams...)
	movf	MIOS_MPROC_IN, W, BANKED
	andlw	0xf0
	xorlw	0xf0
	skpnz
	rgoto	MIOS_MPROC_BranchEvent_Return

	movff	MIOS_MPROC_IN, MIOS_MPROC_LASTEVENT0	; store MIDI byte in LASTEVENT0

	incf	MIOS_MPROC_STATE, F, BANKED		; next jump entry in MIOS_MPROC_TraceEvent1
	rgoto	MIOS_MPROC_BranchEvent_Return

;; ---
	;; trace second byte, called when MIOS_MPROC_STATE == 1
MIOS_MPROC_BranchEvent1
	movff	MIOS_MPROC_IN, MIOS_MPROC_LASTEVENT1	; store MIDI byte in LASTEVENT1
	movff	MIOS_MPROC_IN, MIOS_MPROC_LASTEVENT2	; store MIDI byte also in LASTEVENT2 (for program change and channel pressure)
							; will be overwritten on 3-byte-events

	;; bypass when received event is a two-byte event
	SET_BSR	MIOS_MPROC_LASTEVENT0
	movf	MIOS_MPROC_LASTEVENT0, W, BANKED	; mask out upper nipple of MIDI event
	andlw	0xf0
	xorlw	0xc0				; 0xc0 = program change
	bz	MIOS_MPROC_TraceNow		; special routine which also checks for BOX_CFG1_RECEIVE_PC_FOR_BCHANGE

	movf	MIOS_MPROC_LASTEVENT0, W, BANKED	; mask out upper nipple of MIDI event
	andlw	0xf0
	xorlw	0xd0				; 0xd0 = channel aftertouch
	bz	MIOS_MPROC_TraceNow

	incf	MIOS_MPROC_STATE, F, BANKED	; next jump entry in MIDI_TraceEvent2
	rgoto	MIOS_MPROC_BranchEvent_Return

;; ---
	;; trace third byte and execute routine
	;; called when MIOS_MPROC_STATE == 2
MIOS_MPROC_BranchEvent2
	movff	MIOS_MPROC_IN, MIOS_MPROC_LASTEVENT2	; store MIDI byte in LASTEVENT2

MIOS_MPROC_TraceNow		; jump point for MIDI_TraceEvent1 on Cn and En events
	clrwdt			; feed the watchdog

	rcall	MIOS_MPROC_NotifyMIDIEvent; notify new event

	SET_BSR	MIOS_MPROC_STATE
	clrf	MIOS_MPROC_STATE, BANKED		; next state: MIDI event byte 0
	rgoto	MIOS_MPROC_BranchEvent_Return

;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;;  SysEx Actions
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------

	
;; --------------------------------------------------------------------------
;;  Help Functions
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;; 
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_Hlp_ConvAddr
	clrc
	rlf	TMP1, F
	clrf	TMP3
	clrc
	rlf	TMP1, F
	rlf	TMP2, F
	rlf	TMP3, F
	clrc
	rlf	TMP1, F
	rlf	TMP2, F
	rlf	TMP3, F
	return

MIOS_MPROC_Cmd_Hlp_GetAH
	movff	MIOS_MPROC_IN, TMP2
	rgoto	MIOS_MPROC_Handler
MIOS_MPROC_Cmd_Hlp_GetAL
	movff	MIOS_MPROC_IN, TMP1
	rcall	MIOS_MPROC_Cmd_Hlp_ConvAddr
	movff	TMP3, TBLPTRU
	movff	TMP2, TBLPTRH
	movff	TMP1, TBLPTRL
	rgoto	MIOS_MPROC_Handler
MIOS_MPROC_Cmd_Hlp_GetCH
	movff	MIOS_MPROC_IN, TMP2
	rgoto	MIOS_MPROC_Handler
MIOS_MPROC_Cmd_Hlp_GetCL
	movff	MIOS_MPROC_IN, TMP1
	rcall	MIOS_MPROC_Cmd_Hlp_ConvAddr

	;; save counter also in TMP[45]
	movff	TMP1, TMP4
	movff	TMP2, TMP5
	rgoto	MIOS_MPROC_Handler

;; --------------------------------------------------------------------------
;;  MIDI SysEx Command:	Read
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_Read
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Cmd_Read_End	; 0xf7 received? (see also MIOS_MPROC_Handler_SyxEx_End)

	movf	MIOS_MPROC_ACTION, W, BANKED				; command byte received? (see also MIOS_MPROC_Handler_SyxEx_Begin)
	andlw	0xf0
	bnz	MIOS_MPROC_Cmd_ReadBranch

MIOS_MPROC_Cmd_Read_Init
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_ReadBranch
	swapf	MIOS_MPROC_ACTION, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MIOS_MPROC_Cmd_Hlp_GetAH; (dummy)
	rgoto	MIOS_MPROC_Cmd_Hlp_GetAH
	rgoto	MIOS_MPROC_Cmd_Hlp_GetAL
	rgoto	MIOS_MPROC_Cmd_Hlp_GetCH
	rgoto	MIOS_MPROC_Cmd_Hlp_GetCL
	rgoto	MIOS_MPROC_Cmd_DoRead
	rgoto	MIOS_MPROC_Cmd_DoRead
MIOS_MPROC_Cmd_DoRead
	;; stay in this state until 0xf7 received
	movlw	0xf0
	addwf	MIOS_MPROC_ACTION, F, BANKED
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_Read_End
	movlw	0x41
	cpfseq	MIOS_MPROC_ACTION, BANKED
	rgoto MIOS_MPROC_Read_Failed

	;; check if address has been aligned correctly
	rcall	MIOS_MPROC_CheckAddress
	skpz
	rgoto	MIOS_MPROC_ActionInvalid

	;; error if counter is zero
	movf	TMP1, W
	iorwf	TMP2, W
	iorwf	TMP3, W
	skpnz
	rgoto	MIOS_MPROC_Read_Failed

	;; suspend all MIOS IRQs, except for the MIDI handlers
	;; box will be reset after upload
	call	MIOS_SystemSuspend

	SET_BSR	MIOS_MPROC_SYXSTATE
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED

MIOS_MPROC_Cmd_ReadLoop
	;; feed watchdog
	clrwdt

	;; send header
	rcall	MIOS_MPROC_Send_SysExHeader
	;; send write request
	swapf	MIOS_MPROC_ADDRESS_EXT, W, BANKED
	iorlw	0x02
	rcall	_MIOS_MPROC_SendByte
	;; clear checksum
	clrf	MIOS_MPROC_CHECKSUM, BANKED
	;; send High Address
	rrf	TBLPTRH, W
	rrf	WREG, W
	btfsc	TBLPTRU, 0
	iorlw	0x40
	rcall	MIOS_MPROC_SendByte
	rrf	TBLPTRH, W
	rrf	TBLPTRL, W
	rrf	WREG, W
	rrf	WREG, W
	rcall	MIOS_MPROC_SendByte

	;; read 0x400 bytes maximum per block
	movff	TMP1, TMP4
	movff	TMP2, TMP5
	movf	TMP3, W
	bnz	MIOS_MPROC_Cmd_ReadCtrFix
	movf	TMP2, W
	andlw	0xfc
	bz	MIOS_MPROC_Cmd_ReadNoCtrFix
MIOS_MPROC_Cmd_ReadCtrFix
	clrf	TMP4
	movlw	0x04
	movwf	TMP5
MIOS_MPROC_Cmd_ReadNoCtrFix

	;; send high counter
	rrf	TMP5, W
	rrf	WREG, W
	andlw	0x01
	rcall	MIOS_MPROC_SendByte
	;; send low counter
	rrf	TMP5, W
	rrf	TMP4, W
	rrf	WREG, W
	rrf	WREG, W
	rcall	MIOS_MPROC_SendByte

	;; branch depending on address
	movf	TBLPTRU, W
	bz	MIOS_MPROC_Cmd_ReadE_F

	;; read from BankStick
MIOS_MPROC_Cmd_ReadBS
	movf	MIOS_MPROC_ADDRESS_EXT, W, BANKED
	call	MIOS_BANKSTICK_CtrlSet
	movff	TBLPTRL, MIOS_PARAMETER1
	movff	TBLPTRH, MIOS_PARAMETER2
	lfsr	FSR0, MIOS_UPLOAD_BUFFER
MIOS_MPROC_Cmd_ReadBSLoop
	call	MIOS_BANKSTICK_Read
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_MPROC_Cmd_ReadErrorBS
	movwf	POSTINC0
	incf	TBLPTRL, F
	skpnz
	incf	TBLPTRH, F
	decf	TMP4, F
	skpc
	decf	TMP5, F
	movf	TMP4, W
	iorwf	TMP5, W
	bnz	MIOS_MPROC_Cmd_ReadBSLoop
	rgoto	MIOS_MPROC_Cmd_ReadCont

MIOS_MPROC_Cmd_ReadE_F
	BRA_IFCLR TBLPTRH, 7, ACCESS, MIOS_MPROC_Cmd_ReadF

	;; read from EEPROM
MIOS_MPROC_Cmd_ReadE
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	movff	TBLPTRH, EEADRH
#endif
	movff	TBLPTRL, EEADR
	lfsr	FSR0, MIOS_UPLOAD_BUFFER
MIOS_MPROC_Cmd_ReadELoop
	call	MIOS_EEPROM_Read
	movwf	POSTINC0

#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	;; TK: MIOS_EEPROM_* routines don't auto-increment EEADRH due to compatibility reasons
	movf	EEADR, W
	skpnz
	incf	EEADRH, F			; increment EEADRH on overrun
#endif

	decf	TMP4, F
	skpc
	decf	TMP5, F
	movf	TMP4, W
	iorwf	TMP5, W
	bnz	MIOS_MPROC_Cmd_ReadELoop
	rgoto	MIOS_MPROC_Cmd_ReadCont

	;; read from FLASH
MIOS_MPROC_Cmd_ReadF
	lfsr	FSR1, MIOS_UPLOAD_BUFFER
MIOS_MPROC_Cmd_ReadFLoop
	;; SysEx address format -> TBLPTR[LHU] format
	bcf	TBLPTRH, 7
	btfsc	MIOS_MPROC_ADDRESS_EXT, 0, BANKED
	bsf	TBLPTRH, 7
	rrf	MIOS_MPROC_ADDRESS_EXT, W, BANKED
	andlw	0x03
	movwf	TBLPTRU

	;; read from flash
	call	MIOS_FLASH_Read

	;; TBLPTR[LHU] format -> SysEx address format
	rlf	TBLPTRH, W
	rlf	TBLPTRU, W
	movwf	MIOS_MPROC_ADDRESS_EXT, BANKED
	clrf	TBLPTRU
	bcf	TBLPTRH, 7

	movlw	0x40		; add 0x40 to FSR1
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F

	movlw	-0x40		; subtract 0x40 from TMP[45]
	addwf	TMP4, F
	skpc
	decf	TMP5, F

	movf	TMP4, W		; end if TMP[45] == 0
	iorwf	TMP5, W
	bz	MIOS_MPROC_Cmd_ReadCont
				; ensure that we never get an overrun
	BRA_IFCLR TMP5, 7, ACCESS, MIOS_MPROC_Cmd_ReadFLoop
MIOS_MPROC_Cmd_ReadCont
	
	;; send scrambled data (8bit->7bit)
	rcall	MIOS_MPROC_Send_827
	;; send footer + checksum
	rcall	MIOS_MPROC_Send_SysExFooter

	;; loop until TMP[123] == zero
	movf	TMP1, W
	iorwf	TMP2, W
	iorwf	TMP3, W
	bz	MIOS_MPROC_Cmd_ReadLoopEnd
	;; wait for 750 mS
	movlw	0x03
	movwf	TMP5
MIOS_MPROC_Cmd_Read_DlyLoop
	movlw	250
	call	MIOS_Delay
	decfsz	TMP5, F
	rgoto	MIOS_MPROC_Cmd_Read_DlyLoop
	rgoto	MIOS_MPROC_Cmd_ReadLoop

MIOS_MPROC_Cmd_ReadLoopEnd
	rgoto	MIOS_MPROC_ActionFinished

MIOS_MPROC_Write_Failed
MIOS_MPROC_Read_Failed
	movlw	MIOS_DISACK_LESS_BYTES_THAN_EXP
	rcall	MIOS_MPROC_Send_DisAcknowledge
	rgoto	MIOS_MPROC_ActionInvalid

	;; if BankStick not available
MIOS_MPROC_Cmd_ReadErrorBS
MIOS_MPROC_Cmd_WriteErrorBS
MIOS_MPROC_Cmd_SelBSError
	;; send footer w/o checksum
	rcall	_MIOS_MPROC_Send_SysExFooter
	;; BankStick not available
	movlw	MIOS_DISACK_BS_NOT_AVAILABLE
	rcall	MIOS_MPROC_Send_DisAcknowledge
	rgoto	MIOS_MPROC_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI SysEx Command: Write
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_Write
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Cmd_Write_End; 0xf7 received? (see also MIOS_MPROC_Handler_SyxEx_End)

	movf	MIOS_MPROC_ACTION, W, BANKED				; command byte received? (see also MIOS_MPROC_Handler_SyxEx_Begin)
	andlw	0xf0
	bnz	MIOS_MPROC_Cmd_WriteBranch

MIOS_MPROC_Cmd_Write_Init
	lfsr	FSR0, MIOS_UPLOAD_BUFFER
	movlw	0x08
	movwf	MIOS_MPROC_SYX_BIT8_CTR, BANKED
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_WriteBranch
	swapf	MIOS_MPROC_ACTION, W, BANKED
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MIOS_MPROC_Cmd_Hlp_GetAH; (dummy)
	rgoto	MIOS_MPROC_Cmd_Hlp_GetAH
	rgoto	MIOS_MPROC_Cmd_Hlp_GetAL
	rgoto	MIOS_MPROC_Cmd_Hlp_GetCH
	rgoto	MIOS_MPROC_Cmd_Hlp_GetCL
	rgoto	MIOS_MPROC_Cmd_DoWrite
	rgoto	MIOS_MPROC_Cmd_Write_CmpChk
MIOS_MPROC_Cmd_DoWrite

	movf	MIOS_MPROC_IN, W, BANKED
	movwf	MIOS_MPROC_SYX_BYTE7, BANKED
	clrc
	rlf	MIOS_MPROC_SYX_BYTE7, F, BANKED
	movlw	0x07
	movwf	MIOS_MPROC_SYX_BIT7_CTR, BANKED
MIOS_MPROC_ADW_Bit7Shift
	rlf	MIOS_MPROC_SYX_BYTE7, F, BANKED
	rlf	MIOS_MPROC_SYX_BYTE8, F, BANKED
	dcfsnz	MIOS_MPROC_SYX_BIT8_CTR, F, BANKED
	rgoto	MIOS_MPROC_ADW_NewWord
MIOS_MPROC_ADW_NewWordCont
	decfsz	MIOS_MPROC_SYX_BIT7_CTR, F, BANKED
	rgoto	MIOS_MPROC_ADW_Bit7Shift

	;; stay in this state
	movlw	0xf0
	addwf	MIOS_MPROC_ACTION, F, BANKED
	rgoto	MIOS_MPROC_Handler
	
	
MIOS_MPROC_ADW_NewWord
	movlw	(MIOS_UPLOAD_BUFFER_END+2) & 0xff
	cpfseq	FSR0L, ACCESS
	rgoto MIOS_MPROC_ADW_NewWordOk
	movlw	(MIOS_UPLOAD_BUFFER_END+2) >> 8
	cpfseq	FSR0H, ACCESS
	rgoto MIOS_MPROC_ADW_NewWordOk
	movlw	MIOS_DISACK_MORE_BYTES_THAN_EXP
	rcall	MIOS_MPROC_Send_DisAcknowledge
	rgoto	MIOS_MPROC_ActionInvalid

MIOS_MPROC_ADW_NewWordOk
	;; suspend all MIOS IRQs, except for the MIDI handlers
	;; box will be reset after upload
	call	MIOS_SystemSuspend

	SET_BSR	MIOS_MPROC_SYXSTATE
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED

	;; write byte to buffer
	movff	MIOS_MPROC_SYX_BYTE8, POSTINC0

	movlw	0x08
	movwf	MIOS_MPROC_SYX_BIT8_CTR, BANKED

	;; decrement counter
	decf	TMP1, F
	skpc
	decf	TMP2, F
	skpc
	decf	TMP3, F

	;; stay in this state if counter > 0, else next state (expecting checksum)
	movf	TMP1, W
	iorwf	TMP2, W
	iorwf	TMP3, W
	bnz	MIOS_MPROC_ADW_NewWordCont

	;; else next byte will initiate a branch to MIOS_MPROC_Cmd_Write_CmpChk
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_Write_CmpChk
	movlw	0xf0		; stay in this state
	addwf	MIOS_MPROC_ACTION, F, BANKED

	;; fix checksum
	movf	MIOS_MPROC_IN, W, BANKED
	subwf	MIOS_MPROC_CHECKSUM, W, BANKED
	sublw	0
	andlw	0x7f
	movwf	MIOS_MPROC_CHECKSUM, BANKED

	;; if checksum not equal, send disacknowledge and stop receiving
	cpfseq	MIOS_MPROC_IN, BANKED
	rgoto MIOS_MPROC_Cmd_Write_Failed
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_Write_Failed
	movlw	MIOS_DISACK_WRONG_CHECKSUM
	rcall	MIOS_MPROC_Send_DisAcknowledge
	rgoto	MIOS_MPROC_ActionInvalid

MIOS_MPROC_Cmd_Write_End
	movlw	0x52
	cpfseq	MIOS_MPROC_ACTION, BANKED
	rgoto MIOS_MPROC_Write_Failed

	;; check if address has been aligned correctly
	rcall	MIOS_MPROC_CheckAddress
	skpz
	rgoto	MIOS_MPROC_ActionInvalid

	;; branch depending on address
	movf	TBLPTRU, W
	bz	MIOS_MPROC_Cmd_WriteE_F

	;; write to BankStick
MIOS_MPROC_Cmd_WriteBS
	movf	MIOS_MPROC_ADDRESS_EXT, W, BANKED
	call	MIOS_BANKSTICK_CtrlSet
	movff	TBLPTRL, MIOS_PARAMETER1
	movff	TBLPTRH, MIOS_PARAMETER2
	lfsr	FSR1, MIOS_UPLOAD_BUFFER
MIOS_MPROC_Cmd_WriteBSLoop
	call	MIOS_BANKSTICK_WritePage
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_MPROC_Cmd_WriteErrorBS

	movlw	0x40		; add 0x40 to FSR1
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F

	movlw	-0x40		; subtract 0x40 from TMP[45]
	addwf	TMP4, F
	skpc
	decf	TMP5, F

	movf	TMP4, W		; end if TMP[45] == 0
	iorwf	TMP5, W
	bz	MIOS_MPROC_Cmd_WriteDone
				; ensure that we never get an overrun
	BRA_IFCLR TMP5, 7, ACCESS, MIOS_MPROC_Cmd_WriteBSLoop
	rgoto	MIOS_MPROC_Cmd_WriteDone

MIOS_MPROC_Cmd_WriteE_F
	BRA_IFCLR TBLPTRH, 7, ACCESS, MIOS_MPROC_Cmd_WriteF

	;; write max 0x100 to EEPROM
MIOS_MPROC_Cmd_WriteE
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	movff	TBLPTRH, EEADRH
#endif
	movff	TBLPTRL, EEADR
	lfsr	FSR0, MIOS_UPLOAD_BUFFER
MIOS_MPROC_Cmd_WriteELoop
	movf	POSTINC0, W
	call	MIOS_EEPROM_Write
	bnz	MIOS_MPROC_Cmd_WriteError
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	;; TK: MIOS_EEPROM_* routines don't auto-increment EEADRH due to compatibility reasons
	movf	EEADR, W
	skpnz
	incf	EEADRH, F			; increment EEADRH on overrun
#endif
	decf	TMP4, F
	skpc
	decf	TMP5, F
	movf	TMP4, W
	iorwf	TMP5, W
	bnz	MIOS_MPROC_Cmd_WriteELoop

	rgoto	MIOS_MPROC_Cmd_WriteDone


MIOS_MPROC_Cmd_WriteF
	;; now write 0x400/0x40 = 16 blocks to flash
	;; table address already preloaded
	lfsr	FSR1, MIOS_UPLOAD_BUFFER
MIOS_MPROC_Cmd_WriteFLoop
	;; copy address extension to TBLPTRH.7 (e.g. for PIC18F4620) --- address range is checked within MIOS_FLASH_Write
	btfsc	MIOS_MPROC_ADDRESS_EXT, 0, BANKED
	bsf	TBLPTRH, 7
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	rrf	MIOS_MPROC_ADDRESS_EXT, W, BANKED
	andlw	0x03
	movwf	TBLPTRU
#endif

	call	MIOS_FLASH_Write
	bnz	MIOS_MPROC_Cmd_WriteError

	movlw	0x40		; add 0x40 to FSR1
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F

	movlw	-0x40		; subtract 0x40 from TMP[45]
	addwf	TMP4, F
	skpc
	decf	TMP5, F

	movf	TMP4, W		; end if TMP[45] == 0
	iorwf	TMP5, W
	bz	MIOS_MPROC_Cmd_WriteDone
				; ensure that we never get an overrun
	BRA_IFCLR TMP5, 7, ACCESS, MIOS_MPROC_Cmd_WriteFLoop

MIOS_MPROC_Cmd_WriteDone
	;; well done! :)
	rcall	MIOS_MPROC_Send_Acknowledge
	rgoto	MIOS_MPROC_ActionFinished

MIOS_MPROC_Cmd_WriteError
	movlw	MIOS_DISACK_WRITE_FAILED
	rcall	MIOS_MPROC_Send_DisAcknowledge
	rgoto	MIOS_MPROC_ActionInvalid

;; --------------------------------------------------------------------------
;;  MIDI Action: Debug
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_Debug
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Cmd_Debug_End; 0xf7 received? (see also MIOS_MPROC_Handler_SyxEx_End)

	movf	MIOS_MPROC_ACTION, W, BANKED				; command byte received? (see also MIOS_MPROC_Handler_SyxEx_Begin)
	andlw	0xf0
	bnz	MIOS_MPROC_Cmd_DebugBranch

MIOS_MPROC_Cmd_Debug_Init
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_DebugBranch
	swapf	MIOS_MPROC_ACTION, W, BANKED
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MIOS_MPROC_Cmd_DebugGetCmd	; (dummy)
	rgoto	MIOS_MPROC_Cmd_DebugGetCmd
	rgoto	MIOS_MPROC_Cmd_DebugGetPAU
	rgoto	MIOS_MPROC_Cmd_DebugGetPAH
	rgoto	MIOS_MPROC_Cmd_DebugGetPAL
	rgoto	MIOS_MPROC_Cmd_DebugGetP1H
	rgoto	MIOS_MPROC_Cmd_DebugGetP1L
	rgoto	MIOS_MPROC_Cmd_DebugGetP2H
	rgoto	MIOS_MPROC_Cmd_DebugGetP2L
	rgoto	MIOS_MPROC_Cmd_DebugGetP3H
	rgoto	MIOS_MPROC_Cmd_DebugGetP3L
	rgoto	MIOS_MPROC_Cmd_DebugGetP4H
	rgoto	MIOS_MPROC_Cmd_DebugGetP4L
	rgoto	MIOS_MPROC_Cmd_DoDebug
	rgoto	MIOS_MPROC_Cmd_DoDebug		; (fail save)

MIOS_MPROC_Cmd_DebugGetCmd
	movff	MIOS_MPROC_IN, TMP5
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_DebugGetPHlpH
	movwf	FSR0L
	clrf	FSR0H
	swapf	MIOS_MPROC_IN, W, BANKED
	andlw	0xf0
	movwf	INDF0
	rgoto	MIOS_MPROC_Handler
MIOS_MPROC_Cmd_DebugGetPHlpL
	movwf	FSR0L
	clrf	FSR0H
	movf	MIOS_MPROC_IN, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_DebugGetPAU
	movff	MIOS_MPROC_IN, TMP1
	rgoto	MIOS_MPROC_Handler
MIOS_MPROC_Cmd_DebugGetPAH
	movff	MIOS_MPROC_IN, TMP2
	rgoto	MIOS_MPROC_Handler
MIOS_MPROC_Cmd_DebugGetPAL
	movff	MIOS_MPROC_IN, TMP3
	clrc
	rrf	TMP1, F
	skpnc
	bsf	TMP2, 7

	clrc
	rrf	TMP1, F
	rrf	TMP2, F
	skpnc
	bsf	TMP3, 7

	;; store address in TBLPTR
	movff	TMP2, TBLPTRH
	movff	TMP3, TBLPTRL
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_DebugGetP1H
	movlw	TMP1
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpH
MIOS_MPROC_Cmd_DebugGetP1L
	movlw	TMP1
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpL
	
MIOS_MPROC_Cmd_DebugGetP2H
	movlw	TMP2
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpH
MIOS_MPROC_Cmd_DebugGetP2L
	movlw	TMP2
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpL
	
MIOS_MPROC_Cmd_DebugGetP3H
	movlw	TMP3
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpH
MIOS_MPROC_Cmd_DebugGetP3L
	movlw	TMP3
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpL
	
MIOS_MPROC_Cmd_DebugGetP4H
	movlw	TMP4
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpH
MIOS_MPROC_Cmd_DebugGetP4L
	movlw	TMP4
	rgoto	MIOS_MPROC_Cmd_DebugGetPHlpL
	

MIOS_MPROC_Cmd_DoDebug
	;; stay in this state until 0xf7 received
	movlw	0xf0
	addwf	MIOS_MPROC_ACTION, F, BANKED
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_Debug_End
	movlw	0xcd
	cpfseq	MIOS_MPROC_ACTION, BANKED
	rgoto MIOS_MPROC_Debug_Failed

	;; if command == 0x01: call function
	movlw	0x01
	cpfseq	TMP5, ACCESS
	rgoto MIOS_MPROC_Debug_Cmd02C

MIOS_MPROC_Debug_Cmd01
	movff	TMP2, MIOS_PARAMETER1
	movff	TMP3, MIOS_PARAMETER2
	movff	TMP4, MIOS_PARAMETER3

	;; call command indirectly
	rcall	MIOS_MPROC_Cmd_DebugDo
MIOS_MPROC_Cmd_DebugDo_Ret

	SET_BSR MIOS_MPROC_IN
	;; save return parameters
	movwf	TMP2
	movff	MIOS_PARAMETER1, TMP3
	movff	MIOS_PARAMETER2, TMP4
	movff	MIOS_PARAMETER3, TMP5

	;; send header
	rcall	MIOS_MPROC_Send_SysExHeader

	;; send Acknowledge
	movlw	MIOS_MIDI_MPROC_ACK
	rcall	_MIOS_MPROC_SendByte

	;; send WREG, etc.
	rcall	MIOS_MPROC_Cmd_DebugSendHlp
	movff	TMP3, TMP2
	rcall	MIOS_MPROC_Cmd_DebugSendHlp
	movff	TMP4, TMP2
	rcall	MIOS_MPROC_Cmd_DebugSendHlp
	movff	TMP5, TMP2
	rcall	MIOS_MPROC_Cmd_DebugSendHlp

	;; send footer (without checksum)
	rcall	_MIOS_MPROC_Send_SysExFooter

	rgoto	MIOS_MPROC_ActionFinished

MIOS_MPROC_Debug_Cmd02C
	;; if command == 0x02: read SRAM
	movlw	0x02
	cpfseq	TMP5, ACCESS
	rgoto MIOS_MPROC_Debug_Cmd03C
MIOS_MPROC_Debug_Cmd02
	movff	TBLPTRL, FSR0L
	movff	TBLPTRH, FSR0H
	movff	TMP2, TMP3
	movff	TMP1, TMP2

	;; send header
	rcall	MIOS_MPROC_Send_SysExHeader

	;; send Acknowledge
	movlw	MIOS_MIDI_MPROC_ACK
	rcall	_MIOS_MPROC_SendByte

	;; send SRAM content
MIOS_MPROC_Debug_Cmd02Loop
	swapf	INDF0, W
	andlw	0x0f
	rcall	_MIOS_MPROC_SendByte
	movf	POSTINC0, W
	andlw	0x0f
	rcall	_MIOS_MPROC_SendByte

	decf	TMP3, F
	skpc
	decf	TMP2, F
	movf	TMP3, W
	iorwf	TMP2, W
	bnz	MIOS_MPROC_Debug_Cmd02Loop

	;; send footer (without checksum)
	rcall	_MIOS_MPROC_Send_SysExFooter

	rgoto	MIOS_MPROC_ActionFinished
	
MIOS_MPROC_Debug_Cmd03C
	;; if command == 0x03: write SRAM
	movlw	0x03
	cpfseq	TMP5, ACCESS
	rgoto MIOS_MPROC_Debug_Failed
MIOS_MPROC_Debug_Cmd03
	movff	TBLPTRL, FSR0L
	movff	TBLPTRH, FSR0H
	movff	TMP1, INDF0
	rcall	MIOS_MPROC_Send_Acknowledge
	rgoto	MIOS_MPROC_ActionFinished

MIOS_MPROC_Debug_Failed
	movlw	MIOS_DISACK_WRONG_DEBUG_CMD
	rcall	MIOS_MPROC_Send_DisAcknowledge
	rgoto	MIOS_MPROC_ActionInvalid

	;; --
MIOS_MPROC_Cmd_DebugSendHlp
	swapf	TMP2, W
	andlw	0x0f
	rcall	_MIOS_MPROC_SendByte
	movf	TMP2, W
	andlw	0x0f
	rgoto	_MIOS_MPROC_SendByte

MIOS_MPROC_Cmd_DebugDo
	rcall	_MIOS_MPROC_Cmd_DebugDo
	rgoto	MIOS_MPROC_Cmd_DebugDo_Ret

_MIOS_MPROC_Cmd_DebugDo
	movf	TBLPTRL, W
	movwf	TOSL
	movf	TBLPTRH, W
	movwf	TOSH
	movf	TMP1, W
	return

;; --------------------------------------------------------------------------
;;  MIDI Action: Select BankStick
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_SelBS
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Cmd_SelBS_End; 0xf7 received? (see also MIOS_MPROC_Handler_SyxEx_End)

	movf	MIOS_MPROC_ACTION, W, BANKED				; command byte received? (see also MIOS_MPROC_Handler_SyxEx_Begin)
	andlw	0xf0
	bnz	MIOS_MPROC_Cmd_SelBSCont

MIOS_MPROC_Cmd_SelBS_Init
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_SelBSCont
	;; suspend all MIOS IRQs, except for the MIDI handlers
	;; box will be reset within 2 seconds
	call	MIOS_SystemSuspend

	SET_BSR	MIOS_MPROC_SYXSTATE
	bsf	MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_RESET, BANKED

	;; note: since MIOS V1.8 this command is expired!
	;; the BankStick will be selected with the address extension of the command byte
#if 0
	movf	MIOS_MPROC_IN, W, BANKED
	andlw	0x07
	call	MIOS_BANKSTICK_CtrlSet
#endif
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_SelBS_End
	;; check if BankStick available
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_MPROC_Cmd_SelBSError
	rcall	MIOS_MPROC_Send_Acknowledge
	rgoto	MIOS_MPROC_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: LCD
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_LCD
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Cmd_LCD_End	; 0xf7 received? (see also MIOS_MPROC_Handler_SyxEx_End)

	movf	MIOS_MPROC_ACTION, W, BANKED				; command byte received? (see also MIOS_MPROC_Handler_SyxEx_Begin)
	andlw	0xf0
	bnz	MIOS_MPROC_Cmd_LCDBranch

MIOS_MPROC_Cmd_LCD_Init
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_LCDBranch
	swapf	MIOS_MPROC_ACTION, W, BANKED
	andlw	0x03
	bz	MIOS_MPROC_Cmd_LCDGetCmd	; (dummy)
	addlw	-1
	bnz	MIOS_MPROC_Cmd_LCDData
MIOS_MPROC_Cmd_LCDGetCmd
	;; data counter in TMP4
	clrf	TMP4
	;; move command to TMP5
	movf	MIOS_MPROC_IN, W, BANKED
	movwf	TMP5

	;; if command == 0x00, clear screen and start message
	movf	TMP5, W
	bnz	MIOS_MPROC_Cmd_LCDGetCmdEnd
	call	MIOS_LCD_Clear
	movlw	0xff
	call	MIOS_LCD_MessageStart
MIOS_MPROC_Cmd_LCDGetCmdEnd	
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_LCDData
	;; stay in this state
	movlw	0xf0
	addwf	MIOS_MPROC_ACTION, F, BANKED

	;; branch depending on command
	movf	TMP5, W
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MIOS_MPROC_Cmd_LCD_Cmd0
	rgoto	MIOS_MPROC_Cmd_LCD_Cmd1
	rgoto	MIOS_MPROC_Cmd_LCD_Cmd2
	rgoto	MIOS_MPROC_Cmd_LCD_Cmd3

	;; LCD Command 0: start message, clear screen and print characters
MIOS_MPROC_Cmd_LCD_Cmd0
	;; screen already cleared, print character
	rgoto	MIOS_MPROC_Cmd_LCD_Print

	;; LCD Command 1: set cursor and print characters
MIOS_MPROC_Cmd_LCD_Cmd1
	;; first byte: X position
MIOS_MPROC_Cmd_LCD_Cmd1_Chk0
	movf	TMP4, W
	bnz	MIOS_MPROC_Cmd_LCD_Cmd1_Chk1
MIOS_MPROC_Cmd_LCD_Cmd1_0
	movff	MIOS_MPROC_IN, TMP1
	rgoto	MIOS_MPROC_Cmd_LCD_Next

	;; second byte: Y position
MIOS_MPROC_Cmd_LCD_Cmd1_Chk1
	decf	TMP4, W
	bnz	MIOS_MPROC_Cmd_LCD_Print; remaining bytes: print
	;; cursor = y*0x40 + x
	movf	MIOS_MPROC_IN, W, BANKED
	mullw	0x40
	movf	TMP1, W
	andlw	0x3f
	iorwf	PRODL, W
	call	MIOS_LCD_CursorSet
	rgoto	MIOS_MPROC_Cmd_LCD_Next
	
	;; LCD Command 2: print character(s)
MIOS_MPROC_Cmd_LCD_Cmd2
	rgoto	MIOS_MPROC_Cmd_LCD_Print

	;; LCD Command 3: stop message
MIOS_MPROC_Cmd_LCD_Cmd3
	call	MIOS_LCD_MessageStop
	rgoto	MIOS_MPROC_Cmd_LCD_Next

	;; 
	;; Help functions
	;; 
MIOS_MPROC_Cmd_LCD_Print
	SET_BSR	MIOS_MPROC_IN
	movf	MIOS_MPROC_IN, W, BANKED
	call	MIOS_LCD_PrintChar
	;; start message
	movlw	0xff
	call	MIOS_LCD_MessageStart
MIOS_MPROC_Cmd_LCD_Next
	incf	TMP4, F			; increment data counter (with saturation)
	skpnz
	decf	TMP4, F
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_LCD_End
	;; no acknowledge!
	;; send 0xf7 if merger enabled for a proper protocol
	movlw	0xf7
	RCALL_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER, ACCESS, _MIOS_MPROC_SendByte
	rgoto	MIOS_MPROC_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
MIOS_MPROC_Cmd_Ping
	BRA_IFSET MIOS_MPROC_IN, 7, BANKED, MIOS_MPROC_Cmd_Ping_End	; 0xf7 received? (see also MIOS_MPROC_Handler_SyxEx_End)

	;; loop until 0xf7 has been received
	rgoto	MIOS_MPROC_Handler

MIOS_MPROC_Cmd_Ping_End
	rcall	MIOS_MPROC_Send_Acknowledge
	rgoto	MIOS_MPROC_ActionFinished



;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  Sends a byte - if called without underscore, adds the byte to checksum
;; --------------------------------------------------------------------------
MIOS_MPROC_SendByte
	andlw	0x7f
	addwf	MIOS_MPROC_CHECKSUM, F, BANKED
_MIOS_MPROC_SendByte
	rgoto	MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
MIOS_MPROC_Send_Acknowledge
	rcall	MIOS_MPROC_Send_SysExHeader

	movlw	MIOS_MIDI_MPROC_ACK
	rcall	_MIOS_MPROC_SendByte

	rgoto	MIOS_MPROC_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  MIDI Send DisAcknowledge, Error status in WREG (Util function)
;; --------------------------------------------------------------------------
MIOS_MPROC_Send_DisAcknowledge
	movwf	TMP1		; error code in WREG

	rcall	MIOS_MPROC_Send_SysExHeader

	movlw	MIOS_MIDI_MPROC_DISACK
	rcall	_MIOS_MPROC_SendByte
	movf	TMP1, W
	rcall	_MIOS_MPROC_SendByte

	rgoto	MIOS_MPROC_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Header (Util function)
;; --------------------------------------------------------------------------
MIOS_MPROC_Send_SysExHeader
	SET_BSR	MIOS_MPROC_SYXSTATE
	;; if MIOS_MPROC_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR MIOS_MPROC_SYXSTATE, MIOS_MPROC_SYXSTATE_MYSYSEX, BANKED, MIOS_MPROC_Send_SysExHeader_Skp
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER, ACCESS, MIOS_MPROC_Send_SysExHeader_Skp
	movlw	0x05		; send only Device ID
	movwf	MIOS_MPROC_CTR, BANKED
	rgoto	MIOS_MPROC_Send_SysExHeaderLoop
MIOS_MPROC_Send_SysExHeader_Skp

	clrf	MIOS_MPROC_CTR, BANKED
MIOS_MPROC_Send_SysExHeaderLoop
	movf	MIOS_MPROC_CTR, W, BANKED
	rcall	MIOS_MPROC_SysExHeaderGet
	rcall	_MIOS_MPROC_SendByte
	incf	MIOS_MPROC_CTR, F, BANKED
	movlw	0x06
	cpfseq	MIOS_MPROC_CTR, BANKED
	rgoto MIOS_MPROC_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
MIOS_MPROC_Send_SysExFooter
	SET_BSR	MIOS_MPROC_CHECKSUM
	movf	MIOS_MPROC_CHECKSUM, W, BANKED
	andlw	0x7f
	rcall	_MIOS_MPROC_SendByte
_MIOS_MPROC_Send_SysExFooter
	movlw	0xf7
	rgoto	_MIOS_MPROC_SendByte

;; --------------------------------------------------------------------------
;;  called by the MIDI handler when a complete MIDI event has been received
;;  In:	MIOS_MPROC_LASTEVENT0 \
;;      MIOS_MPROC_LASTEVENT1  > MIDI Event (2 or 3 bytes, depending on MIDI event type)
;;      MIOS_MPROC_LASTEVENT2 /
;; --------------------------------------------------------------------------
MIOS_MPROC_NotifyMIDIEvent
	movff	MIOS_MPROC_LASTEVENT0, MIOS_PARAMETER1
	movff	MIOS_MPROC_LASTEVENT1, MIOS_PARAMETER2
	movff	MIOS_MPROC_LASTEVENT2, MIOS_PARAMETER3
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_MPROC_NotifyReceivedEvent

	TABLE_ADDR MIOS_MPROC_EVENT_TABLE
	SET_BSR MIOS_MPROC_CTR
	clrf	MIOS_MPROC_CTR, BANKED

MIOS_MPROC_NotifyMIDIEventLoop
	tblrd*+
	movff	TABLAT, MIOS_MPROC_EVNT0
	tblrd*+

	incf	MIOS_MPROC_EVNT0, W, BANKED
	bz	MIOS_MPROC_NotifyMIDIEventEnd
	movf	MIOS_MPROC_EVNT0, W, BANKED
	cpfseq	MIOS_MPROC_LASTEVENT0, BANKED
	rgoto MIOS_MPROC_NotifyMIDIEventNext
	movf	TABLAT, W
	cpfseq	MIOS_MPROC_LASTEVENT1, BANKED
	rgoto MIOS_MPROC_NotifyMIDIEventNext
	rgoto	MIOS_MPROC_NotifyMIDIEventFound
MIOS_MPROC_NotifyMIDIEventNext

	incfsz	MIOS_MPROC_CTR, F, BANKED
	rgoto	MIOS_MPROC_NotifyMIDIEventLoop
MIOS_MPROC_NotifyMIDIEventEnd
	return			; event not found

MIOS_MPROC_NotifyMIDIEventFound
	movff	MIOS_MPROC_LASTEVENT0, MIOS_PARAMETER1
	movff	MIOS_MPROC_LASTEVENT1, MIOS_PARAMETER2
	movff	MIOS_MPROC_LASTEVENT2, MIOS_PARAMETER3
	movf	MIOS_MPROC_CTR, W, BANKED
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_MPROC_NotifyFoundEvent
	return


;; --------------------------------------------------------------------------
;;  sends data in scrambled format, expects:
;;     o data in MIOS_UPLOAD_BUFFER (0x400 bytes)
;;     o data counter in TMP[123] (routine stops when 0x400 bytes are sent)
;; --------------------------------------------------------------------------
MIOS_MPROC_Send_827
	lfsr	FSR0, MIOS_UPLOAD_BUFFER
	movlw	0x07
	SET_BSR	MIOS_MPROC_SYX_BIT7_CTR
	movwf	MIOS_MPROC_SYX_BIT7_CTR, BANKED
	clrf	MIOS_MPROC_SYX_BYTE7, BANKED
MIOS_MPROC_Send_827_OLoop
	movlw	0x08
	movwf	MIOS_MPROC_SYX_BIT8_CTR, BANKED
	movff	POSTINC0, MIOS_MPROC_SYX_BYTE8
	
MIOS_MPROC_Send_827_ILoop
	rlf	MIOS_MPROC_SYX_BYTE8, F, BANKED
	rlf	MIOS_MPROC_SYX_BYTE7, F, BANKED
	decfsz	MIOS_MPROC_SYX_BIT7_CTR, F, BANKED
	rgoto	MIOS_MPROC_Send_827_INext
MIOS_MPROC_Send_827_ISend
	movf	MIOS_MPROC_SYX_BYTE7, W, BANKED
	rcall	MIOS_MPROC_SendByte
	movlw	0x07
	SET_BSR	MIOS_MPROC_SYX_BIT7_CTR
	movwf	MIOS_MPROC_SYX_BIT7_CTR, BANKED
	clrf	MIOS_MPROC_SYX_BYTE7, BANKED
MIOS_MPROC_Send_827_INext
	decfsz	MIOS_MPROC_SYX_BIT8_CTR, F, BANKED
	rgoto	MIOS_MPROC_Send_827_ILoop

	;; decrement counter
	decf	TMP1, F
	skpc
	decf	TMP2, F
	skpc
	decf	TMP3, F

	;; loop until 0x400 byte sent
	movf	TMP2, W
	andlw	0x03
	iorwf	TMP1, W
	bnz	MIOS_MPROC_Send_827_OLoop

	;; fill last bits with zero if required
	movf	MIOS_MPROC_SYX_BIT7_CTR, W, BANKED
	xorlw	0x07
	bz	MIOS_MPROC_Send_827_ILoop2Skip
MIOS_MPROC_Send_827_ILoop2
	clrc
	rlf	MIOS_MPROC_SYX_BYTE7, F, BANKED
	decfsz	MIOS_MPROC_SYX_BIT7_CTR, F, BANKED
	rgoto	MIOS_MPROC_Send_827_ILoop2
	movf	MIOS_MPROC_SYX_BYTE7, W, BANKED
	rcall	MIOS_MPROC_SendByte
MIOS_MPROC_Send_827_ILoop2Skip

	;; convert checksum
	comf	MIOS_MPROC_CHECKSUM, F, BANKED
	incf	MIOS_MPROC_CHECKSUM, F, BANKED

	return


;; --------------------------------------------------------------------------
;;  used by read/write routine
;;  check that address has been aligned correctly and that it is within allowed range
;;  IN: address in TBLPTR[LHU]
;;      TBLPTR >= 0x00000 <= 0x07fff: internal flash (upper bits in MIOS_MPROC_ADDRESS_EXT)
;;      TBLPTR >= 0x08000 <= 0x0ffff: internal EEPROM
;;      TBLPTR >= 0x10000 <= 0x17fff: bankstick (BS number in MIOS_MPROC_ADDRESS_EXT)
;; --------------------------------------------------------------------------
MIOS_MPROC_CheckAddress
	;; upper memory region not legal
	movf	TBLPTRU, W
	andlw	0xfe
	bnz	MIOS_MPROC_CheckAddress_ER
	BRA_IFCLR TBLPTRU, 0, ACCESS, MIOS_MPROC_CheckAddress_EEpFl

MIOS_MPROC_CheckAddress_BS
	;; BankStick: 16-bit address range... valid
	rgoto	MIOS_MPROC_CheckAddress_Ok

MIOS_MPROC_CheckAddress_EEpFl
	BRA_IFCLR TBLPTRH, 7, ACCESS, MIOS_MPROC_CheckAddress_Flash

MIOS_MPROC_CheckAddress_EEPR
	;; EEPROM: address must be within 0x00-0xff (0x000-0x3ff) (PIC18F4620: 0x000-0x3ff), etc...
	movlw	HIGH(PIC_DERIVATIVE_EEPROM_SIZE) | 0x80
	cpfslt	TBLPTRH, ACCESS
	rgoto MIOS_MPROC_CheckAddress_ER
	rgoto	MIOS_MPROC_CheckAddress_Ok

MIOS_MPROC_CheckAddress_Flash
	;; due to all the variations, address space now only checked in MIOS_FLASH_Write
	;; flash: must be aligned to 0x40
	movf	TBLPTRL, W
	andlw	0x3f
	bz	MIOS_MPROC_CheckAddress_Ok
MIOS_MPROC_CheckAddress_ER
	movlw	MIOS_DISACK_WRONG_ADDR_RANGE
	rgoto	MIOS_MPROC_CheckAddress_E
MIOS_MPROC_CheckAddress_EA
	movlw	MIOS_DISACK_ADDR_NOT_ALIGNED
MIOS_MPROC_CheckAddress_E
	rcall	MIOS_MPROC_Send_DisAcknowledge
	movlw	0xee
	rgoto	MIOS_MPROC_CheckAddress_End


MIOS_MPROC_CheckAddress_Ok
	movlw	0x00
MIOS_MPROC_CheckAddress_End
	andlw	0xff		; fix STATUS
	return
	
