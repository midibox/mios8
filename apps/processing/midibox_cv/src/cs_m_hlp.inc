; $Id$
;
; Help Functions for the MBCV Menu pages
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Help routine to transfer the menu parameter to CV_SELECTED_OUT
;; --------------------------------------------------------------------------
CS_M_HLP_OutChanged
	SET_BSR	CV_BASE
	movff	CS_MENU_PARAMETER_L, CV_SELECTED_OUT
	return
