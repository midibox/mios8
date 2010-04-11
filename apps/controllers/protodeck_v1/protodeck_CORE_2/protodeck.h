#ifndef _PROTODECK_H
#define _PROTODECK_H

#define _COLOR_OFF 				0x00	//0
#define _COLOR_RED 				0x10	//16
#define _COLOR_GREEN 			0x20	//32
#define _COLOR_BLUE 			0x40	//64
#define _COLOR_CYAN 			0x60	//96
#define _COLOR_MAGENTA 			0x50	//80
#define _COLOR_YELLOW 			0x30	//48
#define _COLOR_WHITE 			0x70	//112

#define _BLINKING_OFF_VELOCITY 			0x7E
#define _BLINKING_ON_VELOCITY 			0x7F

extern void DisplayLED(unsigned int column, unsigned char color) __wparam;
extern void DoStartupSequence(void) __wparam;
extern void DoConnectSequence(void) __wparam;
extern void DoDisconnectSequence(void) __wparam;
extern void ClearMatrix() __wparam;
#endif
