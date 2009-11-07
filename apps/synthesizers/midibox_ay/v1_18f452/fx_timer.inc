;; ------------
;; fx_timer.inc
;; ------------


;; Function: Fx timer (timer0 polling)
;; IF timer0 overrun flag had been set by timer0: RESET timer0 overrun flag AND INC FX_TIMER +1
;; IF timer0 overrun flag is clear do nothing

FX_TIMER_POLLING
	
	;; check timer0 overrun flag is set
        BRA_IFCLR INTCON, TMR0IF, ACCESS, Fx_Clk_NoInc
        	; timer0 overrun flag has been set, so
        	; clear overrun flag (reset bit2 @ INTCON to 0)
        	bcf INTCON, TMR0IF
	; now inc FX_TIMER
Fx_Clk_Inc
	;; INC FX TIMER +1
			incf FX_TIMER, F 
			; Then Quit
				return


Fx_Clk_NoInc
	; timer0 overrun flag has NOT been set, so
	; quit here witout Inc of FX_TIMER
				return