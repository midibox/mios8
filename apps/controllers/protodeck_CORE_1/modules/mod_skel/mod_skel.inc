; $Id: mod_skel.inc 69 2008-02-01 00:20:18Z tk $
;
; MIOS Module Skeleton Driver
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


; mod_skel  .... Your code starts here!!

; mod_skel defines
#ifndef _mod_skel_def	; if it's not already defined in the makefile (see readme.txt)
#define _mod_skel_def 1	; default to 1
#endif					; (see also mod_skel.asm for GPASM fix!)


;; --------------------------------------------------------------------------
;;  FUNCTION: mod_skel 
;;  DESCRIPTION: mod_skel. does something stupid. Put your code in here instead.
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: - mod_skel_var
;; --------------------------------------------------------------------------

_mod_skel_function ; (for C)
mod_skel_function
#if _mod_skel_def==1	; note this is using the define from the fix in the asm file!
	incf _mod_skel_var, F, BANKED
#endif
	call _mod_skel_extfunc
	return



; mod_skel  .... Your code ends here!!

; ==========================================================================
