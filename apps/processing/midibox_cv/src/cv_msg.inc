; $Id$
;
; MBCV Message Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Use this function to start the message
;; --------------------------------------------------------------------------
CV_MSG_Start
	;; begin with CS_MENU message
	goto	CS_MENU_StartMessage


;; --------------------------------------------------------------------------
;;  Use this function to hold the message
;;  IN: message ID in WREG
;; --------------------------------------------------------------------------
CV_MSG_Hold
	;; hold a CS_MENU message
	goto	CS_MENU_HoldMessage


;; --------------------------------------------------------------------------
;;  Use this function to stop a message
;;  IN: -
;; --------------------------------------------------------------------------
CV_MSG_Stop
	;; stop a CS_MENU message
	goto	CS_MENU_StopMessage


;; --------------------------------------------------------------------------
;;  This function is called by the CS when a message update has been requested
;;  (see cs_menu_tables.inc)
;; --------------------------------------------------------------------------
CV_MSG_Handler
	return
