; $Id$

;; Divide 16 bit int by 8 bit int to 8 bit int and remainder 
;; Code written by Lou Zher 
;; from the PICLIST site: http://www.piclist.com

;-[ Div ]--------------------------------------------------------------
; Call w/: Number in f_divhi:f_divlo, divisor in W.
; Returns: Quotient in f_divlo, remainder in f_divhi. W preserved.
;          Carry set if error. Z if divide by zero, NZ if divide overflow.
; Notes:   Works by left shifted subtraction.
;          Size = 29, Speed(w/ call&ret) = 7 cycles if div by zero
;          Speed = 94 minimum, 129 maximum cycles

MATH_DIV16_8
	addlw 0		; w+=0 (to test for div by zero)
	setc		; set carry in case of error
	skpnz		; if zero
	return		;   return (error C,Z)

	rcall	MATH_DIV16_8_HiShift
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code
	rcall	MATH_DIV16_8_Code

	rlf	PRODL, F	; C << lo << C

	; If the first subtract didn't underflow, and the carry was shifted
	; into the quotient, then it will be shifted back off the end by this
	; last RLF. This will automatically raise carry to indicate an error.
	; The divide will be accurate to quotients of 9-bits, but past that
	; the quotient and remainder will be bogus and carry will be set.

	bcf	STATUS, Z	; NZ (in case of overflow error)
	return			; we are done!

MATH_DIV16_8_Code
	rlf	PRODL, F	; C << lo << C
	rlf	PRODH, F	; C << hi << C
	bnc	MATH_DIV16_8_HiShift
	subwf	PRODH, F	;   hi-=w
	setc			;   ignore carry
	return			;   done
                         ; endif
MATH_DIV16_8_HiShift
	subwf	PRODH, F	; hi-=w
	skpnc			; if carry set
	return			;   done
	addwf	PRODH, F	; hi+=w
	clrc			; clear carry
	return			; done
