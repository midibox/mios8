; $Id$
;
; Control Surface Menu Tables Structures
; (outsourced as they are really unreadable ;-)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


; ==========================================================================
;  CS_MENU_TABLES contains the number of tables and a pointer to all
;  available menu tables and the appr. table handler function
; ==========================================================================
CS_MENU_T_ENTRY_REF_LEN		EQU	2
CS_MENU_T_ENTRY_HANDLER_OFFSET	EQU	CS_MENU_T_ENTRY_REF_LEN
CS_MENU_T_ENTRY_HANDLER_LEN	EQU	2
CS_MENU_T_ENTRY_EXEC_OFFSET	EQU	CS_MENU_T_ENTRY_HANDLER_OFFSET+CS_MENU_T_ENTRY_HANDLER_LEN
CS_MENU_T_ENTRY_EXEC_LEN	EQU	2
CS_MENU_T_ENTRY_LEN		EQU	CS_MENU_T_ENTRY_EXEC_OFFSET+CS_MENU_T_ENTRY_EXEC_LEN

CS_MENU_TABLES_NUMBER		EQU	2	; number of entries
	
; ==========================================================================
;  This defines the structure of a table entry
; ==========================================================================
CS_MENU_ENTRY_REG_OFFSET	EQU	0
CS_MENU_ENTRY_REG_LEN		EQU	1
CS_MENU_ENTRY_SLABEL_OFFSET	EQU	CS_MENU_ENTRY_REG_OFFSET+CS_MENU_ENTRY_REG_LEN
CS_MENU_ENTRY_SLABEL_LEN	EQU	3
CS_MENU_ENTRY_SPARE_OFFSET	EQU	CS_MENU_ENTRY_SLABEL_OFFSET+CS_MENU_ENTRY_SLABEL_LEN
CS_MENU_ENTRY_SPARE_LEN		EQU	1
CS_MENU_ENTRY_MAX_OFFSET	EQU	CS_MENU_ENTRY_SPARE_OFFSET+CS_MENU_ENTRY_SPARE_LEN
CS_MENU_ENTRY_MAX_LEN		EQU	1
CS_MENU_ENTRY_PRINT_IX_OFFSET	EQU	CS_MENU_ENTRY_MAX_OFFSET+CS_MENU_ENTRY_MAX_LEN
CS_MENU_ENTRY_EXEC_IX_OFFSET	EQU	CS_MENU_ENTRY_PRINT_IX_OFFSET
CS_MENU_ENTRY_PRINT_IX_LEN	EQU	1
CS_MENU_ENTRY_R2PP2R_IX_OFFSET	EQU	CS_MENU_ENTRY_PRINT_IX_OFFSET+CS_MENU_ENTRY_PRINT_IX_LEN
CS_MENU_ENTRY_R2PP2R_IX_LEN	EQU	1
CS_MENU_ENTRY_LEN		EQU	CS_MENU_ENTRY_R2PP2R_IX_OFFSET+CS_MENU_ENTRY_R2PP2R_IX_LEN

CS_MENU_ENTRY MACRO reg, short, max, print_ix, sel_ix, p2r_r2p_ix
	db	reg, short, 0, max, ((sel_ix & 0x03) << 6) | (print_ix & 0x3f), ((sel_ix & 0x0c) << 4) | (p2r_r2p_ix & 0x3f)
	ENDM

	;; the first two bytes contain
	;;    o the length of the table
	;;    o and a dummy byte (necessary due to assembler imperfection)

	;; we are using the following structure for every entry:
	;;   o 1 byte for the parameter register offset
	;;   o 3 characters for the short label
	;;   o 1 spare byte
	;;   o the 8-bit max value
	;;   o an index to the print function
	;;   o an index to the select function
	;;   o an index to the p2r-r2p function

