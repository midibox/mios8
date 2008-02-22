; $Id$
;; Multiply 16x16 bit routine from the PIC18F452 datasheet, Chapter 7.2 "8 X 8 HARDWARE MULTIPLIER, Operation"

;; MUL_R_3:MUL_R_0 = MUL_A_H:MUL_A_L * MUL_B_H:MUL_B_L
;;           = (MUL_A_H * MUL_B_H * 2^16) +
;;             (MUL_A_H * MUL_B_L * 2^8) +
;;             (MUL_A_L * MUL_B_H * 2^8) +
;;             (MUL_A_L * MUL_B_L)

MATH_MUL16_16
	SET_BSR	MUL_R_0
	
	movf	MUL_A_L, W
	mulwf	MUL_B_L		; MUL_A_L * MUL_B_L -> PRODH:PRODL
	movff	PRODH, MUL_R_1
	movff	PRODL, MUL_R_0
	
	movf	MUL_A_H, W
	mulwf	MUL_B_H		; MUL_A_H * MUL_B_H -> PRODH:PRODL
	movff	PRODH, MUL_R_3
	movff	PRODL, MUL_R_2

	movf	MUL_A_L, W
	mulwf	MUL_B_H		; MUL_A_L * MUL_B_H -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F	; Add cross
	movf	PRODH, W	; products
	addwfc	MUL_R_2, F
	clrf	WREG
	addwfc	MUL_R_3, F

	movf	MUL_A_H, W
	mulwf	MUL_B_L		; MUL_A_H * MUL_B_L -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F	; Add cross
	movf	PRODH, W	; products
	addwfc	MUL_R_2, F
	clrf	WREG
	addwfc	MUL_R_3, F
	movf	MUL_A_L, W
	mulwf	MUL_B_L		; MUL_A_L * MUL_B_L -> PRODH:PRODL
	movff	PRODH, MUL_R_1
	movff	PRODL, MUL_R_0

	movf	MUL_A_H, W
	mulwf	MUL_B_H		; MUL_A_H * MUL_B_H -> PRODH:PRODL
	movff	PRODH, MUL_R_3
	movff	PRODL, MUL_R_2

	movf	MUL_A_L, W
	mulwf	MUL_B_H		; MUL_A_L * MUL_B_H -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F	; Add cross
	movf	PRODH, W	; products
	addwfc	MUL_R_2, F
	clrf	WREG
	addwfc	MUL_R_3, F

	movf	MUL_A_H, W
	mulwf	MUL_B_L		; MUL_A_H * MUL_B_L -> PRODH:PRODL
	movf	PRODL, W
	addwf	MUL_R_1, F	; Add cross
	movf	PRODH, W	; products
	addwfc	MUL_R_2, F
	clrf	WREG
	return
