; $Id$
;
; MIOS MIDI LCD Driver (1st layer routines)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; driver can be optionally disabled
#if MIOS_ENABLE_MLCD


;; --------------------------------------------------------------------------
;;  Init Routine for LC Display
;; --------------------------------------------------------------------------
MIOS_MLCD_Init
	rgoto	MIOS_MLCD_Clear
	
;; --------------------------------------------------------------------------
;;  Print Character
;; --------------------------------------------------------------------------
MIOS_MLCD_PrintChar
	;; notify that no graphical LCD is connected
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD

	SET_BSR	MIOS_GLCD_BUFFER
	movwf	MIOS_GLCD_BUFFER, BANKED

	RCALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_MLCD_TRANSFER, ACCESS, MIOS_MLCD_SendSyxHeaderChr

	SET_BSR	MIOS_GLCD_BUFFER
	movf	MIOS_GLCD_BUFFER, W, BANKED
	andlw	0x7f
	goto	MIOS_MIDI_TxBufferPut

;; ==========================================================================

MIOS_MLCD_Clear
	rcall	MIOS_MLCD_SendSyxHeaderClr
	rgoto	MIOS_MLCD_SendSyxFooter

;; ==========================================================================

MIOS_MLCD_CursorSet
	rcall	MIOS_MLCD_SendSyxHeaderPosChr
	SET_BSR	MIOS_LCD_CURSOR_POS
	movf	MIOS_LCD_CURSOR_POS, W, BANKED
	andlw	0x3f
	call	MIOS_MIDI_TxBufferPut
	swapf	MIOS_LCD_CURSOR_POS, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	call	MIOS_MIDI_TxBufferPut
	rgoto	MIOS_MLCD_SendSyxFooter

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  MLCD: send SysEx header
;; --------------------------------------------------------------------------
MIOS_MLCD_SendSyxHeader
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_MLCD_TRANSFER

	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	movlw	0x7e
	call	MIOS_MIDI_TxBufferPut
	movlw	0x40
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MIOS_LCD_OPTION1	; (device ID)
	movf	MIOS_LCD_OPTION1, W, BANKED
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movlw	0x08
	goto	MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
;;  MLCD: send SysEx header for Clear Command
;; --------------------------------------------------------------------------
MIOS_MLCD_SendSyxHeaderClr
	call	MIOS_MLCD_SendSyxHeader
	movlw	0x00
	goto	MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
;;  MLCD: send SysEx header for PosChr Command
;; --------------------------------------------------------------------------
MIOS_MLCD_SendSyxHeaderPosChr
	call	MIOS_MLCD_SendSyxHeader
	movlw	0x01
	goto	MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
;;  MLCD: send SysEx header for Chr Command
;; --------------------------------------------------------------------------
MIOS_MLCD_SendSyxHeaderChr
	call	MIOS_MLCD_SendSyxHeader
	movlw	0x02
	goto	MIOS_MIDI_TxBufferPut

	
;; --------------------------------------------------------------------------
;;  MLCD: send SysEx footer
;; --------------------------------------------------------------------------
MIOS_MLCD_SendSyxFooter
	btfss	MIOS_BOX_STAT, MIOS_BOX_STAT_MLCD_TRANSFER
	return
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_MLCD_TRANSFER
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut

#endif	; MIOS_ENABLE_MLCD
