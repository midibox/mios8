; $Id$
;
; Source Code of MIDI Merger
;
; set this flag to 0 if the no or the old MIOS bootloader is used (program located at 0x0000)
; set this flag to 1 if the new MIOS bootloader is used (program located at 0x0400)
#ifndef MIOS_BSL_FLAG
#define MIOS_BSL_FLAG 0
#endif
; 
; 
; NOTE:	if you are using MIOS Studio to upload the merger_v1_4a_18f_with_bootloader.hex
; file, the program will print:
; 
;    Starting upload of merger_v1_4a_18f_with_bootloader.hex
;    Hex file contains code in MIOS range, forcing reboot!
; 
; and thereafter maybe stop, since the merger doesn't use MIOS, and
; therefore a reset cannot be triggered via SysEx.
; In this case just power-off/on the core manually, the bootloader will send an
; upload request, and MIOS Studio will start to upload the code
; 
;   Received Upload Request
;   Sending block 00000400-000004FF
;   Received Checksum: 79 - OK
;   Sending block 00000500-000005FF
;   Received Checksum: 15 - OK
;   Sending block 00000600-000006FF
;   Received Checksum: 16 - OK
;   Upload process complete
;   
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#include "midibox.h"
#include "macros.h"

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

;; ==========================================================================

	END
