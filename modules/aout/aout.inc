; $Id$
;
; Toplevel of AOUT driver
; 
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; Pin Configuration
; Can be overloaded from external
#ifndef AOUT_LAT_CS
#define AOUT_LAT_CS	LATC	; The chip select pin CS#
#endif
#ifndef AOUT_TRIS_CS
#define AOUT_TRIS_CS	TRISC	; is connected to Port C.5
#endif
#ifndef AOUT_PIN_CS
#define AOUT_PIN_CS	5	; (CANNOT be shared with other outputs!)
#endif
;
#ifndef AOUT_LAT_DIN
#define AOUT_LAT_DIN	LATC	; The data input pin DIN
#endif
#ifndef AOUT_TRIS_DIN
#define AOUT_TRIS_DIN	TRISC	; is connected to Port C.4
#endif
#ifndef AOUT_PIN_DIN
#define AOUT_PIN_DIN	4	; (can be shared with other outputs)
#endif
;
#ifndef AOUT_LAT_SCLK
#define AOUT_LAT_SCLK	LATD	; The shift clock input pin SCLK
#endif
#ifndef AOUT_TRIS_SCLK
#define AOUT_TRIS_SCLK	TRISD	; is connected to Port D.5
#endif
#ifndef AOUT_PIN_SCLK
#define AOUT_PIN_SCLK	5	; (can be shared with other outputs)
#endif
;
; ==========================================================================
; include driver defined by AOUT_INTERFACE_TYPE
; (unfortunately, gpasm doesn't support #elif :-( 
#if AOUT_INTERFACE_TYPE == 0
#include "aout_dummy.inc"
#endif
#if AOUT_INTERFACE_TYPE == 1
#include "aout_max525.inc"
#endif
#if AOUT_INTERFACE_TYPE == 2
#include "aout_74hc595.inc"
#endif
#if AOUT_INTERFACE_TYPE == 3
#include "aout_tlv5630.inc"
#endif

#if AOUT_INTERFACE_TYPE < 0 || AOUT_INTERFACE_TYPE >= 4
  error "specified AOUT_INTERFACE_TYPE not supported by AOUT driver!!!"
#endif

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Init
;;  DESCRIPTION: This function initializes the connected AOUT module
;;  It should be called from USER_Init during startup
;;  IN:   inversion flags in MIOS_PARAMETER1
;;        future extension flags in MIOS_PARAMETER[23] (should be initialized with zero)
;;  OUT:  -
;;  USES: BSR, MIOS_PARAMETER[123] as temporary registers
;; --------------------------------------------------------------------------
AOUT_Init
	;; transfer inversion flags to AOUT_INVERTED
	movff	MIOS_PARAMETER1, AOUT_INVERTED

	;; call hardware specific initialisation
	;; (function located in selected aout_*.inc)
	rcall	AOUT_InitHW

	;; set all voltages to 0V
	clrf	MIOS_PARAMETER1	; using MIOS_PARAMETER1 as loop counter
	lfsr	FSR1, AOUT_VALUES
AOUT_Init_Loop
	clrf	POSTINC1	; low-byte
	clrf	POSTINC1	; high-byte
	incf	MIOS_PARAMETER1, F
	BRA_IFCLR MIOS_PARAMETER1, 3, ACCESS, AOUT_Init_Loop	; loop 8 times

	;; request update of all channels
	SET_BSR	AOUT_UPDATE_REQ
	setf	AOUT_UPDATE_REQ, BANKED

	;; update channels and exit
	rgoto	AOUT_Update


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Pin16bitSet
;;  DESCRIPTION: This function sets an output channel to a given 16-bit value.
;;  The output value won't be transfered to the module immediately, but will 
;;  be buffered in AOUT_[LH] instead. By calling AOUT_Update the requested
;;  changes will take place.
;;  IN:   channel number in WREG, 16bit value in MIOS_PARAMETER[12]
;;  OUT:  -
;;  USES: FSR1, BSR, MIOS_PARAMETER3 as temporary register
;; --------------------------------------------------------------------------
AOUT_Pin16bitSet
	;; temporary store channel number in MIOS_PARAMETER3
	movwf	MIOS_PARAMETER3

	;; calculate address to array
	lfsr	FSR1, AOUT_VALUES
	rlf	WREG, W
	andlw	0xfe
	addwf	FSR1L, F

	;; check if value has been changed
	movf	INDF1, W
	xorwf	MIOS_PARAMETER1, W
	bnz	AOUT_Pin16bitSet_Changed
	movlw	1
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	bz	AOUT_Pin16bitSet_NotChanged

AOUT_Pin16bitSet_Changed	
	;; transfer value to AOUT_[LH] array element
	movff	MIOS_PARAMETER1, POSTINC1
	movff	MIOS_PARAMETER2, POSTDEC1

	;; notify that value has to be updated
	movf	MIOS_PARAMETER3, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	AOUT_UPDATE_REQ
	iorwf	AOUT_UPDATE_REQ, F, BANKED

AOUT_Pin16bitSet_NotChanged

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Pin7bitSet
;;  DESCRIPTION: This function sets an output channel to a given 7-bit value.
;;  Since the AOUT driver supports up to 16bit resolution, the lower bits
;;  will be padded with zero
;;  The output value won't be transfered to the module immediately, but will 
;;  be buffered in AOUT_[LH] instead. By calling AOUT_Update the requested
;;  changes will take place.
;;  IN:   channel number in WREG, 7bit value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: FSR1, BSR, MIOS_PARAMETER[23] as temporary registers
;; --------------------------------------------------------------------------
AOUT_Pin7bitSet
	;; convert 7bit to 16bit value - LSBs padded with zeros
	movff	MIOS_PARAMETER1, MIOS_PARAMETER2
	clrc
	rlf	MIOS_PARAMETER2, F
	clrf	MIOS_PARAMETER1

	;; change channel
	rgoto	AOUT_Pin16bitSet


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Pin12bitSet
;;  DESCRIPTION: This function sets an output channel to a given 12-bit value.
;;  Since the AOUT driver supports up to 16bit resolution, the lower bits
;;  will be padded with zero
;;  The output value won't be transfered to the module immediately, but will 
;;  be buffered in AOUT_[LH] instead. By calling AOUT_Update the requested
;;  changes will take place.
;;  IN:   channel number in WREG, 12bit value in MIOS_PARAMETER[12]
;;  OUT:  -
;;  USES: FSR1, BSR, MIOS_PARAMETER3 as temporary register
;; --------------------------------------------------------------------------
AOUT_Pin12bitSet
	;; temporary store channel number in MIOS_PARAMETER3
	movwf	MIOS_PARAMETER3

	;; convert 12bit to 16bit value - LSBs padded with zeros
	swapf	MIOS_PARAMETER2, W
	andlw	0xf0
	movwf	MIOS_PARAMETER2

	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER2, F

	swapf	MIOS_PARAMETER1, F
	movlw	0xf0
	andwf	MIOS_PARAMETER1, F

	;; change channel
	movf	MIOS_PARAMETER3, W
	rgoto	AOUT_Pin16bitSet


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_DigitalPinsSet
;;  DESCRIPTION: This function sets the digital pins of an AOUT module
;;  Currently only the MAX525 based MBHP_AOUT module supports two
;;  digital pins - they are addressed with WREG[0] and WREG[1]
;;  IN:   state of digital pins in WREG
;;  OUT:  -
;;  USES: FSR1, BSR, MIOS_PARAMETER3 as temporary register
;; --------------------------------------------------------------------------
;; (function located in selected aout_*.inc)


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Update
;;  DESCRIPTION: updates the output channels of the connected AOUT module
;;  Should be called, whenever changes have been requested via AOUT_Pin*Set
;;  OUT:  -
;;  USES: PROD[LH] and MIOS_PARAMETER[123] as temporary registers
;; --------------------------------------------------------------------------
;; (function located in selected aout_*.inc)


