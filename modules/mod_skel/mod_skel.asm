; $Id: mod_skel.asm 69 2008-02-01 00:20:18Z tk $
;
; Wrapper for mod_skel.inc
; To be used for relocatable programs
;
; ==========================================================================

	radix	dec

#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================
; Import/Export Labels
; ==========================================================================

	global	_mod_skel_function		; (for C, declaration in mod_skel.h)
	global	_mod_skel_var

	;; import lables
	extern	_mod_skel_extfunc		; function in your app, which you can call from mod_skel


; ==========================================================================
;  Declare variables 
; ==========================================================================

mod_skel_vars		udata
_mod_skel_var res	1		; exported to C, therefore an "_" has been added

mod_skel_int res	2		; not exported to C, or ASM, therefore only accesible to the driver


; ==========================================================================
; Start code section and include driver code
; ==========================================================================
mod_skel CODE
#include "mod_skel.inc"

; ==========================================================================

	;; C function wrappers
	;; These move the arguments of your C functions into the appropriate variables
	
	;; YOU ONLY NEED ONE OF THESE FOR EACH FUNCTION IN YOUR MODULE
	;; There are three examples here, but this skeleton only uses the first
	

;_mod_skel_function		; (no arguments, or direct passing of variables)


; _mod_skel_function	; example shows 
; 						; (one byte argument, function declared with  __wparam keyword)
;						; EG: void mod_skel_function(unsigned char myvar);
; 	movff	WREG,	_mod_skel_var
; 	rgoto	mod_skel_function


; _mod_skel_function	; example shows 
; 						; a char (1 byte, stared in W thanks to __wparam), 
; 						; an int (2 bytes)
;						; EG: void mod_skel_function(unsigned char myvar, signed int myint);
; 	movff	WREG,	_mod_skel_var		; get first byte of arguments from W
; 
; 	movff	FSR0L, FSR2L		; get other arguments from stack
; 	movff	PREINC2, _yourint+0 ; move the least significant byte of the int-param
; 	movff	PREINC2, _yourint+1 ; move the most significant byte of the int-param
; 	rgoto	mod_skel_function


; ==========================================================================
	END
