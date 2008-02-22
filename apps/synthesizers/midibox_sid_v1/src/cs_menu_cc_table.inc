; $Id$
;
; Control Surface CC String tables
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; ==========================================================================
;  this function prints "<C>|<string>" (16 characters)
;  CC number in WREG
; ==========================================================================
CS_MENU_CC_TABLE_Print
	;; temporary store CC in FSR0L
	movwf	FSR0L
	;; print CC number
	call	MIOS_LCD_PrintBCD3
	;; print |
	movlw	'|'
	call	MIOS_LCD_PrintChar

	;; due to memory limitations we have to get rid of this nice feature
	;; print 12 spaces instead and exit
	movlw	0x12
	goto	CS_MENU_Hlp_PrintSpaces
	
#if 0
	;; restore CC number
	movf	FSR0L, W
	;; offset = WREG * 12
	mullw	12
	;; add base address and store in TBLPTR
	movf	PRODL, W
	addlw	CS_MENU_CC_TABLE & 0xff
	movwf	TBLPTRL
	movf	PRODH, W
	skpnc
	incf	PRODH, W
	addlw	CS_MENU_CC_TABLE >> 8
	movwf	TBLPTRH
	;; print 12 characters and exit
	movlw	12
	goto	MIOS_LCD_PrintPreconfString
		

CS_MENU_CC_TABLE
	;;      <------------>
	dw	"-           "              
	dw	"Mod. Wheel  "
	dw	"Vel. Init   "
	dw	"Mod. InitVal"
	dw	"Aft. InitVal"
	dw	"V123 Porta. "
	dw	"-           "
	dw	"Volume      "
	dw	"V123 Note   "
	dw	"V1 Note     "
	dw	"V2 Note     "
	dw	"V3 Note     "
	dw	"WT Patch #  "               
	dw	"Vel. Depth  "
	dw	"Mod. Depth  "
	dw	"Aft. Depth  "
	dw	"V123 Transp."
	dw	"V1 Transpose"
	dw	"V2 Transpose"
	dw	"V3 Transpose"
	dw	"V123 Finet. "
	dw	"V1 Finetune "
	dw	"V2 Finetune "
	dw	"V3 Finetune "
	dw	"V123 Pitch  "
	dw	"V1 Pitch    "
	dw	"V2 Pitch    "
	dw	"V3 Pitch    "
	dw	"V123 Porta. "
	dw	"V1 Portam.  "
	dw	"V2 Portam.  "
	dw	"V3 Portam.  "
	dw	"V123 Wavef. "
	dw	"V1 Waveform "
	dw	"V2 Waveform "
	dw	"V3 Waveform "
	dw	"V123 Pulsew."
	dw	"V1 Pulsewid."
	dw	"V2 Pulsewid."
	dw	"V3 Pulsewid."
	dw	"V123 Arp.   "
	dw	"V1 Arpegg.  "
	dw	"V2 Arpegg.  "
	dw	"V3 Arpegg.  "
	dw	"Filt. Chn.  "
	dw	"Filt. Mode  "      
	dw	"Filt. CutOff"
	dw	"Filt. Reson."
	dw	"V123 Attack "
	dw	"V1 Attack   "
	dw	"V2 Attack   "
	dw	"V3 Attack   "
	dw	"V123 Decay  "
	dw	"V1 Decay    "
	dw	"V2 Decay    "
	dw	"V3 Decay    "
	dw	"V123 Sustain"
	dw	"V1 Sustain  "
	dw	"V2 Sustain  "
	dw	"V3 Sustain  "
	dw	"V123 Release"
	dw	"V1 Release  "
	dw	"V2 Release  "
	dw	"V3 Release  "
	dw	"LFO1 Rate   "        
	dw	"LFO2 Rate   "        
	dw	"LFO3 Rate   "        
	dw	"LFO4 Rate   "        
	dw	"LFO5 Rate   "        
	dw	"LFO6 Rate   "        
	dw	"-           "                
	dw	"-           "                
	dw	"LFO1 Depth  "       
	dw	"LFO2 Depth  "       
	dw	"LFO3 Depth  "       
	dw	"LFO4 Depth  "       
	dw	"LFO5 Depth  "       
	dw	"LFO6 Depth  "       
	dw	"ENV1 Depth  "       
	dw	"ENV2 Depth  "       
	dw	"LFO1 Mode   "        
	dw	"LFO2 Mode   "        
	dw	"LFO3 Mode   "        
	dw	"LFO4 Mode   "        
	dw	"LFO5 Mode   "        
	dw	"LFO6 Mode   "        
	dw	"-           "                
	dw	"-           "                
	dw	"Env1 Attack "
	dw	"Env1 Decay  "
	dw	"Env1 Sustain"
	dw	"Env1 Release"
	dw	"Env2 Attack "
	dw	"Env2 Decay  "
	dw	"Env2 Sustain"
	dw	"Env2 Release"
	dw	"V123 a.LFO P"
	dw	"V1 a.LFO Pit"
	dw	"V2 a.LFO Pit"
	dw	"V3 a.LFO Pit"
	dw	"V123 a.LFO p"
	dw	"V1 a.LFO PW "
	dw	"V2 a.LFO PW "
	dw	"V3 a.LFO PW "
	dw	"V123 a.LFO E"
	dw	"V1 a.LFO ENV"
	dw	"V2 a.LFO ENV"
	dw	"V3 a.LFO ENV"
	dw	"Asg. LFO Fil"
	dw	"Asg. ENV Fil"
	dw	"-           "                
	dw	"-           "                
	dw	"V123 Delay  "
	dw	"V1 Delay    "
	dw	"V2 Delay    "
	dw	"V3 Delay    "
	dw	"Sus-Key beh."
	dw	"Vel. Asgn.  "
	dw	"Mod. Asgn.  "
	dw	"Aft. Asgn.  "
	dw	"WT Rate     "
	dw	"WT asgn.CCP1"
	dw	"WT asgn.CCP2"
	dw	"WT asgn.CCP3"
	dw	"-           "                
	dw	"MIDI Sync   "                
	dw	"Mono On     "
	dw	"Poly On     "
#endif
