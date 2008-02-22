; $Id$
;
; AOUT_LC driver
; 
#if 1
	;; common AOUT_LC connections
#define CV_AOUT_LC_LAT_RCLK	LATC	; The latch enable input
#define CV_AOUT_LC_TRIS_RCLK	TRISC	; is connected to Port C.5
#define CV_AOUT_LC_PIN_RCLK	5	; (CANNOT be shared with other outputs!)
;
#define CV_AOUT_LC_LAT_DOUT	LATC	; The data pin
#define CV_AOUT_LC_TRIS_DOUT	TRISC	; is connected to Port C.4
#define CV_AOUT_LC_PIN_DOUT	4	; (can be shared with other outputs)
;
#define CV_AOUT_LC_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define CV_AOUT_LC_TRIS_SCLK	TRISD	; is connected to Port D.5
#define CV_AOUT_LC_PIN_SCLK	5	; (can be shared with other outputs)
#endif
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_LC_Init
;;  DESCRIPTION: This function initializes all connected 74HC595
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
CV_AOUT_LC_Init
	;; enable pin drivers
	bcf	CV_AOUT_LC_TRIS_RCLK, CV_AOUT_LC_PIN_RCLK
	bcf	CV_AOUT_LC_TRIS_DOUT, CV_AOUT_LC_PIN_DOUT
	bcf	CV_AOUT_LC_TRIS_SCLK, CV_AOUT_LC_PIN_SCLK

	;; clear the gate pins
	clrf	AOUT_GATE
	setf	AOUT_GATE_LAST

	;; set voltages to 0V
	lfsr	FSR0, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
CV_AOUT_LC_InitVoutLoop
	clrf	POSTINC0	; AOUTx_L
	clrf	POSTINC0	; AOUTx_H
	decfsz	MIOS_PARAMETER3, F
	rgoto	CV_AOUT_LC_InitVoutLoop

	;; update the AOUT pins
	rcall	CV_AOUT_LC_Update

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_LC_Load2SR
;;  DESCRIPTION: This function loads a 16bit value into the shift register
;;  IN:   o low byte of SR value in TMP1
;;        o high byte of SR value in TMP2
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
CV_AOUT_LC_Load2SR
        bcf     CV_AOUT_LC_LAT_SCLK, CV_AOUT_LC_PIN_SCLK	; clear clock

	;; superfast transfer with unrolled loop (takes some memory, but guarantees the
	;; lowest system load :)
CV_AOUT_LC_WRITE_BIT MACRO reg, bit
	bcf	CV_AOUT_LC_LAT_DOUT, CV_AOUT_LC_PIN_DOUT	; set out pin depending on register content (reg.bit)
	btfsc	reg, bit
	bsf	CV_AOUT_LC_LAT_DOUT, CV_AOUT_LC_PIN_DOUT
	nop
        bsf     CV_AOUT_LC_LAT_SCLK, CV_AOUT_LC_PIN_SCLK	; rising clock edge
        bcf     CV_AOUT_LC_LAT_SCLK, CV_AOUT_LC_PIN_SCLK	; falling clock edge
	ENDM

	CV_AOUT_LC_WRITE_BIT TMP1, 7
	CV_AOUT_LC_WRITE_BIT TMP1, 6
	CV_AOUT_LC_WRITE_BIT TMP1, 5
	CV_AOUT_LC_WRITE_BIT TMP1, 4
	CV_AOUT_LC_WRITE_BIT TMP1, 3
	CV_AOUT_LC_WRITE_BIT TMP1, 2
	CV_AOUT_LC_WRITE_BIT TMP1, 1
	CV_AOUT_LC_WRITE_BIT TMP1, 0

	CV_AOUT_LC_WRITE_BIT TMP2, 7
	CV_AOUT_LC_WRITE_BIT TMP2, 6
	CV_AOUT_LC_WRITE_BIT TMP2, 5
	CV_AOUT_LC_WRITE_BIT TMP2, 4
	CV_AOUT_LC_WRITE_BIT TMP2, 3
	CV_AOUT_LC_WRITE_BIT TMP2, 2
	CV_AOUT_LC_WRITE_BIT TMP2, 1
	CV_AOUT_LC_WRITE_BIT TMP2, 0

	;; this must be done once all SRs in the chain have been uploaded!
#if 0
        bsf     CV_AOUT_LC_LAT_RCLK, CV_AOUT_LC_PIN_RCLK	; latch SID values
	bcf	CV_AOUT_LC_LAT_DOUT, CV_AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     CV_AOUT_LC_LAT_RCLK, CV_AOUT_LC_PIN_RCLK	; release latch
#endif

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_LC_LoadAOUTx
;;  DESCRIPTION: This function loads two AOUT values
;;  IN:   o pointer to first AOUTx_L in FSR1
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
CV_AOUT_LC_LoadAOUTx_12_8
	;; 12bit/8bit
	swapf	PREINC1, W	; AOUT0_H[3:0] -> TMP2[7:4]
	andlw	0xf0
	movwf	TMP2

	movf	POSTDEC1, W	; AOUT0_L
	swapf	INDF1, W	; AOUT0_L[7:4] -> TMP2[3:0]
	andlw	0x0f
	iorwf	TMP2, F

	swapf	INDF1, W	; AOUT0_L[3:0] -> TMP1[7:4]
	andlw	0xf0
	movwf	TMP1

	movf	POSTINC1, W	; AOUT0_H
	movf	POSTINC1, W	; AOUT1_L
	movf	PREINC1, W	; AOUT1_H[3:0] -> TMP1[3:0]
	andlw	0x0f
	iorwf	TMP1, F

	bsf	INDF1, 7	; AOUT1_H
	movf	POSTDEC1, W	; AOUT1_L
	movf	POSTDEC1, W	; AOUT0_H
	bsf	INDF1, 7	; AOUT0_H
	movf	POSTDEC1, W	; AOUT0_L
	rgoto	CV_AOUT_LC_Load2SR

CV_AOUT_LC_LoadAOUTx_8_8
	;; 8bit/8bit
	swapf	PREINC1, W	; AOUT0_H[3:0] -> TMP2[7:4]
	andlw	0xf0
	movwf	TMP2

	movf	POSTDEC1, W	; AOUT0_L
	swapf	INDF1, W	; AOUT0_L[7:4] -> TMP2[3:0]
	andlw	0x0f
	iorwf	TMP2, F

	movf	POSTINC1, W	; AOUT0_H
	movf	POSTINC1, W	; AOUT1_L
	swapf	PREINC1, W	; AOUT1_H[3:0] -> TMP1[7:4]
	andlw	0xf0
	movwf	TMP1

	movf	POSTDEC1, W	; AOUT1_L
	swapf	INDF1, W	; AOUT1_L[7:4] -> TMP1[3:0]
	andlw	0x0f
	iorwf	TMP1, F

	bsf	POSTINC1, 7	; AOUT1_H
	movf	POSTDEC1, W	; AOUT1_L
	movf	POSTDEC1, W	; AOUT0_H
	bsf	INDF1, 7	; AOUT0_H
	movf	POSTDEC1, W	; AOUT0_L
	rgoto	CV_AOUT_LC_Load2SR

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_LC_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
CV_AOUT_LC_Update

	;; check if any output has to be updated
	BRA_IFCLR AOUT0_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT1_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT2_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT3_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT4_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT5_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT6_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	BRA_IFCLR AOUT7_H, 7, ACCESS, CV_AOUT_LC_Update_SRs
	rgoto	CV_AOUT_LC_Update_NoSRs
CV_AOUT_LC_Update_SRs
	lfsr	FSR1, AOUT6_L		; loads AOUT6_[LH] and AOUT7_[LH]
#if DEFAULT_AOUT_LC_4_RESOLUTION == 1
	rcall	CV_AOUT_LC_LoadAOUTx_12_8
#else
	rcall	CV_AOUT_LC_LoadAOUTx_8_8
#endif

	lfsr	FSR1, AOUT4_L		; loads AOUT4_[LH] and AOUT5_[LH]
#if DEFAULT_AOUT_LC_3_RESOLUTION == 1
	rcall	CV_AOUT_LC_LoadAOUTx_12_8
#else
	rcall	CV_AOUT_LC_LoadAOUTx_8_8
#endif

	lfsr	FSR1, AOUT2_L		; loads AOUT2_[LH] and AOUT3_[LH]
#if DEFAULT_AOUT_LC_2_RESOLUTION == 1
	rcall	CV_AOUT_LC_LoadAOUTx_12_8
#else
	rcall	CV_AOUT_LC_LoadAOUTx_8_8
#endif

	lfsr	FSR1, AOUT0_L		; loads AOUT0_[LH] and AOUT1_[LH]
#if DEFAULT_AOUT_LC_1_RESOLUTION == 1
	rcall	CV_AOUT_LC_LoadAOUTx_12_8
#else
	rcall	CV_AOUT_LC_LoadAOUTx_8_8
#endif

        bsf     CV_AOUT_LC_LAT_RCLK, CV_AOUT_LC_PIN_RCLK	; latch SID values
	bcf	CV_AOUT_LC_LAT_DOUT, CV_AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     CV_AOUT_LC_LAT_RCLK, CV_AOUT_LC_PIN_RCLK	; release latch

CV_AOUT_LC_Update_NoSRs

CV_AOUT_LC_Update_Gates	
	;; do nothing if gate bits have not been changed
	movf	AOUT_GATE, W
	xorwf	AOUT_GATE_LAST, W
	bz	CV_AOUT_LC_UpdateGates_End

	movff	AOUT_GATE, AOUT_GATE_LAST

	SET_BSR	CV_BASE
	;; set J5 pins if enabled
#if ENABLE_J5
	movf	AOUT_GATE, W
	xorwf	CV_GATE_POLARITY, W		; change polarity if required
	call	J5_DOUT_Set
#endif	

CV_AOUT_LC_UpdateGates_End
CV_AOUT_LC_Update_End

	return
