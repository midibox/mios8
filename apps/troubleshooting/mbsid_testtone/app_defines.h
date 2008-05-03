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

	;; free memory for user applications:	
	;; 0x010-0x37f

CS_PIN_TOGGLE		EQU	0x010	; bit 0 will be set/cleared every timer event

;; ==========================================================================
;;  SID register set
;; ==========================================================================
SID_BASE		EQU	0x100		; address space of SID chip (used: 0x00-0x18)
SID_V1_FRQ_L		EQU	SID_BASE+0x00
SID_V1_FRQ_H		EQU	SID_BASE+0x01
SID_V1_PW_L		EQU	SID_BASE+0x02
SID_V1_PW_H		EQU	SID_BASE+0x03
SID_V1_CTRL		EQU	SID_BASE+0x04
SID_V1_ENV_AD		EQU	SID_BASE+0x05
SID_V1_ENV_SR		EQU	SID_BASE+0x06

SID_V2_FRQ_L		EQU	SID_BASE+0x07
SID_V2_FRQ_H		EQU	SID_BASE+0x08
SID_V2_PW_L		EQU	SID_BASE+0x09
SID_V2_PW_H		EQU	SID_BASE+0x0a
SID_V2_CTRL		EQU	SID_BASE+0x0b
SID_V2_ENV_AD		EQU	SID_BASE+0x0c
SID_V2_ENV_SR		EQU	SID_BASE+0x0d

SID_V3_FRQ_L		EQU	SID_BASE+0x0e
SID_V3_FRQ_H		EQU	SID_BASE+0x0f
SID_V3_PW_L		EQU	SID_BASE+0x10
SID_V3_PW_H		EQU	SID_BASE+0x11
SID_V3_CTRL		EQU	SID_BASE+0x12
SID_V3_ENV_AD		EQU	SID_BASE+0x13
SID_V3_ENV_SR		EQU	SID_BASE+0x14

SID_FC_L		EQU	SID_BASE+0x15
SID_FC_H		EQU	SID_BASE+0x16
SID_RES_FCHN		EQU	SID_BASE+0x17
SID_MODE_VOL		EQU	SID_BASE+0x18

SID_BASE_END		EQU	0x11f

;; ==========================================================================

SID_SHADOW_BASE		EQU	0x120		; shadow addresses, necessary to determine changes (used: 0x00-0x18)

	;; (SID_SHADOW_BASE + 0x00) to (SID_SHADOW_BASE + 0x18) allocated by shadow registers

SID_SHADOW_END		EQU	0x13f

