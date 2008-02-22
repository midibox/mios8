; $Id$
;
; SID Control Surface Master/Slave SID Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Determines the current SID number depending on the selected SIDs
;;  Result in WREG and CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_GetSIDNumber
	;; set CS_MENU_SID depending on first selected SID
	movlw	0x03
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 2
	movlw	0x02
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 1
	movlw	0x01
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 0
	movlw	0x00
	movwf	CS_MENU_SID
	return


;; --------------------------------------------------------------------------
;;  copy base address of SID to FSR0
;; --------------------------------------------------------------------------
CS_MENU_MS_GetSIDBase
	;; branch depending on selected SID
	BRA_IFSET CS_MENU_SID, 1, ACCESS, CS_MENU_MS_GetSIDBase_23

CS_MENU_MS_GetSIDBase_01
	BRA_IFSET CS_MENU_SID, 0, ACCESS, CS_MENU_MS_GetSIDBase_1
CS_MENU_MS_GetSIDBase_0
	lfsr	FSR0, CS_SIDM_BASE
	return
CS_MENU_MS_GetSIDBase_1
	lfsr	FSR0, CS_SIDS1_BASE
	return

CS_MENU_MS_GetSIDBase_23
	BRA_IFSET CS_MENU_SID, 0, ACCESS, CS_MENU_MS_GetSIDBase_3
CS_MENU_MS_GetSIDBase_2
	lfsr	FSR0, CS_SIDS2_BASE
	return
CS_MENU_MS_GetSIDBase_3
	lfsr	FSR0, CS_SIDS3_BASE
	return

;; --------------------------------------------------------------------------
;;  If a parameter should be sent
;;  value in INDF0 pointer (and sysex index) of parameter in FSR0L & 0x7f
;; --------------------------------------------------------------------------
CS_MENU_MS_SendParameter
	;; get corresponding CC parameter from address
	movf	FSR0L, W
	andlw	0x7f
	call	SID_SYSEX_TABLE_Get
	movwf	TMP1		; save result in TMP1

	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_SendParameter_Slave

CS_MENU_MS_SendParameter_Master
	;; set parameter directly if split point (TMP1[7..4] == 9)
	movf	TMP1, W
	andlw	0xf0
	xorlw	0x90
	bnz	CS_MENU_MS_SendParameter_Master2
CS_MENU_MS_SendParameter_Master1
	movf	TMP1, W
	call	SID_PATCH_GetSplitPointer
	movff	INDF0, INDF1
	rgoto	CS_MENU_MS_SendParameter_MasterE

CS_MENU_MS_SendParameter_Master2
	;; do nothing if EEPROM data
	btfsc	TMP1, 7
	return

	;; send CC value to internal SID engine
	movf	TMP1, W
	movff	INDF0, MIOS_PARAMETER1
	call	SID_CCIN_Set

	;; send CC if flag set
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_CC, ACCESS, CS_MENU_MS_SendParameter_CC
	;; else exit

CS_MENU_MS_SendParameter_MasterE
	return
	

CS_MENU_MS_SendParameter_Slave
	;; continue if split point (TMP1[7..4] == 9)
	movf	TMP1, W
	andlw	0xf0
	xorlw	0x90
	bz	CS_MENU_MS_SendParameter_SlaveS

	;; do nothing if EEPROM data
	btfsc	TMP1, 7
	return

	;; branch to CC if flag set
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_CC, ACCESS, CS_MENU_MS_SendParameter_CC

CS_MENU_MS_SendParameter_SlaveS
	;; else send SysEx
	
	;; send SysEx header
	rcall	CS_MENU_MS_Send_SysExHeader

	;; send PAR_Write ID
	movlw	0x06
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send AH
	movlw	0x00
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send AL
	movf	FSR0L, W
	andlw	0x7f
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send value
	movf	INDF0, W
	andlw	0x7f
	rcall	CS_MENU_MS_Send_MIDIByte
	
	;; send of SysEx footer and exit
	rgoto	CS_MENU_MS_Send_SysExFooter


	;; CC handler
CS_MENU_MS_SendParameter_CC
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	movf	PLUSW1, W
	andlw	0x0f
	iorlw	0xb0
	rcall	CS_MENU_MS_Send_MIDIByte
	movf	FSR0L, W
	andlw	0x7f
	movf	TMP1, W
	rcall	CS_MENU_MS_Send_MIDIByte
	movf	INDF0, W
	andlw	0x7f
	rgoto	CS_MENU_MS_Send_MIDIByte

;; --------------------------------------------------------------------------
;;  If update of all parameters has been requested
;;  expecting SID number in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateAllParameters
	;; prevent buffer loss of currently edited parameters
	;; grant EEPROM if requested
	btfsc	CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ
	bsf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_GNT
	;; update EEPROM data
	call	CS_MENU_Write_EEPROM

	;; send dump
	rcall	CS_MENU_MS_Send_SysExDump

	;; update patch name
	rcall	CS_MENU_MS_UpdatePatchName

	;; copy first half of dump to CS registers

	;; fetch 127 SID parameters from EEPROM
	rcall	CS_MENU_MS_GetSIDBase
	clrf	EEADR
CS_MENU_UpdateAllParam_Loop
	rcall	CS_MENU_BANK_Read
	movwf	POSTINC0
	movlw	0x7c-1
	cpfsgt	EEADR, ACCESS
	rgoto CS_MENU_UpdateAllParam_Loop

	;; if in WT mode: update WT buffer
	movf	CS_MENU, W
	xorlw	CS_MENU_WT
	skpnz
	rcall	CS_MENU_MS_Update_WT_Buffer

	;; get current patch number
	;; copy current patch number to WT patch number
	rcall	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_WT_PATCH
	movff	SID_PATCH, PLUSW0

	;; force a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; if in main page: force complete re-initialization
	btfsc	CS_MENU, 7
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

;; --------------------------------------------------------------------------
;;  This help function updates the WT buffer depending on CS_MENU_WT_POS
;;  (meaning here: WT position)
;;  In: position in CS_MENU_WT_POS
;;      if position >= 0x20, always the first entry will be copied into the buffer
;; --------------------------------------------------------------------------
CS_MENU_MS_Update_WT_Buffer
	;; copy pos to CS_MENU_WT_POS_BUFFER
	movff	CS_MENU_WT_POS, CS_MENU_WT_POS_BUFFER
	;; clear modified flags
	clrf	CS_MENU_WT_MODIFIED

	;; prepare WT access
	rcall	CS_MENU_MS_Update_WT_Hlp

	clrf	TMP1
CS_MENU_MS_Update_WT_Buffer_Loop
	rcall	CS_MENU_BANK_Read
	andlw	0x7f
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 2, ACCESS, CS_MENU_MS_Update_WT_Buffer_Loop

	;; copy "relative mode" flags
	movf	POSTDEC0, W	; switch back to first byte (first decrement via POSTDEC to take FSR0H into account)
	movlw	-3
	addwf	FSR0L, F
	movff	INDF0, TMP1	; copy first byte to TMP1
	movlw	0x0f		; clear flags
	andwf	POSTINC0, F

	bcf	INDF0, 7	; copy bit 4 of first byte to bit 7 of second byte
	btfsc	TMP1, 4
	bsf	INDF0, 7
	btfss	TMP1, 4		; toggle negate flag if relative mode
	btg	INDF0, 6
	incf	FSR0L, F	; copy bit 5 of first byte to bit 7 of third byte
	bcf	INDF0, 7
	btfsc	TMP1, 5
	bsf	INDF0, 7
	btfss	TMP1, 5		; toggle negate flag if relative mode
	btg	INDF0, 6
	incf	FSR0L, F	; copy bit 6 of first byte to bit 7 of fourth byte
	bcf	INDF0, 7
	btfsc	TMP1, 6
	bsf	INDF0, 7
	btfss	TMP1, 6		; toggle negate flag if relative mode
	btg	INDF0, 6

	return

;; -- prepare WT access
CS_MENU_MS_Update_WT_Hlp
	;; calc pointer to EEPROM: 0x80 + CS_MENU_WT_POS << 2
	;; if WT pos >= 0x20: select first position
	movlw	0x20-1
	cpfsgt	CS_MENU_WT_POS, ACCESS
	rgoto CS_MENU_MS_Update_WT_Hlp_Normal
CS_MENU_MS_Update_WT_Hlp_Comb
	movlw	0x80
	movwf	EEADR
	rgoto	CS_MENU_MS_Update_WT_Hlp_Cont
CS_MENU_MS_Update_WT_Hlp_Normal
	rlf	CS_MENU_WT_POS, W
	rlf	WREG, W
	andlw	0x7c
	addlw	0x80
	movwf	EEADR
CS_MENU_MS_Update_WT_Hlp_Cont

	;; calc pointer to buffer
	rcall	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_WT_MODE
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function writes the WT buffer to EEPROM
;;  In: position in CS_MENU_WT_POS
;; --------------------------------------------------------------------------
CS_MENU_MS_Update_WT_EEPROM
	;; don't write if current pos != buffer pos
	movf	CS_MENU_WT_POS_BUFFER, W
	xorwf	CS_MENU_WT_POS, W
	bnz	CS_MENU_MS_Update_WT_EEPROM_End; (will clear CS_MENU_WT_MODIFIED!)

	;; don't write if nothing has been updated
	movf	CS_MENU_WT_MODIFIED, W
	bz	CS_MENU_MS_Update_WT_EEPROM_End

	clrf	TMP5		; using TMP5 as loop counter if Pos == "All"
CS_MENU_MS_Update_WT_EEPROMLoop
	;; prepare WT access
	rcall	CS_MENU_MS_Update_WT_Hlp	; always returns first entry if CS_MENU_WT_POS >= 0x20
	movf	TMP5, W				; add loop counter to EEADR (will be incremented by 4 on each loop)
	addwf	EEADR, F			; (only relevant if Pos == "All"

	;; copy old content to TMP[1234]
	call	CS_MENU_BANK_Read
	movwf	TMP1
	call	CS_MENU_BANK_Read
	movwf	TMP2
	call	CS_MENU_BANK_Read
	movwf	TMP3
	call	CS_MENU_BANK_Read
	movwf	TMP4
	movlw	-4		; switch back to first byte
	addwf	EEADR, F

	;; copy all *modified* parameters to TMP[1234]

	;; if modify flag for step mode is set, overwrite bitfield[3:0]
	BRA_IFCLR CS_MENU_WT_MODIFIED, 0, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M0N
CS_MENU_MS_Update_WT_EEPROM_M0
	movlw	0xf0
	andwf	TMP1, F
	movf	INDF0, W
	andlw	0x0f
	iorwf	TMP1, F
CS_MENU_MS_Update_WT_EEPROM_M0N;ot

	;; if modify flag of parameter 1 is set, copy new value to TMP2/TMP1[4]
	incf	FSR0L, F
	BRA_IFCLR CS_MENU_WT_MODIFIED, 1, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M1N
CS_MENU_MS_Update_WT_EEPROM_M1
	bcf	TMP1, 4		; 7th bit of second value to 4th bit of first byte
	btfsc	INDF0, 7
	bsf	TMP1, 4
	movf	INDF0, W	; copy value to TMP2
	btfss	TMP1, 4		; toggle negate flag if relative mode
	xorlw	0x40
	andlw	0x7f
	movwf	TMP2
CS_MENU_MS_Update_WT_EEPROM_M1N;ot

	;; if modify flag of parameter 2 is set, copy new value into TMP3/TMP1[5]
	incf	FSR0L, F
	BRA_IFCLR CS_MENU_WT_MODIFIED, 2, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M2N
CS_MENU_MS_Update_WT_EEPROM_M2
	bcf	TMP1, 5		; 7th bit of third value to 5th bit of first byte
	btfsc	INDF0, 7
	bsf	TMP1, 5
	movf	INDF0, W	; copy value to TMP3
	btfss	TMP1, 5		; toggle negate flag if relative mode
	xorlw	0x40
	andlw	0x7f
	movwf	TMP3
CS_MENU_MS_Update_WT_EEPROM_M2N;ot

	;; if modify flag of parameter 3 is set, copy new value into TMP4/TMP1[6]
	incf	FSR0L, F
	BRA_IFCLR CS_MENU_WT_MODIFIED, 3, ACCESS, CS_MENU_MS_Update_WT_EEPROM_M3N
CS_MENU_MS_Update_WT_EEPROM_M3
	bcf	TMP1, 6		; 7th bit of fourth value to 6th bit of first byte
	btfsc	INDF0, 7
	bsf	TMP1, 6
	movf	INDF0, W	; copy value to TMP4
	btfss	TMP1, 6		; toggle negate flag if relative mode
	xorlw	0x40
	andlw	0x7f
	movwf	TMP4
CS_MENU_MS_Update_WT_EEPROM_M3N;ot

	;; store modified EEPROM content
	movf	TMP1, W
	rcall	CS_MENU_BANK_Write
	movf	TMP2, W
	rcall	CS_MENU_BANK_Write
	movf	TMP3, W
	rcall	CS_MENU_BANK_Write
	movf	TMP4, W
	rcall	CS_MENU_BANK_Write

	;; if slave is selected: transfer new data immediately
	movf	CS_MENU_SID, W
	bz	CS_MENU_MS_Update_WT_EEPROMLpNxt

CS_MENU_MS_Update_WT_EEPROM_S
	lfsr	FSR0, TMP1
CS_MENU_MS_Update_WT_EEPROM_SLp
	;; send SysEx header
	rcall	CS_MENU_MS_Send_SysExHeader

	;; send PAR_Write ID
	movlw	0x06
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send AH
	movlw	0x01
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send AL
	movf	EEADR, W
	andlw	0x7f
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send value
	movf	POSTINC0, W
	andlw	0x7f
	rcall	CS_MENU_MS_Send_MIDIByte
	
	;; send of SysEx footer
	rcall	CS_MENU_MS_Send_SysExFooter

	;; increment EEADR
	incf	EEADR, F

	;; wait for 5 ms
	movlw	5
	call	MIOS_Delay

	;; loop 4 times (until EEADR & 0x03 is 0 again)
	movf	EEADR, W
	andlw	0x03
	bnz	CS_MENU_MS_Update_WT_EEPROM_SLp

CS_MENU_MS_Update_WT_EEPROMLpNxt
	;; if WT_POS >= 0x20 ("All"), increment TMP5 by 4 and loop until it has reached 0x80
	movlw	0x20-1
	cpfsgt	CS_MENU_WT_POS, ACCESS
	rgoto CS_MENU_MS_Update_WT_EEPROM_End
	clrwdt			; feed watchdog (for long loops)
	movlw	4
	addwf	TMP5, F
	BRA_IFCLR TMP5, 7, ACCESS, CS_MENU_MS_Update_WT_EEPROMLoop

CS_MENU_MS_Update_WT_EEPROM_End
	clrf	CS_MENU_WT_MODIFIED	; clear all modified flags
	return


;; --------------------------------------------------------------------------
;;  copy patch name to SID_CS buffer
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdatePatchName
	movlw	EEPROM_PATCHNAME & 0xff
	movwf	EEADR
	rcall	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_NAME_x
	addwf	FSR0L, F
	clrf	TMP1			; TMP1 used as loop counter
CS_MENU_MS_UpdatePatchName_Loop
	rcall	CS_MENU_BANK_Read
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_MENU_MS_UpdatePatchName_Loop; loop 16 times
	return

;; --------------------------------------------------------------------------
;;  some util routines for the SID_PATCH and SID_BANK handling
;; --------------------------------------------------------------------------
	;; set SID_PATCH/BANK to patch/bank number of current selected SID
	;; doesn't overwrite WREG!
CS_MENU_MS_SetSIDPatch_C
	;; branch depending on selected SID
	BRA_IFSET CS_MENU_SID, 1, ACCESS, CS_MENU_MS_SetSIDPatch_C_23

CS_MENU_MS_SetSIDPatch_C_01
	BRA_IFSET CS_MENU_SID, 0, ACCESS, CS_MENU_MS_SetSIDPatch_C_1
CS_MENU_MS_SetSIDPatch_C_0
	movff	CS_MENU_SID_M_PATCH, SID_PATCH
	movff	CS_MENU_SID_M_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End
CS_MENU_MS_SetSIDPatch_C_1
	movff	CS_MENU_SID_S1_PATCH, SID_PATCH
	movff	CS_MENU_SID_S1_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End

CS_MENU_MS_SetSIDPatch_C_23
	BRA_IFSET CS_MENU_SID, 0, ACCESS, CS_MENU_MS_SetSIDPatch_C_3
CS_MENU_MS_SetSIDPatch_C_2
	movff	CS_MENU_SID_S2_PATCH, SID_PATCH
	movff	CS_MENU_SID_S2_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End
CS_MENU_MS_SetSIDPatch_C_3
	movff	CS_MENU_SID_S3_PATCH, SID_PATCH
	movff	CS_MENU_SID_S3_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End

CS_MENU_MS_SetSIDPatch_C_End
	;; mask out 'sent' flag
	bcf	SID_PATCH, 7
	return


	;; set SID_BANK to patch number of master SID
	;; doesn't overwrite WREG!
CS_MENU_MS_SetSIDPatch_M
	movff	CS_MENU_SID_M_PATCH, SID_PATCH
	movff	CS_MENU_SID_M_BANK, SID_BANK
	;; mask out 'sent' flag
	bcf	SID_PATCH, 7
	return

	;; get patch number of current selected SID
	;; uses FSR1!
CS_MENU_MS_GetSIDPatch
	lfsr	FSR1, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x7f
	return

	;; get bank number of current selected SID
	;; uses FSR1!
CS_MENU_MS_GetSIDBank
	lfsr	FSR1, CS_MENU_SID_M_BANK
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x07
	return

;; --------------------------------------------------------------------------
;;  This function is called to forward a Program Change event to the control surface
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second  MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
CS_MENU_MS_NotifyProgramChange
	;; store channel and patch number
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1		; chn
	movf	MIOS_PARAMETER2, W
	andlw	0x7f
	movwf	TMP2		; patch

	;; request patch send to SIDs with matching channel number
	clrf	TMP3				; TMP3 used as counter
CS_MENU_MS_NotifyProgramChangeL
	;; calc pointer to channel entry
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	TMP3, W
	addwf	FSR0L, F

	;; check if channel number matches
	movf	INDF0, W
	xorwf	TMP1, W
	andlw	0x0f
	bnz	CS_MENU_MS_NotifyProgramChangeLN
	;; store patch number
	movlw	(CS_MENU_SID_M_PATCH-CS_MENU_SID_M_CHN) & 0xff
	addwf	FSR0L, F
	movff	TMP2, INDF0
	;; skip if patch change already requested
	movlw	(CS_MENU_TX_M_CTR-CS_MENU_SID_M_PATCH) & 0xff
	addwf	FSR0L, F
	BRA_IFSET INDF0, 7, ACCESS, CS_MENU_MS_NotifyProgramChangeLN
	;; request patch change
	movlw	0x81
	movwf	INDF0
CS_MENU_MS_NotifyProgramChangeLN
	incf	TMP3, F
	BRA_IFCLR TMP3, 2, ACCESS, CS_MENU_MS_NotifyProgramChangeL

	return
	

;; --------------------------------------------------------------------------
;;  This function is called to forward a Bank Change event to the control surface
;;  Input:
;;     o first MIDI event byte in MIOS_PARAMETER1
;;     o third MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CS_MENU_MS_NotifyBankChange
	;; store channel and bank number
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1		; chn
	movf	MIOS_PARAMETER3, W
	andlw	0x07
	movwf	TMP2		; bank

	;; request patch send to SIDs with matching channel number
	clrf	TMP3				; TMP3 used as counter
CS_MENU_MS_NotifyBankChangeL
	;; calc pointer to channel entry
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	TMP3, W
	addwf	FSR0L, F

	;; check if channel number matches
	movf	INDF0, W
	xorwf	TMP1, W
	andlw	0x0f
	bnz	CS_MENU_MS_NotifyBankChangeLN
	;; store bank number
	movlw	(CS_MENU_SID_M_BANK-CS_MENU_SID_M_CHN) & 0xff
	addwf	FSR0L, F
	movff	TMP2, INDF0
	movlw	(CS_MENU_SID_M_PATCH-CS_MENU_SID_M_BANK) & 0xff
	addwf	FSR0L, F	; clear 'sent' flag of patch
	bcf	INDF0, 7
	;; skip if patch change already requested
	movlw	(CS_MENU_TX_M_CTR-CS_MENU_SID_M_PATCH) & 0xff
	addwf	FSR0L, F
	BRA_IFSET INDF0, 7, ACCESS, CS_MENU_MS_NotifyBankChangeLN
	;; request bank/patch change
	movlw	0x81
	movwf	INDF0
CS_MENU_MS_NotifyBankChangeLN
	incf	TMP3, F
	BRA_IFCLR TMP3, 2, ACCESS, CS_MENU_MS_NotifyBankChangeL

	return

	
;; --------------------------------------------------------------------------
;;  this function sends the whole patch to the SID which is
;;  selected by CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_Send_SysExDump
	;; check TX counter, if > 6, delay the sending
	;; this register is decremented by cs_menu_timer.inc every 100 ms
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	
	;; if counter != 0, request delayed TX of dump and exit
	movf	INDF1, W
	bnz	CS_MENU_MS_Send_SysExDump_TxReq

	;; extra: clear the PLAY_SIDx flag
	movf	CS_MENU_SID, W
	rcall	CS_MENU_MS_CancelNote

	;; branch depending on master/slave SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_Send_SysExDump_S

CS_MENU_MS_Send_SysExDump_M
	;; reload TX counter
	movlw	0x01
	movwf	INDF1

	;; don't send if 'sent' flag set
	BRA_IFSET CS_MENU_SID_M_CHN, 7, ACCESS, CS_MENU_MS_Send_SysExDump_M_DS

	;; set new channel
	movff	CS_MENU_SID_M_CHN, SID_V1_BASE + SID_Vx_MIDI_CHANNEL
	movff	CS_MENU_SID_M_CHN, SID_V2_BASE + SID_Vx_MIDI_CHANNEL
	movff	CS_MENU_SID_M_CHN, SID_V3_BASE + SID_Vx_MIDI_CHANNEL

	bsf	CS_MENU_SID_M_CHN, 7	; set 'sent' flag

	;; store it in EEPROM
	movlw	0x7c
	movwf	EEADR
	movff	SID_V3_BASE + SID_Vx_MIDI_CHANNEL, WREG
	call	MIOS_EEPROM_Write
	movff	SID_V2_BASE + SID_Vx_MIDI_CHANNEL, WREG
	call	MIOS_EEPROM_Write
	movff	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, WREG
	call	MIOS_EEPROM_Write

	;; store channel in BankStick
	call	CS_MENU_BANK_SaveCfgChannel
CS_MENU_MS_Send_SysExDump_M_DS

	;; skip if 'sent' flag set
	BRA_IFSET CS_MENU_SID_M_PATCH, 7, ACCESS, CS_MENU_MS_Send_SysExDump_End

	;; store patch/bank number of master in SID_PATCH/SID_BANK
	movff	CS_MENU_SID_M_PATCH, SID_PATCH
	movff	CS_MENU_SID_M_BANK, SID_BANK

	;; store patch number in BankStick
	rcall	CS_MENU_BANK_SaveCfgPatchNumber

	bsf	CS_MENU_SID_M_PATCH, 7	; set 'sent' flag

	;; update sound parameters
	call	SID_PATCH_Init

	;; copy current patch number to WT patch number
	rcall	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_WT_PATCH
	movff	SID_PATCH, PLUSW0

	;; restore CS_MENU_SID
	rgoto	CS_MENU_MS_Send_SysExDump_End

CS_MENU_MS_Send_SysExDump_S
	;; else send dump of current patch to slave SID

	;; reload TX counter
	movlw	0x06
	movwf	INDF1

	;; skip send channel if 'sent' flag set
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	BRA_IFSET INDF1, 7, ACCESS, CS_MENU_MS_Send_SysExDump_S_DS
	bsf	INDF1, 7	; set 'sent' flag
	
	clrf	TMP3
CS_MENU_MS_Send_SysExDump_S_CLp
	;; send SysEx header
	rcall	CS_MENU_MS_Send_SysExHeader

	;; send CFG
	movlw	0x0d
	rcall	CS_MENU_MS_Send_MIDIByte

	;; address: 0x00-0x02
	movf	TMP3, W
	rcall	CS_MENU_MS_Send_MIDIByte

	;; MSN
	movlw	0x00
	rcall	CS_MENU_MS_Send_MIDIByte

	;; LSN
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	movf	PLUSW1, W
	andlw	0x0f
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send SysEx footer
	rcall	CS_MENU_MS_Send_SysExFooter

	;; wait until Tx FIFO empty
	call	MIOS_MIDI_TxBufferFlush

	;; wait at least 5 ms (writing to EEPROM needs some time)
	movlw	5
	call	MIOS_Delay

	;; loop 3 times
	incf	TMP3, F
	movlw	0x02
	cpfsgt	TMP3, ACCESS
	rgoto CS_MENU_MS_Send_SysExDump_S_CLp

	;; store channel number in BankStick
	rcall	CS_MENU_BANK_SaveCfgChannel

CS_MENU_MS_Send_SysExDump_S_DS

	;; skip send patch if 'sent' flag set
	lfsr	FSR1, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	BRA_IFSET PLUSW1, 7, ACCESS, CS_MENU_MS_Send_SysExDump_End
	bsf	PLUSW1, 7	; set 'sent' flag

	;; send SysEx header
	rcall	CS_MENU_MS_Send_SysExHeader

	;; send PATCH_Write ID
	movlw	0x02
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send to internal patch #0
	movlw	0x00
	rcall	CS_MENU_MS_Send_MIDIByte

	;; clear checksum
	clrf	SID_SYSEX_CHECKSUM

	;; send dump
	clrf	EEADR			; 0x100 bytes to send, use EEADR as counter
CS_MENU_MS_Init_SID_SendS_Loop
	rcall	CS_MENU_BANK_Read	; read patch content (EEADR will be incremented)

	andlw	0x7f		; add to checksum
	addwf	SID_SYSEX_CHECKSUM, F

	rcall	CS_MENU_MS_Send_MIDIByte; send byte

	movf	EEADR, W
	bnz	CS_MENU_MS_Init_SID_SendS_Loop; loop 256 times
	
	;; send checksum
	movf	SID_SYSEX_CHECKSUM, W
	sublw	0x80
	andlw	0x7f
	rcall	CS_MENU_MS_Send_MIDIByte

	;; send SysEx footer and exit
	rcall	CS_MENU_MS_Send_SysExFooter

	;; store patch number in BankStick
	rcall	CS_MENU_BANK_SaveCfgPatchNumber

CS_MENU_MS_Send_SysExDump_End
	return


;; --------------------------------------------------------------------------
	;; request delayed TX of dump and exit
CS_MENU_MS_Send_SysExDump_TxReq
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	bsf	PLUSW1, 7
	return

	;; check if transfer can be granted
CS_MENU_MS_Send_SysExDump_TxTst
	clrf	CS_MENU_SID		; handle all 4 SIDs
CS_MENU_MS_Send_SysExDump_TxTstL
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	addwf	FSR1L, F

	;; skip if CTR[7] not set (no sync requested)
	BRA_IFCLR INDF1, 7, ACCESS, CS_MENU_MS_Send_SysExDump_TxTstN

	;; skip if CTR[6..0] is != 0
	movf	INDF1, W
	andlw	0x7f
	bnz	CS_MENU_MS_Send_SysExDump_TxTstN

	;; else clear request
	bcf	INDF1, 7

	;; and update parameters
	rcall	CS_MENU_MS_UpdateAllParameters

CS_MENU_MS_Send_SysExDump_TxTstN
	incf	CS_MENU_SID, F		; loop 4 times
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_MS_Send_SysExDump_TxTstL

	return


;; --------------------------------------------------------------------------
;;  Send SysEx Header to Slave
;; --------------------------------------------------------------------------
CS_MENU_MS_Send_SysExHeader
	movlw	0xf0		; Vendor ID
	rcall	CS_MENU_MS_Send_MIDIByte
	movlw	0x00
	rcall	CS_MENU_MS_Send_MIDIByte
	movlw	0x00
	rcall	CS_MENU_MS_Send_MIDIByte
	movlw	0x7e
	rcall	CS_MENU_MS_Send_MIDIByte
	movlw	0x46		; MIDIbox SID ID (46 - the ultimative number + 4)
	rcall	CS_MENU_MS_Send_MIDIByte
	movf	CS_MENU_SID, W	; device ID (== SID number)
	rgoto	CS_MENU_MS_Send_MIDIByte; and exit...

;; --------------------------------------------------------------------------
;;  Send SysEx Footer to Slave
;; --------------------------------------------------------------------------
CS_MENU_MS_Send_SysExFooter
	movlw	0xf7
	rgoto	CS_MENU_MS_Send_MIDIByte

;; --------------------------------------------------------------------------
;;  this routine is used when a note should be played on a given SID
;;  It's only used by CS_MENU_BUTTON_Exec when a SIDx button is hold
;;  and the menu button is pressed
;;  Note On/Off status will be toggled each time this function is executed
;;  IN: SID number in WREG
;; --------------------------------------------------------------------------
CS_MENU_MS_PlayNote
	movwf	TMP1

	;; toggle play state which is stored in CS_STAT2
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andlw	0xf0
	movwf	TMP2

	IRQ_DISABLE		; must be atomic since the IRQ handler modifies this register
	movf	CS_STAT2, W
	xorwf	TMP2, W
	movwf	CS_STAT2
	IRQ_ENABLE

	;; determine first MIDI byte (0x90 | MIDI channel)
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	TMP1, W
	movf	PLUSW0, W
	andlw	0x0f
	iorlw	0x90
	movwf	TMP2

	;; determine third MIDI byte (0x7f or 0x00)
	clrf	TMP3
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andwf	CS_STAT2, W
	bz	CS_MENU_MS_PlayNoteOff
CS_MENU_MS_PlayNoteOn
	movlw	0x7f
	movwf	TMP3
CS_MENU_MS_PlayNoteOff	

	;; forward Note On/Off event to all SIDs
	;; external
	movf	TMP2, W
	rcall	CS_MENU_MS_Send_MIDIByte
	movlw	0x3c		; C-4
	rcall	CS_MENU_MS_Send_MIDIByte
	movf	TMP3, W
	rcall	CS_MENU_MS_Send_MIDIByte

	;; and internal
	movff	TMP2, MIOS_PARAMETER1
	movlw	0x3c		; C-4
	movwf	MIOS_PARAMETER2
	movff	TMP3, MIOS_PARAMETER3
	goto	USER_MPROC_NotifyReceivedEvent

;; --------------------------------------------------------------------------
;;  this routine is called by CS_MENU_MS_NotifyProgramChange and
;;  SID_PATCH_Init when a note has been switched off
;;  IN: SID number in WREG
;; --------------------------------------------------------------------------
CS_MENU_MS_CancelNote
	IRQ_DISABLE		; must be atomic since the IRQ handler writes also to this register
	andlw	0x03
	call	MIOS_HLP_GetBitANDMask
	swapf	WREG, W
	andwf	CS_STAT2, F
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  this routine sends a MIDI byte
;;  in fact it calls MIOS_MIDI_TxBufferPut, this function is only used to
;;  save code space (a rcall takes 2 bytes, a call 4 bytes)
;; --------------------------------------------------------------------------
CS_MENU_MS_Send_MIDIByte
	goto	MIOS_MIDI_TxBufferPut


