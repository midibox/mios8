; $Id$
;
; MIDIbox64
; Presets (EEPROM/Flash Content)
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


        ERRORLEVEL      -220    ; suppress "Address exceeds maximum range for this processor."
	;; this is (unfortunately) required for gpasm

; ==========================================================================
;  Data structure:
;                      BankStick Addr.   Internal Addr.   
;   +---------------+  0x7ff             0x7bff (FLASH)
;   | Pot Strings   |
;   +---------------+  0x400             0x7800 (FLASH)
;   | Reserved      |
;   +---------------+  0x300             -
;   | Configuration |
;   +---------------+  0x250             0x50 (EEPROM)
;   | Button Values |
;   +---------------+  0x240             0x40 (EEPROM)
;   | Pot Values    |
;   +---------------+  0x200             0x00 (EEPROM)
;   | Button Events |
;   +---------------+  0x100             0x7700 (FLASH)
;   | Pot Events    |
;   +---------------+  0x000             0x7600 (FLASH)
;

; ==========================================================================

	org	0x07600		; pot/button events stored at the end of internal flash
	
;; This macro generates a POT entry
;; Parameters: event_0, event_1, min_value, max_value
;; normaly we need 8 bits for every event byte, but since we are only able to 
;; embedd 7-bit events within a SysEx stream, the MSB is masked out. 
;; However, the MSB (bit 8) is 1 for every MIDI event (0x80-0xff), therefore 
;; this doesn't hurt. Simmlar approach for event_1: the MSB is 0 for every MIDI 
;; event (0x00-0x7f), so we can get rid of it.

POT_ENTRY MACRO event_0, event_1, min, max
	db	event_0, event_1, min, max
	ENDM

;; This macro generates a BUTTON entry
;; Parameters: event_0, event_1, event_2, mode
BUTTON_ENTRY MACRO event_0, event_1, event_2, mode
	db	event_0, event_1, event_2, mode
	ENDM

;; This macro generates a pot label entry
;; Parameters: mode, str
POT_LABEL_ENTRY MACRO mode, str
	db	str, mode
	ENDM

;; ---[ Controller Pot Bank ]------------------------------------------------
MB64_Presets
MB64_Presets_FLASH_Lower
MB64_Presets_OffsetPots
MB64_Presets_OffsetPotsRow0		; preset: reverb (Bn 5B)
	POT_ENTRY 0xb0, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb1, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb2, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb3, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb4, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb5, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb6, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb7, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb8, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xb9, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xba, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xbb, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xbc, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xbd, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xbe, 0x5b, 0x00, 0x7f
	POT_ENTRY 0xbf, 0x5b, 0x00, 0x7f

MB64_Presets_OffsetPotsRow1		; preset: chorus (Bn 5D)
	POT_ENTRY 0xb0, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb1, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb2, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb3, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb4, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb5, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb6, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb7, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb8, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xb9, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xba, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xbb, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xbc, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xbd, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xbe, 0x5d, 0x00, 0x7f
	POT_ENTRY 0xbf, 0x5d, 0x00, 0x7f

MB64_Presets_OffsetPotsRow2		; preset: CutOff (Bn 4A)
	POT_ENTRY 0xb0, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb1, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb2, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb3, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb4, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb5, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb6, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb7, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb8, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xb9, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xba, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xbb, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xbc, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xbd, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xbe, 0x4a, 0x00, 0x7f
	POT_ENTRY 0xbf, 0x4a, 0x00, 0x7f

MB64_Presets_OffsetPotsRow3		; preset: volume (Bn 07)
	POT_ENTRY 0xb0, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb1, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb2, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb3, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb4, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb5, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb6, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb7, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb8, 0x07, 0x00, 0x7f
	POT_ENTRY 0xb9, 0x07, 0x00, 0x7f
	POT_ENTRY 0xba, 0x07, 0x00, 0x7f
	POT_ENTRY 0xbb, 0x07, 0x00, 0x7f
	POT_ENTRY 0xbc, 0x07, 0x00, 0x7f
	POT_ENTRY 0xbd, 0x07, 0x00, 0x7f
	POT_ENTRY 0xbe, 0x07, 0x00, 0x7f
	POT_ENTRY 0xbf, 0x07, 0x00, 0x7f

MB64_Presets_OffsetButtons
	;; DIN/DOUT group 0
	BUTTON_ENTRY 0x90, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x91, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x92, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x93, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x94, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x95, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x96, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x97, 0x30, 0x7f, 0x00

	;; DIN/DOUT group 1
	BUTTON_ENTRY 0x98, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x99, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x9a, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x9b, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x9c, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x9d, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x9e, 0x30, 0x7f, 0x00
	BUTTON_ENTRY 0x9f, 0x30, 0x7f, 0x00

	;; DIN/DOUT group 2
#if DEFAULT_ENABLE_PAGE_HANDLING
	BUTTON_ENTRY 0xff, 0x05, 0x00, 0x00
	BUTTON_ENTRY 0xff, 0x05, 0x01, 0x00
	BUTTON_ENTRY 0xff, 0x05, 0x02, 0x00
	BUTTON_ENTRY 0xff, 0x05, 0x03, 0x00
#else
	BUTTON_ENTRY 0x90, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x90, 0x3d, 0x7f, 0x00
	BUTTON_ENTRY 0x90, 0x3e, 0x7f, 0x00
	BUTTON_ENTRY 0x90, 0x3f, 0x7f, 0x00
#endif
	BUTTON_ENTRY 0x90, 0x40, 0x7f, 0x00	; (Snapshot Button, not used)
	BUTTON_ENTRY 0x90, 0x41, 0x7f, 0x00	; (Menu Left Button, not used)
	BUTTON_ENTRY 0x90, 0x42, 0x7f, 0x00	; (Menu Right Button, not used)
	BUTTON_ENTRY 0x90, 0x43, 0x7f, 0x00	; (Menu Exec Button, not used)

	;; DIN/DOUT group 3
	BUTTON_ENTRY 0x90, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x91, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x92, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x93, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x94, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x95, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x96, 0x54, 0x7f, 0x00
	BUTTON_ENTRY 0x97, 0x54, 0x7f, 0x00

	;; DIN/DOUT group 4
	BUTTON_ENTRY 0x90, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x91, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x92, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x93, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x94, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x95, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x96, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x97, 0x3c, 0x7f, 0x00

	;; DIN/DOUT group 5
	BUTTON_ENTRY 0x98, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x99, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x9a, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x9b, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x9c, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x9d, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x9e, 0x3c, 0x7f, 0x00
	BUTTON_ENTRY 0x9f, 0x3c, 0x7f, 0x00

	;; DIN/DOUT group 6
	BUTTON_ENTRY 0x90, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x91, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x92, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x93, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x94, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x95, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x96, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x97, 0x48, 0x7f, 0x00

	;; DIN/DOUT group 7
	BUTTON_ENTRY 0x98, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x99, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x9a, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x9b, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x9c, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x9d, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x9e, 0x48, 0x7f, 0x00
	BUTTON_ENTRY 0x9f, 0x48, 0x7f, 0x00


	;; NOTE: in internal bank, all reads and writes from offset 0x200
	;; with be forwarded to the EEPROM address 0xf00000!
	;; by MB64_BANK_Write, MB64_BANK_Read

	org	0xf00000	; eeprom base address

MB64_Presets_OffsetPotValues
	;; first pot row
	db	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
	;; second pot row
	db	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
	;; third pot row
	db	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
	;; fourth pot row
	db	0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f

MB64_Presets_OffsetButtonValues
	;; button:
	;;            6543         3210
	db	b'00001111', b'00001111' ; 1st SR
	db	b'00001111', b'00001111' ; 2nd SR
	db	b'00001111', b'00001111' ; 3rd SR
	db	b'00001111', b'00001111' ; 4th SR
	db	b'00001111', b'00001111' ; 5th SR
	db	b'00001111', b'00001111' ; 6th SR
	db	b'00001111', b'00001111' ; 7th SR
	db	b'00001111', b'00001111' ; 8th SR


;; ---[ Misc Configuration ]-------------------------------------------------
MB64_Presets_OffsetGlobal
MB64_Presets_OffsetMagicNumbers
	db	BANKSTICK_MAGIC0, BANKSTICK_MAGIC1	; don't change these values, required to identify bankstick format
MB64_Presets_OffsetCFG
	;; note: since SysEx doesn't allow to transfer 8-bit values, they
	;; are splitted here: first byte is the low-nibble, second byte the high-nibble
MB64_Presets_CFG0
	db	0x00, 0x00
MB64_Presets_CFG1
	db	0x00, 0x00
MB64_Presets_DISPLAY_PAGE
	db	0x00, 0x00
MB64_Presets_MORPH_BANK
	db	0x00, 0x00
MB64_Presets_MORPH_POT_GLOBAL
	db	0x00, 0x00
MB64_Presets_MORPH_POT_G0
	db	0x00, 0x00
MB64_Presets_MORPH_POT_G1
	db	0x00, 0x00
MB64_Presets_MORPH_POT_G2
	db	0x00, 0x00
MB64_Presets_MORPH_POT_G3
	db	0x00, 0x00
MB64_Presets_RESERVED5
	db	0x00, 0x00
MB64_Presets_RESERVED6
	db	0x00, 0x00
MB64_Presets_NUMBER_POTS
	db	0x00, 0x04	; (0x40 == 64)
MB64_Presets_BANKNAME
	db	'I','n','t','e','r','n','a','l',' ','B','a','n','k',' ',' ',' '
MB64_Presets_TS_SENSITIVITY
	db	DEFAULT_TOUCH_SENSOR_SENSITIVITY, 0x00
MB64_Presets_RESERVED7
	db	0x00, 0x00
MB64_Presets_LED_MAP
	db	0x00, 0x00
	db	0x00, 0x00
	db	0x00, 0x00
	db	0x00, 0x00
	db	0x00, 0x00
	db	0x00, 0x00
	db	0x00, 0x00
	db	0x00, 0x00
MB64_Presets_GLOBAL_CHANNEL
	db	0x00, 0x00

	;; fill unused area
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	db	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


	;; these two entries cannot be overwritten via SysEx.
	;; it contains the device ID and the to-COM option
EEPROM_READONLY
	db	0x00, DEFAULT_DEVICE_ID


	org	0x07800		; pot strings
MB64_Presets_FLASH_Upper
MB64_Presets_OffsetLabels

#define POT_LABEL_MODE_HEX		0x40
#define POT_LABEL_MODE_PM_HEX		0x41
#define POT_LABEL_MODE_DEC		0x42
#define POT_LABEL_MODE_PM_DEC		0x43
#define POT_LABEL_MODE_HEX_B		0x60
#define POT_LABEL_MODE_PM_HEX_B		0x61
#define POT_LABEL_MODE_DEC_B		0x62
#define POT_LABEL_MODE_PM_DEC_B		0x63

		;;      <--------------->  mode
	POT_LABEL_ENTRY	"Reverb # 1     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 2     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 3     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 4     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 5     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 6     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 7     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 8     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb # 9     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #10     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #11     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #12     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #13     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #14     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #15     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Reverb #16     ", POT_LABEL_MODE_DEC_B
	
		;;      <--------------->  mode
	POT_LABEL_ENTRY	"Chorus # 1     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 2     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 3     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 4     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 5     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 6     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 7     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 8     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus # 9     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #10     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #11     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #12     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #13     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #14     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #15     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Chorus #16     ", POT_LABEL_MODE_DEC_B

		;;      <--------------->  mode
	POT_LABEL_ENTRY	"CutOff # 1     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 2     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 3     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 4     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 5     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 6     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 7     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 8     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff # 9     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #10     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #11     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #12     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #13     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #14     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #15     ", POT_LABEL_MODE_PM_DEC_B
	POT_LABEL_ENTRY	"CutOff #16     ", POT_LABEL_MODE_PM_DEC_B

		;;      <--------------->  mode
	POT_LABEL_ENTRY	"Volume # 1     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 2     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 3     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 4     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 5     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 6     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 7     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 8     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume # 9     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #10     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #11     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #12     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #13     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #14     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #15     ", POT_LABEL_MODE_DEC_B
	POT_LABEL_ENTRY	"Volume #16     ", POT_LABEL_MODE_DEC_B

