;; $Id$
	;; included from mclock.c
	;; this macro is stored in a seperate file, since with SDCC it
	;; is not possible to declare macros within inline assembly blocks


	;; this macro sends a single bit and waits for 32 uS
SEND_BIT: MACRO bit
	LOCAL	SEND_BIT_0
	LOCAL	SEND_BIT_1
	LOCAL	SEND_BIT_Cont
	LOCAL	SEND_BIT_Loop

	btfsc	_TBLPTRL, bit	; branch depending on bit value
	bra	SEND_BIT_1
	nop			; (balance cylces)

SEND_BIT_0:
	movlw	~0x2f		; send 0
	iorwf	_PRODL, W	; apply OR mask (pins which should not sent are set to 1)
	andwf	_LATA, F
	movlw	~0x07
	iorwf	_PRODH, W	; apply OR mask (pins which should not sent are set to 1)
	andwf	_LATE, F
	bra	SEND_BIT_Cont

SEND_BIT_1:
	movlw	0x2f		; send 1
	nop			; (balance cycles)
	iorwf	_LATA, F
	movlw	0x07
	nop			; (balance cycles)
	iorwf	_LATE, F
	bra	SEND_BIT_Cont	; (dont remove - balance cycles)

SEND_BIT_Cont:
	movlw	102		; delay loop (required cycles have been measured with simulator)
	movwf	_TABLAT
SEND_BIT_Loop:
	decfsz	_TABLAT, F
	bra	SEND_BIT_Loop
	nop
	nop
	ENDM
