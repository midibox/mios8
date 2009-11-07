; ay_sr.inc - Shift Register Service Routine
;
; FUNCTION:
; CORE -> AY Chip SERIAL COMMUNICATION
; this is the {MBHP CORE writes a byte data to the AY 3 8912 SOUND CHIP BOARD} ROUTINE 
;
; AY_SR.INC FUNCTIONS IN DETAIL:
; 1) AY_SR_Write:
; 	a) AY ADRESS 	=> AY_REG_ADDRESS  [8BIT, DEC. 0-16, HEX 0x00-0x10]
; 	b) AY DATA	=> AY_REG_DATA	   [8BIT, DEC. 0-255, HEX 0x00-0xFF]
;
;  
; ----------------[MEMO: 74HC595 - SHIFT REGISTER]-----------------------
; DEFINITION of the PINS which are connected to the AY MODULE /
; TTL 74HC595: 8-BIT SERIAL-IN REGISTER WITH 8-BIT PARALLEL-OUTPUT LATCH
; SERIAL CLOCK:   SCLK (SCK ==> shift register clock input (PIN 11))
; REGISTER CLOCK: RCLK (RCK ==> storage register clock input (PIN 12))
; SERIAL DATA:    SER  (SI  ==>	serial data input (PIN 14))
; -----------------------------------------------------------------------

;; -----------[MEMO: BUS CONTROL SIGNALS OF THE AY CHIP (BDIR, BC1)]------
;; -----------------------------------------------------------------------
;; MEMO AY 3 8912 BUS CONTROL [PIN CONNETION]
;; AY BDIR 	=> PIN 18
;; AY BC1 	=> PIN 20
;; [AY BC2	=> PIN 19 (BC2 ALWAYS => 1)]
;;
;; 74HC595 - SHIFT REGISTER INTERCONNECTION
;; ASSIGNMENT => AY [8 BIT ADDRESS/DATA]
;; BIT 	   	 [  7       6       5       4       3      2       1       0   ] BIT
;; BIT 	  	 [A7/D7 | A6/D6 | A5/D5 | A4/D4 | A3/D3 | A2/D2 | A1/D1 | A0/D0] ADRESS / DATA
;; 74HC595	 [Q0/P15| Q1/P01| Q2/P02| Q3/P03| Q4/P04| Q5/P05| Q6/P06|Q7/P07] PARALLEL OUTPUT / PIN
;;
;; AY ADRESS/DATA TRANSPHERE: WITH TWO WRITING CYCLES 
;; CYCLE ONE: WRITE AY ADDRESS 
;; CYCLE TWO: WRITE AY DATA 
;;
;;
;; MEMO: WRITE ADRESS AND DATA TO THE AY CHIP (PROCEDURE):
;; [INACTIVE]->[OUTPUT ADDRESS]->[INACTIVE]->[OUTPUT DATA]->[INACTIVE]
;;
;;
;; THE AY DATA-BUS CONTROL (BDIR, [BC2], BC1)
;; [BDIR]    -     [BC2]     -     [BC1]
;;   0		     1		     0	==> AY BUS INACTIVE	
;;   0		     1		     1	==> READ FROM THE AY CHIP
;;   1		     1  	     0	==> WRITE TO THE AY CHIP
;;   1		     1		     1	==> LATCH AY REGISTER ADDRESS
;;
;;
;; WRITE ADRESS AND DATA TO THE AY CHIP (AY BUS CONTROL)
;; BUS CONTROL 	     [BDIR]     -     [BC1]
;; POS1		1		1			==> WRITE ADDRESS
;; POS2		0		0			==> INACTIVE (END CYCLE 1)
;; POS3		1		0			==> WRITE DATA
;; POS4		0		0			==> INACTIVE (END CYCLE 2)
;; 
;;
;; 	
;; ------[MEMO SET/CLEAR THE CONTROL SIGNALS]---
;; 
;; MEMO: CLEAR BDIR (AY Chip)
;; MEMO: bcf	AY_SR_LAT_BDIR, AY_SR_PIN_BDIR	
;; 
;; MEMO: SET BDIR (AY Chip)
;; MEMO: bsf	AY_SR_LAT_BDIR, AY_SR_PIN_BDIR	
;;
;; MEMO: CLEAR BC1 (AY Chip)
;; MEMO: bcf	AY_SR_LAT_BC1, AY_SR_PIN_BC1	
;;
;; MEMO: SET BC1 (AY Chip)
;; MEMO: bsf	AY_SR_LAT_BC1, AY_SR_PIN_BC1	
;;
;; ----------------[MEMO END]-----------------


 
; SET THE PINS FOR SERIAL COMMUNICATION WITH THE 74HC595 SHIFT REGISTER(S)
; -------------------------------------------------------------------------
AY_SR_LAT_SCLK		EQU	LATC	; SCLK 
AY_SR_PIN_SCLK		EQU	0 	; Pin15 RC0 CORE:J7 RC => 74HC595 PIN 11 [SCLK] 

AY_SR_LAT_RCLK		EQU	LATC	; RCLK
AY_SR_PIN_RCLK		EQU	1 	; Pin16 RC1 CORE:J7 SC => 74HC595 PIN 12 [RCLK]

AY_SR_LAT_SER_OUT	EQU	LATC	; SER
AY_SR_PIN_SER_OUT	EQU	2 	; Pin17 RC2 CORE:J7 SO => 74HC595 PIN 14 [SER]


; SET THE PINS FOR AY-3-8912 BUS CONTROL / DIRECTION SIGNALS (BC1/BDIR)
; --------------------------------------------------------
AY_SR_LAT_BDIR	  EQU	LATC	;BDIR
AY_SR_PIN_BDIR	  EQU	4 	;Pin23 RC4 / CORE J10 RC [TO =>] TO AY BDIR
AY_SR_LAT_BC1	  EQU	LATC	;BC1
AY_SR_PIN_BC1	  EQU	5 	;Pin24 RC5 / CORE J10 SO [TO =>] TO AY BC1


;; ---------------------[AY_SR_WRITE_BIT MACRO]---------------------	
;;------------------------------------------------------------------
;;------------------------------------------------------------------
;;------------------------------------------------------------------

;; -----------[MACRO: AY_SR_WRITE_BIT SERIAL DATA BIT BY BIT]-------
AY_SR_WRITE_BIT MACRO reg, bit
	;; MACRO FOR WRITING SERIAL DATA TO THE 74HC595 SHIFT REGISTER
	;; THE MACRO WRITES ONE BIT [AY ADRESS / AY DATA] TO SERIAL OUT
		
	
	bcf AY_SR_LAT_SER_OUT, AY_SR_PIN_SER_OUT
	; SET PIN [SER] TO 0	
	
		
		btfsc reg, bit ; BIT TEST f, SKIP IF CLEAR
		;; reg => MIOS_PARAMETER1
		;; bit => BIT [0-7] OF MIOS_PARAMETER1 
		; SET SER PIN DEPENDING ON REGISTER CONTENT (reg.bit)
		; IS BIT X [0-7] in REG Y [MIOS_PARAMETER1] = 0 or 1 ?
		; CHECK IT WITH btfsc => BIT TEST f, SKIP IF CLEAR
			
			bsf AY_SR_LAT_SER_OUT, AY_SR_PIN_SER_OUT
			; GO HERE, IF THE BIT X in Y IS SET 
			; AND SET PIN [SER] TO 1
			; AND MAKE ANOTHER SCLK TRIGGER

			
			; IF THE BIT X IS *NOT* SET
			; PIN [SER] REMAIN 0 
		
			 
        ; ONE MORE SERIAL CLOCK TRIGGER => SCLK - 74HC595
        bsf     AY_SR_LAT_SCLK, AY_SR_PIN_SCLK	; rising clock edge
        bcf     AY_SR_LAT_SCLK, AY_SR_PIN_SCLK	; falling clock edge

	ENDM
	
;; ---------------------[AY_SR_WRITE_BIT MACRO END]--------------------------	
;;------------------------------------------------------------------
;;------------------------------------------------------------------
;;------------------------------------------------------------------



;; **************************************************************************
;; --------------------------------------------------------------------------
;;  AY_SR_Write: WRITE A BYTE TO AN AY-3-8912 REGSITER
;; --------------------------------------------------------------------------
;; **************************************************************************
LABEL_AY_SR_Write

   ;; WRITING SERIAL Data to AY Chip is a time-critical routine
   ;; So DISABLE Interrupt Request (IRQ)
   ;; a macro @ macros.h
   IRQ_DISABLE

  ;; MEMO: AY BUS CONTROL
  ;; BUS CONTROL 	     	   [BDIR]       -      [BC1]
  ;; POS1		             1	          	 1			==> WRITE ADDRESS
  ;; POS2		             0	          	 0			==> INACTIVE (END CYCLE 1)
  ;; POS3		             1	          	 0			==> WRITE DATA
  ;; POS4		             0	          	 0			==> INACTIVE (END CYCLE 2)
  
		
	;; - ------------[START WRITE AY ADRESS]-------------------
	
	;; SET BDIR TO 1
	;; SET BC1 TO 1
	bsf	AY_SR_LAT_BDIR, AY_SR_PIN_BDIR	;; SET BDIR (AY Chip)
	bsf	AY_SR_LAT_BC1, AY_SR_PIN_BC1	;; SET BC1 (AY Chip)
	

	
	; ---------[AY ADRESS :: WRITING 8-BIT SERIAL DATA TO 1x 74HC595]----------------------
	; ------------------------------------------------------------------------	
	; AY ADRESS => MIOS_PARAMETER1
	; => SEE AY_SR_WRITE_BIT MACRO!
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 0	;; AY1 		ADDR. BYTE => [B x x x x x x x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 1	;; AY1 		ADDR. BYTE => [x B x x x x x x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 2	;; AY1		ADDR. BYTE => [x x B x x x x x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 3	;; AY1 		ADDR. BYTE => [x x x B x x x x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 4	;; AY1 		ADDR. BYTE => [x x x x B x x x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 5	;; AY1 		ADDR. BYTE => [x x x x x B x x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 6	;; AY1 		ADDR. BYTE => [x x x x x x B x]
	AY_SR_WRITE_BIT AY_REG_ADDRESS, 7	;; AY1 		ADDR. BYTE => [x x x x x x x B]

		; AFTER A SERIAL BYTE [MIOS_PARAMETER1] BYTE HAS BEEN TRANSMITTED
		; STORE THE DATA IN THE 8BIT PARALLEL DATA LATCH [=>74HC595]
		; ----------------------------------------------------------
     		bsf 	AY_SR_LAT_RCLK, AY_SR_PIN_RCLK			; latch AY values
		bcf	AY_SR_LAT_SER_OUT, AY_SR_PIN_SER_OUT		; clear out pin (standby)
     		bcf	AY_SR_LAT_RCLK, AY_SR_PIN_RCLK			; release latch
		

	;; ------------[END WRITE AY ADRESS (AY BUS CONTROL)]---------------
	;; SET AY BUS INACTIVE (END CYCLE 1)
	;; AY BUS CONTROL: SET BDIR TO 0, SET BC1 TO 0
	bcf	AY_SR_LAT_BDIR, AY_SR_PIN_BDIR	;; CLEAR BDIR (AY Chip)
	bcf	AY_SR_LAT_BC1, AY_SR_PIN_BC1	;; CLEAR BC1 (AY Chip)
	

	
	;; THE ADDRESS BYTE HAS NOW BEEN WRITTEN TO THE AY CHIP

	;; ------------[NOW LET'S START TO WRITE THE AY DATA BYTE!]-----------
	;; AY BUS CONTROL: SET BDIR TO 1, SET BC1 TO 0
	bsf	AY_SR_LAT_BDIR, AY_SR_PIN_BDIR	;; SET BDIR (AY Chip)
	bcf	AY_SR_LAT_BC1, AY_SR_PIN_BC1	;; SET BC1 (AY Chip)
	
	
	;; ---------[WRITING 8-BIT SERIAL DATA TO 1x 74HC595]------------------
	;; --------------------------------------------------------------------	
	;; => SEE AY_SR_WRITE_BIT MACRO!
	AY_SR_WRITE_BIT AY_REG_DATA, 0	;; AY1 		DATA. BYTE => [B x x x x x x x]
	AY_SR_WRITE_BIT AY_REG_DATA, 1	;; AY1 		DATA. BYTE => [x B x x x x x x]
	AY_SR_WRITE_BIT AY_REG_DATA, 2	;; AY1		DATA. BYTE => [x x B x x x x x]
	AY_SR_WRITE_BIT AY_REG_DATA, 3	;; AY1 		DATA. BYTE => [x x x B x x x x]
	AY_SR_WRITE_BIT AY_REG_DATA, 4	;; AY1 		DATA. BYTE => [x x x x B x x x]
	AY_SR_WRITE_BIT AY_REG_DATA, 5	;; AY1 		DATA. BYTE => [x x x x x B x x]
	AY_SR_WRITE_BIT AY_REG_DATA, 6	;; AY1 		DATA. BYTE => [x x x x x x B x]
	AY_SR_WRITE_BIT AY_REG_DATA, 7	;; AY1 		DATA. BYTE => [x x x x x x x B]

		; AFTER A SERIAL BYTE [AY_REG_DATA] HAS BEEN TRANSMITTED
		; STORE THE DATA IN THE 8BIT PARALLEL DATA LATCH [=>74HC595]
		; ----------------------------------------------------------
     		bsf 	AY_SR_LAT_RCLK, AY_SR_PIN_RCLK		; latch AY values
		bcf	AY_SR_LAT_SER_OUT, AY_SR_PIN_SER_OUT	; clear out pin (standby)
     		bcf	AY_SR_LAT_RCLK, AY_SR_PIN_RCLK		; release latch
		

	
	;; ------------[SET AY BUS INACTIVE (END CYCLE 2)]-------------------
	;; SET BDIR TO 0
	;; SET BC1 TO 0
	bcf	AY_SR_LAT_BDIR, AY_SR_PIN_BDIR	;; CLEAR BDIR (AY Chip)
	bcf	AY_SR_LAT_BC1, AY_SR_PIN_BC1	;; CLEAR BC1 (AY Chip)
	
	;; NOW A BYTE HAS BEEN WRITTEN TO AN AY REGISTER
	;; ENABLE Interrupt Request (IRQ)
	IRQ_ENABLE		

          ;; A BYTE HAS BEEN TRANSMITTED TO A AY-3-8912 REGISTER
	  ;; END OF THE SERIAL WRITING AND AY BUS CONTROL ROUTINE
      		return
	


