// $Id$
/*
 * MBFM Controller
 *
 * ==========================================================================
 *
 *  Copyright (C) 2011 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */


/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

// globally used MIDI channel (0..15)
unsigned char global_voice_midi_channel;
unsigned char global_drum_midi_channel;


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

static unsigned char last_pot;
static unsigned char sent_value[64];

/////////////////////////////////////////////////////////////////////////////
// Table of all MBFM parameter names
/////////////////////////////////////////////////////////////////////////////
static const char mbfm_parameter_names[128*16] = {
  "Bank Change     "	// 0x00
  "ModWheel        "	// 0x01
  "Transpose   All "	// 0x02
  "Unisono         "	// 0x03
  "--------        "	// 0x04
  "--------        "	// 0x05
  "--------        "	// 0x06
  "Volume      All "	// 0x07
  "--------        "	// 0x08
  "--------        "	// 0x09
  "--------        "	// 0x0a
  "--------        "	// 0x0b
  "--------        "	// 0x0c
  "AudioChn.   All "	// 0x0d
  "AudioChn.   OP12"	// 0x0e
  "AudioChn.   OP34"	// 0x0f
  
  "OP Flags     OP1"	// 0x10
  "OP Flags     OP2"	// 0x11
  "OP Flags     OP3"	// 0x12
  "OP Flags     OP4"	// 0x13
  "OP Mult.     OP1"	// 0x14
  "OP Mult.     OP2"	// 0x15
  "OP Mult.     OP3"	// 0x16
  "OP Mult.     OP4"	// 0x17
  "OP KSL       OP1"	// 0x18
  "OP KSL       OP2"	// 0x19
  "OP KSL       OP3"	// 0x1a
  "OP KSL       OP4"	// 0x1b
  "OP Volume    OP1"	// 0x1c
  "OP Volume    OP2"	// 0x1d
  "OP Volume    OP3"	// 0x1e
  "OP Volume    OP4"	// 0x1f
  
  "OP Attack    OP1"	// 0x20
  "OP Attack    OP2"	// 0x21
  "OP Attack    OP3"	// 0x22
  "OP Attack    OP4"	// 0x23
  "OP Decay     OP1"	// 0x24
  "OP Decay     OP2"	// 0x25
  "OP Decay     OP3"	// 0x26
  "OP Decay     OP4"	// 0x27
  "OP Sustain   OP1"	// 0x28
  "OP Sustain   OP2"	// 0x29
  "OP Sustain   OP3"	// 0x2a
  "OP Sustain   OP4"	// 0x2b
  "OP Release   OP1"	// 0x2c
  "OP Release   OP2"	// 0x2d
  "OP Release   OP3"	// 0x2e
  "OP Release   OP4"	// 0x2f
  
  "OP Waveform  OP1"	// 0x30
  "OP Waveform  OP2"	// 0x31
  "OP Waveform  OP3"	// 0x32
  "OP Waveform  OP4"	// 0x33
  "--------        "	// 0x34
  "--------        "	// 0x35
  "--------        "	// 0x36
  "--------        "	// 0x37
  "OP Feedback  OP1"	// 0x38
  "--------        "	// 0x39
  "--------        "	// 0x3a
  "--------        "	// 0x3b
  "OP Algorithm    "	// 0x3c
  "--------        "	// 0x3d
  "--------        "	// 0x3e
  "--------        "	// 0x3f
  
  "Mode        LFO1"	// 0x40
  "Phase       LFO1"	// 0x41
  "Rate        LFO1"	// 0x42
  "Pitch Depth LFO1"	// 0x43
  "Vol1 Depth  LFO1"	// 0x44
  "Vol2 Depth  LFO1"	// 0x45
  "Vol3 Depth  LFO1"	// 0x46
  "Vol4 Depth  LFO1"	// 0x47
  "LFO2Depth   LFO1"	// 0x48
  "AOUTDepth   LFO1"	// 0x49
  "--------        "	// 0x4a
  "--------        "	// 0x4b
  "CC Assign.   Vel"	// 0x4c
  "Init Value   Vel"	// 0x4d
  "Depth        Vel"	// 0x4e
  "--------        "	// 0x4f
  
  "Mode        LFO2"	// 0x50
  "Phase       LFO2"	// 0x51
  "Rate        LFO2"	// 0x52
  "Pitch Depth LFO2"	// 0x53
  "Vol1 Depth  LFO2"	// 0x54
  "Vol2 Depth  LFO2"	// 0x55
  "Vol3 Depth  LFO2"	// 0x56
  "Vol4 Depth  LFO2"	// 0x57
  "LFO2 Depth  LFO2"	// 0x58
  "AOUT Depth  LFO2"	// 0x59
  "--------        "	// 0x5a
  "--------        "	// 0x5b
  "CC Assign.   Aft"	// 0x5c
  "Init Value   Aft"	// 0x5d
  "Depth        Aft"	// 0x5e
  "--------        "	// 0x5f
  
  "Mode         EG5"	// 0x60
  "Attack       EG5"	// 0x61
  "AttLevel     EG5"	// 0x62
  "Decay1       EG5"	// 0x63
  "DecLevel     EG5"	// 0x64
  "Decay2       EG5"	// 0x65
  "Sustain      EG5"	// 0x66
  "Release      EG5"	// 0x67
  "Curve        EG5"	// 0x68
  "Pitch Depth  EG5"	// 0x69
  "Vol1 Depth   EG5"	// 0x6a
  "Vol2 Depth   EG5"	// 0x6b
  "Vol3 Depth   EG5"	// 0x6c
  "Vol4 Depth   EG5"	// 0x6d
  "LFO1 Depth   EG5"	// 0x6e
  "AOUT Depth   EG5"	// 0x6f
  
  "Play Mode       "	// 0x70
  "--------        "	// 0x71
  "MIDI Sync       "	// 0x72
  "WT Mode       WT"	// 0x73
  "Finetune        "	// 0x74
  "Pitch Range     "	// 0x75
  "Portamento Rate "	// 0x76
  "WT Rate       WT"	// 0x77
  "WT Par1 Asg.  WT"	// 0x78
  "WT Par2 Asg.  WT"	// 0x79
  "WT Par3 Asg.  WT"	// 0x7a
  "--------        "	// 0x7b
  "CC Assign.   Mod"	// 0x7c
  "Init Value   Mod"	// 0x7d
  "Depth        Mod"	// 0x7e
  "--------        "	// 0x7f
};


/////////////////////////////////////////////////////////////////////////////
// Pot->CC Assignments
/////////////////////////////////////////////////////////////////////////////

#define POT_ASSIGNMENT_TYPE_OFF      0
#define POT_ASSIGNMENT_TYPE_VOICE    1
#define POT_ASSIGNMENT_TYPE_VOICE_PM 2
#define POT_ASSIGNMENT_TYPE_LFOWAVE  3
#define POT_ASSIGNMENT_TYPE_DRUM     4

typedef struct {
  unsigned char type;
  unsigned char cc;
  unsigned char min;
  unsigned char max;
  unsigned char init;
} pot_assignment_t;

static const pot_assignment_t mbfm_pot_assignments[64] = {
  //                    type     CC  Min  Max  Init
  { POT_ASSIGNMENT_TYPE_VOICE,    0x1c,   0,  63,  40 }, // Volume OP1
  { POT_ASSIGNMENT_TYPE_VOICE,    0x14,   0,  15,   6 }, // Multiplier OP1
  { POT_ASSIGNMENT_TYPE_VOICE,    0x30,   0,   7,   0 }, // Wave OP1
  { POT_ASSIGNMENT_TYPE_VOICE,    0x20,   0,  15,   0 }, // Attack OP1
  { POT_ASSIGNMENT_TYPE_VOICE,    0x24,   0,  15,   0 }, // Decay OP1
  { POT_ASSIGNMENT_TYPE_VOICE,    0x28,   0,  15,   7 }, // Sustain OP1
  { POT_ASSIGNMENT_TYPE_VOICE,    0x2c,   0,  15,   5 }, // Release OP1
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x44,   0, 127,  64 }, // LFO1->OP1 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x54,   0, 127,  64 }, // LFO2->OP1 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x6a,   0, 127,  64 }, // EG5->OP1 Depth
  { POT_ASSIGNMENT_TYPE_LFOWAVE,  0x40, 0,   4,   0 }, // LFO1 Wave
  { POT_ASSIGNMENT_TYPE_VOICE,    0x41,   0, 127,   0 }, // LFO1 Phase
  { POT_ASSIGNMENT_TYPE_VOICE,    0x42,   0, 127,  80 }, // LFO1 Rate
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x43,   0, 127,  64 }, // LFO1 Pitch Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x48,   0, 127,  64 }, // LFO1->LFO2 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x49,   0, 127,  64 }, // LFO1->AOUT Depth

  { POT_ASSIGNMENT_TYPE_VOICE,    0x1d,   0,  63,  55 }, // Volume OP2
  { POT_ASSIGNMENT_TYPE_VOICE,    0x15,   0,  15,   1 }, // Multiplier OP2
  { POT_ASSIGNMENT_TYPE_VOICE,    0x31,   0,   7,   0 }, // Wave OP2
  { POT_ASSIGNMENT_TYPE_VOICE,    0x21,   0,  15,   0 }, // Attack OP2
  { POT_ASSIGNMENT_TYPE_VOICE,    0x25,   0,  15,  12 }, // Decay OP2
  { POT_ASSIGNMENT_TYPE_VOICE,    0x29,   0,  15,   9 }, // Sustain OP2
  { POT_ASSIGNMENT_TYPE_VOICE,    0x2d,   0,  15,   9 }, // Release OP2
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x45,   0, 127,  64 }, // LFO1->OP2 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x55,   0, 127,  64 }, // LFO2->OP2 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x6b,   0, 127,  64 }, // EG5->OP2 Depth
  { POT_ASSIGNMENT_TYPE_LFOWAVE,  0x50, 0,   4,   0 }, // LFO2 Wave
  { POT_ASSIGNMENT_TYPE_VOICE,    0x51,   0, 127,   0 }, // LFO2 Phase
  { POT_ASSIGNMENT_TYPE_VOICE,    0x52,   0, 127,  80 }, // LFO2 Rate
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x53,   0, 127,  64 }, // LFO2 Pitch Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x58,   0, 127,  64 }, // LFO2->LFO1 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x59,   0, 127,  64 }, // LFO2->AOUT Depth

  { POT_ASSIGNMENT_TYPE_VOICE,    0x1e,   0,  63,  20 }, // Volume OP3
  { POT_ASSIGNMENT_TYPE_VOICE,    0x16,   0,  15,  11 }, // Multiplier OP3
  { POT_ASSIGNMENT_TYPE_VOICE,    0x32,   0,   7,   0 }, // Wave OP3
  { POT_ASSIGNMENT_TYPE_VOICE,    0x22,   0,  15,   1 }, // Attack OP3
  { POT_ASSIGNMENT_TYPE_VOICE,    0x26,   0,  15,  10 }, // Decay OP3
  { POT_ASSIGNMENT_TYPE_VOICE,    0x2a,   0,  15,   4 }, // Sustain OP3
  { POT_ASSIGNMENT_TYPE_VOICE,    0x2e,   0,  15,  10 }, // Release OP3
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x46,   0, 127,  64 }, // LFO1->OP3 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x56,   0, 127,  64 }, // LFO2->OP3 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x6c,   0, 127,  64 }, // EG5->OP3 Depth
  { POT_ASSIGNMENT_TYPE_VOICE,    0x61,   0, 127,  32 }, // EG5 Attack Rate
  { POT_ASSIGNMENT_TYPE_VOICE,    0x62,   0, 127,  96 }, // EG5 Attack Level
  { POT_ASSIGNMENT_TYPE_VOICE,    0x63,   0, 127,  32 }, // EG5 Decay1 Rate
  { POT_ASSIGNMENT_TYPE_VOICE,    0x64,   0, 127,  32 }, // EG5 Decay Level
  { POT_ASSIGNMENT_TYPE_VOICE,    0x65,   0, 127,  32 }, // EG5 Decay2 Rate
  { POT_ASSIGNMENT_TYPE_VOICE,    0x66,   0, 127,  64 }, // EG5 Sustain Level

  { POT_ASSIGNMENT_TYPE_VOICE,    0x1f,   0,  63,  55 }, // Volume OP4
  { POT_ASSIGNMENT_TYPE_VOICE,    0x17,   0,  15,   1 }, // Multiplier OP4
  { POT_ASSIGNMENT_TYPE_VOICE,    0x33,   0,   7,   0 }, // Wave OP4
  { POT_ASSIGNMENT_TYPE_VOICE,    0x23,   0,  15,   1 }, // Attack OP4
  { POT_ASSIGNMENT_TYPE_VOICE,    0x27,   0,  15,  13 }, // Decay OP4
  { POT_ASSIGNMENT_TYPE_VOICE,    0x2b,   0,  15,   7 }, // Sustain OP4
  { POT_ASSIGNMENT_TYPE_VOICE,    0x2f,   0,  15,   9 }, // Release OP4
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x47,   0, 127,  64 }, // LFO1->OP4 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x57,   0, 127,  64 }, // LFO2->OP4 Depth
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x6d,   0, 127,  64 }, // EG5->OP4 Depth
  { POT_ASSIGNMENT_TYPE_VOICE,    0x3c,   0,   3,   2 }, // OP Connections
  { POT_ASSIGNMENT_TYPE_VOICE,    0x38,   0,   7,   6 }, // OP1 Feedback
  { POT_ASSIGNMENT_TYPE_VOICE,    0x76,   0, 127,   0 }, // Portamento Rate
  { POT_ASSIGNMENT_TYPE_VOICE,    0x03,   0, 127,   0 }, // Unisono
  { POT_ASSIGNMENT_TYPE_VOICE_PM, 0x02,   0, 127,  64 }, // Instrument Transpose
  { POT_ASSIGNMENT_TYPE_VOICE,    0x67,   0, 127,  32 }, // EG5 Release Rate
};


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize AIN driver
  MIOS_AIN_NumberSet(64);
  MIOS_AIN_Muxed();
  MIOS_AIN_DeadbandSet(7);

  // default MIDI channel (0..15 for channel #1..#16)
  global_voice_midi_channel =  1-1;
  global_drum_midi_channel  = 16-1;

  // last pot which has been moved (0..63)
  last_pot = 0;

  // enable MIDI merger, so that a keyboard can be connected in a chain
  MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_ENABLED);  
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam
{
  MIOS_LCD_Clear();

  // print static screen
  MIOS_LCD_PrintCString("MBFM");

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  unsigned char i;
  unsigned int offset;
  unsigned char cc = mbfm_pot_assignments[last_pot].cc;
  unsigned char value = sent_value[last_pot];

  MIOS_LCD_CursorSet(0x00 + 8);
  MIOS_LCD_PrintBCD1((last_pot >> 4)+1);
  MIOS_LCD_PrintChar('.');
  MIOS_LCD_PrintBCD2((last_pot & 0x0f)+1);
  MIOS_LCD_PrintChar(':');

  switch( mbfm_pot_assignments[last_pot].type ) {
  case POT_ASSIGNMENT_TYPE_OFF:
    MIOS_LCD_PrintChar('-');
    MIOS_LCD_PrintChar('-');
    MIOS_LCD_PrintChar('-');
    break;

  case POT_ASSIGNMENT_TYPE_LFOWAVE:
    MIOS_LCD_PrintBCD3(value >> 4);
    break;

  case POT_ASSIGNMENT_TYPE_VOICE_PM:
    if( value < 0x40 ) {
      MIOS_LCD_PrintChar('-');
      MIOS_LCD_PrintBCD2(64 - value);
    } else {
      MIOS_LCD_PrintBCD3(value - 64);
    }
    break;

  default: // e.g. VOICE and DRUM
    MIOS_LCD_PrintBCD3(value);
  }

  MIOS_LCD_CursorSet(0x40 + 0);
#if 0
  // SDCC can't handle this properly! :-/
  MIOS_LCD_PrintPreconfString(16, &mbfm_parameter_names[(unsigned int)cc << 4]);
#else
  offset = cc << 4;
  for(i=0; i<16; ++i)
    MIOS_LCD_PrintChar(mbfm_parameter_names[offset + i]);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
}


/////////////////////////////////////////////////////////////////////////////
// This is an assembly optimized function which scales a 7bit value between
// a minimum and maximum value
/////////////////////////////////////////////////////////////////////////////
unsigned char Scale_7bit(unsigned char value, unsigned char min, unsigned char max)
{
  // scaled value is (2*<7bit value> * ) >> 8
  PRODL = value << 1; // 8bit value
  PRODH = max-min+1;  // range
__asm
    movf _PRODL, W
    mulwf _PRODH, 0
__endasm;

  return min + PRODH;
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
  unsigned char val7;

  last_pot = pin;
  val7 = MIOS_AIN_Pin7bitGet(pin);

  switch( mbfm_pot_assignments[pin].type ) {
  case POT_ASSIGNMENT_TYPE_OFF:
    break; // do nothing

  case POT_ASSIGNMENT_TYPE_VOICE:
  case POT_ASSIGNMENT_TYPE_VOICE_PM: {
    unsigned char new_value = Scale_7bit(val7, mbfm_pot_assignments[pin].min, mbfm_pot_assignments[pin].max);
    if( new_value != sent_value[pin] ) {
      sent_value[pin] = new_value;
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0xb0 | (global_voice_midi_channel & 0x0f));
      MIOS_MIDI_TxBufferPut(mbfm_pot_assignments[pin].cc);
      MIOS_MIDI_TxBufferPut(new_value);
      MIOS_MIDI_EndStream();
    }
  } break;

  case POT_ASSIGNMENT_TYPE_LFOWAVE: {
    unsigned char new_value = 0x01 | (Scale_7bit(val7, mbfm_pot_assignments[pin].min, mbfm_pot_assignments[pin].max) << 4);
    if( new_value != sent_value[pin] ) {
      sent_value[pin] = new_value;
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0xb0 | (global_voice_midi_channel & 0x0f));
      MIOS_MIDI_TxBufferPut(mbfm_pot_assignments[pin].cc);
      MIOS_MIDI_TxBufferPut(new_value);
      MIOS_MIDI_EndStream();
    }
  } break;

  case POT_ASSIGNMENT_TYPE_DRUM: {
    unsigned char new_value = Scale_7bit(val7, mbfm_pot_assignments[pin].min, mbfm_pot_assignments[pin].max);
    if( new_value != sent_value[pin] ) {
      sent_value[pin] = new_value;
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0xb0 | (global_drum_midi_channel & 0x0f));
      MIOS_MIDI_TxBufferPut(mbfm_pot_assignments[pin].cc);
      MIOS_MIDI_TxBufferPut(new_value);
      MIOS_MIDI_EndStream();
    }
  } break;
  }

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}
