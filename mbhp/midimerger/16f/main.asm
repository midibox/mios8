; $Id$
;
; Source Code of MIDI Merger

; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#include "midibox.h"
#include "macros.inc"

; ==========================================================================
; Explanations of the include files
;
;  CODE PAGE #0
;    midibox.h:    A lot of flags, defines, register locations
;    macros.h:     my private macros
;    reset.inc:    reset handler
;    irq.inc:      interrupt handler, the MIDI IN part
;    init.inc:	   various initialization routines
;    midi.inc:	   Send and Learn MIDI
;    mainloop.inc: The MIDI Receive Handler
; ==========================================================================

	;; org in include file
#include "reset.inc"
#include "irq.inc"
#include "init.inc"
#include "midi.inc"

Start
	call	Init
#include "mainloop.inc"

	org	0x0700
#include "contab.inc"

;; ==========================================================================

	END
