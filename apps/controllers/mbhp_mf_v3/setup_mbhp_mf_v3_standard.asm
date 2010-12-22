; $Id$
	LIST R=DEC
;
; Standard Setup File for MBHP_MF_V3 firmware
;

;; port/pin of Status LED
#define STATUS_LED_PORT		LATD
#define STATUS_LED_PIN		5

;; port/pin defines for motor control
#define M1_DOWN_PORT		LATB
#define M1_DOWN_PIN		7
#define M1_UP_PORT		LATB
#define M1_UP_PIN		6

#define M2_DOWN_PORT		LATB
#define M2_DOWN_PIN		5
#define M2_UP_PORT		LATB
#define M2_UP_PIN		4

#define M3_DOWN_PORT		LATB
#define M3_DOWN_PIN		3
#define M3_UP_PORT		LATB
#define M3_UP_PIN		2

#define M4_DOWN_PORT		LATB
#define M4_DOWN_PIN		1
#define M4_UP_PORT		LATB
#define M4_UP_PIN		0

#define M5_DOWN_PORT		LATD
#define M5_DOWN_PIN		7
#define M5_UP_PORT		LATD
#define M5_UP_PIN		6

#define M6_DOWN_PORT		LATC
#define M6_DOWN_PIN		5
#define M6_UP_PORT		LATC
#define M6_UP_PIN		4

#define M7_DOWN_PORT		LATC
#define M7_DOWN_PIN		3
#define M7_UP_PORT		LATC
#define M7_UP_PIN		2

#define M8_DOWN_PORT		LATC
#define M8_DOWN_PIN		1
#define M8_UP_PORT		LATC
#define M8_UP_PIN		0

#include "src/main.inc"
