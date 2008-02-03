; $Id$
;
; MB64 Menu MIDI Learn Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	
;; ==========================================================================
;;  stores an event in bank
;;  IN: pot or button number in WREG, 
;;      event in MB64_LEARN_EVNT[012]
;;      button mode (on/off, on only or toggle) in MB64_LEARN_BMODE
;;      learn mode (single/row/add) in TMP5[1..0]
;;      TMP5[7] == 0: store pot event
;;      TMP5[7] == 1: store button event
;;  USES: TMP[123]
;; ==========================================================================
MB64_MEM_StoreEvent
	;; calculate pot/button entry offset: number << 2, save it in TMP1
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xfc
	movwf	TMP1

	;; align address in "learn row" mode
	movf	TMP5, W
	andlw	0x03
	bz	MB64_MEM_StoreEvent_DontAlign
	movlw	0xc0
	andwf	TMP1, F
MB64_MEM_StoreEvent_DontAlign

	;; low address for events:
	movf	TMP1, W		; (aliged for page read)
	andlw	0xc0
	movwf	MB_ADDRL

	;; high address for pot/button events:
	movlw	((MB64_Presets_OffsetPots-MB64_Presets_FLASH_Lower) >> 8) & 0x01
	btfsc	TMP5, 7
	movlw	((MB64_Presets_OffsetButtons-MB64_Presets_FLASH_Lower) >> 8) & 0x01
	movwf	MB_ADDRH

	;; read page
	lfsr	FSR1, MB64_DATA_BUFFER
	call	MB64_BANK_ReadPage

	;; fix address
	movlw	-0x40
	addwf	MB_ADDRL, F
	movlw	0xff
	addwfc	MB_ADDRH, F

	;; branch depending on pot/button
	BRA_IFSET TMP5, 7, ACCESS, MB64_MEM_StoreEvent_Button
MB64_MEM_StoreEvent_Pot
	;; min value stored in TMP2: 0x00
	clrf	TMP2
	;; max value stored in TMP3: 0x7f
	movlw	0x7f
	movwf	TMP3
	rgoto	MB64_MEM_StoreEvent_Cont
MB64_MEM_StoreEvent_Button
	;; third byte stored in TMP2
	movff	MB64_LEARN_EVNT2, TMP2
	;; button mode stored in TMP3
	movff	MB64_LEARN_BMODE, TMP3
MB64_MEM_StoreEvent_Cont

	SET_BSR MB64_BASE

	;; if "row chn", start at channel 1
	movlw	0xf0
	btfsc	TMP5, 0
	andwf	MB64_LEARN_EVNT0, F, BANKED
	
	movlw	0x3f
	andwf	TMP1, F
MB64_MEM_StoreEvent_Loop
	;; modify value(s) in page
	movf	TMP1, W
	movff	MB64_LEARN_EVNT0, PLUSW1
	addlw	1
	movff	MB64_LEARN_EVNT1, PLUSW1
	addlw	1
	movff	TMP2, PLUSW1
	addlw	1
	movff	TMP3, PLUSW1
	addlw	1
	movwf	TMP1

	movf	TMP5, W		; if neither "row chn", nor "row add" flag set, exit loop
	andlw	0x03
	bz	MB64_MEM_StoreEvent_LoopBreak

	;; if "row chn", increment channel
	btfsc	TMP5, 0
	incf	MB64_LEARN_EVNT0, F, BANKED

	;; if "row add", increment second byte, take care for overrun
	btfsc	TMP5, 1
	incf	MB64_LEARN_EVNT1, F, BANKED
	btfsc	TMP5, 1
	bcf	MB64_LEARN_EVNT1, 7, BANKED

	;; loop until end of page reached
	BRA_IFCLR TMP1, 6, ACCESS, MB64_MEM_StoreEvent_Loop
MB64_MEM_StoreEvent_LoopBreak

	;; write page and exit
	goto	MB64_BANK_WritePage


;; ==========================================================================
;;  stores a pot label in bank
;;  IN: pot number in WREG
;;      label in MB64_BANKNAME_BEGIN buffer
;;  USES: MB64_CURRENT_POT, TMP1
;; ==========================================================================
MB64_MEM_StorePotLabel
	;; calculate address to label entry
	movff	WREG, MB64_CURRENT_POT
	call	MB64_ADDR_LabelEntry

	;; read page into buffer
	lfsr	FSR1, MB64_DATA_BUFFER
	call	MB64_BANK_ReadPage

	;; (offset in 64 byte buffer is: (MB64_CURRENT_ELEMENT & 0x3) << 4
	SET_BSR	MB64_BASE
	swapf	MB64_CURRENT_ELEMENT, W, BANKED
	andlw	0x30
	addwf	FSR1L, F

	;; copy BANKNAME buffer to data buffer
	lfsr	FSR0, MB64_BANKNAME_BEGIN
	clrf	TMP1		; used as loop counter
MB64_MEM_StorePotLabelLoop
	movff	POSTINC0, POSTINC1
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_MEM_StorePotLabelLoop

	;; store page and exit
	call	MB64_ADDR_LabelEntry
	lfsr	FSR1, MB64_DATA_BUFFER
	goto	MB64_BANK_WritePage
