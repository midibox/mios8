; $Id$
;
; MIDIbox SID
; CC Input Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SID_CCIN_Set
	rcall	SID_CC_TABLE_Get	; prepare a CC action
	;; now:
	;; value     is in MIOS_PARAMETER1
	;; command   is in MIOS_PARAMETER2
	;; parameter is in FSR1 (pointer)

	;; branch depending on command
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SID_CCIN_Cmd_NOP
	rgoto	SID_CCIN_Cmd_V123
	rgoto	SID_CCIN_Cmd_7BIT
	rgoto	SID_CCIN_Cmd_7BIT_W_PORTA
	rgoto	SID_CCIN_Cmd_7BIT_4L
	rgoto	SID_CCIN_Cmd_7BIT_4U
	rgoto	SID_CCIN_Cmd_7BIT_TO_7_4
	rgoto	SID_CCIN_Cmd_7BIT_TO_3_0
	rgoto	SID_CCIN_Cmd_SUSKEY
	rgoto	SID_CCIN_Cmd_VELOCITY
	rgoto	SID_CCIN_Cmd_AFTERTOUCH
	rgoto	SID_CCIN_Cmd_MODWHEEL
	rgoto	SID_CCIN_Cmd_WAVEFORM
	rgoto	SID_CCIN_Cmd_MONO
	rgoto	SID_CCIN_Cmd_POLY
	rgoto	SID_CCIN_Cmd_PlayNote

; ==========================================================================

	;; ------------------------------------------------------------------
	;; Controller Handler Macro
	;; ------------------------------------------------------------------
SID_CCIN_SETHLP_MACRO_DO_CTRL MACRO LAST_VALUE, DEPTH_VALUE, INIT_VALUE, ASSIGN_VALUE
	LOCAL	SID_CCIN_CmdHlp_DoCtrl_Neg
	LOCAL	SID_CCIN_CmdHlp_DoCtrl_Pos
	LOCAL	SID_CCIN_CmdHlp_DoCtrl_Cont
	
	movf	DEPTH_VALUE, W, BANKED	; calc (DEPTH+1)*4 * LAST_VALUE
	andlw	0x3f
	btfss	DEPTH_VALUE, 6, BANKED
	sublw 0x3f
	clrc
	rlf	WREG, F
	rlf	WREG, W
	addlw	3
	mulwf	LAST_VALUE, BANKED

	movf	PRODH, W
	BRA_IFSET DEPTH_VALUE, 6, BANKED, SID_CCIN_CmdHlp_DoCtrl_Pos
SID_CCIN_CmdHlp_DoCtrl_Neg
	xorlw	0xff
	addwf	INIT_VALUE, W, BANKED
	btfsc	WREG, 7
	movlw 0x00
	goto	SID_CCIN_CmdHlp_DoCtrl_Cont
SID_CCIN_CmdHlp_DoCtrl_Pos
	addwf	INIT_VALUE, W, BANKED
	btfsc	WREG, 7
	movlw 0x7f
SID_CCIN_CmdHlp_DoCtrl_Cont
	movwf	MIOS_PARAMETER1
	movf	ASSIGN_VALUE, W, BANKED
	ENDM

; ==========================================================================

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_NOP
	;; no jump to end, since no CS update is required
	;; 	rgoto	SID_CCIN_Cmd_End
	return

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_V123
	movf	FSR1L, W		; parameter contains first CC, save it
	movwf	MIOS_PARAMETER3
	rcall	SID_CCIN_Set		; set first voice
	incf	MIOS_PARAMETER3, F
	movf	MIOS_PARAMETER3, W
	rcall	SID_CCIN_Set		; set second voice
	incf	MIOS_PARAMETER3, W
	rgoto	SID_CCIN_Set		; set third voice

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_7BIT
	movff	MIOS_PARAMETER1, INDF1
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_7BIT_W_PORTA
	bcf	SID_STAT, SID_STAT_SAVED_GIE	; for nested IRQ disables
	btfsc	INTCON, GIE
	bsf	SID_STAT, SID_STAT_SAVED_GIE
	IRQ_DISABLE

	;; works only with SID_Vx_TRANSPOSE, _FINETUNE, _PITCHRANGE
	movff	MIOS_PARAMETER1, INDF1

#if 0
	;; enable portamento if rate is > 0
	;; switch to SID_Vx_PORTA_RATE
	movf	FSR1L, W
	andlw	0x03
	iorlw	SID_V1_PORTA_RATE & 0xfc
	movwf	FSR1L
	movf	INDF1, W
	bz	SID_CCIN_Cmd_7BIT_W_PORTA_End

	movf	FSR1L, W
	andlw	0x03
	iorlw	SID_V1_MODE & 0xfc
	movwf	FSR1L
	bsf	INDF1, SID_MODE_PORTA_ENABLE
#endif

SID_CCIN_Cmd_7BIT_W_PORTA_End
	;; 	IRQ_ENABLE
	btfsc	SID_STAT, SID_STAT_SAVED_GIE	; nested IRQ enable
	bsf	INTCON, GIE
	
	rgoto	SID_CCIN_Cmd_End


;; --------------------------------------------------------------------------
SID_CCIN_Cmd_7BIT_4L
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_7BIT_4U
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_7BIT_TO_7_4
	movlw	0x0f
	andwf	INDF1, F
	rlf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_7BIT_TO_3_0
	movlw	0xf0
	andwf	INDF1, F
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x0f
	iorwf	INDF1, F
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_SUSKEY
	bcf	SID_PLAY_MODE, SID_PLAY_MODE_SUS_KEY, BANKED
	movf	MIOS_PARAMETER1, W
	skpz
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_SUS_KEY, BANKED

	rrf	MIOS_PARAMETER1, W
	andlw	0x3f
	movwf	SID_KEYTRACKING, BANKED

	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_VELOCITY
	movff	MIOS_PARAMETER1, INDF1

	movf	SID_ASSIGN_VELOCITY, W, BANKED	; exit when velocity assigned to 0
	bz	SID_CCIN_Cmd_VELOCITY_End
	xorlw	2				; CC#2 is Velocity Init
	bz	SID_CCIN_Cmd_VELOCITY_End	; prevent endless loop
	movf	SID_ASSIGN_VELOCITY, W, BANKED
	xorlw	13				; CC#13 is Velocity Depth
	bz	SID_CCIN_Cmd_VELOCITY_End	; prevent endless loop

	SID_CCIN_SETHLP_MACRO_DO_CTRL SID_LAST_VELOCITY, SID_DEPTH_VELOCITY, SID_INIT_VELOCITY, SID_ASSIGN_VELOCITY
	rgoto	SID_CCIN_Set
	
SID_CCIN_Cmd_VELOCITY_End
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_AFTERTOUCH
	movff	MIOS_PARAMETER1, INDF1

	movf	SID_ASSIGN_AFTERTOUCH, W, BANKED; exit when aftertouch assigned to 0
	bz	SID_CCIN_Cmd_AFTERTOUCH_End
	xorlw	4				; CC#4 is Aftertouch Init
	bz	SID_CCIN_Cmd_AFTERTOUCH_End	; prevent endless loop
	movf	SID_ASSIGN_AFTERTOUCH, W, BANKED
	xorlw	15				; CC#15 is Aftertouch Depth
	bz	SID_CCIN_Cmd_AFTERTOUCH_End	; prevent endless loop

	SID_CCIN_SETHLP_MACRO_DO_CTRL SID_LAST_AFTERTOUCH, SID_DEPTH_AFTERTOUCH, SID_INIT_AFTERTOUCH, SID_ASSIGN_AFTERTOUCH
	rgoto	SID_CCIN_Set
	
SID_CCIN_Cmd_AFTERTOUCH_End
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_MODWHEEL
	movff	MIOS_PARAMETER1, INDF1
	movf	SID_ASSIGN_MODWHEEL, W, BANKED	; exit when modwheel assigned to 0
	bz	SID_CCIN_Cmd_MODWHEEL_End
	decf	SID_ASSIGN_MODWHEEL, W, BANKED	; prevent endless loop if assigned to CC#1
	bz	SID_CCIN_Cmd_MODWHEEL_End
	movf	SID_ASSIGN_MODWHEEL, W, BANKED
	xorlw	3				; CC#3 is Modwheel Init
	bz	SID_CCIN_Cmd_MODWHEEL_End	; prevent endless loop
	movf	SID_ASSIGN_MODWHEEL, W, BANKED
	xorlw	14				; CC#14 is Modulation Wheel Depth
	bz	SID_CCIN_Cmd_MODWHEEL_End	; prevent endless loop

	SID_CCIN_SETHLP_MACRO_DO_CTRL SID_LAST_MODWHEEL, SID_DEPTH_MODWHEEL, SID_INIT_MODWHEEL, SID_ASSIGN_MODWHEEL
	rgoto	SID_CCIN_Set

SID_CCIN_Cmd_MODWHEEL_End
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_WAVEFORM
	movlw	0x01
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf6
	btfsc	MIOS_PARAMETER1, 3; ensure that noise will not "lock on" like described in 6581 spec
	andlw 0x8f 
	iorwf	INDF1, F

	movf	FSR1L, W
	xorlw	SID_V3_CTRL & 0xff
	bz	SID_CCIN_Cmd_WAVEFORM_V3
	movf	FSR1L, W
	xorlw	SID_V2_CTRL & 0xff
	bz	SID_CCIN_Cmd_WAVEFORM_V2
SID_CCIN_Cmd_WAVEFORM_V1
	movlw	(SID_V1_BASE + SID_Vx_MODE) & 0xff
	rgoto	SID_CCIN_Cmd_WAVEFORM_Cont
SID_CCIN_Cmd_WAVEFORM_V2
	movlw	(SID_V2_BASE + SID_Vx_MODE) & 0xff
	rgoto	SID_CCIN_Cmd_WAVEFORM_Cont
SID_CCIN_Cmd_WAVEFORM_V3
	movlw	(SID_V3_BASE + SID_Vx_MODE) & 0xff
SID_CCIN_Cmd_WAVEFORM_Cont
	movwf	FSR1L
	BRA_IFSET INDF1, SID_MODE_VOICE_DISABLED, ACCESS, SID_CCIN_CmdHlp_Waveform_WD
SID_CCIN_CmdHlp_Waveform_WND
	BRA_IFCLR MIOS_PARAMETER1, 4, ACCESS, SID_CCIN_Cmd_WAVEFORM_End
	bsf	INDF1, SID_MODE_VOICE_DISABLED
	bsf	INDF1, SID_MODE_GATE_CLR_REQ
	rgoto	SID_CCIN_Cmd_WAVEFORM_End

SID_CCIN_CmdHlp_Waveform_WD
	BRA_IFSET MIOS_PARAMETER1, 4, ACCESS, SID_CCIN_Cmd_WAVEFORM_End
	bcf	INDF1, SID_MODE_VOICE_DISABLED
	btfsc	INDF1, SID_MODE_VOICE_ACTIVE
	bsf	INDF1, SID_MODE_GATE_SET_REQ

SID_CCIN_Cmd_WAVEFORM_End
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_MONO
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED
	movf	MIOS_PARAMETER1, W
	skpz
	bcf	SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED
	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_POLY
	bcf	SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_ONLY_WT_OFF, BANKED

	btfsc	MIOS_PARAMETER1, 0
	bcf	SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED

	btfsc	MIOS_PARAMETER1, 1
	bcf	SID_PLAY_MODE, SID_PLAY_MODE_ONLY_WT_OFF, BANKED

	btfsc	MIOS_PARAMETER1, 2
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED

	btfsc	SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED
	bsf	SID_PLAY_MODE, SID_PLAY_MODE_ONLY_WT_OFF, BANKED

	rgoto	SID_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_PlayNote
	decf	MIOS_PARAMETER1, W	; do nothing if 0x01 played (hold note)
	bz	SID_CCIN_Cmd_PlayNote_End

	bcf	SID_STAT, SID_STAT_SAVED_GIE	; for nested IRQ disables
	btfsc	INTCON, GIE
	bsf	SID_STAT, SID_STAT_SAVED_GIE
	IRQ_DISABLE

	movf	FSR1L, W		; parameter contains voice offset, calc base address
	mullw	SID_Vx_RECORD_LEN
	lfsr	FSR1, SID_V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	;; set/clear gate bit
	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SID_CCIN_Cmd_PlayNote_NotTB303
	BRA_IFCLR SID_WT_STATE, WT_STATE_PLAY_2ND, BANKED, SID_CCIN_Cmd_PlayNote_NotTB303
SID_CCIN_Cmd_PlayNote_TB303
	BRA_IFSET SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED, SID_CCIN_Cmd_PlayNote_Cont
	BRA_IFSET SID_WT_STATE, WT_STATE_GATE, BANKED, SID_CCIN_Cmd_PlayNote_On
	;; gate will be released by sequencer
	rgoto	SID_CCIN_Cmd_PlayNote_Cont
SID_CCIN_Cmd_PlayNote_NotTB303

	movf	MIOS_PARAMETER1, W
	bnz	SID_CCIN_Cmd_PlayNote_On
SID_CCIN_Cmd_PlayNote_Off
	;; clear gate bit + note and branch to end
	movlw	SID_Vx_MODE
	bcf	PLUSW1, SID_MODE_GATE_SET_REQ
	bsf	PLUSW1, SID_MODE_GATE_CLR_REQ
	rgoto	SID_CCIN_Cmd_PlayNote_End

SID_CCIN_Cmd_PlayNote_On
	;; set gate bit
	movlw	SID_Vx_MODE
	BRA_IFCLR PLUSW1, SID_MODE_VOICE_ACTIVE, ACCESS, SID_CCIN_Cmd_PlayNote_Cont
	bsf	PLUSW1, SID_MODE_GATE_CLR_REQ
	bsf	PLUSW1, SID_MODE_GATE_SET_REQ

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SID_CCIN_Cmd_PlayNote_On_NoTB303
	BRA_IFCLR SID_WT_STATE, WT_STATE_PLAY_2ND, BANKED, SID_CCIN_Cmd_PlayNote_On_NoTB303
	rgoto	SID_CCIN_Cmd_PlayNote_Cont
SID_CCIN_Cmd_PlayNote_On_NoTB303
	rcall	SID_CCIN_Cmd_PlayNote_PHelp	; help function for alternative portamento
	;; enable portamento if rate > 0
	movlw	SID_Vx_PORTA_RATE
	movf	PLUSW1, F
	bz	SID_CCIN_Cmd_PlayNote_Cont
	movlw	SID_Vx_MODE
	bsf	PLUSW1, SID_MODE_PORTA_ENABLE
SID_CCIN_Cmd_PlayNote_Cont

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SID_CCIN_Cmd_PlayNote_NoTB303Sl
	BRA_IFCLR SID_WT_STATE, WT_STATE_PLAY_2ND, BANKED, SID_CCIN_Cmd_PlayNote_NoTB303Sl
SID_CCIN_Cmd_PlayNote_TB303Sl
	;; enable portamento depending on previous step
	bcf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_PORTA_ENABLE, BANKED
	btfsc	SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED
	bsf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_PORTA_ENABLE, BANKED
	rcall	SID_CCIN_Cmd_PlayNote_PHelp	; help function for alternative portamento
SID_CCIN_Cmd_PlayNote_NoTB303Sl

	;; set new note
	;; if > 0x7c, play arpeggiator note
	movlw	0x7c
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto SID_CCIN_Cmd_PlayNote_Arp
SID_CCIN_Cmd_PlayNote_Norm
	movlw	SID_Vx_NOTE
	movff	MIOS_PARAMETER1, PLUSW1
	rgoto	SID_CCIN_Cmd_PlayNote_End
SID_CCIN_Cmd_PlayNote_Arp
	movlw	SID_Vx_ARP_NOTE_0
	addwf	MIOS_PARAMETER1, W
	addlw	-0x7c
	movf	PLUSW1, W
	bnz	SID_CCIN_Cmd_PlayNote_ArpNow
	movlw	SID_Vx_ARP_NOTE_0	; if xth key not pressed: play first ARP note
	movf	PLUSW1, W
SID_CCIN_Cmd_PlayNote_ArpNow
	movwf	PRODL
	movlw	SID_Vx_NOTE
	movff	PRODL, PLUSW1

SID_CCIN_Cmd_PlayNote_End
	;; 	IRQ_ENABLE
	btfsc	SID_STAT, SID_STAT_SAVED_GIE	; nested IRQ enable
	bsf	INTCON, GIE
	;; 	rgoto	SID_CCIN_Cmd_End

;; --------
;; help function for alternative portamento effect
SID_CCIN_Cmd_PlayNote_PHelp
	btfss	SID_SE_OPTION, SE_OPTION_ENV2PORTA, BANKED
	return

	;; reset ENV2
	call	SID_SW_Hlp_ENV2Reset
	
	;; store current frequency in SID_Vx_PORTA_FRQ_L
	lfsr	FSR2, SID_V1_FRQ_L
	movf	FSR1L, W
	xorlw	SID_V2_BASE & 0xff
	skpnz
	lfsr	FSR2, SID_V2_FRQ_L
	movf	FSR1L, W
	xorlw	SID_V3_BASE & 0xff
	skpnz
	lfsr	FSR2, SID_V3_FRQ_L

	movlw	SID_Vx_PORTA_FRQ_L
	movff	POSTINC2, PLUSW1
	movlw	SID_Vx_PORTA_FRQ_H
	movff	POSTDEC2, PLUSW1
	
	return

;; --------------------------------------------------------------------------
SID_CCIN_Cmd_End
	return

