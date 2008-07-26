; $Id$

;; Multiply 8x16 bit routine from the PIC18F452 datasheet, Chapter 7.2 "8 X 8 HARDWARE MULTIPLIER, Operation"

;; MUL_R_2:MUL_R_0 = MUL_A_H:MUL_A_L * MUL_B_L
;;           = (MUL_A_H * MUL_B_L * 2^8) +
;;             (MUL_A_L * MUL_B_L)

MATH_MUL16_8
	movf	MUL_A_L, W, BANKED
	mulwf	MUL_B_L, BANKED		; MUL_A_L * MUL_B_L -> PRODH:PRODL
	movff	PRODH, MUL_R_1
	movff	PRODL, MUL_R_0
	
	movf	MUL_A_H, W, BANKED
	mulwf	MUL_B_L, BANKED		; MUL_A_H * MUL_B_L -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F, BANKED	; Add cross
	clrf	MUL_R_2, BANKED
	movf	PRODH, W		; products
	addwfc	MUL_R_2, F, BANKED

	return
