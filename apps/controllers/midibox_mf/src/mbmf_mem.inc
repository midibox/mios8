; $Id$
;
; MBMF Menu MIDI Learn Handler
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
;;  IN: fader or button number in WREG, 
;;      event in MBMF_LEARN_EVNT[012]
;;      button mode (on/off, on only or toggle) in MBMF_LEARN_BMODE
;;      learn mode (single/row/add) in TMP5[1..0]
;;      TMP5[7] == 0: store fader event
;;      TMP5[7] == 1: store button event
;;  USES: TMP[123]
;; ==========================================================================
MBMF_MEM_StoreEvent
	andlw	0x7f
	;; calculate fader/button entry offset: number*4, saved it in TMP1 and PROD[LH]
	mullw	4
	movff	PRODL, TMP1

	;; align address in "learn row" mode
	movf	TMP5, W
	andlw	0x03
	bz	MBMF_MEM_StoreEvent_DontAlign
	movlw	0xc0
	andwf	TMP1, F
MBMF_MEM_StoreEvent_DontAlign

	;; low address for events:
	movf	TMP1, W		; (aliged for page read)
	andlw	0xc0
	movwf	MB_ADDRL

	;; high address for fader/button events:
	movlw	((MBMF_Presets_OffsetFaders-MBMF_Presets_FLASH_Lower) >> 8) & 0x03
	btfsc	TMP5, 7
	movlw ((MBMF_Presets_OffsetButtons-MBMF_Presets_FLASH_Lower) >> 8) & 0x03
	addwf	PRODH, W
	movwf	MB_ADDRH

	;; read page
	lfsr	FSR1, MBMF_DATA_BUFFER
	call	MBMF_BANK_ReadPage

	;; fix address
	movlw	-0x40
	addwf	MB_ADDRL, F
	movlw	0xff
	addwfc	MB_ADDRH, F

	;; branch depending on fader/button
	BRA_IFSET TMP5, 7, ACCESS, MBMF_MEM_StoreEvent_Button
MBMF_MEM_StoreEvent_Fader
	;; min value stored in TMP2: 0x00
	clrf	TMP2
	;; max value stored in TMP3: 0x7f
	movlw	0x7f
	movwf	TMP3
	rgoto	MBMF_MEM_StoreEvent_Cont
MBMF_MEM_StoreEvent_Button
	;; third byte stored in TMP2
	movff	MBMF_LEARN_EVNT2, TMP2
	;; button mode stored in TMP3
	movff	MBMF_LEARN_BMODE, TMP3
MBMF_MEM_StoreEvent_Cont

	SET_BSR MBMF_BASE

	;; if "row chn", start at channel 1
	movlw	0xf0
	btfsc	TMP5, 0
	andwf	MBMF_LEARN_EVNT0, F, BANKED
	
	movlw	0x3f
	andwf	TMP1, F
MBMF_MEM_StoreEvent_Loop
	;; modify value(s) in page
	movf	TMP1, W
	movff	MBMF_LEARN_EVNT0, PLUSW1
	addlw	1
	movff	MBMF_LEARN_EVNT1, PLUSW1
	addlw	1
	movff	TMP2, PLUSW1
	addlw	1
	movff	TMP3, PLUSW1
	addlw	1
	movwf	TMP1

	movf	TMP5, W		; if neither "row chn", nor "row add" flag set, exit loop
	andlw	0x03
	bz	MBMF_MEM_StoreEvent_LoopBreak

	;; if "row chn", increment channel
	btfsc	TMP5, 0
	incf	MBMF_LEARN_EVNT0, F, BANKED

	;; if "row add", increment second byte, take care for overrun
	btfsc	TMP5, 1
	incf	MBMF_LEARN_EVNT1, F, BANKED
	btfsc	TMP5, 1
	bcf	MBMF_LEARN_EVNT1, 7, BANKED

	;; loop until end of page reached
	BRA_IFCLR TMP1, 6, ACCESS, MBMF_MEM_StoreEvent_Loop
MBMF_MEM_StoreEvent_LoopBreak

	;; write page and exit
	goto	MBMF_BANK_WritePage


;; ==========================================================================
;;  stores a fader label in bank
;;  IN: fader number in WREG
;;      label in MBMF_BANKNAME_BEGIN buffer
;;  USES: MBMF_CURRENT_FADER, TMP1
;; ==========================================================================
MBMF_MEM_StoreFaderLabel
	;; calculate address to label entry
	movff	WREG, MBMF_CURRENT_FADER
	call	MBMF_ADDR_LabelEntry

	;; read page into buffer
	lfsr	FSR1, MBMF_DATA_BUFFER
	call	MBMF_BANK_ReadPage

	;; (offset in 64 byte buffer is: (MBMF_CURRENT_ELEMENT & 0x3) << 4
	SET_BSR	MBMF_BASE
	swapf	MBMF_CURRENT_ELEMENT, W, BANKED
	andlw	0x30
	addwf	FSR1L, F

	;; copy BANKNAME buffer to data buffer
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
	clrf	TMP1		; used as loop counter
MBMF_MEM_StoreFaderLabelLoop
	movff	POSTINC0, POSTINC1
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_MEM_StoreFaderLabelLoop

	;; store page and exit
	call	MBMF_ADDR_LabelEntry
	lfsr	FSR1, MBMF_DATA_BUFFER
	goto	MBMF_BANK_WritePage
