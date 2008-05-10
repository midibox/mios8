;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
; Copyright (C) 2003  Thorsten Klose (Thorsten.Klose@gmx.de)
; 
; ==========================================================================
; 
; This file is part of MIDIbox TC
;
; This application is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This application is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this application; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

; ==========================================================================
	;; how much values should be displayed per page
	;; for 2x20 displays: use 5
	;; for 2x16 displays: use 4
#define VALUES_PER_PAGE		5

; ==========================================================================

	;; purpose: see tc_labels.inc and tc_display.inc
#define PARA_VALUE_HEX		0
#define PARA_VALUE_PM_HEX	1
#define PARA_VALUE_DEC		2
#define PARA_VALUE_PM_DEC	3

; ==========================================================================

TC_MODE				EQU	0x010	; contains the current mode
#define TC_MODE_MENU	0x00			; mode 1: Menu
#define TC_MODE_PARA	0x01			; mode 2: show parameters

; ==========================================================================

TC_STAT				EQU	0x011	; status flags of MIDIbox TC
#define TC_STAT_DISPLAY_UPDATE_REQ	0	; flag 0: request a display update
#define TC_STAT_DISPLAY_LONGLABEL_REQ	1	; flag 1: request a long label message
#define TC_STAT_CURSOR_FLASH		2	; flag 2: handled by tc_timer.inc

; ==========================================================================

TC_BANK				EQU	0x012	; the current bank (0-49)
TC_PARA				EQU	0x013	; the parameter number (0-4)
TC_TIMER_CTR			EQU	0x014	; used by tc_timer.inc
TC_SELECT_CTR			EQU	0x015	; used by tc_display.inc, counted by tc_timer.inc

; ==========================================================================

TC_VALUES_BEGIN			EQU	0x100	; all values are stored in 0x100-0x1ff
TC_VALUES_END			EQU	0x1ff
