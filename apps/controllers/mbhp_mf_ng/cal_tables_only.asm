; $Id$

;; Wrapper to create a .hex file which only contains the calibration tables

        LIST R=DEC

	org 0x6000
CAL_MOTOR_TABLE
#include "src/cal_motor.inc"

	org 0x6800
CAL_FADER_TABLE
#include "src/cal_fader.inc"


        END

