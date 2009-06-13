/*
 *  ACSim_console.c
 *  v 0.0.9
 *
 *  see header for version info and details
 */




// ACSim_mios.c contains all MIOS_FUNCTIONS() that are used throughout the program
// eg MIOS_MIDI_AIN_PinGet(0) returns some random numbers in a range 0..127
// open up ACSim_mios.c to tweak around some values...
// normally there should be no need to change the content of these files:
#import "ACSim_mios.h"
// toolbox, handy functions like random number generation, hex-output...
#import "ACSim_toolbox.h"

// import c files that are included in "main.h"
#include "../ACToolbox/ACToolbox.c"
#include "../ACMidi/ACMidi.c"
#include "../ACHarmonizer/ACHarmonizer.c"
#include "../ACSynchronizer/ACSyncronizer.c"
#include "../ACSensorizer/ACSensorizer.c"


// import main c source file
#include "../main.c"

// globals
unsigned char debug_LCD_line[DEBUG_LCD_LINES][DEBUG_LCD_SIZE];


// declarations for global function (see below)
void debug_init();
void debug_timer();

void debug_lcd_print_char(unsigned char c);
void debug_lcd_print_string(unsigned char len, char *str[len]);

void debug_notify_ain(unsigned char pin, unsigned int pin_value);



// ***** RUNLOOP *****

void runloop(int loopcount) {
	_Bool continueRunloop = TRUE;
	char str[64] = "";
	char choice = 'q';
	unsigned char c = 0;
	unsigned int i;
	unsigned int pin = 0;
	unsigned int value = 0;
	unsigned int value2 = 0;
	// increment tick counter
	if(mios_counter_incr >= 255) {
		mios_counter_incr = 0;
	} else {
		mios_counter_incr ++;
	}
	// poll Timer()
	if(debug_user_timer.TIMER_ENABLED) {
		Timer();
	}
	// poll Tick()
	Tick();
	// poll LCD_Display
	DISPLAY_Tick();
	// debug info
	printf("\n\nMIOS.ACSim(%i) >> ", loopcount);
	// read input
	fgets(str, 32, stdin);	// MAX chars = 32; this is NOT protected against buffer overflows!!!
	choice = str[0];
	switch (choice) {
		
		case 'q': // quit
		case 'x':
			continueRunloop = FALSE;
			break;
			
		case ' ': // OK
			debug_din_value[DEBUG_BUTTON_OK] = 0;
			DIN_NotifyToggle(DEBUG_BUTTON_OK, debug_din_value[DEBUG_BUTTON_OK]);
			break;
			
		case '+': // ENC++
			for(i=0;i<32;i++) { if(str[i]=='+') { c++; } }
				ENC_NotifyChange(DEBUG_ENCODER, c);
			break;	
		case '-': // ENC --
			for(i=0;i<32;i++) { if(str[i]=='-') { c--; } }
			ENC_NotifyChange(DEBUG_ENCODER, c);
			break;
			
		case 'a': // AIN...
			sscanf(str, "a%i,%i", &pin, &value);
			if((pin > DEBUG_AIN_NUM) || (value > 1023)) {
				printf("! max_pin=9, max_value=1023 !");
				pin = 0; value = 0;
			} else {
				printf("scanning pin %i: %i", pin, value);
				// "set" pin values:
				debug_ain_value[pin] = value;
				AIN_NotifyChange(pin, value);
			}
			break;
			
		case 'b': // System Realtime BYTE
			debug_MIDI_byteNum = 0;	// sending single byte, maybe without MIDI_START/_STOP
			sscanf(str, "b%i", &pin);
			MPROC_NotifyReceivedByte(pin);
			debug_MIDI_byteNum = 0;
			break;
			
		case 'd': // DIN...
			sscanf(str, "d%i,%i", &pin, &value);
			// resolve pin shortcuts
			switch(str[1]) {
				case 'l': case 'o': case 'r':	// load, open, read
					pin = DIN_LOAD;
					value = str[3] - 0x30;
					break;
				case 's': case 'w': // save/store, write
					pin = DIN_STORE;
					value = str[3] - 0x30;
					break;
				case 'p':			// pedal
					pin = DIN_PEDAL;
					value = str[3] - 0x30;
					break;
				case '!':			// PANIC!
					pin = DIN_PANIC;
					value = str[3] - 0x30;
					break;
			}
			// invert value for easier handling (1=PRESSED,0=RELEASED)
			value = value ^ 0x1;
			DIN_NotifyToggle(pin, value);
			break;
			
		case 'e': // ENC # ++/--
			sscanf(str, "e%i", &pin);
			if(pin > DEBUG_ENC_NUM) {
				printf("! max_enc = %i", (DEBUG_ENC_NUM - 1));
				return;
			}
			for(i=1;i<32;i++) {
				switch(str[i]) {
					case '+': 
						c++;
						break;
					case '-':
						c--;
						break;
					case 's':
						pin = ENC_SENSOR_SELECT;
						break;
					case 'm':
						pin = ENC_MENU_SELECT;
						break;
					case 'a':
						pin = ENC_PARAM_A;
						break;
					case 'b':
						pin = ENC_PARAM_B;
						break;
				}
			}
			ENC_NotifyChange(pin, c);
			break;
			
		case 'h': // show help
			printf("\n+++++++ MIOS-DEBUG-CONSOLE ++++++++\n");
#if DEBUG_SYSTEM_VERBOSE
			printf("\nVirtual System Config:\n");
			printf("\tAIN: %i \tDIN: %i \tDOUT: %i \tENC: %i \n", DEBUG_AIN_NUM, DEBUG_DIN_NUM, DEBUG_DOUT_NUM, DEBUG_ENC_NUM);
			printf("\tLCD: %ix%i \n", DEBUG_LCD_LINES, DEBUG_LCD_SIZE);
			printf("\tBANKSTICK: %ix%i bytes \tEEPROM 1x%i bytes\n", DEBUG_BANKSTICK_NUM, DEBUG_BANKSTICK_SIZE, DEBUG_EEPROM_SIZE);
#endif /* DEBUG_SYSTEM_VERBOSE */
			printf("\nACSim Commands:\n");
//			printf("\t- Testfunc: t                      eg: t \n");
			printf("\tQuit:       q|x                    eg: q \n");
			printf("\nInput Commands:\n");
			printf("\tAIN: \n");
			printf("\t- input:    a(pin),(10-bit-value)  eg: a0,1000 \n");
			printf("\t- random:   r                      eg: r \n");
			printf("\tDIN: \n");
			printf("\t- input:    d(pin|{opt}),(state)   eg: d5,1 \n");
			printf("\t   {opt}=(l)oad|(o)pen|(r)ead      eg: dl,1 \n");
			printf("\t   {opt}=(s)ave|(w)rite            eg: ds,1 \n");
			printf("\t   {opt}=(p)edal                   eg: dp,1 \n");
			printf("\t   {opt}=(!)panic                  eg: d!,1 \n");
			printf("\t- jumper:   j(pin)                 eg: j10 or j14 \n");
			printf("\tENC: \n");
			printf("\t- input:    e{opt}(++)(-)          eg: e0++++ \n");
			printf("\t   {opt}=(s)ensor/(m)enu/(a)/(b)   eg: em- \n");
			printf("\t- incr all: +{++++..}              eg: ++ \n");
			printf("\t- decr all: -{----..}              eg: - \n");
			printf("\tMIDI-IN: \n");
			printf("\t- message:  m(b0),(b1),(b2)        eg: m176,20,64 \n");
			printf("\t- note:     n(pitch),(velo),{ch}   eg: n64,100,16 \n");
			printf("+++++++++++++++++++++++++++++++++++\n");
			
			break;
			
		case 'j': // set jumper pin
			sscanf(str, "j%i", &pin);
			switch(pin) {
				case 10:
					PORTCbits.RC5 = 1;
					Timer();
					PORTCbits.RC5 = 0;
					Timer();
					break;
				case 14:
					PORTDbits.RD4 = 1;
					Timer();
					PORTDbits.RD4 = 0;
					Timer();
					break;
				default:
					printf("jumper pin #%i not (yet?) supported", pin);
					break;
			}
			break;
			
		case 'm': // MIDI Receive (m msgType, argA, argB), 3byte command (see b to send system realtime)
			sscanf(str, "m%i,%i,%i", &pin, &value, &value2);
			MPROC_NotifyReceivedEvnt(pin,value,value2);
			break;
		
		case 'n': // send note_on (n noteValue, volume, {channel})
			sscanf(str, "n%i,%i,%i", &pin, &value, &value2);
			if((value2 < 1) || (value2 > 16)) { value2 = 1; }
			MPROC_NotifyReceivedEvnt((143+value2),pin,value);
			break;
			
		case 'r': // random
			pin = ACRandomPin();
			value = ACRandomInt();
			printf("scanning pin %i: %i", pin, value);
			// "set" pin values:
			debug_ain_value[pin] = value;
			AIN_NotifyChange(pin, value);
			break;
			
		case 't': // test function
			
			sCC[0] = 0;
			sync_q[0] = 0;
			sensor[0].releaseDetect = FALSE;
			sensorizer.pedalUp = FALSE;
			sMin[0] = 0;
			sMax[0] = 100;
			sensor[0].autoSense = AUTOSENSE_MAX;
			
			for(value = 0; value < 1024; value ++) {
				AIN_NotifyChange(0,value);
			}

			
//			AIN_NotifyChange(0, 250);
			
//			sscanf(str, "t%i", &value);
//			sscanf(str, "t%i, %i", &pin, &value);
//
//			if(value) {
//				printf("getting random number in range: %i", ACMath_RandomInRange(value) );
//			} else {
//				printf("getting random number: %i", ACMath_Random() );
//			}
//			
//			ACHarmonizer_SetBase(value);
//			printf("New BaseNote: %i \t now: %i", value, harmonizer_base);
			
//			printf("OSC %i \t=> note %i", pin, value);
//			IIC_SPEAKJET_OSCFreq(pin,value);
//			IIC_SPEAKJET_Distortion(value);
//			IIC_SPEAKJET_TransmitNumber(value);
//			MPROC_NotifyReceivedEvnt(MIDI_CC,SJCC_OSC1_LVL,value);
//			IIC_SPEAKJET_TransmitStart(0);
//			IIC_SPEAKJET_NoteToFreqChars(value);
//			IIC_SPEAKJET_TransmitStop();
//			printf("\nDEC: %i \tPARAM3: %X \tPARAM2: %X \tPARAM1: %X", value, MIOS_PARAMETER3, MIOS_PARAMETER2, MIOS_PARAMETER1);
			break;
			
		default:
			sscanf(str, "%i", &pin);
			if(str[0] != '\n') {
				// command not recognized
				printf("**ACSIM: Command not recognized. Type 'h' for help. \n");
			}
			break;
	}
	
	if(continueRunloop) { 

		runloop(++loopcount);
	}
}




#ifndef _DEBUG_GUI
// ***** MAIN *****
int main(int argc, char **argv) {
	int exit_code = 0;
	
	debug_init();
	
	// runloop
	runloop(0);
	
	return exit_code;
}
#endif /* _DEBUG_GUI */




void debug_init() {
	// manual:
	printf("\n+++++++ MIOS-DEBUG-CONSOLE ++++++++\n");
	printf("(h) for help \n\n");
	
	// init debug
	srandomdev(); // set the random seed
	
	// init tick counter
	mios_counter_incr = 0;
	
	// init MIOS
	MIOS_BOX_STAT.BS_AVAILABLE = 1;
	Init();
	DISPLAY_Init();
	
	printf("\n");
}


void debug_timer() {
	// timer for graphical stuff here
	// poll Timer()
	if(debug_user_timer.TIMER_ENABLED) {
		Timer();
	}
	// poll Tick()
	Tick();
	// poll LCD_Display
	DISPLAY_Tick();
}

void debug_lcd_print_char(unsigned char c) {
	unsigned char line = debug_cursor_pos & 0x0F;
	unsigned char cur = debug_cursor_pos - line;
	if(cur>(sizeof(debug_LCD_line[line]))) { return; }
	debug_LCD_line[line][cur] = c;
}

void debug_lcd_print_string(unsigned char len, char *str[len]) {
	unsigned char c;
	unsigned char line = debug_cursor_pos & 0x0F;
	unsigned char cur = debug_cursor_pos - line;
	for(c=0;c<len;c++) {
		if(cur>(len-2)) { return; }
		debug_LCD_line[line][cur+c] = *str[c];
	}
}

void debug_notify_ain(unsigned char pin, unsigned int pin_value) {
	AIN_NotifyChange(pin,pin_value);
}



