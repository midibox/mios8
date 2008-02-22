; $Id$
;
; Register Access Routines for MIDIbox FM
;
; define the pins to which the MBHP_OPL3 module is connected
;
MBFM_LAT_D	EQU	LATB		; Port B
MBFM_LAT_A1	EQU	LATD		; Pin D.6
MBFM_PIN_A1	EQU	6
MBFM_LAT_A0	EQU	LATD		; Pin D.5
MBFM_PIN_A0	EQU	5
MBFM_LAT_CS	EQU	LATC		; Pin C.5
MBFM_PIN_CS	EQU	5
MBFM_LAT_IC	EQU	LATC		; Pin C.4
MBFM_PIN_IC	EQU	4
;
; Activate this #define to measure the performance with a scope
; (connect the probe to J14)
#define MBFM_REG_MEASURE_PERFORMANCE 0
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  Initialize the MBHP_OPL3 module
;; --------------------------------------------------------------------------
MBFM_REG_Init
	;; activate reset of OPL3 module (low-active)
	bcf	MBFM_LAT_IC, MBFM_PIN_IC

	;; release chip select (low-active)
	bsf	MBFM_LAT_CS, MBFM_PIN_CS

	;; wait for at least 400/fM cycles
	rcall	MBFM_REG_Init_Hlp_Wait
	
	;; release reset
	bsf	MBFM_LAT_IC, MBFM_PIN_IC

	;; wait again for some cycles
	rcall	MBFM_REG_Init_Hlp_Wait

	;; enable OPL3 extension and switch to 4OP voices
	movlw	0x05
	movwf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	movlw	0x01
	rcall	MBFM_REG_Write

	rcall	MBFM_REG_WaitOPL

	movlw	0x04
	movwf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	movlw	0x3f
	rgoto	MBFM_REG_Write


;; --- help routine which waits for at least 400/fM cycles
MBFM_REG_Init_Hlp_Wait
	clrf	TMP1
	clrf	TMP2
MBFM_REG_Init_Hlp_WaitLoop
	clrwdt
	decfsz	TMP1, F
	rgoto	MBFM_REG_Init_Hlp_WaitLoop
	decfsz	TMP2, F
	rgoto	MBFM_REG_Init_Hlp_WaitLoop
	return


;; --------------------------------------------------------------------------
;;  Register Write: write to OPL3 register
;;  IN: Address in MIOS_PARAMETER1 and MIOS_PARAMETER2[0]
;;      Data in WREG (will be saved in MIOS_PARAMETER3)
;; --------------------------------------------------------------------------
MBFM_REG_Write
	movwf	MIOS_PARAMETER3			; save data word

	;; transfer address to OPL3
	bcf	MBFM_LAT_A0, MBFM_PIN_A0	; A0=0: address write mode
	bcf	MBFM_LAT_A1, MBFM_PIN_A1	; A1=selected array
	btfsc	MIOS_PARAMETER2, 0
	bsf	MBFM_LAT_A1, MBFM_PIN_A1
	movff	MIOS_PARAMETER1, MBFM_LAT_D	; D0-D7: selected address
	bcf	MBFM_LAT_CS, MBFM_PIN_CS	; activate chip select line
	nop
	bsf	MBFM_LAT_CS, MBFM_PIN_CS	; release chip select line

	movlw	5				; wait for 32 OPL3 clock cycles (ca. 2.2 uS)
	movwf	TMP2				; (we are calculating from the movlw to the next CS)
MBFM_REG_Write_WaitLoop1
	decfsz	TMP2, F
	rgoto	MBFM_REG_Write_WaitLoop1

	;; transfer data to OPL3
	bsf	MBFM_LAT_A0, MBFM_PIN_A0	; A0=1: data write mode, don't change A0
	movff	MIOS_PARAMETER3, MBFM_LAT_D	; D0-D7: data word
	bcf	MBFM_LAT_CS, MBFM_PIN_CS	; activate chip select line
	nop
	bsf	MBFM_LAT_CS, MBFM_PIN_CS	; release chip select line

	;; the additional 32 waitcycles will be inserted by return/next call

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_REG_WaitOPL
;;  DESCRIPTION: this delay function can be used to wait for ca. 32 OPL3 cycles
;;  OUT:  -
;;  USES: PRODL
;; --------------------------------------------------------------------------
MBFM_REG_WaitOPL
	movlw	5				; wait for 32 OPL3 clock cycles (ca. 2.2 uS)
	movwf	PRODL				; (we are calculating from the movlw to the next CS)
MBFM_REG_WaitOPLLoop
	decfsz	PRODL, F
	rgoto	MBFM_REG_WaitOPLLoop
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_REG_Update
;;  DESCRIPTION: called to update the registers
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MBFM_REG_Update
	;; exit if engine is disabled
	btfsc	MBFM_STAT, MBFM_STAT_ENGINE_DISABLE
	return

	SET_BSR	MBFM_BASE

	;; do nothing if no update request from sound engine (mbfm_se.inc)
	btfss	MBFM_SE_STAT, MBFM_SE_STAT_UPDATE_REGS, BANKED
	return
	bcf	MBFM_SE_STAT, MBFM_SE_STAT_UPDATE_REGS, BANKED

#if MBFM_REG_MEASURE_PERFORMANCE
	bsf	PORTD, 4
#endif

	;; update drum registers
	clrf	MIOS_PARAMETER2	; (MIOS_PARAMETER2 always zero here!)

	BRA_IFCLR MBFM_UPDATE_DR, MBFM_UPDATE_DR_BD_FNUM, BANKED, MBFM_REG_Update_NotBDFNUM
MBFM_REG_Update_BD
	lfsr	FSR1, MBFM_DR_BD_FRQL
	movlw	0xa6
	rcall	MBFM_REG_Update_DrumFrq
MBFM_REG_Update_NotBDFNUM

	BRA_IFCLR MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_FNUM, BANKED, MBFM_REG_Update_NotHHFNUM
MBFM_REG_Update_HH
	lfsr	FSR1, MBFM_DR_HH_FRQL
	movlw	0xa7
	rcall	MBFM_REG_Update_DrumFrq
MBFM_REG_Update_NotHHFNUM

	BRA_IFCLR MBFM_UPDATE_DR, MBFM_UPDATE_DR_TOM_FNUM, BANKED, MBFM_REG_Update_NotTOMFNUM
MBFM_REG_Update_TOM
	lfsr	FSR1, MBFM_DR_TOM_FRQL
	movlw	0xa8
	rcall	MBFM_REG_Update_DrumFrq
MBFM_REG_Update_NotTOMFNUM

	BRA_IFCLR MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_OPEN, BANKED, MBFM_REG_Update_NotHHOpen
MBFM_REG_Update_HHOpen
	movlw	0x31
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_O_FLAGS_MULT, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL

	;; KSL_TL already set
	
	movlw	0x71
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_O_AR_DR, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL

	movlw	0x91
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_O_SL_RR, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL

	movlw	0xf1
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_O_WS, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL
MBFM_REG_Update_NotHHOpen

	BRA_IFCLR MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_CLOSE, BANKED, MBFM_REG_Update_NotHHClose
MBFM_REG_Update_HHClose
	movlw	0x31
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_C_FLAGS_MULT, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL

	;; KSL_TL already set
	
	movlw	0x71
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_C_AR_DR, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL

	movlw	0x91
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_C_SL_RR, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL

	movlw	0xf1
	movwf	MIOS_PARAMETER1
	movf	MBFM_DR_HH_C_WS, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL
MBFM_REG_Update_NotHHClose

	BRA_IFCLR MBFM_UPDATE_DR, MBFM_UPDATE_DR_GATES, BANKED, MBFM_REG_Update_NotGates
MBFM_REG_Update_Gates
	movlw	0xbd
	movwf	MIOS_PARAMETER1
	movf	MBFM_DRUM_GATES, W, BANKED
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL
MBFM_REG_Update_NotGates

	;; all drums done
	clrf	MBFM_UPDATE_DR, BANKED

	;; update voice registers
	clrf	MBFM_CURRENT_VOICE, BANKED		; voice number
	clrf	MBFM_REG_VOICE_OFFSET, BANKED		; offset to voice register
	clrf	MIOS_PARAMETER2				; switches between left and right half of OPL3 register set
	lfsr	FSR1, MBFM_V1_BASE

MBFM_REG_Update_Loop
	;; disable interrupts to avoid inconsistencies
	IRQ_DISABLE

	;; do nothing if no update requested
	movlw	MBFM_Vx_UPDATE0
	movf	PLUSW1, W
	bnz	MBFM_REG_Update_Loop_Start

	movlw	MBFM_Vx_UPDATE1
	movf	PLUSW1, W
	bnz	MBFM_REG_Update_Loop_Start
	rgoto	MBFM_REG_Update_Loop_Next

MBFM_REG_Update_Loop_Start
	;; get pointer to instrument (skip if instrument is 0xff!)
	movlw	MBFM_Vx_ASSIGNED_INSTR
	incf	PLUSW1, W
	skpnz
	rgoto	MBFM_REG_Update_Loop_Next
	
	lfsr	FSR0, MBFM_I1_BASE
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F

	;; check which register(s) should be updated
	
	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_FLAGS_MULT, ACCESS, MBFM_REG_Update_Loop_NotMULT
MBFM_REG_Update_Loop_MULT
	lfsr	FSR2, MBFM_Ix_OP1_FLAGS_MULT
	movlw	0x20
	rcall	MBFM_REG_Update_InstrPar
MBFM_REG_Update_Loop_NotMULT

	movlw	MBFM_Vx_UPDATE1
	BRA_IFCLR PLUSW1, MBFM_UPDATE1_OP1_KSL_TL, ACCESS, MBFM_REG_Update_Loop_NotOP1KSLTL
MBFM_REG_Update_Loop_OP1KSLTL
	movlw	0x40
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Vx_OP1_KSL_TL
	movf	PLUSW1, W
	rcall	MBFM_REG_Write
MBFM_REG_Update_Loop_NotOP1KSLTL

	movlw	MBFM_Vx_UPDATE1
	BRA_IFCLR PLUSW1, MBFM_UPDATE1_OP2_KSL_TL, ACCESS, MBFM_REG_Update_Loop_NotOP2KSLTL
MBFM_REG_Update_Loop_OP2KSLTL
	movlw	0x43
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Vx_OP2_KSL_TL
	movf	PLUSW1, W
	rcall	MBFM_REG_Write
MBFM_REG_Update_Loop_NotOP2KSLTL
	
	movlw	MBFM_Vx_UPDATE1
	BRA_IFCLR PLUSW1, MBFM_UPDATE1_OP3_KSL_TL, ACCESS, MBFM_REG_Update_Loop_NotOP3KSLTL
MBFM_REG_Update_Loop_OP3KSLTL
	movlw	0x48
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Vx_OP3_KSL_TL
	movf	PLUSW1, W
	rcall	MBFM_REG_Write
MBFM_REG_Update_Loop_NotOP3KSLTL
	
	movlw	MBFM_Vx_UPDATE1
	BRA_IFCLR PLUSW1, MBFM_UPDATE1_OP4_KSL_TL, ACCESS, MBFM_REG_Update_Loop_NotOP4KSLTL
MBFM_REG_Update_Loop_OP4KSLTL
	movlw	0x4b
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Vx_OP4_KSL_TL
	movf	PLUSW1, W
	rcall	MBFM_REG_Write
MBFM_REG_Update_Loop_NotOP4KSLTL


	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_AR_DR, ACCESS, MBFM_REG_Update_Loop_NotARDR
MBFM_REG_Update_Loop_ARDR
	lfsr	FSR2, MBFM_Ix_OP1_AR_DR
	movlw	0x60
	rcall	MBFM_REG_Update_InstrPar
MBFM_REG_Update_Loop_NotARDR

	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_SL_RR, ACCESS, MBFM_REG_Update_Loop_NotSLRR
MBFM_REG_Update_Loop_SLRR
	lfsr	FSR2, MBFM_Ix_OP1_SL_RR
	movlw	0x80
	rcall	MBFM_REG_Update_InstrPar
MBFM_REG_Update_Loop_NotSLRR

	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_CH_FB_CNT, ACCESS, MBFM_REG_Update_Loop_NotCHFBCNT
MBFM_REG_Update_Loop_CHFBCNT
	movlw	0xc0
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1

#if SUPERPOLY_EXPERIMENT
	;; use static OP->channel assignments for operators
	;; (dirty implementation - we bypass MBFM_INSTR_SendPar and the instrument setting!)
	movlw	4
	cpfslt	MBFM_CURRENT_VOICE, BANKED
	rgoto MBFM_REG_Update_Loop_NotCHFBCNT
	;; determine channel depending on voice
	movf	MBFM_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andlw	0xf0
	movwf	PRODL		; -> PRODL
	;; extract feedback
	movlw	MBFM_Ix_OP12_CH_FB_CNT
	movf	PLUSW0, W
	andlw	0x0f		; insert static channel setting
	iorwf	PRODL, W
 	rcall	MBFM_REG_Write

	movlw	0x03-0x00
	addwf	MIOS_PARAMETER1, F
	movlw	MBFM_Ix_OP34_CH_FB_CNT
	movf	PLUSW0, W
	andlw	0x0f		; insert static channel setting
	iorwf	PRODL, W
 	rcall	MBFM_REG_Write
	
#else
	movlw	MBFM_Ix_OP12_CH_FB_CNT
	movf	PLUSW0, W
 	rcall	MBFM_REG_Write
	
	movlw	0x03-0x00
	addwf	MIOS_PARAMETER1, F
	movlw	MBFM_Ix_OP34_CH_FB_CNT
	movf	PLUSW0, W
 	rcall	MBFM_REG_Write
#endif
MBFM_REG_Update_Loop_NotCHFBCNT

	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_WS, ACCESS, MBFM_REG_Update_Loop_NotWS
MBFM_REG_Update_Loop_WS
	lfsr	FSR2, MBFM_Ix_OP1_WS
	movlw	0xe0
	rcall	MBFM_REG_Update_InstrPar
MBFM_REG_Update_Loop_NotWS

	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_FNUML, ACCESS, MBFM_REG_Update_Loop_NotFNUML
MBFM_REG_Update_Loop_FNUML
	movlw	0xa0
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Vx_FNUML
	movf	PLUSW1, W
 	rcall	MBFM_REG_Write
MBFM_REG_Update_Loop_NotFNUML

	movlw	MBFM_Vx_UPDATE0
	BRA_IFCLR PLUSW1, MBFM_UPDATE0_FNUMH, ACCESS, MBFM_REG_Update_Loop_NotFNUMH
MBFM_REG_Update_Loop_FNUMH
	movlw	0xb0
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	MBFM_Vx_FNUMH
	movf	PLUSW1, W
	rcall	MBFM_REG_Write
MBFM_REG_Update_Loop_NotFNUMH

	;; clear all update flags (saves some bcf's)
	movlw	MBFM_Vx_UPDATE0
	clrf	PLUSW1
	movlw	MBFM_Vx_UPDATE1
	clrf	PLUSW1

MBFM_REG_Update_Loop_Next
	;; clear "not done" flag to sync with interrupt driven sound engine
	movlw	MBFM_Vx_STAT
	bcf	PLUSW1, MBFM_STAT_UPDATE_NOT_DONE

	;; enable interrupts again
	IRQ_ENABLE

	;; increment base pointer to next voice and loop until last voice reached
	incf	MBFM_CURRENT_VOICE, F, BANKED
	movlw	MBFM_Vx_NUM
	cpfslt	MBFM_CURRENT_VOICE, BANKED
	rgoto MBFM_REG_Update_Loop_End

	incf	MBFM_REG_VOICE_OFFSET, F, BANKED
	movlw	3-1
	cpfsgt	MBFM_REG_VOICE_OFFSET, BANKED
	rgoto MBFM_REG_Update_Loop_Next_NoSw
MBFM_REG_Update_Loop_Next_Sw
	;; switch to second half of OPL3 address range
	clrf	MBFM_REG_VOICE_OFFSET, BANKED
	bsf	MIOS_PARAMETER2, 0
MBFM_REG_Update_Loop_Next_NoSw
	
	movlw	MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F
	SET_BSR	MBFM_BASE
	rgoto	MBFM_REG_Update_Loop
MBFM_REG_Update_Loop_End

#if MBFM_REG_MEASURE_PERFORMANCE
	bcf	PORTD, 4
#endif

	return


;; --------------------------------------------------------------------------
;;  Help function to update the drum frequency
;;  IN: pointer to MBFM_DR_BD_FRQL in FSR1
;;      offset to FNUML register in WREG
;; --------------------------------------------------------------------------
MBFM_REG_Update_DrumFrq
	movwf	MIOS_PARAMETER1
	swapf	POSTINC1, W	; FRQL
	andlw	0x0f
	movwf	MIOS_PARAMETER3
	swapf	INDF1, W	; FRQH
	andlw	0xf0
	iorwf	MIOS_PARAMETER3, W
	rcall	MBFM_REG_Write
	rcall	MBFM_REG_WaitOPL
	bsf	MIOS_PARAMETER1, 4	; 0xa. -> 0xb. (e.g. 0xa6 -> 0xb6)
	swapf	INDF1, W
	andlw	0x03
	iorlw	2 << 2		; always increase by 2 octaves
	rcall	MBFM_REG_Write
	rgoto	MBFM_REG_WaitOPL

;; --------------------------------------------------------------------------
;;  Help function to update an instrument parameter
;;  IN: pointer to MBFM_Ix_OP1_xxx in FSR2
;;      offset to OP1 register in WREG
;; --------------------------------------------------------------------------
MBFM_REG_Update_InstrPar
	addwf	MBFM_REG_VOICE_OFFSET, W, BANKED
	movwf	MIOS_PARAMETER1

	movf	FSR0L, W
	addwf	FSR2L, F
	movf	FSR0H, W
	addwfc	FSR2H, F

	movf	POSTINC2, W
	rcall	MBFM_REG_Write
	
	movlw	0x03-0x00
	addwf	MIOS_PARAMETER1, F
	movf	POSTINC2, W
	rcall	MBFM_REG_Write
	
	movlw	0x08-0x03
	addwf	MIOS_PARAMETER1, F
	movf	POSTINC2, W
	rcall	MBFM_REG_Write
	
	movlw	0x0b-0x08
	addwf	MIOS_PARAMETER1, F
	movf	POSTINC2, W
	rgoto	MBFM_REG_Write
