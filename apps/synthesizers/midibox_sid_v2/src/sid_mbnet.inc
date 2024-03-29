; $Id$
;
;  SID specific MBNET functions and hooks
;
;  See also mbnet.inc and cs_menu_mbnet.inc
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called from MBNET_RxHandler when a special command
;;  has been received (TOS=0)
;;  IN: special command number 16-127 in WREG
;;      receive buffer in RXB0
;;  OUT: must return the acknowledge TOS (MBNET_ACK_TOS_*)
;; --------------------------------------------------------------------------
SID_MBNET_RxSpecial
	SET_BSR	MBNET_BASE
	addlw	-16
	bz	SID_MBNET_RxSpecial_IP		; ETOS=16 -> init patch
	addlw	-1
	bz	SID_MBNET_RxSpecial_EPAR	; ETOS=17 -> Ensemble Parameter
	addlw	-1
	bz	SID_MBNET_RxSpecial_PAR		; ETOS=18 -> Sound Parameter
	addlw	-1
	bz	SID_MBNET_RxSpecial_KNB		; ETOS=19 -> Knob Parameter
	addlw	-1
	bz	SID_MBNET_RxSpecial_UKNB	; ETOS=20 -> Update Knob Parameter
	addlw	-1
	bz	SID_MBNET_RxSpecial_USHD	; ETOS=21 -> Update Shadow Buffer
	addlw	-1
	bz	SID_MBNET_RxSpecial_DISE	; ETOS=22 -> Disable Engine
	addlw	-1
	bz	SID_MBNET_RxSpecial_IE		; ETOS=23 -> Init Ensemble
	addlw	-1
	bz	SID_MBNET_RxSpecial_NON		; ETOS=24 -> Notes On
	addlw	-1
	bz	SID_MBNET_RxSpecial_NOFF	; ETOS=25 -> Notes Off
	addlw	-1
	bz	SID_MBNET_RxSpecial_VINI	; ETOS=26 -> Voice ReInit
	addlw	-1
	bz	SID_MBNET_RxSpecial_SPNOTE	; ETOS=27 -> Superpoly Note
	addlw	-1
	bz	SID_MBNET_RxSpecial_TMPBNK0	; ETOS=28 -> Write Bank 0 (temporary!)
	retlw	MBNET_ACK_TOS_ERROR		; all others: error

SID_MBNET_RxSpecial_IP
	call	SID_PATCH_Init
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_EPAR
	;; error if != 4 byte received
	movlw	4
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR

	movff	RXB0D2, PRODL
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; determine address to ensemble buffer -> FSR0
	lfsr	FSR0, SID_ENS_BUFFER
	movf	RXB0D0, W, ACCESS; low byte
	andlw	0x3f		; 64 byte range
	addwf	FSR0L, F

	;; transfer value
	movff	PRODL, INDF0

	;; re-init local ensemble
	call	SID_ENS_Init_Local

	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_PAR
	;; error if != 4 byte received
	movlw	4
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR

	movf	RXB0D2, W, ACCESS
	movwf	PRODL
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; determine patch buffer address -> FSR0
	;; shadow buffer address -> FSR2
	movf	RXB0D0, W, ACCESS; low byte
	movwf	FSR0L
	movwf	FSR2L
	movf	RXB0D1, W, ACCESS; high byte
	andlw	0x01
	addlw	HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	addlw	HIGH(SID_PATCH_BUFFER_SHADOW-SID_PATCH_BUFFER)
	movwf	FSR2H

	;; transfer byte into patch buffer and patch buffer shadow
	movff	PRODL, INDF0
	movff	PRODL, INDF2

	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_KNB
	;; error if != 4 byte received
	movlw	4
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR

	movff	RXB0D2, MIOS_PARAMETER1
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; knob number in D0
	movf	RXB0D0, W, ACCESS
	andlw	0x07		; 8 knobs
	call	SID_KNOB_SetValue

	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_UKNB
	;; error if != 1 byte received
	movlw	1
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR

	movf	RXB0D0, W, ACCESS
	call	SID_KNOB_UpdateValue
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_USHD
	call	SID_PATCH_UpdateShadowBuffer
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_DISE
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE_LEVEL0
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_IE
	lfsr	FSR1, SID_ENS_BUFFER	; master has (hopefully) filled this buffer before
	call	SID_ENS_Init_Local
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_NON
	;; error if != 2 byte received
	movlw	2
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR
	
	;; received instrument and selected SID number
	movff	RXB0D0, CS_MENU_SELECTED_INS
	movff	RXB0D1, CS_MENU_SELECTED_SID_LR
	;; (selected) notes on
	call	SID_PATCH_NotesOn
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_NOFF
	call	SID_PATCH_NotesOff
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_VINI
	call	SID_VOICE_ReInit
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_SPNOTE
	;; error if != 2 byte received
	movlw	2
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR

	;; received note and velocity
	movff	RXB0D0, SID_MIDI_PARAMETER1
	movff	RXB0D1, SID_MIDI_PARAMETER2
	;; exit if lead engine not selected
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bnz	SID_MBNET_RxSpecial_SPNOTE_End
	;; branch depending on velocity
	IRQ_DISABLE		; required to stall SID engine
	SET_BSR	SID_BASE
	movf	SID_MIDI_PARAMETER2, W, BANKED
	bz	SID_MBNET_RxSpecial_SPNOTEOff
SID_MBNET_RxSpecial_SPNOTEOn
	call	SID_MIDI_L_NoteOn_Direct
	rgoto	SID_MBNET_RxSpecial_SPNOTE_End
SID_MBNET_RxSpecial_SPNOTEOff
	call	SID_MIDI_L_NoteOff_Direct
SID_MBNET_RxSpecial_SPNOTE_End
	IRQ_ENABLE
	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_TMPBNK0
	;; error if != 4 byte received
	movlw	4
	cpfseq	RXB0DLC, ACCESS
	retlw	MBNET_ACK_TOS_ERROR

	movff	RXB0D2, PRODL
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; determine address to bank 0 -> FSR0
	movff	RXB0D0, FSR0L	; low byte
	clrf	FSR0H

	;; transfer value
	movff	PRODL, INDF0

	retlw	MBNET_ACK_TOS_OK



;; --------------------------------------------------------------------------
;;  This hook is called from MBNET_RxHandler when a RAM read has been requested
;;  IN: pointer to receive buffer in FSR1
;;      pointer to DLC of transmit buffer in FSR2
;;  OUT: this function has to copy the payload into the transmit buffer
;; --------------------------------------------------------------------------
SID_MBNET_RAM_Read
	SET_BSR	MBNET_BASE

	;; address HIGH byte = 0xff selects ensemble buffer
	movf	RXB0EIDH, W, ACCESS
	xorlw	0xff
	bz	SID_MBNET_RAM_Read_Ens

	;; address HIGH byte = 0xfe selects direct access to SID registers
	xorlw	0xfe ^ 0xff
	bz	SID_MBNET_RAM_Read_SID

SID_MBNET_RAM_Read_Patch
	;; determine address -> FSR0
	movff	RXB0EIDL, FSR0L
	movf	RXB0EIDH, W, ACCESS
	addlw	HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	rgoto	SID_MBNET_RAM_Read_Cont

SID_MBNET_RAM_Read_Ens
	;; determine address -> FSR0
	lfsr	FSR0, SID_ENS_BUFFER
	movf	RXB0EIDL, W, ACCESS
	andlw	0x3f
	addwf	FSR0L, F
 	rgoto	SID_MBNET_RAM_Read_Cont

SID_MBNET_RAM_Read_SID
	;; determine address -> FSR0
	;; SIDL mapped to 0xfe00..0xfe1f
	;; SIDR mapped to 0xfe20..0xfe3f
	movf	RXB0EIDL, W, ACCESS
	lfsr	FSR0, SIDL_BASE
	btfsc	WREG, 5
	lfsr	FSR0, SIDR_BASE
	andlw	0x1f
	addwf	FSR0L, F
	;; 	rgoto	SID_MBNET_RAM_Read_SID_Cont

SID_MBNET_RAM_Read_Cont
	;; 8 byte
	movlw	8
	movwf	POSTINC2

	movff	POSTINC0, POSTINC2	; D0
	movff	POSTINC0, POSTINC2	; D1
	movff	POSTINC0, POSTINC2	; D2
	movff	POSTINC0, POSTINC2	; D3
	movff	POSTINC0, POSTINC2	; D4
	movff	POSTINC0, POSTINC2	; D5
	movff	POSTINC0, POSTINC2	; D6
	movff	POSTINC0, POSTINC2	; D7

	return


;; --------------------------------------------------------------------------
;;  This hook is called from MBNET_RxHandler when a RAM write message has been
;;  received (TOS=2)
;;  IN: pointer to receive buffer in RXB0
;;  OUT: must return the acknowledge TOS (MBNET_ACK_TOS_*)
;; --------------------------------------------------------------------------
SID_MBNET_RAM_Write
	;; error if no byte has been received
	movf	RXB0DLC, W, ACCESS
	skpnz
	retlw	MBNET_ACK_TOS_ERROR

	;; address HIGH byte = 0xff selects ensemble buffer
	movf	RXB0EIDH, W, ACCESS
	xorlw	0xff
	bz	SID_MBNET_RAM_Write_Ens

	;; address HIGH byte = 0xfe selects direct access to SID registers
	xorlw	0xfe ^ 0xff
	bz	SID_MBNET_RAM_Write_SID
	;; address HIGH byte = 0xfd: same like 0xfe, but SID registers will be updated
	xorlw	0xfd ^ 0xfe
	bz	SID_MBNET_RAM_Write_SID

SID_MBNET_RAM_Write_Patch
	;; FSR0 contains patch buffer address, which is extracted from the EID
	;; FSR2 points to the shadow buffer
	movf	RXB0EIDL, W, ACCESS
	movwf	FSR0L
	movwf	FSR2L
	movf	RXB0EIDH, W, ACCESS
	andlw	0x01		; (buffer size 0x200 prepared for MBSID V2)
	addlw	HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	addlw	HIGH(SID_PATCH_BUFFER_SHADOW-SID_PATCH_BUFFER)
	movwf	FSR2H
	rgoto	SID_MBNET_RAM_Write_Cont


SID_MBNET_RAM_Write_Ens
	;; determine address -> FSR0
	lfsr	FSR0, SID_ENS_BUFFER
	movf	RXB0EIDL, W, ACCESS
	andlw	0x3f
	addwf	FSR0L, F
	;; shadow buffer: not available, use same address!
	movff	FSR0L, FSR2L
	movff	FSR0H, FSR2H

	;; 	rgoto	SID_MBNET_RAM_Write_Cont

SID_MBNET_RAM_Write_Cont
	movff	RXB0DLC, PRODL

	;; FSR1 contains pointer to data buffer
	call	MBNET_GetRxPtr
	movlw	RXB0D0 & 0xf
	addwf	FSR1L, F

SID_MBNET_RAM_Write_CopyLoop
	movff	INDF1, POSTINC0			; copy content of MBNET receive buffer to patch buffer
	movff	POSTINC1, POSTINC2		; and into shadow buffer
	decfsz	PRODL, F			; continue until loop counter is zero
	rgoto	SID_MBNET_RAM_Write_CopyLoop

	;; exit with Ok status
	retlw	MBNET_ACK_TOS_OK



SID_MBNET_RAM_Write_SID
	;; enable direct access to SID registers
	;; cleared on each patch change
	bsf	SID_STAT, SID_STAT_MBNET_ACCESS

	;; determine address -> FSR0
	;; SIDL mapped to 0xfe00..0xfe1f and 0xfd00..0xfd1f (0xfd: with reg update)
	;; SIDR mapped to 0xfe20..0xfe3f and 0xfd20..0xfd3f (0xfd: with reg update)
	;; in addition, all gates (6 bits, located in [2:0] and [6:4]) are mapped to 0xfe80
	;; TODO: provide more special address ranges to access certain registers in a single package
	;;       e.g. L/R filters and all 6 waveform registers
	movf	RXB0EIDL, W, ACCESS
	xorlw	0x80
	bnz	SID_MBNET_RAM_Write_SID_Normal

SID_MBNET_RAM_Write_SID_Gates
	SET_BSR	SIDL_BASE
	bcf	SIDL_BASE + SIDx_V1_CTRL, 0, BANKED
	btfsc	RXB0D0, 0, ACCESS
	bsf	SIDL_BASE + SIDx_V1_CTRL, 0, BANKED

	bcf	SIDL_BASE + SIDx_V2_CTRL, 0, BANKED
	btfsc	RXB0D0, 1, ACCESS
	bsf	SIDL_BASE + SIDx_V2_CTRL, 0, BANKED

	bcf	SIDL_BASE + SIDx_V3_CTRL, 0, BANKED
	btfsc	RXB0D0, 2, ACCESS
	bsf	SIDL_BASE + SIDx_V3_CTRL, 0, BANKED

	bcf	SIDR_BASE + SIDx_V1_CTRL, 0, BANKED
	btfsc	RXB0D0, 4, ACCESS
	bsf	SIDR_BASE + SIDx_V1_CTRL, 0, BANKED

	bcf	SIDR_BASE + SIDx_V2_CTRL, 0, BANKED
	btfsc	RXB0D0, 5, ACCESS
	bsf	SIDR_BASE + SIDx_V2_CTRL, 0, BANKED

	bcf	SIDR_BASE + SIDx_V3_CTRL, 0, BANKED
	btfsc	RXB0D0, 6, ACCESS
	bsf	SIDR_BASE + SIDx_V3_CTRL, 0, BANKED

	rgoto	SID_MBNET_RAM_Write_SID_End


SID_MBNET_RAM_Write_SID_Normal
	movf	RXB0EIDL, W, ACCESS
	lfsr	FSR0, SIDL_BASE
	btfsc	WREG, 5
	lfsr	FSR0, SIDR_BASE
	andlw	0x1f
	addwf	FSR0L, F
	;; 	rgoto	SID_MBNET_RAM_Write_SIDCopy


SID_MBNET_RAM_Write_SIDCopy
	movff	RXB0DLC, PRODL

	;; FSR1 contains pointer to data buffer
	call	MBNET_GetRxPtr
	movlw	RXB0D0 & 0xf
	addwf	FSR1L, F

SID_MBNET_RAM_Write_SIDCopyLoop
	movff	POSTINC1, POSTINC0		; copy content of MBNET receive buffer to SID buffer
	decfsz	PRODL, F			; continue until loop counter is zero
	rgoto	SID_MBNET_RAM_Write_SIDCopyLoop

SID_MBNET_RAM_Write_SID_End
	;; disable sound engine
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE

	;; SID access: no acknowledge
	movf	RXB0EIDH, W, ACCESS
	xorlw	0xfe
	bz	SID_MBNET_RAM_Write_SID_NoAck0

SID_MBNET_RAM_Write_SID_NoAck1
	;; 0xfd: send to SIDs
	call _SID_SR_Handler

SID_MBNET_RAM_Write_SID_NoAck0
	;; 0xfe and 0xfd: don't send acknowledge for higher bandwidth
	retlw	-1
