;; ----------------------
;; # note_to_hz.inc     #
;; ----------------------

;; THERE WAS A *Note On* MIDI Event
;; JUMPED IN HERE FROM => note_on.inc


;; --------------------------------------------------------------------------------------
;; BACKGROUND: MIDIBox AY 3 8912 MIDI to Hz Chart (MIDI Note On Value to Hz conversation)
;; fT = AY Chip Tone frequency [Frequency (Hz)]
;; fCLOCK = input clock frequency [2Mhz Crystal = 2.000.000 Hz]
;; TP = Decimal equivalent of the Tone Period bit 0 to 11 (12bit Coarse Tune + Fine Tune)
;; MIDI Note 0: TP = (2.000.000Hz) / (16x(8,1757989156 Hz))
;; ...
;; See => http://www.midibox.org/dokuwiki/doku.php?id=mb_ay_3_8912_-_midi_to_hz_chart
;; --------------------------------------------------------------------------------------

MIDI_EVNT_IS_9x_NOTE_ON_WRITE_TUNE_VALUES
      	;; FUNCTION:
      	;; WRITE THE TUNE VALUES
      	;; COASE TUNE TO MIOS_PARAMETER1
      	;; FINE  TUNE TO MIOS_PARAMETER2
      
      ;; FIRST:
      ;; WRITE THE *NOTE ON VALUE* TO W REG
      movf     MIDI_EVNT1, W
		; Determine the Table Address
		; for the corresponding *Note On MIDI Event*
		; // See => Macro Definitions (macros.h)
		; // MACRO: TABLE_ADDR_MUL_W  addr (tablename), multiplicator
		TABLE_ADDR_MUL_W TABLE_NOTE_TO_HZ, 2
			 
			; Table Read with post-increment
			tblrd*+ 
			; WRITE THE *COARSE* VALUE
			; transfer table entry (coarse) to MIOS_PARAMETER1
			movff    TABLAT, MIOS_PARAMETER1
			
			; Table Read with post-increment
			tblrd*+
			; WRITE THE *FINE* VALUE
			; transfer table entry (fine) to MIOS_PARAMETER2
			movff    TABLAT, MIOS_PARAMETER2

				;; NOW WE STORED THE [MIDI NOTE ON TO AY CHIP Hz VALUES] 
				;; IN MIOS_PARAMETER1 (coarse) and MIOS_PARAMETER2 (fine)
				;; GO ON WITH *NOTE ON* PROCESSING
				;; NEXT STEP => JUMP TO note_on_pipeline_a.inc
				goto MIDI_EVNT_NOTE_ON_CHECK_MIDI_CHANNEL_TONE_A_B_C
 



;; -------------[MIDIBox AY 3 8912 MIDI to Hz TABLE]-------------------------

;; -----------------------------------------------------------
;; MIDI NOTE ON (0 to 127) to 12bit AY 3 8912 REG CONVERSATION 
;; VALUES: COARSE REG (4bit), FINE REG (8bit)
;; FORMAT: db (COARSE REG VALUE), (FINE REG VALUE) 
;; -----------------------------------------------------------

TABLE_NOTE_TO_HZ
   db 0x0F,0xD2 ;; Note On Value = 0 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 1 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 2 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 3 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 4 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 5 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 6 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 7 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 8 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 9 - INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 10- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 11- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 12- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 13- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 14- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 15- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 16- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 17- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 18- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 19- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 20- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 21- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 22- INVALID 4 BIT COARSE TUNE (HEX FF-10) - Note On Value = 23 - B1 (substitution)
   db 0x0F,0xD2 ;; Note On Value = 23 - B1
   db 0x0E,0xEE ;; Note On Value = 24 - C1
   db 0x0E,0x18 ;; Note On Value = 25 - Db
   db 0x0D,0x4D ;; Note On Value = 26 - D1
   db 0x0C,0x8E ;; Note On Value = 27 - Dis1
   db 0x0B,0xDA ;; Note On Value = 28 - E1
   db 0x0B,0x2F ;; Note On Value = 29 - F1
   db 0x0A,0x8F ;; Note On Value = 30 - Fis1
   db 0x09,0xF7 ;; Note On Value = 31 - G1
   db 0x09,0x68 ;; etc.
   db 0x08,0xE1
   db 0x08,0x61
   db 0x07,0xE9
   db 0x07,0x77
   db 0x07,0x0C
   db 0x06,0xA7
   db 0x06,0x47
   db 0x05,0xED
   db 0x05,0x98
   db 0x05,0x47
   db 0x04,0xFC
   db 0x04,0xB4
   db 0x04,0x70
   db 0x04,0x31
   db 0x03,0xF4
   db 0x03,0xBC
   db 0x03,0x86
   db 0x03,0x53
   db 0x03,0x24
   db 0x02,0xF6
   db 0x02,0xCC
   db 0x02,0xA4
   db 0x02,0x7E
   db 0x02,0x5A
   db 0x02,0x38
   db 0x02,0x18
   db 0x01,0xFA
   db 0x01,0xDE
   db 0x01,0xC3
   db 0x01,0xAA
   db 0x01,0x92 ;; Note On Value = 63 - Eb
   db 0x01,0x7B ;; Note On Value = 64 - E
   db 0x01,0x66
   db 0x01,0x52
   db 0x01,0x3F
   db 0x01,0x2D
   db 0x01,0x1C
   db 0x01,0x0C
   db 0x00,0xFD
   db 0x00,0xEF
   db 0x00,0xE1
   db 0x00,0xD5
   db 0x00,0xC9
   db 0x00,0xBE
   db 0x00,0xB3
   db 0x00,0xA9
   db 0x00,0x9F
   db 0x00,0x96
   db 0x00,0x8E
   db 0x00,0x86
   db 0x00,0x7F
   db 0x00,0x78
   db 0x00,0x71
   db 0x00,0x6A
   db 0x00,0x64
   db 0x00,0x5F
   db 0x00,0x59
   db 0x00,0x54
   db 0x00,0x50
   db 0x00,0x4B
   db 0x00,0x47
   db 0x00,0x43
   db 0x00,0x3F
   db 0x00,0x3C
   db 0x00,0x38
   db 0x00,0x35
   db 0x00,0x32
   db 0x00,0x2F
   db 0x00,0x2D
   db 0x00,0x2A
   db 0x00,0x28
   db 0x00,0x26
   db 0x00,0x24
   db 0x00,0x22
   db 0x00,0x20
   db 0x00,0x1E
   db 0x00,0x1C
   db 0x00,0x1B
   db 0x00,0x19
   db 0x00,0x18
   db 0x00,0x16
   db 0x00,0x15
   db 0x00,0x14
   db 0x00,0x13
   db 0x00,0x12
   db 0x00,0x11
   db 0x00,0x10
   db 0x00,0x0F
   db 0x00,0x0E
   db 0x00,0x0D
   db 0x00,0x0D
   db 0x00,0x0C
   db 0x00,0x0B
   db 0x00,0x0B
   db 0x00,0x0A ;; NOTE ON value = 127 - G


;; -----------------------[end]------------------------------


