; $Id$
;
; The Mainloop of MIOS
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MIOS_Mainloop
	;; --- clear watchdog timer
	clrwdt

	;; --- call MIDI processor handler
 	call	MIOS_MPROC_Handler

	;; --- call DIN handler
	call	MIOS_DIN_Handler

	;; --- call AIN handler
 	call	MIOS_AIN_Handler

	;; --- call ENC handler
	call	MIOS_ENC_Handler

	;; --- call DISPLAY handler
	call	MIOS_DISPLAY_Handler

	;; --- clear watchdog timer again
	clrwdt

	;; --- call User part if not suspended
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_Tick

	;; --- next loop
	rgoto	MIOS_Mainloop
