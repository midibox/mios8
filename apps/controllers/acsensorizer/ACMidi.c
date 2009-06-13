/*
 *  ACMidi.c
 *  ACSensorizer
 *
 */



#include "ACMidi.h"

#include <cmios.h>
#include <pic18fregs.h>
#include "main.h"





/////////////////////////////////////////////////////////////////////////////
//	send Complete 3-byte Event
/////////////////////////////////////////////////////////////////////////////

// send Midi Event
void ACMidi_SendEvent(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam {
	// send NOTE_ON
	MIOS_MIDI_BeginStream();
	MIOS_MIDI_TxBufferPut(evnt0);
	MIOS_MIDI_TxBufferPut(evnt1);
	MIOS_MIDI_TxBufferPut(evnt2);
	MIOS_MIDI_EndStream();
}


/////////////////////////////////////////////////////////////////////////////
//	send Note Message on channel
/////////////////////////////////////////////////////////////////////////////

// send Note Message
void ACMidi_SendNote(unsigned char channel, unsigned char note, unsigned char velo) __wparam {
	// send NOTE_ON
	MIOS_MIDI_BeginStream();
	MIOS_MIDI_TxBufferPut(MIDI_NOTE_ON + channel);
	MIOS_MIDI_TxBufferPut(note);
	MIOS_MIDI_TxBufferPut(velo);
	MIOS_MIDI_EndStream();
}

/////////////////////////////////////////////////////////////////////////////
//	send CC Message on channel
/////////////////////////////////////////////////////////////////////////////

void ACMidi_SendCC(unsigned char channel, unsigned char cc, unsigned char sevenBitValue) __wparam
{
	// send CC
	if(sevenBitValue < 0x80) {	// only send values < 128
		MIOS_MIDI_BeginStream();
		MIOS_MIDI_TxBufferPut(MIDI_CC + channel);	// CC CH 0..15
		MIOS_MIDI_TxBufferPut(cc);					// CC 0..127
		MIOS_MIDI_TxBufferPut(sevenBitValue);		// value
		MIOS_MIDI_EndStream();
	}
}



/////////////////////////////////////////////////////////////////////////////
//	send Program Change message along with current bank
/////////////////////////////////////////////////////////////////////////////

void ACMidi_SendPRG_CH(unsigned char bank, unsigned char program) __wparam 
{
	MIOS_MIDI_BeginStream();
	// send bankSelect
	MIOS_MIDI_TxBufferPut(MIDI_CC + MIDI_GLOBAL_CH);
	MIOS_MIDI_TxBufferPut(MIDI_CC_BANK_SELECT);
	MIOS_MIDI_TxBufferPut(bank);
	// send PRG-CH
	MIOS_MIDI_TxBufferPut(MIDI_PRG + MIDI_GLOBAL_CH);
	MIOS_MIDI_TxBufferPut(program);
	MIOS_MIDI_EndStream();
}



/////////////////////////////////////////////////////////////////////////////
//	send PANIC! If channel > 15 send panic on all channels (self recursive)
/////////////////////////////////////////////////////////////////////////////

void ACMidi_SendPanic(unsigned char channel) __wparam
{
	unsigned char c;
	if(channel > 15) {
		// send panic on all channels
		for(c=0; c<16; c++) {
			ACMidi_SendPanic(c);
		}
	} else {
		// send panic
		MIOS_MIDI_BeginStream();
		MIOS_MIDI_TxBufferPut(MIDI_CC + channel);
		MIOS_MIDI_TxBufferPut(MIDI_CC_ALL_NOTES_OFF); // CC 123: ALL NOTES OFF
		MIOS_MIDI_TxBufferPut(0x00);
		MIOS_MIDI_EndStream();
	}
}



