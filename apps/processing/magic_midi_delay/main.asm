; $Id$
;
; MIOS based Application:
; "Magic MIDI Delay"
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ---[ MIOS header file ]---
#include <mios.h>

;; ---[ useful macros ]---
#include <macros.h>

;; ---[ vectors to MIOS functions (never change!) ]---
#include <mios_vectors.inc>

;; ---[ user hooks (never change!) ]---
#include <user_vectors.inc>

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[ variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; --------------------------------------------------------------------------
;;  the configuration channel shouldn't be changed via MIDI (ok, you could
;;  add a configuration via SysEx if you want)
#define MIDI_IN_CFG_CHN_VALUE	16

;;  see this define to 1 if you want to use pots
#define USE_POTS 0
;; --------------------------------------------------------------------------
;;  these settings can be changed via MIDI, the appr. CC, pot and encoder numbers are listed
;;  here:	                Value                CC#   Enc           Pot

BPM_VALUE               EQU     120                ; -     jog-wheel     1
QUEUE0_DLY_VALUE	EQU	0x00		   ; 01     1            2
QUEUE1_DLY_VALUE	EQU	0x06		   ; 02     2            3
QUEUE2_DLY_VALUE	EQU	0x0c		   ; 03     3            4
QUEUE3_DLY_VALUE	EQU	0x12		   ; 04     4            5

QUEUE0_TRANSP_VALUE	EQU	0x40		   ; 05     5            6
QUEUE1_TRANSP_VALUE	EQU	0x40		   ; 06     6            7
QUEUE2_TRANSP_VALUE	EQU	0x40		   ; 07     7            8
QUEUE3_TRANSP_VALUE	EQU	0x40		   ; 08     8            9

QUEUE0_CHN_VALUE	EQU	1		   ; 09     -            10
QUEUE1_CHN_VALUE	EQU	2		   ; 10     -            11
QUEUE2_CHN_VALUE	EQU	3		   ; 11     -            12
QUEUE3_CHN_VALUE	EQU	4		   ; 12     -            13

MIDI_IN_CHN_VALUE	EQU	1		   ; 13     -            14

;; ---[ configuration table for MIDI processor and rotary encoders ]---
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include "app_lcd.inc"

;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================
#include "mmd_init.inc"
#include "mmd_tick.inc"	
#include "mmd_timer.inc"
#include "mmd_debug.inc"
#include "mmd_display.inc"
#include "mmd_midi.inc"
#include "mmd_mproc.inc"
#include "mmd_din.inc"
#include "mmd_enc.inc"
#include "mmd_srio.inc"
#include "mmd_ain.inc"

#include "mmd_core.inc"

#include "bpm_table.inc"

	END
