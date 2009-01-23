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
;;      pointer to receive buffer in FSR1
;;      number of received bytes in MBNET_LOOP_CTR
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
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw MBNET_ACK_TOS_ERROR

	movlw	RXB0D2 & 0xf
	movff	PLUSW1, PRODL
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; determine address to ensemble buffer -> FSR0
	lfsr	FSR0, SID_ENS_BUFFER
	movlw	RXB0D0 & 0xf	; low byte
	movf	PLUSW1, W
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
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw MBNET_ACK_TOS_ERROR

	movlw	RXB0D2 & 0xf
	movff	PLUSW1, PRODL
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; determine patch buffer address -> FSR0
	;; shadow buffer address -> FSR2
	movlw	RXB0D0 & 0xf	; low byte
	movff	PLUSW1, FSR0L
	movff	PLUSW1, FSR2L
	movlw	RXB0D1 & 0xf	; high byte
	movf	PLUSW1, W
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
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw MBNET_ACK_TOS_ERROR

	movlw	RXB0D2 & 0xf
	movff	PLUSW1, MIOS_PARAMETER1
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; knob number in D0
	movlw	RXB0D0 & 0xf
	movf	PLUSW1, W
	andlw	0x07		; 8 knobs
	call	SID_KNOB_SetValue

	retlw	MBNET_ACK_TOS_OK

SID_MBNET_RxSpecial_UKNB
	;; error if != 1 byte received
	movlw	1
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw MBNET_ACK_TOS_ERROR

	movlw	RXB0D0 & 0xf
	movf	PLUSW1, W
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
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw	MBNET_ACK_TOS_ERROR
	;; received instrument and selected SID number
	movlw	RXB0D0 & 0xf
	movff	PLUSW1, CS_MENU_SELECTED_INS
	movlw	RXB0D1 & 0xf
	movff	PLUSW1, CS_MENU_SELECTED_SID_LR
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
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw	MBNET_ACK_TOS_ERROR
	;; received note and velocity
	movlw	RXB0D0 & 0xf
	movff	PLUSW1, SID_MIDI_PARAMETER1
	movlw	RXB0D1 & 0xf
	movff	PLUSW1, SID_MIDI_PARAMETER2
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
	cpfseq	MBNET_LOOP_CTR, BANKED
	retlw MBNET_ACK_TOS_ERROR

	movlw	RXB0D2 & 0xf
	movff	PLUSW1, PRODL
	;; ignore high-byte of data (RXB0D3), we currently only use 1 byte writes

	;; determine address to bank 0 -> FSR0
	movlw	RXB0D0 & 0xf	; low byte
	movf	PLUSW1, W
	movwf	FSR0L
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
	movlw	RXB0EIDH & 0xf
	movf	PLUSW1, W
	xorlw	0xff
	bz	SID_MBNET_RAM_Read_Ens

SID_MBNET_RAM_Read_Patch
	;; determine address -> FSR0
	movlw	RXB0EIDL & 0xf
	movff	PLUSW1, FSR0L
	movlw	RXB0EIDH & 0xf
	movf	PLUSW1, W
	addlw	HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	rgoto	SID_MBNET_RAM_Read_Cont

SID_MBNET_RAM_Read_Ens
	;; determine address -> FSR0
	lfsr	FSR0, SID_ENS_BUFFER
	movlw	RXB0EIDL & 0xf
	movf	PLUSW1, W
	andlw	0x3f
	addwf	FSR0L, F

	;; 	rgoto	SID_MBNET_RAM_Read_Cont

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
;;  IN: pointer to receive buffer in FSR1
;;      number of received bytes in MBNET_LOOP_CTR
;;  OUT: must return the acknowledge TOS (MBNET_ACK_TOS_*)
;; --------------------------------------------------------------------------
SID_MBNET_RAM_Write
	;; error if no byte has been received
	movf	MBNET_LOOP_CTR, W, BANKED
	skpnz
	retlw	MBNET_ACK_TOS_ERROR

	;; address HIGH byte = 0xff selects ensemble buffer
	movlw	RXB0EIDH & 0xf
	movf	PLUSW1, W
	xorlw	0xff
	bz	SID_MBNET_RAM_Write_Ens

SID_MBNET_RAM_Write_Patch
	;; FSR0 contains patch buffer address, which is extracted from the EID
	;; FSR2 points to the shadow buffer
	movlw	RXB0EIDL & 0xf
	movff	PLUSW1, FSR0L
	movff	PLUSW1, FSR2L
	movlw	RXB0EIDH & 0xf
	movf	PLUSW1, W
	andlw	0x01		; (buffer size 0x200 prepared for MBSID V2)
	addlw	HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	addlw	HIGH(SID_PATCH_BUFFER_SHADOW-SID_PATCH_BUFFER)
	movwf	FSR2H

	;; FSR1 contains pointer to RXB0 data buffer
	movlw	RXB0D0 & 0xf
	addwf	FSR1L, F

SID_MBNET_RAM_Write_CopyLoop
	movff	INDF1, POSTINC0			; copy content of MBNET receive buffer to patch buffer
	movff	POSTINC1, POSTINC2		; and into shadow buffer
	decfsz	MBNET_LOOP_CTR, F, BANKED	; continue until loop counter is zero
	rgoto	SID_MBNET_RAM_Write_CopyLoop

	;; exit with Ok status
	retlw	MBNET_ACK_TOS_OK


SID_MBNET_RAM_Write_Ens
	;; determine address -> FSR0
	lfsr	FSR0, SID_ENS_BUFFER
	movlw	RXB0EIDL & 0xf
	movf	PLUSW1, W
	andlw	0x3f
	addwf	FSR0L, F

	;; FSR1 contains pointer to RXB0 data buffer
	movlw	RXB0D0 & 0xf
	addwf	FSR1L, F

	movff	POSTINC1, POSTINC0	; D0
	movff	POSTINC1, POSTINC0	; D1
	movff	POSTINC1, POSTINC0	; D2
	movff	POSTINC1, POSTINC0	; D3
	movff	POSTINC1, POSTINC0	; D4
	movff	POSTINC1, POSTINC0	; D5
	movff	POSTINC1, POSTINC0	; D6
	movff	POSTINC1, POSTINC0	; D7

	;; exit with Ok status
	retlw	MBNET_ACK_TOS_OK
