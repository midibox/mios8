; $Id$
;
; MIDIboxSEQ
; Parameter Type decoding
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

				;; these are the flags of the PTYPE
SEQ_PTYPE_LAYER_EVNT1		EQU	0 ; layer of EVNT1 selected
SEQ_PTYPE_LAYER_EVNT2		EQU	1 ; layer of EVNT2 selected
SEQ_PTYPE_LAYER_EVNTL		EQU	2 ; layer of gatelength selected
SEQ_PTYPE_ARP			EQU	3 ; arp activated?
SEQ_PTYPE_ARP_MODIF_VALUE	EQU	4 ; modif value of arp activated?
SEQ_PTYPE_NOTELIKE_EVENT	EQU	5 ; set on Note/Aftertouch/AOUT with Chn <= 4
SEQ_PTYPE_THRESHOLD_VALUE	EQU	6 ; in morph thrs mode the length is used as threshold value
SEQ_PTYPE_DRUMS			EQU	7 ; if drum mode selected

; ==========================================================================

;; --------------------------------------------------------------------------
;;  this function returns the parameter type of the current selected track/layer
;;  IN: SEQ_EVNTT: track number (0-15 for MIDIboxSEQ)
;;      SEQ_SELECTED_LAYERS: selected layer (A/B/C)
;;  OUT: some flags in WREG and SEQ_PTYPE -- see above
;;  USES: FSR1, BSR, MIOS_PARAMETER[12] (as temporary value)
;; --------------------------------------------------------------------------
SEQ_PTYPE_Get
	SET_BSR	SEQ_BASE

	;; clear parameter type
	clrf	SEQ_PTYPE, BANKED

SEQ_PTYPE_Get_Evnt
	;; note-like value?
	call	SEQ_FSR_TrkEvnt0
	BRA_IFSET INDF1, 6, ACCESS, SEQ_PTYPE_Get_Evnt_CDEF
SEQ_PTYPE_Get_Evnt_89AB
	BRA_IFSET INDF1, 5, ACCESS, SEQ_PTYPE_Get_Evnt_AB
SEQ_PTYPE_Get_Evnt_89		; Note On/Off
	bsf	SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED
	rgoto	SEQ_PTYPE_Get_Evnt_Cont
SEQ_PTYPE_Get_Evnt_AB		; Aftertouch/Controller
	btfss	INDF1, 4
	bsf	SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED
	rgoto	SEQ_PTYPE_Get_Evnt_Cont
	
SEQ_PTYPE_Get_Evnt_CDEF
	BRA_IFSET INDF1, 5, ACCESS, SEQ_PTYPE_Get_Evnt_EF
SEQ_PTYPE_Get_Evnt_CD		; Program Change/Channel preasure
	btfsc	INDF1, 4
	bsf	SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED
	rgoto	SEQ_PTYPE_Get_Evnt_Cont

SEQ_PTYPE_Get_Evnt_EF		; Pitch Bend/AOUT
	BRA_IFCLR INDF1, 4, ACCESS, SEQ_PTYPE_Get_Evnt_Cont
	movf	INDF1, W
	andlw	0x0c
	bnz	SEQ_PTYPE_Get_Evnt_Cont
	bsf	SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED
SEQ_PTYPE_Get_Evnt_Cont

	;; set drum flag in drum mode and exit! (no additional flags!!)
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x0f
	xorlw	SEQ_TRKMODE_DRUMS
	bnz	SEQ_PTYPE_Get_NoDrumMode
SEQ_PTYPE_Get_DrumMode
	bsf	SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED
	rgoto	SEQ_PTYPE_Get_End
SEQ_PTYPE_Get_NoDrumMode

	;; get assignments of current track -> MIOS_PARAMETER1
	call	SEQ_FSR_TrkAssgn
	movff	INDF1, MIOS_PARAMETER1

	;; get current layer number (0/1/2 for A/B/C) -> MIOS_PARAMETER2
	movlw	0x00
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	movlw 0x01
	btfsc	SEQ_SELECTED_LAYERS, 2, BANKED
	movlw 0x02
	movwf	MIOS_PARAMETER2

	;; now search for the selected layer
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_PTYPE_Get_Not0
SEQ_PTYPE_Get_0
	bsf	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT1, BANKED
	rgoto	SEQ_PTYPE_Get_Cont
SEQ_PTYPE_Get_Not0
	rrf	MIOS_PARAMETER1, F
	rrf	MIOS_PARAMETER1, W
	andlw	0x03
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_PTYPE_Get_Not1
SEQ_PTYPE_Get_1
	bsf	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT2, BANKED
	rgoto	SEQ_PTYPE_Get_Cont
SEQ_PTYPE_Get_Not1
SEQ_PTYPE_Get_2
	bsf	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED

	;; in morph thrs mode the length is used as threshold value
	call	SEQ_FSR_TrkMorph0
	movf	INDF1, W
	andlw	0x0f
	xorlw	0x02
	bnz	SEQ_PTYPE_Get_2_NoMThrsMode
SEQ_PTYPE_Get_2_MThrsMode
	bcf	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED	; cancel length mode
	bsf	SEQ_PTYPE, SEQ_PTYPE_THRESHOLD_VALUE, BANKED	; set threshold mode
SEQ_PTYPE_Get_2_NoMThrsMode
SEQ_PTYPE_Get_Cont
	;; set ARP flag depending on mode
	rcall	SEQ_PTYPE_ChkArpMode
	bnz	SEQ_PTYPE_Get_NoArpMode
SEQ_PTYPE_Get_ArpMode
	bsf	SEQ_PTYPE, SEQ_PTYPE_ARP, BANKED

	;; if layer of modified value selected, set also the ARP_MODIF_VALUE flag
	call	SEQ_FSR_TrkEvnt0
	movf	INDF1, W
	andlw	0xf0
	xorlw	0xb0
	bz	SEQ_PTYPE_Get_ArpMode_ChkE2
	
SEQ_PTYPE_Get_ArpMode_ChkE1
	btfsc	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT1, BANKED
	bsf	SEQ_PTYPE, SEQ_PTYPE_ARP_MODIF_VALUE, BANKED
	rgoto	SEQ_PTYPE_Get_ArpMode_Cont
SEQ_PTYPE_Get_ArpMode_ChkE2
	btfsc	SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT2, BANKED
	bsf	SEQ_PTYPE, SEQ_PTYPE_ARP_MODIF_VALUE, BANKED
	;; 	rgoto	SEQ_PTYPE_Get_ArpMode_Cont
SEQ_PTYPE_Get_ArpMode_Cont
SEQ_PTYPE_Get_NoArpMode


SEQ_PTYPE_Get_End
	movf	SEQ_PTYPE, W, BANKED
	return


;; --------------------------------------------------------------------------
;;  this function checks if the current track is in Arpeggiator mode
;;  IN: SEQ_EVNTT: track number (0-15 for MIDIboxSEQ)
;;      SEQ_SELECTED_LAYERS: selected layer (A/B/C)
;;  OUT: Z flag set if in ARP mode
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_PTYPE_ChkArpMode
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x0f
	xorlw	SEQ_TRKMODE_ARPEGGIATOR
	bz	SEQ_PTYPE_ChkArpMode_End
	movf	INDF1, W
	andlw	0x0f
	xorlw	SEQ_TRKMODE_ARPEGGIATOR_HOLD
SEQ_PTYPE_ChkArpMode_End
	return
