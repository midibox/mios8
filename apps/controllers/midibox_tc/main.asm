; $Id$
;
; MIDIbox Tiny Control
; A MIOS Application
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

;; ---[ configuration table for MIDI processor and rotary encoders ]---
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================
#include "tc_init.inc"
#include "tc_tick.inc"	
#include "tc_timer.inc"
#include "tc_debug.inc"
#include "tc_display.inc"
#include "tc_mproc.inc"
#include "tc_din.inc"
#include "tc_enc.inc"
#include "tc_srio.inc"
#include "tc_ain.inc"
#include "tc_midi.inc"

#include "tc_labels.inc"
	END
