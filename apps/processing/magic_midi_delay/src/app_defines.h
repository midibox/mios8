;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
; Copyright (C) 2003  Thorsten Klose (Thorsten.Klose@gmx.de)
; 
; ==========================================================================
; 
; This file is part of the Magic MIDI Delay
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

MMD_DISPLAY_REQUEST	EQU	0x010
MMD_MIDI_CLOCK_REQUEST	EQU	0x011

MMD_BPM			EQU	0x020
MMD_MIDI_IN_CHN		EQU 	0x021
MMD_MIDI_IN_CFG_CHN	EQU 	0x022
MMD_QUEUE0_CHN		EQU 	0x023
MMD_QUEUE1_CHN		EQU 	0x024
MMD_QUEUE2_CHN		EQU 	0x025
MMD_QUEUE3_CHN		EQU 	0x026
MMD_QUEUE0_DLY		EQU	0x027
MMD_QUEUE1_DLY		EQU	0x028
MMD_QUEUE2_DLY		EQU	0x029
MMD_QUEUE3_DLY		EQU	0x02a
MMD_QUEUE0_TRANSP	EQU 	0x02b
MMD_QUEUE1_TRANSP	EQU 	0x02c
MMD_QUEUE2_TRANSP	EQU 	0x02d
MMD_QUEUE3_TRANSP	EQU 	0x02e

MMD_QUEUE_SNIPPET_HEAD	EQU	0x02f	; temporary used
MMD_QUEUE_DELAY         EQU	0x030	; temporary used
MMD_QUEUE_CHANNEL       EQU	0x031	; temporary used
MMD_QUEUE_NOTE		EQU	0x032	; temporary used

MMD_QUEUE0_HEAD		EQU	0x033
MMD_QUEUE1_HEAD		EQU	0x034
MMD_QUEUE2_HEAD		EQU	0x035
MMD_QUEUE3_HEAD		EQU	0x036

MMD_QUEUE0_TAIL		EQU	0x037
MMD_QUEUE1_TAIL		EQU	0x038
MMD_QUEUE2_TAIL		EQU	0x039
MMD_QUEUE3_TAIL		EQU	0x03a

MMD_QUEUE0_OVERALL_DLY	EQU	0x03b
MMD_QUEUE1_OVERALL_DLY	EQU	0x03c
MMD_QUEUE2_OVERALL_DLY	EQU	0x03d
MMD_QUEUE3_OVERALL_DLY	EQU	0x03e

	;; queue buffers
	;; size of each buffer: 0x80 == 128 bytes
	;; note: if you change the size: it *must* be a power of 2 (..., 0x08, 0x10, 0x20, 0x40, 0x80, ...)
MMD_QUEUE0_BEGIN	EQU	0x080
MMD_QUEUE0_END		EQU	0x0ff
MMD_QUEUE1_BEGIN	EQU	0x100
MMD_QUEUE1_END		EQU	0x17f
MMD_QUEUE2_BEGIN	EQU	0x180
MMD_QUEUE2_END		EQU	0x1ff
MMD_QUEUE3_BEGIN	EQU	0x200
MMD_QUEUE3_END		EQU	0x27f
