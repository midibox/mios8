; $Id: cs_menu_print.inc 111 2008-02-22 00:41:21Z tk $
;
; MBFM Control Surface Menu Parameter Print Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

CS_MENU_PRINT_WRITE_Waveform
	;; backup index
	call	CS_MENU_Hlp_GetPxOxValue
	movwf	TMP1

	JUMPTABLE_2BYTES 8 ; entries
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set1
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set1
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set1
	rgoto CS_MENU_PRINT_WRITE_Waveform_Set0

CS_MENU_PRINT_WRITE_Waveform_Set0
	movlw	3
	goto	CS_MENU_PRINT_WRITE_Waveform_PrintIt
CS_MENU_PRINT_WRITE_Waveform_Set1
	movlw	4
CS_MENU_PRINT_WRITE_Waveform_PrintIt
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
	TABLE_ADDR CS_MENU_PRINT_WRITE_Waveform_Tab
	movf	TMP1, W
	goto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_WRITE_Waveform_Tab
	dw	0x0403, 0x0605 
	dw	0x0403, 0x2D2D 
	dw	0x0403, 0x0403
	dw	0x2D07, 0x2D07
	dw	0x0403, 0x2D2D
	dw	0x0303, 0x2D2D
	dw	0x0605, 0x075F
	dw	0x2D04, 0x052D	
	return



CS_MENU_PRINT_WRITE_LFO_Waveform
	;; get value
	call	CS_MENU_Hlp_GetPxLFOxValue
	movwf	TMP1
	;; waveform stored in TMP1[6..4]
	;; 5 waveforms provided
	;; add 1, index 0 notifies "LFO Off"
	swapf	TMP1, W
	andlw	0x07
	addlw	1
	btfss	TMP1, 0
	movlw 0x00
	andlw	0x07
	;; backup value 
	movwf	TMP1
	;; load charset (these start @ 5)
	JUMPTABLE_2BYTES 6
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_NoLoad
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set0
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set1
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set2
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Set1
                           
CS_MENU_PRINT_WRITE_LFO_Waveform_Set0
	movlw	5       
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Load
CS_MENU_PRINT_WRITE_LFO_Waveform_Set1
	movlw	6       
	rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Load
CS_MENU_PRINT_WRITE_LFO_Waveform_Set2
	movlw	4
	;; rgoto CS_MENU_PRINT_WRITE_LFO_Waveform_Load
CS_MENU_PRINT_WRITE_LFO_Waveform_Load
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
CS_MENU_PRINT_WRITE_LFO_Waveform_NoLoad
	;; print it
	TABLE_ADDR CS_MENU_PRINT_WRITE_LFO_Waveform_Tab
	movf	TMP1, W
	goto	CS_MENU_PRINT_StringFromTable
CS_MENU_PRINT_WRITE_LFO_Waveform_Tab
	dw	"--- "
	dw	0x0403, 0x2005
	dw	0x5E06, 0x2007
	dw	0x0403, 0x2005
	dw	0x5F06, 0x2007
	dw	0x0706, 0x2006
