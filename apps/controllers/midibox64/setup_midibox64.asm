; $Id$
;
; Setup File of TK's MIDIbox64
;
; Number of connected pots
; Allowed values: 1-64
#define DEFAULT_NUMBER_POTS	64	; number of connected pots
;
; Multiplexers (AINX4 modules) connected? -> set this value to 1
; No multiplexer connected= -> set this value to 0, allowed number of pots in this case: 1-8
#define DEFAULT_MUX_ENABLED	 1	; enable multiplexers
;
; Page handling: special option for MIDIboxes stuffed with 16 pots
; if enabled, 4 pages with 16 pot entries are available which can
; be switched with Meta event FF 05 xx
#define	DEFAULT_ENABLE_PAGE_HANDLING 0
;
; Number of connected buttons
; Allowed values: 1-64
#define DEFAULT_NUMBER_BUTTONS  64	; number of connected buttons
;
; debounce counter (see the function description of MIOS_SRIO_DebounceSet)
; Use 0 for high-quality buttons, use higher values for low-quality buttons
#define DEFAULT_SRIO_DEBOUNCE_CTR 32
;
;
; A difficult decition: define here if you want to store many banks or many patches
; in the (optional) BankStick(s).
;    - Bank based  (PATCHMODE 0): 16 complete configurations are stored in each BankStick
;    - Patch based (PATCHMODE 1): 1 configuration bank and 128 patches are stored in each BankStick
#define DEFAULT_PATCHMODE 0
;
; Chip Select Number of BankStick which should be used:
;    -1: allows the MIDIbox to address up to 8 BankSticks (-> 64 Banks)
;     0: the MIDIbox addresses only the BankStick with CS=0 (-> 8 Banks)
;     ...
;     7: the MIDIbox addresses only the BankStick with CS=7 (-> 8 Banks)
;     all others: don't use any BankStick
;
; * -1 is the most prefered choice, also when only one BankStick is connected.
; * specify a CS number 0...7 when you are using different Applications 
;   with your MIDIbox in order to assign a dedicated EEPROM to it.
; * specifiy any other number to disable BankStick support at all
#define DEFAULT_BANKSTICK_CS	-1
;
; Channel which is used to send a program change event on bank changes
; and to listen for bank change requests
; Allowed values: 1-16
#define DEFAULT_BANK_PRG_CHNG_CHANNEL 16
;
; Channel which is used to send a program change event on patch changes
; and to listen for patch change requests
; Allowed values: 1-16
#define DEFAULT_PATCH_PRG_CHNG_CHANNEL 15
;
; here you can change the default device ID
#define DEFAULT_DEVICE_ID	0x00
;
; if this option is enabled (1), the DEFAULT_DEVICE_ID won't be used, but
; it will be automatically derived from the MIOS Device ID instead
#define DEFAULT_AUTO_DEVICE_ID	1
;
; For MIDI activity monitor: define the DOUT pins for the Rx and Tx LED
#define DEFAULT_MIDI_MONITOR_ENABLED 1  ; if 1, the Tx/Rx LEDs are enabled
#define DEFAULT_MIDI_RX_LED 0x40	; DOUT SR#9, pin D0
#define DEFAULT_MIDI_TX_LED 0x41	; DOUT SR#9, pin D1
;
; This DIN map allows you to customize the MBMF application to different hardwares
; The MBMF dump structure allows the use of up to 64 buttons, they are grouped to 8 buttons per shift register
; Define the used shift registers for the buttons here
; the shift registers are counted from one - means: 1 for the first, 2 for the second, etc...
; mark unused button groups with 0
#define DEFAULT_DIN_SR_PIN_01_08	2
#define DEFAULT_DIN_SR_PIN_09_16	3
#define DEFAULT_DIN_SR_PIN_17_24	1
#define DEFAULT_DIN_SR_PIN_25_32	4
#define DEFAULT_DIN_SR_PIN_33_40	5
#define DEFAULT_DIN_SR_PIN_41_48	6
#define DEFAULT_DIN_SR_PIN_49_56	7
#define DEFAULT_DIN_SR_PIN_57_64	8
;
; Datawheel for menu navigation/data entry connected or not?
; A datawheel can replace the left/right buttons!
; if -1: datawheel not connected
; if >= 0: encoder number assigned to datawheel function
; don't forget to check the pins of the datawheel in MIOS_ENC_PIN_TABLE (-> mios_tables.inc or setup_*.asm)
; it has to be connected to pin 4 and 5 by default
#define DEFAULT_ENC_DATAWHEEL   -1
;
; DIN pin numbers of menu buttons
#if DEFAULT_ENC_DATAWHEEL == -1
  #define DEFAULT_DIN_MENU_EXEC		7	; menu exec button assigned to DIN pin #7
  #define DEFAULT_DIN_MENU_RIGHT	6	; menu right button assigned to DIN pin #6
  #define DEFAULT_DIN_MENU_LEFT		5	; menu left button assigned to DIN pin #5
  #define DEFAULT_DIN_MENU_SNAPSHOT	4	; menu snapshot button assigned to DIN pin #4
#else
  #define DEFAULT_DIN_MENU_EXEC		7	; menu exec button assigned to DIN pin #7
  #define DEFAULT_DIN_MENU_SNAPSHOT	6	; menu snapshot button assigned to DIN pin #4
  #define DEFAULT_DIN_MENU_RIGHT	5	; NOT USED - overlayed by datawheel
  #define DEFAULT_DIN_MENU_LEFT		4	; NOT USED - overlayed by datawheel
#endif
;
; This DOUT map allows you to customize the MBMF application to different hardwares
; The MBMF dump structure allows the use of up to 64 LEDs, they are grouped to 8 LEDs per shift register
; Define the used shift registers for the LEDs here
; the shift registers are counted from one - means: 1 for the first, 2 for the second, etc...
; mark unused LED groups with 0
#define DEFAULT_DOUT_SR_PIN_01_08	1
#define DEFAULT_DOUT_SR_PIN_09_16	2
#define DEFAULT_DOUT_SR_PIN_17_24	3
#define DEFAULT_DOUT_SR_PIN_25_32	4
#define DEFAULT_DOUT_SR_PIN_33_40	5
#define DEFAULT_DOUT_SR_PIN_41_48	6
#define DEFAULT_DOUT_SR_PIN_49_56	7
#define DEFAULT_DOUT_SR_PIN_57_64	8
;
;
; Some menus are providing the possibility to use 16 "general purpose" buttons
; for selecting a parameter - e.g. the Bank Select menu allows to directly change the
; Bank with these buttons. So long as you stay in this menu, the normal function of
; these buttons (triggering MIDI or SFB events) is disabled
; Define the two shift registers which are assigned to this function here:	
#define DEFAULT_GP_DIN_SR0	2	; first GP DIN shift register assigned to SR#2
#define DEFAULT_GP_DIN_SR1	3	; second GP DIN shift register assigned to SR#3
;
; above these buttons LEDs should be mounted to visualize the selected parameter
; (e.g. to visualize the bank which corresponds with the buttons below)
; Define the two shift registers which are assigned to this function here:	
#define DEFAULT_GP_DOUT_SR0	1	; first GP DOUT shift register assigned to SR#1
#define DEFAULT_GP_DOUT_SR1	2	; second GP DOUT shift register assigned to SR#2
;
; define the display size you are using:
;    0: 2x16
;    1: 2x20
;    2: 4x16
;    3:	4x20
#define DEFAULT_LCD_SIZE 0
;
; with following settings it is possible to center the screen on 2x20 and 2x40 LCDs
; see also the functional description of MIOS_LCD_YAddressSet
; you possibly don't have to change the settings here if DEFAULT_LCD_SIZE is
; set correctly
#if DEFAULT_LCD_SIZE == 0	; 4x20
  #define DEFAULT_YOFFSET_LINE0	0x00
  #define DEFAULT_YOFFSET_LINE1	0x40
  #define DEFAULT_YOFFSET_LINE2	0x14
  #define DEFAULT_YOFFSET_LINE3	0x54
  #define CSMD_YOFFSET_LINE0	0x00
  #define CSMD_YOFFSET_LINE1	0x40
  #define CSMD_YOFFSET_LINE2	0x14
  #define CSMD_YOFFSET_LINE3	0x54
#endif
#if DEFAULT_LCD_SIZE == 1	; 2x20
  #define DEFAULT_YOFFSET_LINE0	0x02
  #define DEFAULT_YOFFSET_LINE1	0x42
  #define DEFAULT_YOFFSET_LINE2	0x16
  #define DEFAULT_YOFFSET_LINE3	0x56
  #define CSMD_YOFFSET_LINE0	0x00
  #define CSMD_YOFFSET_LINE1	0x40
  #define CSMD_YOFFSET_LINE2	0x14
  #define CSMD_YOFFSET_LINE3	0x54
#endif
#if DEFAULT_LCD_SIZE == 2	; 4x16
  #define DEFAULT_YOFFSET_LINE0	0x00
  #define DEFAULT_YOFFSET_LINE1	0x40
  #define DEFAULT_YOFFSET_LINE2	0x10
  #define DEFAULT_YOFFSET_LINE3	0x50
  #define CSMD_YOFFSET_LINE0	0x00
  #define CSMD_YOFFSET_LINE1	0x40
  #define CSMD_YOFFSET_LINE2	0x10
  #define CSMD_YOFFSET_LINE3	0x50
#endif
#if DEFAULT_LCD_SIZE == 3	; 4x20
  #define DEFAULT_YOFFSET_LINE0	0x02
  #define DEFAULT_YOFFSET_LINE1	0x42
  #define DEFAULT_YOFFSET_LINE2	0x16
  #define DEFAULT_YOFFSET_LINE3	0x56
  #define CSMD_YOFFSET_LINE0	0x00
  #define CSMD_YOFFSET_LINE1	0x40
  #define CSMD_YOFFSET_LINE2	0x14
  #define CSMD_YOFFSET_LINE3	0x54
#endif

; default value of touch sensor sensitivty (stored in MB64 dump)
#define DEFAULT_TOUCH_SENSOR_SENSITIVITY 0

; enable access to the AOUT module via Port J10
#define DEFAULT_ENABLE_AOUT 0
#define DEFAULT_NUMBER_AOUTS 8
#define DEFAULT_ENABLE_AMUX 0
;
; experimental support for drum triggers
#define DEFAULT_ENABLE_DRUMS 0


#include "src/main.inc"
