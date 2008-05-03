; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 2005 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#define CS_MENU_MATRIX_MODE_VERTICAL	0	; alternative vertical mode for modulation matrix LEDs

; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

LED_DELAY_CTR		EQU	0x010
LED_SELECT_CTR		EQU	0x011
DISPLAY_UPDATE_REQ	EQU	0x012

CS_MENU_MATRIX_MODE	EQU	0x05a	; includes some flags, used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_CTR	EQU	0x05b	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_FLASH_CTR EQU	0x05c	; counter used by CS_MENU_MATRIX_Handler
CS_MENU_MATRIX_SELECTED_ROW EQU	0x05d	; selected matrix row (every button has one flag)
CS_MENU_MATRIX_SELECTED_COL EQU	0x05e	; selected matrix column (every button has one flag)
CS_MENU_MATRIX_BEGIN	EQU	0x05f	; content of modulation matrix (8 entries)
CS_MENU_MATRIX_END	EQU	0x067
