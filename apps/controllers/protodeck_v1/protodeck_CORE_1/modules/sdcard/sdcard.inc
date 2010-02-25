; $Id: sdcard.inc 378 2008-07-06 20:38:32Z tk $
;
; SD card driver
; Inspired from Microchip Application Note AN1003
; 
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; Pin Configuration
; Can be overloaded from external
#ifndef SDCARD_LAT_CS
#define SDCARD_LAT_CS	LATC	; The chip select pin
#endif
#ifndef SDCARD_TRIS_CS
#define SDCARD_TRIS_CS	TRISC	; is connected to Port C.0
#endif
#ifndef SDCARD_PIN_CS
#define SDCARD_PIN_CS	0	; (CANNOT be shared with other outputs!)
#endif
;
#ifndef SDCARD_LAT_SCLK
#define SDCARD_LAT_SCLK	LATC	; The shift clock input pin SCLK
#endif
#ifndef SDCARD_TRIS_SCLK
#define SDCARD_TRIS_SCLK TRISC	; is connected to Port C.1
#endif
#ifndef SDCARD_PIN_SCLK
#define SDCARD_PIN_SCLK	1	; (can be shared with other outputs)
#endif
;
#ifndef SDCARD_LAT_DI
#define SDCARD_LAT_DI	LATC	; The DI pin (data input of SD card)
#endif
#ifndef SDCARD_TRIS_DI
#define SDCARD_TRIS_DI	TRISC	; is connected to Port C.2
#endif
#ifndef SDCARD_PIN_DI
#define SDCARD_PIN_DI	2	; (can be shared with other inputs)
#endif
;
#ifndef SDCARD_PORT_DO
#define SDCARD_PORT_DO	PORTC	; The DO pin (data output of SD card)
#endif
#ifndef SDCARD_TRIS_DO
#define SDCARD_TRIS_DO	TRISC	; is connected to Port C.3
#endif
#ifndef SDCARD_PIN_DO
#define SDCARD_PIN_DO	3	; (can be shared with other outputs)
#endif
;

#define SDCARD_MODE_FLAG_FAST		0 ; flag 0 controls fast/slow bitrate

#define SDCMD_GO_IDLE_STATE	0x40
#define SDCMD_GO_IDLE_STATE_CRC	0x95

#define SDCMD_SEND_OP_COND	0x41
#define SDCMD_SEND_OP_COND_CRC	0xf9

#define SDCMD_READ_SINGLE_BLOCK	0x51
#define SDCMD_READ_SINGLE_BLOCK_CRC 0xff

#define SDCMD_WRITE_SINGLE_BLOCK 0x58
#define SDCMD_WRITE_SINGLE_BLOCK_CRC 0xff


;; --------------------------------------------------------------------------
;;  SDCARD initialisation
;;  IN: -
;;  USES: TMP1, PROD[LH] and TABLAT
;; --------------------------------------------------------------------------
_SDCARD_Init ; (for C)
SDCARD_Init
	;; clear mode flags
	SET_BSR	_sdcard_mode
	clrf	_sdcard_mode, BANKED

	;; ensure that clock starts at high level
        bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK

	;; deactivate chip select
	bsf	SDCARD_LAT_CS, SDCARD_PIN_CS

	;; enable pin drivers
	bcf	SDCARD_TRIS_CS, SDCARD_PIN_CS	; output 
	bcf	SDCARD_TRIS_SCLK, SDCARD_PIN_SCLK	; output
	bcf	SDCARD_TRIS_DI, SDCARD_PIN_DI	; output 
	bsf	SDCARD_TRIS_DI, SDCARD_PIN_DO	; input

	;; send 80 clock cycles to start up
	movlw	0x0a
	movwf	TMP1
SDCARD_Init_StartupLoop1
	rcall	SDCARD_ReadByte
	decfsz	TMP1, F
	rgoto	SDCARD_Init_StartupLoop1

	;; activate chip select
	bcf	SDCARD_LAT_CS, SDCARD_PIN_CS

	;; wait for 1 mS
	movlw	0x01
	call	MIOS_Delay

	;; send CMD0 to reset the media
	SET_BSR	_sdcard_address
	clrf	_sdcard_address+0, BANKED
	clrf	_sdcard_address+1, BANKED
	clrf	_sdcard_address+2, BANKED
	clrf	_sdcard_address+3, BANKED
	movlw	SDCMD_GO_IDLE_STATE_CRC
	movwf	_sdcard_crc, BANKED
	movlw	SDCMD_GO_IDLE_STATE
	movwf	_sdcard_cmdcode, BANKED
	rcall	SDCARD_SendSDCCmd
	
	movlw	0xff
	movwf	TMP1
SDCARD_Init_StartupLoop2
	clrwdt
	;; send CMD0 to reset the media
	SET_BSR	_sdcard_address
	clrf	_sdcard_address+0, BANKED
	clrf	_sdcard_address+1, BANKED
	clrf	_sdcard_address+2, BANKED
	clrf	_sdcard_address+3, BANKED
	movlw	SDCMD_SEND_OP_COND_CRC
	movwf	_sdcard_crc, BANKED
	movlw	SDCMD_SEND_OP_COND
	movwf	_sdcard_cmdcode, BANKED
	rcall	SDCARD_SendSDCCmd

	movf	_sdcard_response, W, BANKED
	bz	SDCARD_Init_StartupLoop_Exit

	decfsz	TMP1, F
	rgoto	SDCARD_Init_StartupLoop2
SDCARD_Init_StartupLoop_Exit

	;; enable fast mode
	SET_BSR	_sdcard_mode
	bsf	_sdcard_mode, SDCARD_MODE_FLAG_FAST, BANKED

	;; deactivate chip select
	bsf	SDCARD_LAT_CS, SDCARD_PIN_CS

	return


;; --------------------------------------------------------------------------
;;  Sends command to SD card
;;  IN: command in _sdcard_cmdcode
;;      address in _sdcard_address[4]
;;      precalculated CRC in _sdcard_crc
;;  OUT: != 0 if error during command transmission
;;    -Bit 0              - In idle state if 1
;;    -Bit 1              - Erase Reset if 1
;;    -Bit 2              - Illgal Command if 1
;;    -Bit 3              - Com CRC Error if 1
;;    -Bit 4              - Erase Sequence Error if 1
;;    -Bit 5              - Address Error if 1
;;    -Bit 6              - Parameter Error if 1
;;    -Bit 7              - Not used, always '0'
;;      512 byte data in _sdcard_buffer_p0 and _sdcard_buffer_p1
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SDCARD_SendSDCCmd
	;; activate chip select
	bcf	SDCARD_LAT_CS, SDCARD_PIN_CS

	;; transfer to SD card
	SET_BSR	_sdcard_cmdcode
	movf	_sdcard_cmdcode, W, BANKED	; send command
	rcall	SDCARD_WriteByte
	movf	_sdcard_address+3, W, BANKED	; most significant byte
	rcall	SDCARD_WriteByte
	movf	_sdcard_address+2, W, BANKED
	rcall	SDCARD_WriteByte
	movf	_sdcard_address+1, W, BANKED
	rcall	SDCARD_WriteByte
	movf	_sdcard_address+0, W, BANKED	; least significant byte
	rcall	SDCARD_WriteByte
	movf	_sdcard_crc, W, BANKED		; send CRC
	rcall	SDCARD_WriteByte

	;; see if we are going to get a response
	movlw	8		; timeout value
	movwf	PRODL
SDCARD_SendSDCCmd_R1_Loop
	rcall	SDCARD_ReadByte
	movwf	_sdcard_response, BANKED

	;; loop until value != 0xff
	xorlw	0xff
	bnz	SDCARD_SendSDCCmd_R1_Cont
	decfsz	PRODL, F
	rgoto	SDCARD_SendSDCCmd_R1_Loop
SDCARD_SendSDCCmd_R1_Cont

	;; required clocking (see spec)
	rcall	SDCARD_ReadByte

	andlw	0x00		; fix ZERO flag (no error)
	return


;; --------------------------------------------------------------------------
;;  Reads 512 bytes from selected sector
;;  IN: 32bit sector number in _sdcard_address[4]
;;  OUT: != 0 if error during read
;;    -Bit 0              - In idle state if 1
;;    -Bit 1              - Erase Reset if 1
;;    -Bit 2              - Illgal Command if 1
;;    -Bit 3              - Com CRC Error if 1
;;    -Bit 4              - Erase Sequence Error if 1
;;    -Bit 5              - Address Error if 1
;;    -Bit 6              - Parameter Error if 1
;;    -Bit 7              - Not used, always '0'
;;      512 byte data in _sdcard_buffer_p0 and _sdcard_buffer_p1
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SDCARD_SectorRead
	;; left-shift address by 9
	rcall	SDCARD_Hlp_LeftShiftAddr

	;; send the cmd
	SET_BSR	_sdcard_crc
	movlw	SDCMD_READ_SINGLE_BLOCK_CRC
	movwf	_sdcard_crc, BANKED
	movlw	SDCMD_READ_SINGLE_BLOCK
	movwf	_sdcard_cmdcode, BANKED
	rcall	SDCARD_SendSDCCmd

	movf	_sdcard_response, W, BANKED
	bz	SDCARD_SectorRead_Ok
SDCARD_SectorRead_Err
	rgoto	SDCARD_SectorRead_End

SDCARD_SectorRead_Ok
	;; now must wait for the start token of data block
	rcall	SDCARD_ReadByte
	xorlw	0xff
	bz	SDCARD_SectorRead_Ok

	clrf	PRODL
	lfsr	FSR1, _sdcard_buffer_p0
SDCARD_SectorRead_Loop1
	rcall	SDCARD_ReadByte
	movwf	POSTINC1
	decfsz	PRODL, F
	rgoto	SDCARD_SectorRead_Loop1
	
	clrf	PRODL
	lfsr	FSR1, _sdcard_buffer_p1
SDCARD_SectorRead_Loop2
	rcall	SDCARD_ReadByte
	movwf	POSTINC1
	decfsz	PRODL, F
	rgoto	SDCARD_SectorRead_Loop2

	;; now ensure CRC
	rcall	SDCARD_ReadByte
	rcall	SDCARD_ReadByte

	;; required clocking (see spec)
	rcall	SDCARD_ReadByte

	;; de-activate chip select
	bsf	SDCARD_LAT_CS, SDCARD_PIN_CS

	movlw	0x00		; no error
SDCARD_SectorRead_End
	andlw	0xff		; ensure that ZERO flag correctly set
	return


;; --------------------------------------------------------------------------
;;  Writes 512 bytes into selected sector
;;  IN: 32bit sector number in _sdcard_address[4]
;;      512 byte data in _sdcard_buffer_p0 and _sdcard_buffer_p1
;;  OUT: != 0 if error during write
;;    -Bit 0              - In idle state if 1
;;    -Bit 1              - Erase Reset if 1
;;    -Bit 2              - Illgal Command if 1
;;    -Bit 3              - Com CRC Error if 1
;;    -Bit 4              - Erase Sequence Error if 1
;;    -Bit 5              - Address Error if 1
;;    -Bit 6              - Parameter Error if 1
;;    -Bit 7              - Not used, always '0'
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SDCARD_SectorWrite
	;; left-shift address by 9
	rcall	SDCARD_Hlp_LeftShiftAddr

	;; send the cmd
	movlw	SDCMD_WRITE_SINGLE_BLOCK_CRC
	movwf	_sdcard_crc, BANKED
	movlw	SDCMD_WRITE_SINGLE_BLOCK
	movwf	_sdcard_cmdcode, BANKED
	rcall	SDCARD_SendSDCCmd

	movf	_sdcard_response, W, BANKED
	bz	SDCARD_SectorWrite_Ok
SDCARD_SectorWrite_Err
	rgoto	SDCARD_SectorWrite_End

SDCARD_SectorWrite_Ok
	;; send data start token
	movlw	0xfe
	rcall	SDCARD_WriteByte

	;; send 512 bytes of data
	clrf	PRODL
	lfsr	FSR1, _sdcard_buffer_p0
SDCARD_SectorWrite_Loop1
	movf	POSTINC1, W
	rcall	SDCARD_WriteByte
	decfsz	PRODL, F
	rgoto	SDCARD_SectorWrite_Loop1
	
	clrf	PRODL
	lfsr	FSR1, _sdcard_buffer_p1
SDCARD_SectorWrite_Loop2
	movf	POSTINC1, W
	rcall	SDCARD_WriteByte
	decfsz	PRODL, F
	rgoto	SDCARD_SectorWrite_Loop2

	;; send CRC
	movlw	0xff
	rcall	SDCARD_WriteByte
	movlw	0xff
	rcall	SDCARD_WriteByte

	;; read response
	rcall	SDCARD_ReadByte

	;; check if data accepted
	andlw	0x0f
	xorlw	0x05
	bz	SDCARD_SectorWrite_WrOk
SDCARD_SectorWrite_WrFail
	rgoto	SDCARD_SectorWrite_End

SDCARD_SectorWrite_WrOk

	;; wait for write completion
SDCARD_SectorWrite_WaitLoop
	rcall	SDCARD_ReadByte
	xorlw	0x00
	bz	SDCARD_SectorWrite_WaitLoop
        
	;; required clocking (see spec)
	rcall	SDCARD_ReadByte

	;; de-activate chip select
	bsf	SDCARD_LAT_CS, SDCARD_PIN_CS

	movlw	0x00		; no error
SDCARD_SectorWrite_End
	andlw	0xff		; ensure that ZERO flag correctly set
	return


;; --------------------------------------------------------------------------
;;  Internal function to read a 8bit value from SDCARD
;;  IN: -
;;  OUT: received byte in WREG
;;  USES: -
;; --------------------------------------------------------------------------
SDCARD_ReadByte
	;; ensure that DI is at passive level
	bsf	SDCARD_LAT_DI, SDCARD_PIN_DI

	;; branch depending on slow/fast mode
	SET_BSR	_sdcard_mode
	BRA_IFSET _sdcard_mode, SDCARD_MODE_FLAG_FAST, BANKED, SDCARD_ReadByte_Fast

SDCARD_ReadByte_Slow
	;; requirement: max clock rate 400 kHz
SDCARD_ReadByte_Slow_HLP MACRO bit
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge
	nop
	nop
	nop
	rcall	SDCARD_1mS_Delay
	bcf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; falling clock edge
	rcall	SDCARD_1mS_Delay
	bcf	WREG, bit				; load value from DO
	btfsc	SDCARD_PORT_DO, SDCARD_PIN_DO
	bsf	WREG, bit
	ENDM

	SDCARD_ReadByte_Slow_HLP 7
	SDCARD_ReadByte_Slow_HLP 6
	SDCARD_ReadByte_Slow_HLP 5
	SDCARD_ReadByte_Slow_HLP 4
	SDCARD_ReadByte_Slow_HLP 3
	SDCARD_ReadByte_Slow_HLP 2
	SDCARD_ReadByte_Slow_HLP 1
	SDCARD_ReadByte_Slow_HLP 0
	nop
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge

	return

SDCARD_ReadByte_Fast
SDCARD_ReadByte_Fast_HLP MACRO bit
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge
	bcf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; falling clock edge
	bcf	WREG, bit				; load value from DO
	btfsc	SDCARD_PORT_DO, SDCARD_PIN_DO
	bsf	WREG, bit
	ENDM

	SDCARD_ReadByte_Fast_HLP 7
	SDCARD_ReadByte_Fast_HLP 6
	SDCARD_ReadByte_Fast_HLP 5
	SDCARD_ReadByte_Fast_HLP 4
	SDCARD_ReadByte_Fast_HLP 3
	SDCARD_ReadByte_Fast_HLP 2
	SDCARD_ReadByte_Fast_HLP 1
	SDCARD_ReadByte_Fast_HLP 0
	nop
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge

	return



;; --------------------------------------------------------------------------
;;  Internal function to send a 8bit value to SDCARD
;;  IN: send byte in WREG
;;  OUT: -
;;  USES: -
;; --------------------------------------------------------------------------
SDCARD_WriteByte
	;; branch depending on slow/fast mode
	SET_BSR	_sdcard_mode
	BRA_IFSET _sdcard_mode, SDCARD_MODE_FLAG_FAST, BANKED, SDCARD_WriteByte_Fast

SDCARD_WriteByte_Slow
	;; requirement: max clock rate 400 kHz
SDCARD_WriteByte_Slow_HLP MACRO bit
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge
	nop
	nop
	nop
	bcf	SDCARD_LAT_DI, SDCARD_PIN_DI		; set DI depending on current MSB
	btfsc	WREG, bit
	bsf	SDCARD_LAT_DI, SDCARD_PIN_DI
	rcall	SDCARD_1mS_Delay
	bcf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; falling clock edge
	rcall	SDCARD_1mS_Delay
	ENDM

	SDCARD_WriteByte_Slow_HLP 7
	SDCARD_WriteByte_Slow_HLP 6
	SDCARD_WriteByte_Slow_HLP 5
	SDCARD_WriteByte_Slow_HLP 4
	SDCARD_WriteByte_Slow_HLP 3
	SDCARD_WriteByte_Slow_HLP 2
	SDCARD_WriteByte_Slow_HLP 1
	SDCARD_WriteByte_Slow_HLP 0
	nop
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge

	return

SDCARD_WriteByte_Fast
SDCARD_WriteByte_Fast_HLP MACRO bit
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge
	nop
	bcf	SDCARD_LAT_DI, SDCARD_PIN_DI		; set DI depending on current MSB
	btfsc	WREG, bit
	bsf	SDCARD_LAT_DI, SDCARD_PIN_DI
	bcf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; falling clock edge
	ENDM

	SDCARD_WriteByte_Fast_HLP 7
	SDCARD_WriteByte_Fast_HLP 6
	SDCARD_WriteByte_Fast_HLP 5
	SDCARD_WriteByte_Fast_HLP 4
	SDCARD_WriteByte_Fast_HLP 3
	SDCARD_WriteByte_Fast_HLP 2
	SDCARD_WriteByte_Fast_HLP 1
	SDCARD_WriteByte_Fast_HLP 0
	nop
	bsf	SDCARD_LAT_SCLK, SDCARD_PIN_SCLK	; rising clock edge

	return


SDCARD_1mS_Delay
	bra	$+2
	bra	$+2
	bra	$+2
	bra	$+2
	bra	$+2
	return


;; --------------------------------------------------------------------------
;;  Help function to left-shift the sdcard address by 9
;; --------------------------------------------------------------------------
SDCARD_Hlp_LeftShiftAddr
	SET_BSR	_sdcard_address
	movff	_sdcard_address+2, _sdcard_address+3
	movff	_sdcard_address+1, _sdcard_address+2
	movff	_sdcard_address+0, _sdcard_address+1
	clrf	_sdcard_address+0, BANKED
	clrc
	rlf	_sdcard_address+1, F, BANKED
	rlf	_sdcard_address+2, F, BANKED
	rlf	_sdcard_address+3, F, BANKED
	return
