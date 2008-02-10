; $Id$
;; the 24 Bit / 16 Bit division routine has been created by Nikolai Golovchenko, 
;; and is published at:
;; http://www.piclist.org/techref/microchip/math/div/24by16.htm

;; mapping of registers to temporal registers
MATH_AARGB0	EQU	TABLAT
MATH_AARGB1	EQU	PRODH
MATH_AARGB2	EQU	PRODL
MATH_BARGB0	EQU	TMP1
MATH_BARGB1	EQU	TMP2
MATH_LOOPCOUNT	EQU	TMP3
MATH_REMB0	EQU	TMP4
MATH_REMB1	EQU	TMP5

;Inputs:
;   Dividend - AARGB0:AARGB1:AARGB2 (0 - most significant!)
;   Divisor  - BARGB0:BARGB1
;Temporary:
;   Counter  - LOOPCOUNT
;   Remainder- REMB0:REMB1
;Output:
;   Quotient - AARGB0:AARGB1:AARGB2
MATH_DIV24_16
	movlw	24
	movwf	MATH_LOOPCOUNT

        CLRF MATH_REMB0
        CLRF MATH_REMB1
        MOVLW 24
        MOVWF MATH_LOOPCOUNT
MATH_DIV24_16_Loop:
        RLCF MATH_AARGB2, W         ;shift dividend left to move next bit to remainder
        RLCF MATH_AARGB1, F         ;
        RLCF MATH_AARGB0, F         ;

        RLCF MATH_REMB1, F          ;shift carry (next dividend bit) into remainder
        RLCF MATH_REMB0, F

        RLCF MATH_AARGB2, F         ;finish shifting the dividend and save  carry in AARGB2.0,
                                ;since remainder can be 17 bit long in some cases
                                ;(e.g. 0x800000/0xFFFF). This bit will also serve
                                ;as the next result bit.
         
        MOVF MATH_BARGB1, W         ;substract divisor from 16-bit remainder
        SUBWF MATH_REMB1, F         ;
        MOVF MATH_BARGB0, W         ;
	skpc                    ;
        INCFSZ MATH_BARGB0, W       ;
        SUBWF MATH_REMB0, F         ;

;here we also need to take into account the 17th bit of remainder, which
;is in AARGB2.0. If we dont have a borrow after subtracting from lower
;16 bits of remainder, then there is no borrow regardless of 17th bit 
;value. But, if we have the borrow, then that will depend on 17th bit 
;value. If it is 1, then no final borrow will occur. If it is 0, borrow
;will occur. These values match the borrow flag polarity.

        skpnc                   ;if no borrow after 16 bit subtraction
         BSF MATH_AARGB2, 0         ;then there is no borrow in result. Overwrite
                                ;AARGB2.0 with 1 to indicate no
                                ;borrow.
                                ;if borrow did occur, AARGB2.0 already
                                ;holds the final borrow value (0-borrow,
                                ;1-no borrow)

        BTFSC MATH_AARGB2, 0        ;if no borrow after 17-bit subtraction
         BRA MATH_DIV24_16_UOK46LL            ;skip remainder restoration.

        ADDWF MATH_REMB0, F         ;restore higher byte of remainder. (w 
                                ;contains the value subtracted from it
                                ;previously)
        MOVF MATH_BARGB1, W         ;restore lower byte of remainder
        ADDWF MATH_REMB1, F         ;

MATH_DIV24_16_UOK46LL

        DECFSZ MATH_LOOPCOUNT, f    ;decrement counter
         BRA MATH_DIV24_16_Loop     ;and repeat the loop if not zero.

      return
