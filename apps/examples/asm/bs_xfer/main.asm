
	;; define processor (note for the experts:
	;; "EEPROM16" doesn't work here due to the automatic wraparound at 0x10000
	;; -> the BankStick memory must be located at 0x410000-0x41ffff)
	LIST	P=PIC18F452, R=DEC

	;; suppress "Address exceeds maximum range for this processor" message
	ERRORLEVEL	-220

	;; include the bankstick content
#include "bankstick_content.inc"

	;; END (never remove this directive)
	END

