; $Id$
;
; MIDIO128
; MB_ADDR pointer return functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the DIN entry,
;;  selected by MIDIO_CURRENT_DIN
;; --------------------------------------------------------------------------
MIDIO_ADDR_DINEntry
	SET_BSR	MIDIO_BASE
	;; low address of DIN event
	movf	MIDIO_CURRENT_DIN, W, BANKED
	mullw	6
	movf	PRODL, W
	addlw	(MIDIO_Presets_OffsetOut) & 0xff
	movwf	MB_ADDRL
	;; high address of DIN event
	movlw	((MIDIO_Presets_OffsetOut-MIDIO_Presets_FLASH_Lower) >> 8) & 0x03
	addwfc	PRODH, W
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns a MB_ADDR[LH] pointer to the DIN mode, 
;;  selected by MIDIO_CURRENT_DIN
;; --------------------------------------------------------------------------
MIDIO_ADDR_DINModeEntry
	SET_BSR	MIDIO_BASE
	;; low address of DIN value
	rrf	MIDIO_CURRENT_DIN, W, BANKED
	andlw	0x3f
	addlw	MIDIO_Presets_DIN_MODES & 0xff
	movwf	MB_ADDRL
	;; high address of DIN mode (EEPROM)
	movlw	0x04
	movwf	MB_ADDRH
	return

;; --------------------------------------------------------------------------
;;  This function returns MB_ADDR[LH] to the global settings page
;; --------------------------------------------------------------------------
MIDIO_ADDR_Global
	movlw	MIDIO_Presets_OffsetGlobal & 0xff
	movwf	MB_ADDRL
	movlw	0x04
	movwf	MB_ADDRH
	return

