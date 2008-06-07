;$Id: app_defines.h 323 2008-05-03 19:06:15Z tk $
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright (C) <year>  <name> (<email>)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f
	
;; for mod_skel module
_mod_skel_var	EQU	0x067
mod_skel_int	EQU	0x068; ..0x69