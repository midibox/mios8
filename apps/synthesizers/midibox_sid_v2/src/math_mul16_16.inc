; $Id$

;; Multiply 16x16 bit routine from the PIC18F452 datasheet, Chapter 7.2 "8 X 8 HARDWARE MULTIPLIER, Operation"

;; MUL_R_3:MUL_R_0 = MUL_A_H:MUL_A_L * MUL_B_H:MUL_B_L
;;           = (MUL_A_H * MUL_B_H * 2^16) +
;;             (MUL_A_H * MUL_B_L * 2^8) +
;;             (MUL_A_L * MUL_B_H * 2^8) +
;;             (MUL_A_L * MUL_B_L)

MATH_MUL16_16
	movf	MUL_A_L, W, BANKED
	mulwf	MUL_B_L, BANKED	; MUL_A_L * MUL_B_L -> PRODH:PRODL
	movff	PRODH, MUL_R_1
	movff	PRODL, MUL_R_0
	
	movf	MUL_A_H, W, BANKED
	mulwf	MUL_B_H, BANKED	; MUL_A_H * MUL_B_H -> PRODH:PRODL
	movff	PRODH, MUL_R_3
	movff	PRODL, MUL_R_2

	movf	MUL_A_L, W, BANKED
	mulwf	MUL_B_H, BANKED	; MUL_A_L * MUL_B_H -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F, BANKED	; Add cross
	movf	PRODH, W	; products
	addwfc	MUL_R_2, F, BANKED
	clrf	WREG
	addwfc	MUL_R_3, F, BANKED

	movf	MUL_A_H, W, BANKED
	mulwf	MUL_B_L, BANKED	; MUL_A_H * MUL_B_L -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F, BANKED	; Add cross
	movf	PRODH, W	; products
	addwfc	MUL_R_2, F, BANKED
	clrf	WREG
	addwfc	MUL_R_3, F, BANKED

	return


MATH_MUL16_16_SIGNED
	rcall	MATH_MUL16_16

	btfss	MUL_B_H, 7, BANKED	; MUL_B_H:MUL_B_L neg?
	bra	MATH_MUL16_16_SIGNED_NB	; no, check MUL_A
	movf	MUL_A_L, W, BANKED
	subwf	MUL_R_2, F, BANKED
	movf	MUL_A_H, W, BANKED
	subwfb	MUL_R_3, F, BANKED
MATH_MUL16_16_SIGNED_NB

	btfss	MUL_A_H, 7, BANKED	; MUL_A_H:MUL_A_L neg?
	bra	MATH_MUL16_16_SIGNED_NA	; no, done
	movf	MUL_B_L, W, BANKED
	subwf	MUL_R_2, F, BANKED
	movf	MUL_B_H, W, BANKED
	subwfb	MUL_R_3, F, BANKED
MATH_MUL16_16_SIGNED_NA
	return
