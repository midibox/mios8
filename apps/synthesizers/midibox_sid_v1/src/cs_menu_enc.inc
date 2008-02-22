; $Id$
;
; Control Surface Encoder Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: incrementer of menu encoder in WREG
;; --------------------------------------------------------------------------
CS_MENU_ENC_Handler
	;; store incrementer in TMP3
	movwf	TMP3

	;; special measure for main page (where the encoder selects a patch) - especially when
	;; the patch for all 4 SIDs is changed, it can take 100 ms and more until this routine
	;; is called again. During this time the "incrementer" value will be increased/decreased
	;; by the encoder driver in background, incrementer values > than 16 could be returned. 
	;; To avoid those unwanted jumps during patch selection, we ensure here that the 
	;; incrementer is either 1 or -1, but never greater than this value
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_ENC_Handler_NotPatchSel
	movlw	1
	btfsc	TMP3, 7
	movlw -1
	movwf	TMP3
CS_MENU_ENC_Handler_NotPatchSel

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, CS_MENU_PARAMETER_L
	;; set max value
	movff	CS_MENU_PARAMETER_MAX_L, MIOS_PARAMETER1	; low-byte of max value
	movff	CS_MENU_PARAMETER_MAX_H, MIOS_PARAMETER2	; high-byte of max value
	;; get incrementer (which has been stored in TMP3)
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; if main page (CS_MENU[7] set): update patch and init sound
	GOTO_IFSET CS_MENU, 7, ACCESS, CS_MENU_PatchUpdate

	;; if not in selection mode: update cursor pos
	GOTO_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_MenuPosUpdate

	;; else do a parameter update
	goto	CS_MENU_ParameterUpdate


;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: encoder number (-1) in MIOS_PARAMETER1
;;      incrementer in MIOS_PARAMETER2
;;  NOTE: the menu encoder will not be counted as encoder #0 here to avoid
;;        problems with the INCDEC_BUTTONS option
;; --------------------------------------------------------------------------
CS_MENU_ENC_CS_Handler
	;; move incrementer to MIOS_PARAMETER3
	movff	MIOS_PARAMETER2, MIOS_PARAMETER3

	;; determine offset (quick & dirty, should be programmed on a more elegant way later)
	movlw	0x04
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_Osc
	movlw	0x06
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_LFO
	movlw	0x08
	cpfsgt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_Filter
	rgoto	CS_MENU_ENC_CS_Env
;; ---
CS_MENU_ENC_CS_Osc
	movf	MIOS_PARAMETER1, W
	btfsc	CS_MENU_SELECTED_OSC_CTRL, 0; Misc Layer
	addlw 0x05   
	btfsc	CS_MENU_SELECTED_OSC_CTRL, 1; Assign Layer
	addlw 0x0a   
	rgoto	CS_MENU_ENC_CS_Cont
CS_MENU_ENC_CS_LFO
	movf	MIOS_PARAMETER1, W
	addlw	0x0f-0x05
	rgoto	CS_MENU_ENC_CS_Cont
CS_MENU_ENC_CS_Filter
	movf	MIOS_PARAMETER1, W
	addlw	0x11-0x07
	rgoto	CS_MENU_ENC_CS_Cont
CS_MENU_ENC_CS_Env
	movf	MIOS_PARAMETER1, W
	addlw	0x13-0x09
	btfsc	CS_MENU_SELECTED_ENV_CTRL, 0
	addlw 0x05
	;; 	rgoto	CS_MENU_ENC_CS_Cont
;; ---
CS_MENU_ENC_CS_Cont
	;; calc pointer to table entry: CS_MENU_ENC_TABLE + index * 4
	mullw	4
	TABLE_ADDR CS_MENU_ENC_TABLE
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; get function and store it in TMP1
	tblrd*+
	movff	TABLAT, TMP1

	;; get offset to SID data base
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDBase
	
	;; get parameter value and add it to FSR0L
	tblrd*+
	movf	TABLAT, W
	addwf	FSR0L, F

	;; get menu number and store it in TMP2
	tblrd*+
	movff	TABLAT, TMP2

	;; get menu offset and pos
	tblrd*+

	;; store cursor position in MIOS_PARAMETER1
	movf	TABLAT, W
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; store page offset in MIOS_PARAMETER2
	swapf	TABLAT, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
	
	;; now branch depending on function
	swapf	TMP1, W
	andlw	0x0f
	JUMPTABLE_2BYTES 7	; 7 entries
	rgoto	CS_MENU_ENC_CS_Change_Sys
	rgoto	CS_MENU_ENC_CS_Change_Osc
	rgoto	CS_MENU_ENC_CS_Change_LFO
	rgoto	CS_MENU_ENC_CS_Change_Env
	rgoto	CS_MENU_ENC_CS_Change_Menu
	rgoto	CS_MENU_ENC_CS_CCRel
	rgoto	CS_MENU_ENC_CS_CCAbs

;; ---	
CS_MENU_ENC_CS_Change_Sys
	;; just modify current parameter
	movf	TMP2, W		; load menu number
	goto	CS_MENU_ENC_CSInc

CS_MENU_ENC_CS_Change_Osc
	;; add voice offset
	call	CS_MENU_Hlp_AddVoiceOffset
	;; and change parameter
	rgoto	CS_MENU_ENC_CS_Change_Sys

CS_MENU_ENC_CS_Change_LFO
	;; add LFO offset
	call	CS_MENU_Hlp_AddLFOOffset
	;; and change parameter
	rgoto	CS_MENU_ENC_CS_Change_Sys

CS_MENU_ENC_CS_Change_Env
	;; add ENV offset
	call	CS_MENU_Hlp_AddENVOffset
	;; and change parameter
	rgoto	CS_MENU_ENC_CS_Change_Sys

CS_MENU_ENC_CS_Change_Menu
	;; ignore if in main or menu page
	movf	CS_MENU, W
	andlw	0x7f
	bz	CS_MENU_ENC_CS_Change_Menu_End
	;; else modify value of menu depending on given offset in TMP1
	movf	TMP1, W
	andlw	0x0f
	addwf	CS_MENU_PAGE_OFFSET, W
        movwf   MIOS_PARAMETER1
        ;; ignore if parameter pos >= number of entries
        movf    CS_MENU_ENTRIES, W
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_ENC_CS_Change_Menu_End
        ;; else modify parameter
        movff   CS_MENU_PAGE_OFFSET, MIOS_PARAMETER2
	movf	CS_MENU, W
	goto	CS_MENU_ENC_CSInc
CS_MENU_ENC_CS_Change_Menu_End	
	return

CS_MENU_ENC_CS_CCRel
	movlw	0x40
	addwf	MIOS_PARAMETER3, F
	movff	FSR0L, TMP2	; FSR0L holds the parameter (here used as CC number)

	;; send a CC, expects channel number in TMP1[3..0], CC number in TMP2, value in MIOS_PARAMETER3
CS_MENU_ENC_CS_CCAbs_Cont
	call	MIOS_MIDI_BeginStream
	movf	TMP1, W		; low-nibble of TMP1 (function) holds the channel
	andlw	0x0f
	iorlw	0xb0
	call	MIOS_MIDI_TxBufferPut
	movf	TMP2, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movf	MIOS_PARAMETER3, W; MIOS_PARAMETER3 holds the incrementer
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	goto	MIOS_MIDI_EndStream

CS_MENU_ENC_CS_CCAbs
	;; send an absolute value
	movff	FSR0L, TMP2	; FSR0L holds the parameter (here used as CC number)
	;; calc address to abs val. register
	lfsr	FSR0, CS_MENU_ENC_ABS_VAL_BEGIN
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	addwf	FSR0L, F

	;; copy value to TMP3, clear TMP4
	movff	INDF0, TMP3
	clrf	TMP4

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to TMP3 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP3
	;; set max value
	movlw	0x7f
	movwf	MIOS_PARAMETER1			; low-byte of max value
	clrf	MIOS_PARAMETER2
	;; get incrementer (which has been stored in MIOS_PARAMETER3)
	movf	MIOS_PARAMETER3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed
	btfss	MIOS_PARAMETER1, 0; do nothing if no change
	return	
	;; else send CC
	movff	INDF1, INDF0
	movff	INDF0, MIOS_PARAMETER3
	rgoto	CS_MENU_ENC_CS_CCAbs_Cont
