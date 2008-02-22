; $Id$
;
; MIDIbox SID
; SysEx reference table
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  IN: index of SysEx entry (0x00-0xff) in WREG
;;  OUT: appr. CC number in WREG - if [7] set, no CC available
;; --------------------------------------------------------------------------
SID_SYSEX_TABLE_Get
	btfsc	WREG, 7; Wavtable area: return 0xff
	retlw 0xff			

	addlw	SID_SYSEX_TABLE & 0xff			; calc: SID_SYSEX_TABLE + index
	movwf	TBLPTRL					; result in TBLPTR[LH]
	clrf	TBLPTRH
	movlw	(SID_SYSEX_TABLE >> 8) & 0xff
	addwfc	TBLPTRH, F
	tblrd*						; read from table
	movf	TABLAT, W				; return table entry
	return


	;; SyxTab maps the SysEx offset to the CC number
SID_SYSEX_TABLE
	;; 0x00-0x0f
	db	0x80, 0x81	; Patch Name
	db	0x82, 0x83
	db	0x84, 0x85
	db	0x86, 0x87
	db	0x88, 0x89
	db	0x8a, 0x8b
	db	0x8c, 0x8d
	db	0x8e, 0x8f

	;; 0x10-0x1f
	db	0x07, 0x7f	; Volume, Poly
	db	0x74, 0x7d	; SusKey & Filter Keytracking, MIDI Sync
	db	0x7c, 0x6e	; OSC sync, ENV curves
	db	0x6f, 0x0c	; Sound Engine Options, (wavetable patch, will be overwritten after init)
	db	0x2c, 0x2d	; Filter Channels, Filter Mode
	db	0x2e, 0x2f	; CutOff, Resonance
	db	0x6c, 0x6d	; Assigned LFOs, Assigned Envelopes
 	db	0x56, 0x57	; ENV1 Curve, ENV2 Curve

	;; 0x20-0x2f
	db	0x11, 0x15	; V1 Transpose, V1 Finetune
	db	0x19, 0x1d	; V1 PitchRange, V1 Portamento
	db	0x21, 0x25	; V1 Waveform, V1 PulseWidth
	db	0x29, 0x71	; V1 Arp Rate, V1 Delay
	db	0x31, 0x35	; V1 Attack, V1 Decay
	db	0x39, 0x3d	; V1 Sustain, V1 Release
	db	0x61, 0x65	; V1 Assigned LFOs Pitch, V1 Assigned LFOs PW
	db	0x69, 0x90	; V1 Assigned Envelopes, Split V1 Lower

	;; 0x30-0x3f
	db	0x12, 0x16	; V2 Transpose, V2 Finetune
	db	0x1a, 0x1e	; V2 PitchRange, V2 Portamento
	db	0x22, 0x26	; V2 Waveform, V2 PulseWidth
	db	0x2a, 0x72	; V2 Arp Rate, V2 Delay
	db	0x32, 0x36	; V2 Attack, V2 Decay
	db	0x3a, 0x3e	; V2 Sustain, V2 Release
	db	0x62, 0x66	; V2 Assigned LFOs Pitch, V2 Assigned LFOs PW
	db	0x6a, 0x92	; V2 Assigned Envelopes, Split V2 Lower

	;; 0x40-0x4f
	db	0x13, 0x17	; V3 Transpose, V3 Finetune
	db	0x1b, 0x1f	; V3 PitchRange, V3 Portamento
	db	0x23, 0x27	; V3 Waveform, V3 PulseWidth
	db	0x2b, 0x73	; V3 Arp Rate, V3 Delay
	db	0x33, 0x37	; V3 Attack, V3 Decay
	db	0x3b, 0x3f	; V3 Sustain, V3 Release
	db	0x63, 0x67	; V3 Assigned LFOs Pitch, V3 Assigned LFOs PW
	db	0x6b, 0x94	; V3 Assigned Envelopes, Split V3 Lower

	;; 0x50-0x5f
	db	0x75, 0x02	; Velocity Assign, Velocity Init Value
	db	0x0d, 0x76	; Velocity Depth, Modulation Assign
	db	0x03, 0x0e	; Modulation Init Value, Modulation Depth
	db	0x77, 0x04	; Aftertouch Assign, Aftertouch Init Value
	db	0x0f, 0x78	; Aftertouch Depth, Wavetable Rate
	db	0x79, 0x7a	; Wavetable P1 Assign, Wavetable P2 Assign
	db	0x7b, 0x91	; Wavetable P3 Assign, Split V1 Upper
	db	0x93, 0x95	; Split V2 Upper, Split V3 Upper

	;; 0x60-0x7f
	db	0x50, 0x40	; LFO1 Mode, LFO1 Rate
	db	0x48, 0x51	; LFO1 Depth, LFO2 Mode
	db	0x41, 0x49	; LFO2 Rate, LFO2 Depth
	db	0x52, 0x42	; LFO3 Mode, LFO3 Rate
	db	0x4a, 0x53	; LFO3 Depth, LFO4 Mode
	db	0x43, 0x4b	; LFO4 Rate, LFO4 Depth
	db	0x54, 0x44	; LFO5 Mode, LFO5 Rate
	db	0x4c, 0x55	; LFO5 Depth, LFO6 Mode
	db	0x45, 0x4d	; LFO6 Rate, LFO6 Depth

	db	0x4e, 0x58	; ENV1 Depth, ENV1 Attack
	db	0x59, 0x5a	; ENV1 Decay, ENV1 Sustain
	db	0x5b, 0x4f	; ENV1 Release, ENV2 Depth
	db	0x5c, 0x5d	; ENV2 Attack, ENV2 Decay
	db	0x5e, 0x5f	; ENV2 Sustain, ENV2 Release

	db	0xff, 0xff
	db	0xff, 0xff
