; $Id$
;
; Video / TV Out Example
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; =========================================================================
;; used registers
;; =========================================================================
SCANLINE_CTR		EQU	0x010
BMAPLINE_CTR		EQU	0x012
CURSOR_POS		EQU	0x013
COPY_FONT_CTR		EQU	0x014
PIR1_TMP		EQU	0x015
MIDI_IN			EQU	0x016
MIDI_RECEIVE_STATE	EQU	0x017
MIDI_CMD		EQU	0x018

MIDIBUFFER_BEGIN	EQU	0x080	; never change this address, otherwise
MIDIBUFFER_END		EQU	0x0bf	; the optimized FIFO handling won't work

	;; bitmap: 128x64 pixel
VIDEO_BITMAP_LINE_LEN	EQU	128/8	; (0x10)

VIDEO_BITMAP_BEGIN	EQU	0x100
VIDEO_BITMAP_END	EQU	0x4ff

VIDEO_BITMAP_LINE0	EQU	0x100
VIDEO_BITMAP_LINE16	EQU	0x200
VIDEO_BITMAP_LINE32	EQU	0x300
VIDEO_BITMAP_LINE48	EQU	0x400
VIDEO_BITMAP_LINE63	EQU	0x4f0
; ==========================================================================


;; some driver specific macros/definitions
#define VIDEO_PORT1	LATB	; with 470R resistor, MUST be connected to pin 7 of a 8-bit port
#define VIDEO_TRIS1	TRISB
#define VIDEO_PIN1	7
#define VIDEO_PORT2	LATD	; with 1k resistor, can be connected to any output pin
#define VIDEO_TRIS2	TRISD
#define VIDEO_PIN2	5


#define VIDEO_MIDI_DEVICE_ID 0x00; can be changed if required


VIDEO_DELAY MACRO delay
	if delay > 124
		ERROR "delay value too high"
	endif

	rcall	VIDEO_DELAY_Hlp_End - 2*((delay) - 4)
	ENDM

VIDEO_OUT_SYNC MACRO		; sync level: 0V
	bcf	VIDEO_PORT1, VIDEO_PIN1
	bcf	VIDEO_PORT2, VIDEO_PIN2
	ENDM

VIDEO_OUT_WHITE MACRO		; white level: 1V
	bsf	VIDEO_PORT1, VIDEO_PIN1
	bsf	VIDEO_PORT2, VIDEO_PIN2
	ENDM

VIDEO_OUT_BLACK MACRO		; black level: 0.33V
	bcf	VIDEO_PORT1, VIDEO_PIN1
	bsf	VIDEO_PORT2, VIDEO_PIN2
	ENDM

VIDEO_OUT_GRAY MACRO		; gray level: 0.67V
	bsf	VIDEO_PORT1, VIDEO_PIN1
	bcf	VIDEO_PORT2, VIDEO_PIN2
	ENDM

VIDEO_HSYNC MACRO
	LOCAL	VIDEO_HSYNC_Poll
VIDEO_HSYNC_Poll
	BRA_IFCLR PIR1, CCP1IF, ACCESS, VIDEO_HSYNC_Poll
	bcf	PIR1, CCP1IF
	btfsc	TMR3L, 0	; compensate jitter (required since the goto instruction needs 2 cycles)
	goto	$+2
	ENDM


;; --------------------------------------------------------------------------
;;  FUNCTION: VIDEO_Handler
;;  DESCRIPTION: This function starts the Video handler, it will run endless
;;               and will never be finished
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
VIDEO_Handler
	;; disable interrupts --- now we can do what we want! :-)
	IRQ_DISABLE

	;; enable output drivers of the video pins
	bcf	VIDEO_TRIS1, 0
	bcf	VIDEO_TRIS2, 0
	
	;; init bitmap with MBHP TV logo
	TABLE_ADDR BITMAP_MBHP_TV
	lfsr	FSR0, VIDEO_BITMAP_BEGIN
VIDEO_Handler_InitLoop1
	tblrd*+
	movff	TABLAT, POSTINC0
	movlw	0x05
	cpfseq	FSR0H, ACCESS
	rgoto VIDEO_Handler_InitLoop1

	;; init cursor
	clrf	CURSOR_POS

        ;; Initialize TMR3/CCP1 as hsync clock generator (event on every 640 cycles)
	movlw   0x0b		; compare mode
	movwf   CCP1CON
	movlw	(640-1) & 0xff	; triggering on this value
	movwf	CCPR1L
	movlw	(640-1) >> 8
	movwf	CCPR1H

	clrf	TMR3L
	clrf	TMR3H
 	bsf	T3CON, T3CCP2	; mode 10: CCP1 event resets TMR3
	bsf	T3CON, T3CCP1
	bsf	T3CON, TMR3ON

	;; initialize MIDI buffer and MIDI state
	lfsr	FSR1, MIDIBUFFER_BEGIN	; (head)
	lfsr	FSR2, MIDIBUFFER_BEGIN	; (tail)
	clrf	MIDI_RECEIVE_STATE

	;; clear RCIF and error flags
	bcf	PIR1, RCIF
	bcf	RCSTA, OERR
	bcf	RCSTA, FERR
	movf	RCREG, W

	;; be aware that this is isyncronous code - every scan line should never 
	;; consume more than 640 cycles (64 uS), the whole loop has to consume 400.000 cycles (40 mS)
	;; for PAL (625 scanlines)
	;; 
	;; The VIDEO_HSYNC macro should be called before an horizontal sync - it
	;; polls the CCP1IF flag which will be set periodically every 64uS
VIDEO_Handler_Loop
#if 0
	;; for debugging to measure cycles (set the breakpoint to the clrwdt instruction)
	VIDEO_HSYNC
#endif
	clrwdt

	rcall	VIDEO_Sync_Wave1	; scanline 621
	rcall	VIDEO_Sync_Wave1	; scanline 622
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave2	; scanline 623
	rcall	VIDEO_Sync_Wave2	; scanline 624
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave2	; scanline 625
	rcall	VIDEO_Sync_Wave3	; scanline 1
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave3	; scanline 2
	rcall	VIDEO_Sync_Wave4	; scanline 3
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave2	; scanline 4
	rcall	VIDEO_Sync_Wave2	; scanline 5
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave1	; scanline 6
	rcall	VIDEO_Sync_Wave1	; scanline 7
	call	VIDEO_MIDI_Check

	movlw	61			; scanline 8-68
	movwf	SCANLINE_CTR
VIDEO_Handler_Loop1
	rcall	VIDEO_Blackline
	call	VIDEO_MIDI_Check
	call	VIDEO_Idle_Task
	decfsz	SCANLINE_CTR, F
	rgoto	VIDEO_Handler_Loop1

	lfsr	FSR0, VIDEO_BITMAP_BEGIN
	movlw	192			; scanline 69-260
	movwf	SCANLINE_CTR
VIDEO_Handler_Loop2
	rcall	VIDEO_Pixelline
	decfsz	SCANLINE_CTR, F
	rgoto	VIDEO_Handler_Loop2

	movlw	48			; scanline 261-308
	movwf	SCANLINE_CTR
VIDEO_Handler_Loop3
	rcall	VIDEO_Blackline
	call	VIDEO_MIDI_Check
	call	VIDEO_Idle_Task
	decfsz	SCANLINE_CTR, F
	rgoto	VIDEO_Handler_Loop3

	rcall	VIDEO_Sync_Wave1	; scanline 309
	rcall	VIDEO_Sync_Wave1	; scanline 310
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave2	; scanline 311
	rcall	VIDEO_Sync_Wave2	; scanline 312
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave5	; scanline 313
	rcall	VIDEO_Sync_Wave3	; scanline 314
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave3	; scanline 315
	rcall	VIDEO_Sync_Wave2	; scanline 316
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave2	; scanline 317
	rcall	VIDEO_Sync_Wave1	; scanline 318
	call	VIDEO_MIDI_Check
	rcall	VIDEO_Sync_Wave1	; scanline 319

	movlw	61			; scanline 320-380
	movwf	SCANLINE_CTR
VIDEO_Handler_Loop4
	rcall	VIDEO_Blackline
	call	VIDEO_MIDI_Check
	call	VIDEO_Idle_Task
	decfsz	SCANLINE_CTR, F
	rgoto	VIDEO_Handler_Loop4

	lfsr	FSR0, VIDEO_BITMAP_BEGIN
	movlw	192			; scanline 381-572
	movwf	SCANLINE_CTR
VIDEO_Handler_Loop5
	rcall	VIDEO_Pixelline
	decfsz	SCANLINE_CTR, F
	rgoto	VIDEO_Handler_Loop5

	movlw	48			; scanline 573-620
	movwf	SCANLINE_CTR
VIDEO_Handler_Loop6
	rcall	VIDEO_Blackline
	call	VIDEO_MIDI_Check
	call	VIDEO_Idle_Task
	decfsz	SCANLINE_CTR, F
	rgoto	VIDEO_Handler_Loop6

	rgoto	VIDEO_Handler_Loop


;; --------------------------------------------------------------------------
VIDEO_Test_Bar
	VIDEO_HSYNC
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS black
	VIDEO_OUT_BLACK
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 4uS white
	VIDEO_OUT_WHITE
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS gray
	VIDEO_OUT_GRAY
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100) - 6
	return

;; --------------------------------------------------------------------------
VIDEO_Blackline
	VIDEO_HSYNC
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 60uS black
	VIDEO_OUT_BLACK
	return

;; --------------------------------------------------------------------------
	;; ...interlaced code... :-)
VIDEO_PIXELLINE_8 MACRO	
	movf	POSTINC0, W
	movwf	VIDEO_PORT1
	movf	PIR1, W			; move content of PIR1 to PIR1_TMP
	movwf	PIR1_TMP
	nop
	rlf	VIDEO_PORT1, F
	btfsc	PIR1_TMP, RCIF		; check if new byte has been received
	movf	RCREG, W
	nop
	rlf	VIDEO_PORT1, F
	btfsc	PIR1_TMP, RCIF		; if so, push it into buffer
	movwf	POSTINC1
	bcf	FSR1L, 6		; wrap at 0xc0
	rlf	VIDEO_PORT1, F
	btfsc	PIR1_TMP, RCIF		; clear RCIF flag if set
	bcf	PIR1, RCIF
	nop
	rlf	VIDEO_PORT1, F
	nop
	nop
	nop
	rlf	VIDEO_PORT1, F
	nop
	nop
	nop
	rlf	VIDEO_PORT1, F
	nop
	nop
	nop
	rlf	VIDEO_PORT1, F
	nop
	nop
	ENDM

VIDEO_Pixelline
	VIDEO_HSYNC
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 7.55uS black
	VIDEO_OUT_BLACK
	VIDEO_DELAY (7500/100) - 3 - 9

	incf	BMAPLINE_CTR, F		; 1
	movf	BMAPLINE_CTR, W		; 1
	xorlw	0x03			; 1
	bz	VIDEO_Pixelline_NoDec	; 1/2
VIDEO_Pixelline_Dec
	movf	POSTDEC0, W		; 1
	movlw	-15			; 1
	addwf	FSR0L, F		; 1
	rgoto	VIDEO_Pixelline_Cont	; 2
VIDEO_Pixelline_NoDec
	clrf	BMAPLINE_CTR		; 1
	nop				; 1
	nop				; 1
	nop				; 1
VIDEO_Pixelline_Cont
	
	;; --> 128 pixels in exactly 16*32 cycles = 51.2uS
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8
	VIDEO_PIXELLINE_8

	;; --> 1.25uS black
	VIDEO_OUT_BLACK
	return


;; --------------------------------------------------------------------------
	;;   ________
	;; ||        |
VIDEO_Sync_Wave1:
	VIDEO_HSYNC
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 60uS white
	VIDEO_OUT_WHITE
	return

;; --------------------------------------------------------------------------
	;;   ___  ___
	;; ||   ||   |
VIDEO_Sync_Wave2:
	VIDEO_HSYNC
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS white
	VIDEO_OUT_WHITE
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS white
	VIDEO_OUT_WHITE
	return

;; --------------------------------------------------------------------------
	;;   
	;; ____||___||
VIDEO_Sync_Wave3:
	VIDEO_HSYNC
	;; --> 28uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 4uS white
	VIDEO_OUT_WHITE
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 4uS white
	VIDEO_OUT_WHITE
	return

;; --------------------------------------------------------------------------
	;;         __
	;; _____|||  |
VIDEO_Sync_Wave4:
	VIDEO_HSYNC
	;; --> 28uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 4uS white
	VIDEO_OUT_WHITE
	VIDEO_DELAY (4000/100) - 2
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS white
	VIDEO_OUT_WHITE
	return

;; --------------------------------------------------------------------------
	;;   ___ 
	;; ||   |___||
VIDEO_Sync_Wave5:
	VIDEO_HSYNC
	;; --> 4uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (4000/100) - 2
	;; --> 28uS white
	VIDEO_OUT_WHITE
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 28uS sync
	VIDEO_OUT_SYNC
	VIDEO_DELAY (10000/100) - 2
	VIDEO_DELAY (10000/100)
	VIDEO_DELAY (8000/100)
	;; --> 4uS white
	VIDEO_OUT_WHITE
	return



;; --------------------------------------------------------------------------
;; this simple function provides a delay of up to 120+5 cycles
;; used by VIDEO_DELAY macro
;; --------------------------------------------------------------------------
VIDEO_DELAY_Hlp
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 110-120

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 101-110

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 91-100

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 81-90

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 71-80

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 61-70

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 51-60

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 41-50

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 31-40

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 21-30

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop			; 11-20

	nop			; 10
	nop			; 9
	nop			; 8
	nop			; 7
	nop			; 6
	nop			; 5 cycles (call=2, nop=1, return=2)
VIDEO_DELAY_Hlp_End
	return

;; --------------------------------------------------------------------------
;; copy font to bitmap and increment cursor
;; cycles: 2 (goto) + 117 (routine) + 2 (return) = 121 cycles (12.1 uS)
;; --------------------------------------------------------------------------
VIDEO_CopyFont_8x8
	;; calculate pointer to font depending on character
	;; address = table_begin + font * 8
	mullw	8
	movlw	FONT_8x8 & 0xff
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	FONT_8x8 >> 8
	addwfc	PRODH, W
	movwf	TBLPTRH
	;; calculate pointer to bitmap depending on cursor pos
	;; address = bitmap_begin + (8*(pos>>4)) + (pos&0x0f)
	lfsr	FSR0, VIDEO_BITMAP_BEGIN
	swapf	CURSOR_POS, W
	andlw	0x0f
	mullw	0x80
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	movf	CURSOR_POS, W
	andlw	0x0f
	addwf	FSR0L, F
	;; copy character to SRAM
	clrf	COPY_FONT_CTR
VIDEO_CopyFont_8x8_Loop
	tblrd*+
	movff	TABLAT, INDF0
	movlw	0x10
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	COPY_FONT_CTR, F
	BRA_IFCLR COPY_FONT_CTR, 3, ACCESS, VIDEO_CopyFont_8x8_Loop
	;; increment cursor
	incf	CURSOR_POS, F
	;; wrap at 128
	bcf	CURSOR_POS, 7
	return


;; --------------------------------------------------------------------------
;; the same code can be found in the VIDEO_PIXELLINE_8 macro: we have to
;; ensure that a MIDI event will be pushed into the buffer in between
;; ca. 640 uS in order to prevent an overrun
;; this code should be called outside VIDEO_Scanline as often as possible
;; --------------------------------------------------------------------------
VIDEO_MIDI_Check
	btfss	PIR1, RCIF; check if new byte has been received, if not, exit
	return	
	btfsc	RCSTA, OERR; reset controller on overrun
	reset	
	btfsc	RCSTA, FERR; reset controller on overrun
	reset	

	;; push new byte into buffer
	movf	RCREG, W
	movwf	POSTINC1
	bcf	FSR1L, 6 ; wrap at 0xc0

	;; clear RCIF flag
	bcf	PIR1, RCIF
	return

;; --------------------------------------------------------------------------
;; this handler is called by the idle task when a new byte has been pushed
;; onto the FIFO buffer. It gets the byte and parses for MIDI events
;; --------------------------------------------------------------------------

	;; used by VIDEO_MIDI_Handler_CmdChr
#define MIDI_RECEIVE_STATE__POS_RECEIVED 7

VIDEO_MIDI_Handler
	movf	POSTINC2, W
	bcf	FSR2L, 6 ; wrap at 0xc0
	movwf	MIDI_IN

	;; ignore realtime events (which can be sent in between other events)
	movlw	0xf8
	cpfslt	MIDI_IN, ACCESS
	return

	;; clear state on status bytes (like 0xf0 or 0xf7...)
	btfsc	MIDI_IN, 7
	clrf	MIDI_RECEIVE_STATE

	;; parse for SysEx sequence
	;; branch depending on MIDI_RECEIVE_STATE
	movf	MIDI_RECEIVE_STATE, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE	; 8 states
	rgoto	VIDEO_MIDI_Handler_Syx0	; checks for 0xf0
	rgoto	VIDEO_MIDI_Handler_Syx1	; checks for 0x00
	rgoto	VIDEO_MIDI_Handler_Syx2	; checks for 0x00
	rgoto	VIDEO_MIDI_Handler_Syx3	; checks for 0x7e
	rgoto	VIDEO_MIDI_Handler_Syx4	; checks for 0x3f
	rgoto	VIDEO_MIDI_Handler_Syx5	; checks for <device_id>
	rgoto	VIDEO_MIDI_Handler_GetCmd; gets the command
	rgoto	VIDEO_MIDI_Handler_GetData; handles the remaining data

VIDEO_MIDI_Handler_Syx0	; checks for 0xf0
	movlw	0xf0
	rgoto	VIDEO_MIDI_Handler_Syx_Check
VIDEO_MIDI_Handler_Syx1	; checks for 0x00
	movlw	0x00
	rgoto	VIDEO_MIDI_Handler_Syx_Check
VIDEO_MIDI_Handler_Syx2	; checks for 0x00
	movlw	0x00
	rgoto	VIDEO_MIDI_Handler_Syx_Check
VIDEO_MIDI_Handler_Syx3	; checks for 0x7e
	movlw	0x7e
	rgoto	VIDEO_MIDI_Handler_Syx_Check
VIDEO_MIDI_Handler_Syx4	; checks for 0x3f
	movlw	0x3f
	rgoto	VIDEO_MIDI_Handler_Syx_Check
VIDEO_MIDI_Handler_Syx5	; checks for <device id>
	movlw	VIDEO_MIDI_DEVICE_ID
	;; 	rgoto	MTC_Handler_Syx_Check
VIDEO_MIDI_Handler_Syx_Check
	cpfseq	MIDI_IN, ACCESS
	rgoto VIDEO_MIDI_HANDLER_Syx_Inv

	;; if byte matches, increment the state number and exit handler
VIDEO_MIDI_Handler_Syx_Matched
	incf	MIDI_RECEIVE_STATE, F
	return
	
	;; if byte doesn't match, clear state number and exit handler
VIDEO_MIDI_HANDLER_Syx_Inv
	clrf	MIDI_RECEIVE_STATE
	return

	
VIDEO_MIDI_Handler_GetCmd ; gets the command
	movff	MIDI_IN, MIDI_CMD
	incf	MIDI_RECEIVE_STATE, F
	movf	MIDI_IN, W
	movwf	MIDI_CMD
	;; branch depending on command
	JUMPTABLE_2BYTES 3 ; entries
	rgoto	VIDEO_MIDI_Handler_InitClr
	rgoto	VIDEO_MIDI_Handler_InitPosChr
	rgoto	VIDEO_MIDI_Handler_InitChr
	return

VIDEO_MIDI_Handler_GetData	; handles the remaining data
	;; branch depending on command
	movf	MIDI_CMD, W
	JUMPTABLE_2BYTES 3 ; entries
	rgoto	VIDEO_MIDI_Handler_CmdClr
	rgoto	VIDEO_MIDI_Handler_CmdPosChr
	rgoto	VIDEO_MIDI_Handler_CmdChr


VIDEO_MIDI_Handler_InitClr
VIDEO_MIDI_Handler_CmdClr
	;; clear screen has been requested
	goto	VIDEO_ClearScreen

VIDEO_MIDI_Handler_InitPosChr
	return
VIDEO_MIDI_Handler_CmdPosChr
	;; first byte is the cursor pos
	BRA_IFSET MIDI_RECEIVE_STATE, MIDI_RECEIVE_STATE__POS_RECEIVED, ACCESS, VIDEO_MIDI_Handler_CmdChr
	movff	MIDI_IN, CURSOR_POS
	bsf	MIDI_RECEIVE_STATE, MIDI_RECEIVE_STATE__POS_RECEIVED
	return
	
	;; all other bytes will be print on screen
VIDEO_MIDI_Handler_InitChr
	return
VIDEO_MIDI_Handler_CmdChr
	movf	MIDI_IN, W
	goto	VIDEO_CopyFont_8x8


;; --------------------------------------------------------------------------
;; clears the screen
;; --------------------------------------------------------------------------
VIDEO_ClearScreen
	lfsr	FSR0, VIDEO_BITMAP_BEGIN
VIDEO_ClearScreen_Loop
	clrf	POSTINC0
	movlw	0x05
	cpfseq	FSR0H, ACCESS
	rgoto VIDEO_ClearScreen_Loop

	;; init cursor
	clrf	CURSOR_POS

	return


;; --------------------------------------------------------------------------
;; called during a "blackline", you've about 580 cycles to do anything!
;; --------------------------------------------------------------------------
VIDEO_Idle_Task
	;; check if a MIDI byte has been received
	movf	FSR1L, W
	cpfseq	FSR2L, ACCESS
	rgoto VIDEO_MIDI_Handler
	;; else do nothing
	return
