; $Id$
;
; SIDplayer - SysEx Parser
;
; This MIOS applications interprets the SIDstation "ASID" protocol and 
; forwards the requested register changes to a MBHP_SID module
;
; compatible with the ASID player and SIDPLAY/Windows
; -> http://www.d.kth.se/~d93-alo/c64/spw/sidplayw.html
; -> http://www.sidstation.com/sidsupport.php
; -> http://www.sidstation.com/files/asid_xp_1.0.zip
;
; based on http://sammal.ton.tut.fi/~paulus/asid_protocol.txt
; by Jouni Paulus (jobe@iki.fi)
;
; The biggest collection of SID files can be found here:
; http://www.hvsc.c64.org/
;
; This include file provides following functions:
;    o SIDPLAY_Init:     initializes the player
;    o SIDPLAY_Handler:  see function header
;
; NOTE: some registers have to be inserted in app_defines.h to get this
;       parser working:
;
;SIDPLAY_STATE		EQU	0x0140	; the SysEx state
;SIDPLAY_CMD		EQU	0x0141	; the SysEx command
;SIDPLAY_DATA_CTR	EQU	0x0142	; the SysEx Data counter
;SIDPLAY_REG_CTR	EQU	0x0143	; the SID register counter
;SIDPLAY_BUFFER		EQU	0x0144	; the mask/msb buffer (8 bytes)
;SIDPLAY_BUFFER_END	EQU	0x014b
;
;       they have to be located to free register addresses!
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDPLAY_Init
;;  DESCRIPTION: This function initializes the SIDplayer
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
SIDPLAY_Init
	SET_BSR	SIDPLAY_STATE
	clrf	SIDPLAY_STATE, BANKED
	clrf	SIDPLAY_CMD, BANKED

	;; disable SID player by default (will be enabled once first stream has been received)
	bcf	SID_STAT, SID_STAT_SIDPLAYER
	bcf	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SIDPLAY_Enable
;;  DESCRIPTION: This function enables the SIDplayer
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
SIDPLAY_Enable
	;; enable SID player mode
	bsf	SID_STAT, SID_STAT_SIDPLAYER
	bsf	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE
	;; (SID_STAT_SIDPLAYER_CS_DISABLE status will be determined again in USER_Tick)
	
	;; init LCD
	call	SIDPLAY_InitLCD
	
	;; clear LED matrix
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	clrf	PRODL
SIDPLAY_Enable_InitMatrixLoop
	clrf	POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 3, ACCESS, SIDPLAY_Enable_InitMatrixLoop

#if 0
	;; reset SID
	goto	SID_SR_Init
#else
	;; (don't reset SID --- this leads to misbehaviour when ASIDXP restarts a .sid)
	return
#endif	


;; --------------------------------------------------------------------------
;;  FUNCTION: SIDPLAY_Disable
;;  DESCRIPTION: This function disables the SIDplayer
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
SIDPLAY_Disable
	;; disable SID player mode
	bcf	SID_STAT, SID_STAT_SIDPLAYER
	bcf	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE

	;; re-initialize display
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; init patch
	goto	SID_PATCH_Init


;; --------------------------------------------------------------------------
;;  FUNCTION: SIDPLAY_InitLCD
;;  DESCRIPTION: This function initializes the LCD in SIDplayer mode
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
SIDPLAY_HEADER_STR STRING 20, 0x00, "SIDplayer Mode      "
SIDPLAY_InitLCD
	;; don't clear LCD (takes longer than overwriting characters)
	TABLE_ADDR SIDPLAY_HEADER_STR
	call	MIOS_LCD_PrintString

SIDPLAY_InitLCD_LowerLineOnly
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	20
	call	SID_LCD_PrintSpaces

	movlw	0x40
	goto	MIOS_LCD_CursorSet
	
;; --------------------------------------------------------------------------
;;  FUNCTION: SIDPLAY_NoCSMessage
;;  DESCRIPTION: Message print when CS function used in SIDPLAY mode
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
SIDPLAY_NOCS_MSG_STR STRING 18, 0x40, "Press MENU to exit"
SIDPLAY_NoCSMessage
	TABLE_ADDR SIDPLAY_NOCS_MSG_STR
	call	MIOS_LCD_PrintString
	movlw	0x40
	goto	MIOS_LCD_MessageStart

	
;; --------------------------------------------------------------------------
;;  FUNCTION: SIDPLAY_Handler
;;  DESCRIPTION: This function parses the incoming stream for ASID commands
;;  and forwards register changes to the MBHP_SID module.
;;  This function should be called from USER_MPROC_NotifyReceivedByte
;;  to forward the incoming byte
;; 
;;  The coding is described under http://sammal.ton.tut.fi/~paulus/asid_protocol.txt
;; 
;;  IN:   incoming MIDI byte in WREG
;;  OUT:  ZERO flag set if SID player active
;;  USES: -
;; --------------------------------------------------------------------------
SIDPLAY_Handler
	;; store incoming byte in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; ignore realtime events (which can be sent in between other events)
	movlw	0xf8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto SIDPLAY_Handler_End

	;; use banked accesses, select bank where SIDPLAY_STATE has been located
	;; (allows to move the SIDPLAY_* addresses above 0x7f)
	SET_BSR	SIDPLAY_STATE

	;; clear state if status byte (like 0xf0 or 0xf7...)
	btfsc	MIOS_PARAMETER1, 7
	clrf	SIDPLAY_STATE, BANKED

	;; check SysEx sequence (SIDPLAY_STATE[1:0] used as byte counter here)
	movf	SIDPLAY_STATE, W, BANKED
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE	; 4 states
	rgoto	SIDPLAY_Handler_Syx0	; checks for 0xf0
	rgoto	SIDPLAY_Handler_Syx1	; checks for 0x2d
	rgoto	SIDPLAY_Handler_GetCmd	; checks for <command>
	rgoto	SIDPLAY_Handler_GetData ; receives the <data>

	;; 0xf7 (or any other status message which is < 0xf8) will reset SIDPLAY_STATE

SIDPLAY_Handler_Syx0	; checks for 0xf0
	movlw	0xf0
	rgoto	SIDPLAY_Handler_Syx_Check
SIDPLAY_Handler_Syx1	; checks for 0x2d
	movlw	0x2d
	;; 	rgoto	SIDPLAY_Handler_Syx_Check
SIDPLAY_Handler_Syx_Check
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto SIDPLAY_Handler_Syx_Inv

	;; if byte matches, increment the state number and exit handler
SIDPLAY_Handler_Syx_Matched
	incf	SIDPLAY_STATE, F, BANKED
	rgoto	SIDPLAY_Handler_End
	
	;; if byte doesn't match, clear state number and exit handler
SIDPLAY_Handler_Syx_Inv
	clrf	SIDPLAY_STATE, BANKED
	rgoto	SIDPLAY_Handler_End


SIDPLAY_Handler_GetCmd ; gets the command
	;; increment state - next time we will continue at GetData
	incf	SIDPLAY_STATE, F, BANKED
	;; clear data counter
	clrf	SIDPLAY_DATA_CTR, BANKED
	;; copy MIDI input to CMD
	movf	MIOS_PARAMETER1, W
	movwf	SIDPLAY_CMD, BANKED
	xorlw	0x4c
	bz	SIDPLAY_Handler_InitStart
	xorlw	0x4d ^ 0x4c
	bz	SIDPLAY_Handler_InitStop
	xorlw	0x4e ^ 0x4d
	bz	SIDPLAY_Handler_InitSeq
	xorlw	0x4f ^ 0x4e
	bz	SIDPLAY_Handler_InitLCD
	rgoto	SIDPLAY_Handler_End

SIDPLAY_Handler_GetData	; handles the remaining data
	;; increment data counter
	incf	SIDPLAY_DATA_CTR, F, BANKED

	;; branch depending on command
	movf	SIDPLAY_CMD, W, BANKED
	xorlw	0x4c
	bz	SIDPLAY_Handler_CmdStart
	xorlw	0x4d ^ 0x4c
	bz	SIDPLAY_Handler_CmdStop
	xorlw	0x4e ^ 0x4d
	bz	SIDPLAY_Handler_CmdSeq
	xorlw	0x4f ^ 0x4e
	bz	SIDPLAY_Handler_CmdLCD
	rgoto	SIDPLAY_Handler_End

	;; ------------------------------------------------------------------
SIDPLAY_Handler_InitStart
	;; initialize player
	rcall	SIDPLAY_Enable	
	rgoto	SIDPLAY_Handler_End

SIDPLAY_Handler_CmdStart
	rgoto	SIDPLAY_Handler_End

	;; ------------------------------------------------------------------
SIDPLAY_Handler_InitStop
	;; disable player
	rcall	SIDPLAY_Disable
	rgoto	SIDPLAY_Handler_End

SIDPLAY_Handler_CmdStop
	rgoto	SIDPLAY_Handler_End

	;; ------------------------------------------------------------------
SIDPLAY_Handler_InitLCD
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; ensure that SID player is properly initialized (some players don't send start command!)
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER, ACCESS, SIDPLAY_Handler_InitLCD_SP_1
SIDPLAY_Handler_InitLCD_SP_0
	rcall	SIDPLAY_Enable			; (will also initialize the LCD)
	rgoto	SIDPLAY_Handler_InitLCD_SP_C
SIDPLAY_Handler_InitLCD_SP_1
	rcall	SIDPLAY_InitLCD_LowerLineOnly	; always clear lower line to remove artifacts
	;; 	rgoto	SIDPLAY_Handler_InitLCD_SP_C
SIDPLAY_Handler_InitLCD_SP_C

	rgoto	SIDPLAY_Handler_End

SIDPLAY_Handler_CmdLCD
	movf	MIOS_PARAMETER1, W
	call	MIOS_LCD_PrintChar
	rgoto	SIDPLAY_Handler_End

	;; ------------------------------------------------------------------
SIDPLAY_Handler_InitSeq
	;; clear register counter
	clrf	SIDPLAY_REG_CTR, BANKED

	;; ensure that SID player is properly initialized (some players don't send start command!)
	RCALL_IFCLR SID_STAT, SID_STAT_SIDPLAYER, ACCESS, SIDPLAY_Enable

	rgoto	SIDPLAY_Handler_End

SIDPLAY_Handler_CmdSeq
	;; data ctr <=8: receiving masks and msbs
	;; data ctr > 8: receiving data
	movlw	0x09
	cpfslt	SIDPLAY_DATA_CTR, BANKED
	rgoto SIDPLAY_Handler_CmdSeqD
SIDPLAY_Handler_CmdSeqB
	;; store mask/msb in buffer (8 bytes maximum)
	lfsr	FSR0, SIDPLAY_BUFFER
	decf	SIDPLAY_DATA_CTR, W, BANKED
	movff	MIOS_PARAMETER1, PLUSW0
	rgoto	SIDPLAY_Handler_End

SIDPLAY_Handler_CmdSeqD
SIDPLAY_Handler_CmdSeqDLoop
	;; if register counter > 0x1b, ignore next bytes
	movlw	0x1b + 1
	cpfslt	SIDPLAY_REG_CTR, BANKED
	rgoto SIDPLAY_Handler_End

	;; scan for next flag
	;; if mask flag set, transfer value to SID register
	BRA_IFSET SIDPLAY_BUFFER+0, 0, BANKED, SIDPLAY_Handler_CmdSeqDF
	;; else shift chain
	rcall	SIDPLAY_Handler_CmdSeqShift
	;; loop until next flag found
	rgoto	SIDPLAY_Handler_CmdSeqDLoop

SIDPLAY_Handler_CmdSeqDF
	;; value found:
	;; register offset in SIDPLAY_REG_CTR (partly decoded)
	;; bit 7 of register in SIDPLAY_BUFFER+4[0]
	;; bit 6-0 of register in MIOS_PARAMETER1

	;; copy MIDI In value to MIOS_PARAMETER2
	movff	MIOS_PARAMETER1, MIOS_PARAMETER2
	;; map register number to SID register address
	movf	SIDPLAY_REG_CTR, W, BANKED
	TABLE_ADDR_MUL_W SIDPLAY_Handler_CmdSeq_TAB, 1
	tblrd*+
	movf	TABLAT, W
	;; (note: reset line must stay 1)
	iorlw	0xe0
	movwf	MIOS_PARAMETER1
	;; add MSB to data word
	btfsc	SIDPLAY_BUFFER+4, 0, BANKED
	bsf	MIOS_PARAMETER2, 7

	;; EXTRA:
	;; hold ext. filter flag disabled to avoid unwanted background noise
	movlw	SIDx_RES_FCHN
	cpfseq	TABLAT, ACCESS
	rgoto SIDPLAY_Handler_CmdSeqDF_NGF
SIDPLAY_Handler_CmdSeqDF_F
	movlw	~(1 << 3)
	andwf	MIOS_PARAMETER2, F
SIDPLAY_Handler_CmdSeqDF_NGF


	;; EXTRA:
	;; check gate transition (0->1) to service animated level meters on LED matrix
	;; only if SID1 selected
	BRA_IFCLR SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, SIDPLAY_Handler_CmdSeqDF_Cont

	movf	MIOS_PARAMETER1, W
	andlw	0x1f
	xorlw	SIDx_V1_CTRL
	bnz	SIDPLAY_Handler_CmdSeqDF_NG1
SIDPLAY_Handler_CmdSeqDF_G1
	movff	SIDL_SHADOW_BASE + 0x04, WREG
	andlw	0x01
	bnz	SIDPLAY_Handler_CmdSeqDF_NG1
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, SIDPLAY_Handler_CmdSeqDF_NG1
	movlw	0x3f
#if DEFAULT_SAMMICHSID_CS
	movff	WREG, METER_VALUES1 ; sammichSID layout
	movff	WREG, METER_VALUES4
#else
	movff	WREG, METER_VALUES0
	movff	WREG, METER_VALUES1
#endif
	rgoto	SIDPLAY_Handler_CmdSeqDF_Cont
SIDPLAY_Handler_CmdSeqDF_NG1

	xorlw	SIDx_V2_CTRL ^ SIDx_V1_CTRL
	bnz	SIDPLAY_Handler_CmdSeqDF_NG2
SIDPLAY_Handler_CmdSeqDF_G2
	movff	SIDL_SHADOW_BASE + 0x0b, WREG
	andlw	0x01
	bnz	SIDPLAY_Handler_CmdSeqDF_NG2
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, SIDPLAY_Handler_CmdSeqDF_NG2
	movlw	0x3f
#if DEFAULT_SAMMICHSID_CS
	movff	WREG, METER_VALUES2 ; sammichSID layout
	movff	WREG, METER_VALUES5
#else
	movff	WREG, METER_VALUES3
	movff	WREG, METER_VALUES4
#endif
	rgoto	SIDPLAY_Handler_CmdSeqDF_Cont
SIDPLAY_Handler_CmdSeqDF_NG2

	xorlw	SIDx_V3_CTRL ^ SIDx_V2_CTRL
	bnz	SIDPLAY_Handler_CmdSeqDF_NG3
SIDPLAY_Handler_CmdSeqDF_G3
	movff	SIDL_SHADOW_BASE + 0x12, WREG
	andlw	0x01
	bnz	SIDPLAY_Handler_CmdSeqDF_NG3
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, SIDPLAY_Handler_CmdSeqDF_NG3
	movlw	0x3f
#if DEFAULT_SAMMICHSID_CS
	movff	WREG, METER_VALUES3 ; sammichSID layout
	movff	WREG, METER_VALUES6
#else
	movff	WREG, METER_VALUES6
	movff	WREG, METER_VALUES7
#endif
	rgoto	SIDPLAY_Handler_CmdSeqDF_Cont
SIDPLAY_Handler_CmdSeqDF_NG3

SIDPLAY_Handler_CmdSeqDF_Cont
	;; write to SID register
	lfsr	FSR0, SIDL_SHADOW_BASE
	lfsr	FSR1, SIDR_SHADOW_BASE
	IRQ_DISABLE
	movf	MIOS_PARAMETER2, W
	call	SID_SR_TransferB
	IRQ_ENABLE

	;; shift chain
	rcall	SIDPLAY_Handler_CmdSeqShift

	;; waiting for next MIDI byte
	rgoto	SIDPLAY_Handler_End

	
	;; ---
	;; subroutine: shift masks/msbs
SIDPLAY_Handler_CmdSeqShift
	;; shift masks and msbs to the right - take 7bit format into account!
	clrc
	rrf	SIDPLAY_BUFFER+3, F, BANKED
	skpnc
	bsf	SIDPLAY_BUFFER+2, 7, BANKED
	clrc
	rrf	SIDPLAY_BUFFER+2, F, BANKED
	skpnc
	bsf	SIDPLAY_BUFFER+1, 7, BANKED
	clrc
	rrf	SIDPLAY_BUFFER+1, F, BANKED
	skpnc
	bsf	SIDPLAY_BUFFER+0, 7, BANKED
	clrc
	rrf	SIDPLAY_BUFFER+0, F, BANKED

	;; shift masks and msbs to the right
	clrc
	rrf	SIDPLAY_BUFFER+7, F, BANKED
	skpnc
	bsf	SIDPLAY_BUFFER+6, 7, BANKED
	clrc
	rrf	SIDPLAY_BUFFER+6, F, BANKED
	skpnc
	bsf	SIDPLAY_BUFFER+5, 7, BANKED
	clrc
	rrf	SIDPLAY_BUFFER+5, F, BANKED
	skpnc
	bsf	SIDPLAY_BUFFER+4, 7, BANKED
	clrc
	rrf	SIDPLAY_BUFFER+4, F, BANKED

	;; increment register number
	incf	SIDPLAY_REG_CTR, F, BANKED

	return

	;; ------------------------------------------------------------------
SIDPLAY_Handler_End
	return

	;; ---
	;; map register number to SID register address
SIDPLAY_Handler_CmdSeq_TAB
	db	0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0c, 0x0d, 0x0e, 0x0f
	db	0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x04, 0x0b, 0x12, 0x04, 0x0b, 0x12
