#!/usr/bin/perl -w
# $Id$
#########################################################################################
#
# This script converts MIDIbox SID V1 patches to the new MIDIbox SID V2 format
# The .syx file can contain a single or multiple patches.
#
# Author: tk@midibox.org (2007-04-27)
#
# SYNTAX: v1_to_v2.pl <source-syx-file> <target-syx-file> [<-bank n>] [<-debug>]
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;        # enabled on debug mode
$bank  = 0;        # default bank
$max_file_len = 0x80000;

# default settings, taken from sid_preset_lead.inc
@v2_default_patch = (
	      ord('L'),ord('e'),ord('a'),ord('d'),ord(' '),ord('P'),ord('a'),ord('t'),ord('c'),ord('h'),ord(' '),ord(' '),ord(' '),ord(' '),ord(' '),ord(' '),

	##      ENGINE  HWFLAGS  OPT1  OPT2  Phase Swtch -     -   
		0x00,   0x00,    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	##      PAR1  PAR2  INIT  MIN   MAX     PAR1  PAR2  INIT  MIN   MAX
		0x00, 0x00, 0x80, 0x00, 0xff,   0x00, 0x00, 0x80, 0x00, 0xff,	# Knob1/2
		0x00, 0x00, 0x80, 0x00, 0xff,   0x00, 0x00, 0x80, 0x00, 0xff,	# Knob3/4
		0x00, 0x00, 0x80, 0x00, 0xff,   0x00, 0x00, 0x80, 0x00, 0xff,	# Knob5/Vel
		0x50, 0x00, 0x80, 0x00, 0xff,   0x00, 0x00, 0x80, 0x00, 0xff,	# PB/Aftertouch

	##	EXT Parameters 1-8 - 16 bit values (low/high), normaly used as AOUT offset
		0x00, 0x80,	# AOUT#1
		0x00, 0x80,	# AOUT#2
		0x00, 0x80,	# AOUT#3
		0x00, 0x80,	# AOUT#4
		0x00, 0x80,	# AOUT#5
		0x00, 0x80,	# AOUT#6
		0x00, 0x80,	# AOUT#7
		0x00, 0x80,	# AOUT#8

	##	FLAGS  OPT1  VOL   BAL
		0x00,  0x00, 0x7f, 0x40,

	##	C/M    CutOff      Res.  -     -
		0x10,  0x00, 0x02, 0x00, 0x00, 0x00,	# Filter1
		0x10,  0x00, 0x02, 0x00, 0x00, 0x00,	# Filter2

	##	Flgs  Wave   AD    SR   PW_L  PW_H  Res1  Delay Trns  Fine  PRng  Port. ArpM  A.S/D A.G/R Res2
		0x00, 0x04, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,	# SID1 Voice1
		0x00, 0x14, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,	# SID1 Voice2
		0x00, 0x14, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,	# SID1 Voice3
		0x00, 0x04, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,	# SID2 Voice1
		0x00, 0x14, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,	# SID2 Voice2
		0x00, 0x14, 0x00, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x80, 0x02, 0x00, 0x00, 0x1f, 0x05, 0x00,	# SID2 Voice3

	##	Mode  Dep.  Rate  Dly   Phs    Mode  Dep.  Rate  Dly   Phs
		0x01, 0x80, 0x00, 0x00, 0x00,  0x01, 0x80, 0x00, 0x00, 0x00,	# LFO1/2
		0x01, 0x80, 0x00, 0x00, 0x00,  0x01, 0x80, 0x00, 0x00, 0x00,	# LFO3/4
		0x01, 0x80, 0x00, 0x00, 0x00,  0x01, 0x80, 0x00, 0x00, 0x00,	# LFO5/6

	## reserved
		0,0,

	##	Mode  Dep.  Del.  Atk1  AtkL  Atk2  Dec1  DecL  Dec2  Sust  Rel1  RelL  Rel2  ACur  DCur  SCur
		0x00, 0xc0, 0x00, 0x30, 0xff, 0x00, 0x00, 0xff, 0x30, 0x40, 0x20, 0x00, 0x00, 0x80, 0x80, 0x80,	# ENV1
		0x00, 0x40, 0x00, 0x50, 0xff, 0x00, 0x00, 0xff, 0x50, 0x10, 0x30, 0x00, 0x00, 0x80, 0x80, 0x80,	# ENV2

	##	SRC1  SRC2  OP    Depth TrgL  TrgR  X.#1  X.#2
		0x01, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod1
		0x02, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod2
		0x03, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod3
		0x04, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod4
		0x05, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod5
		0x06, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod6
		0x07, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod7
		0x08, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,	# Mod8

	##	Trigger Matrix
		0xff, 0x00, 0x0f,   0x3f, 0x03, 0x00,	# Note On / Note Off
		0x00, 0x00, 0x00,   0x00, 0x00, 0x00,	# ENV1/2 Sustain Phase
		0x00, 0x00, 0x00,   0x00, 0x00, 0x00,	# LFO1/2 Period
		0x00, 0x00, 0x00,   0x00, 0x00, 0x00,	# LFO3/4 Period
		0x00, 0x00, 0x00,   0x00, 0x00, 0x00,	# LFO5/6 Period
		0x00, 0x00, 0xf0,   0x00, 0x00, 0x00,	# Clock, Clock/4
		0x00, 0x00, 0x00,   0x00, 0x00, 0x00,	# Clock/16, MIDI Clock Start

	## reserved
		0,0,

	##	Speed Asgn  Begin End   Loop    Speed Asgn  Begin End   Loop
		0xc7, 0x00, 0x00, 0x1f, 0x80,   0xc7, 0x00, 0x20, 0x3f, 0xa0,	# WT#1 and WT#2
		0xc7, 0x00, 0x40, 0x5f, 0xc0,   0xc7, 0x00, 0x60, 0x7f, 0xe0,	# WT#3 and WT#4

		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
		0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	# Wavetable Preload
  );

# address offsets taken from app_defines.inc

# NOTE: the structure descriptions and bit assignments are not complete
# the reference table is located under doc/mbsidv2_sysex_implementation.txt

$SID_Ix_NAME_x		=	0x000;  # 0x00-0x0f

## === general parameters, structure identical for each engine ===
$SID_Ix_ENGINE		=	0x010;	# [1:0] engine (Lead/Bassline/Drum//Multi), [7:2]: reserved
$SID_Ix_HW_FLAGS	=	0x011;	# hardware info flags: [1:0] 0=no SID restriction, 1=6581, 2=6582/8580, 3=SwinSID# [3]: stereo, [7:4] caps: 0=470pF, 1=1nF, 2=2.2nF, 3=4.7nF, 4=6.8nF, 5=10nF, 6=22nF, 7=47nF, 8=100nF
$SID_Ix_OPT1_FLAGS	=	0x012;	# reserved for future engine options
$SID_Ix_OPT2_FLAGS	=	0x013;	# reserved for future engine options

$SID_Ix_CUSTOM_SW	=	0x014;	# custom switch flags #1-8

$SID_Ix_Px_ASSIGN1	=	0x00;	# Knob/Controller assignment #1
$SID_Ix_Px_ASSIGN2	=	0x01;	# Knob/Controller assignment #2
$SID_Ix_Px_VALUE	=	0x02;	# Knob/Controller initial value
$SID_Ix_Px_MIN		=	0x03;	# Knob/Controller min value
$SID_Ix_Px_MAX		=	0x04;	# Knob/Controller max value
$SID_Ix_P_K1_BASE	=	0x018;#-0x1c	Knob #1
$SID_Ix_P_K2_BASE	=	0x01d;#-0x21	Knob #2
$SID_Ix_P_K3_BASE	=	0x022;#-0x26	Knob #3
$SID_Ix_P_K4_BASE	=	0x027;#-0x2b	Knob #4
$SID_Ix_P_K5_BASE	=	0x02c;#-0x30	Knob #5
$SID_Ix_P_KV_BASE	=	0x031;#-0x35	Velocity
$SID_Ix_P_KP_BASE	=	0x036;#-0x3a	Pitch Bender
$SID_Ix_P_KA_BASE	=	0x03b;#-0x3f     Aftertouch

$SID_Ix_EXT_PAR1_L	=	0x040;	# external parameter #1 (e.g. for AOUT)
$SID_Ix_EXT_PAR1_H	=	0x041;
$SID_Ix_EXT_PAR2_L	=	0x042;	# external parameter #2
$SID_Ix_EXT_PAR2_H	=	0x043;
$SID_Ix_EXT_PAR3_L	=	0x044;	# external parameter #3
$SID_Ix_EXT_PAR3_H	=	0x045;
$SID_Ix_EXT_PAR4_L	=	0x046;	# external parameter #4
$SID_Ix_EXT_PAR4_H	=	0x047;
$SID_Ix_EXT_PAR5_L	=	0x048;	# external parameter #5
$SID_Ix_EXT_PAR5_H	=	0x049;
$SID_Ix_EXT_PAR6_L	=	0x04a;	# external parameter #6
$SID_Ix_EXT_PAR6_H	=	0x04b;
$SID_Ix_EXT_PAR7_L	=	0x04c;	# external parameter #7
$SID_Ix_EXT_PAR7_H	=	0x04d;
$SID_Ix_EXT_PAR8_L	=	0x04e;	# external parameter #8
$SID_Ix_EXT_PAR8_H	=	0x04f;

## === engine specific parameters ===

## --- Lead ---
$SID_Ix_LEAD_FLAGS1	=	0x050;	# mono/legato [0], WTO [1], SusKey [2]
$SID_Ix_LEAD_OSC_DETUNE	=	0x051;  # detune left/right oscillators
$SID_Ix_LEAD_VOLUME	=	0x052;  # 7 bit value, only 4 bit used
$SID_Ix_LEAD_OSC_PHASE	=	0x053;  # oscillator phase offset

$SID_Ix_LEAD_Fx_CHN_MODE =	0x0;	# offset: SID1/2 [3:0] assigned channels, [7:4] filter mode
$SID_Ix_LEAD_Fx_CUTOFF_L =	0x1;	# offset: SID1/2 cut-off frequency (12 bit, 11 bit used)
$SID_Ix_LEAD_Fx_CUTOFF_H =	0x2;
$SID_Ix_LEAD_Fx_RESONANCE =	0x3;	# offset: SID1/2 resonance (8bit, 4 bit used)
$SID_Ix_LEAD_Fx_RESERVED1 =	0x4;	# offset: reserved for future expansions
$SID_Ix_LEAD_Fx_RESERVED2 =	0x5;	# offset: reserved for future expansions
$SID_Ix_LEAD_S1F_BASE	=	0x054;#..0x059
$SID_Ix_LEAD_S2F_BASE	=	0x05a;#..0x05f

$SID_Ix_Vx_FLAGS1	=	0x00;	# [1:0] portamento (0), constant time glide (1), glissando (2), [2] GAE (gate always enabled), [3] AWA (ADSR workaround)
$SID_Ix_Vx_WAVEFORM	=	0x01;	# waveform and sync/ring, 2 flags reserved
$SID_Ix_Vx_AD		=	0x02;	# 2*4bit value for attack [7:4] and decay [3:0]
$SID_Ix_Vx_SR		=	0x03;	# 2*4bit value for sustain [7:4] and sustain [3:0]
$SID_Ix_Vx_PULSEWIDTH_L	=	0x04;	# 14 value, 12 bit used
$SID_Ix_Vx_PULSEWIDTH_H	=	0x05;
$SID_Ix_Vx_RESERVED1	=	0x06;
$SID_Ix_Vx_DELAY	=	0x07;	# 8bit
$SID_Ix_Vx_TRANSPOSE	=	0x08;	# 7bit
$SID_Ix_Vx_FINETUNE	=	0x09;	# 8bit
$SID_Ix_Vx_PITCHRANGE	=	0x0a;	# 7bit
$SID_Ix_Vx_PORTAMENTO	=	0x0b;	# 8bit
$SID_Ix_Vx_ARP_MODE	=	0x0c;	# [0] enable, [2:1] dir up/down/U&D/D&U, [3] random, [4] sorted, [5] hold, [6] sync, [7] CAC
$SID_Ix_Vx_ARP_SPEED_DIV =	0x0d;	# [7] oneshot, [5:0] own speed or derived from global clock (dividable)
$SID_Ix_Vx_ARP_GL_RNG	=	0x0e;	# [4:0] gatelength [7:5] arp octave range (seperate for each voice)
$SID_Ix_Vx_RESERVED2	=	0x0f;

$SID_Ix_S1V1_BASE	=	0x060;#..0x06f    SID1 Voice1
$SID_Ix_S1V2_BASE	=	0x070;#..0x07f    SID1 Voice2
$SID_Ix_S1V3_BASE	=	0x080;#..0x08f    SID1 Voice3
$SID_Ix_S2V1_BASE	=	0x090;#..0x09f    SID2 Voice1
$SID_Ix_S2V2_BASE	=	0x0a0;#..0x0af    SID2 Voice2
$SID_Ix_S2V3_BASE	=	0x0b0;#..0x0bf    SID2 Voice3

$SID_Ix_LFOx_MODE	=	0x00;	# [0] enable, [2] clksync, [3] oneshot, [7:4] waveform
$SID_Ix_LFOx_DEPTH	=	0x01;	# LFO depth (8 bit instead of 7 like MBSID V1!)
$SID_Ix_LFOx_RATE	=	0x02;	# LFO rate (8 bit instead of 7 like MBSID V1!)
$SID_Ix_LFOx_DELAY	=	0x03;	# LFO depth (8 bit instead of 7 like MBSID V1!)
$SID_Ix_LFOx_PHASE	=	0x04;	# LFO phase

$SID_Ix_LFO1_BASE	=	0x0c0;#..0x0c4    LFO1
$SID_Ix_LFO2_BASE	=	0x0c5;#..0x0c9    LFO2
$SID_Ix_LFO3_BASE	=	0x0ca;#..0x0ce    LFO3
$SID_Ix_LFO4_BASE	=	0x0cf;#..0x0d3    LFO4
$SID_Ix_LFO5_BASE	=	0x0d4;#..0x0d8    LFO5
$SID_Ix_LFO6_BASE	=	0x0d9;#..0x0dd    LFO6

	## free: 0xde and 0xdf

$SID_Ix_ENVx_MODE	=	0x00;	# ENV mode (BPM sync, loop point... ADSR..ADDSR..HADASR)
$SID_Ix_ENVx_DEPTH	=	0x01;	# 8bit value
$SID_Ix_ENVx_DELAY	=	0x02;	# 8bit value
$SID_Ix_ENVx_ATTACK1	=	0x03;	# 8bit value
$SID_Ix_ENVx_ATTLVL	=	0x04;	# 8bit value
$SID_Ix_ENVx_ATTACK2	=	0x05;	# 8bit value
$SID_Ix_ENVx_DECAY1	=	0x06;	# 8bit value
$SID_Ix_ENVx_DECLVL	=	0x07;	# 8bit value
$SID_Ix_ENVx_DECAY2	=	0x08;	# 8bit value
$SID_Ix_ENVx_SUSTAIN	=	0x09;	# 8bit value
$SID_Ix_ENVx_RELEASE1	=	0x0a;	# 8bit value
$SID_Ix_ENVx_RELLVL	=	0x0b;	# 8bit value
$SID_Ix_ENVx_RELEASE2	=	0x0c;	# 8bit value
$SID_Ix_ENVx_ATT_CURVE	=	0x0d;	# 8bit value
$SID_Ix_ENVx_DEC_CURVE	=	0x0e;	# 8bit value
$SID_Ix_ENVx_REL_CURVE	=	0x0f;	# 8bit value

$SID_Ix_ENV1_BASE	=	0x0e0;#..0x0ef    ENV1
$SID_Ix_ENV2_BASE	=	0x0f0;#..0x0ff    ENV2

$SID_Ix_MODx_SRC1	=	0x00;	# modulation source #1 - LFO, ENV, Analog In, Velocity, Modwheel, Aftertouch, PitchBend, Keynumber, Knob #1-5
$SID_Ix_MODx_SRC2	=	0x01;	# modulation source #2
$SID_Ix_MODx_OP		=	0x02;	# [7] invert target 1/L [6] invert target 2/R [5:4] reserved [3:0] modulation operation (SRC1 only, SRC2 only, SRC1+SRC2, SRC1-SRC2, SRC1*SRC2, XOR, OR, AND, Min, Max)
$SID_Ix_MODx_DEPTH	=	0x03;	# 8bit value (-128..+127)
$SID_Ix_MODx_TARG_L	=	0x04;	# modulation target SIDL [2:0] Pitch Voice 1/2/3, [5:3] PW Voice 1/2/3, [6] Filter, [7] Volume
$SID_Ix_MODx_TARG_R	=	0x05;	# modulation target SIDR [2:0] Pitch Voice 1/2/3, [5:3] PW Voice 1/2/3, [6] Filter, [7] Volume
$SID_Ix_MODx_TARG_X1	=	0x06;	# additional selectable target #1
$SID_Ix_MODx_TARG_X2	=	0x07;	# additional selectable target #2

$SID_Ix_MOD1_BASE	=	0x100;#..0x107   Modulator1
$SID_Ix_MOD2_BASE	=	0x108;#..0x10f   Modulator2
$SID_Ix_MOD3_BASE	=	0x110;#..0x117   Modulator3
$SID_Ix_MOD4_BASE	=	0x118;#..0x11f   Modulator4
$SID_Ix_MOD5_BASE	=	0x120;#..0x127   Modulator5
$SID_Ix_MOD6_BASE	=	0x128;#..0x12f   Modulator6
$SID_Ix_MOD7_BASE	=	0x130;#..0x137   Modulator7
$SID_Ix_MOD8_BASE	=	0x138;#..0x13f   Modulator8

$SID_Ix_TRG_BASE	=	0x140;#..0x16f
$SID_Ix_TRG_NOn_BASE	=	0x140;#..0x142	Note On
$SID_Ix_TRG_NOf_BASE	=	0x143;#..0x145	Note Off
$SID_Ix_TRG_E1S_BASE	=	0x146;#..0x148	ENV1 Sustain Phase
$SID_Ix_TRG_E2S_BASE	=	0x149;#..0x14b	ENV2 Sustain Phase
$SID_Ix_TRG_L1P_BASE	=	0x14c;#..0x14e	LFO1 Period
$SID_Ix_TRG_L2P_BASE	=	0x14f;#..0x151	LFO2 Period
$SID_Ix_TRG_L3P_BASE	=	0x152;#..0x154	LFO3 Period
$SID_Ix_TRG_L4P_BASE	=	0x155;#..0x157	LFO4 Period
$SID_Ix_TRG_L5P_BASE	=	0x158;#..0x15a	LFO5 Period
$SID_Ix_TRG_L6P_BASE	=	0x15b;#..0x15d	LFO6 Period
$SID_Ix_TRG_Clk_BASE	=	0x15e;#..0x160	Clock
$SID_Ix_TRG_Cl4_BASE	=	0x161;#..0x163	Clock/4
$SID_Ix_TRG_C16_BASE	=	0x164;#..0x166	Clock/16
$SID_Ix_TRG_MSt_BASE	=	0x167;#..0x169	MIDI Clock Start
	## free: 0x6a-0x6b

$SID_Ix_WTx_SPEED	=	0x00;	# [5:0] clock divider, [6] channel target SIDL, [7] channel target SIDR
$SID_Ix_WTx_ASSGN	=	0x01;	# parameter assignment
$SID_Ix_WTx_BEGIN	=	0x02;	# [6:0] start position in wavetable, [7] position controlled by MOD
$SID_Ix_WTx_END		=	0x03;	# [6:0] end position in wavetable
$SID_Ix_WTx_LOOP	=	0x04;	# [6:0] loop position in wavetable, [7] one shot
$SID_Ix_WT1_BASE	=	0x16c;#..0x170
$SID_Ix_WT2_BASE	=	0x171;#..0x175
$SID_Ix_WT3_BASE	=	0x176;#..0x17a
$SID_Ix_WT4_BASE	=	0x17b;#..0x17f

$SID_Ix_WAVETABLE	=	0x180;#..0x1ff


#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug"  => \$debug,
   "bank=s" => \$bank,
   );

if( scalar(@ARGV) != 2 )
{
   die "SYNTAX:  v1_to_v2.pl <source-syx-file> <target-syx-file> [<-bank n>] [<-debug>]\n" .
       "EXAMPLE: v1_to_v2.pl -bank 0 midibox_sid_preset_patches_8580.syx mbsid_v2_preset_patches.syx\n";
}

if( $bank < 0 || $bank > 7 ) {
  die "ERROR: bank number must be in range 0..7 (A..H)\n";
}

$in_file = $ARGV[0];
$out_file = $ARGV[1];

#########################################################################################
# Read V1 Syx File
#########################################################################################

open(FILE_IN, "<${in_file}") || die "ERROR: Cannot open ${in_file} for reading!\n";
binmode(FILE_IN);
my $len = 0;
my $dump = 0;
if( ($len=sysread(FILE_IN, $dump, $max_file_len)) <= 0 )
{
   print "WARNING: ${in_file} is empty - skipping!\n";
}
elsif( $len > $max_file_len )
{
   print "WARNING: ${in_file} bigger than ${max_file_len} (definitely too large for MIOS - skipping!\n";
}
else
{
   print "Loading ${in_file} (${len} bytes)\n";
}
close(FILE_IN);

my $state = "WAIT_HEADER";
my $invalid_found = 0;
my $device_id_found = -1;
my @syx_header = (0xf0, 0x00, 0x00, 0x7e, 0x46);
my $syx_header_pos = 0;
my $cmd = -1;
my $cmd_pos = -1;
my $checksum;
my $patch;
my $expected_patches = 0;
my @v1_patch;
my @v2_syx = ();
my $patches_found = 0;

for($i=0;$i<$len;++$i) {
  my $byte = ord(substr($dump, $i, 1));

  if( $debug ) {
    printf "[%s] %02X\n", $state, $byte;
  }

  if( $byte >= 0xf8 ) {
    # ignore realtime events

  } elsif( $state ne "WAIT_HEADER" && ($byte & 0x80) ) {
    # reset state machine on any status byte (also F7...)
    $state = "WAIT_HEADER";

  } elsif( $state eq "WAIT_HEADER" ) {
    my $expected = $syx_header[$syx_header_pos];

    if( $byte != $expected ) {
      $syx_header_pos = 0;
      $invalid_found = 1;
    }

    if( $byte == $expected ) {
      ++$syx_header_pos;
      if( $syx_header_pos == scalar(@syx_header) ) {
        $syx_header_pos = 0;
        $state = "WAIT_DEVICE_ID";
      }
    }

  } elsif( $state eq "WAIT_DEVICE_ID" ) {
    if( $device_id_found == -1 ) {
      $device_id_found = $byte;
    }

    $state = "WAIT_CMD";

  } elsif( $state eq "WAIT_CMD" ) {
    $cmd = $byte;
    $cmd_pos = 0;

    if( ($cmd & 0x0f) == 0x02 ) {
      $expected_patches = 1;
      $state = "WRITE_CMD";
    } elsif( ($cmd & 0x0f) == 0x04 ) {
      $expected_patches = 128;
      @v1_patch = ();
      $checksum = 0;
      $patch=0;
      $state = "PATCH";
    } elsif( ($cmd & 0x0f) == 0x0b ) {
      $state = "WAIT_HEADER"; # ignore BS selection
    } else {
      printf "WARNING: skipping command %02X block!\n", $cmd;
      $state = "WAIT_HEADER";
    }

  } elsif( $state eq "WRITE_CMD" ) {
    if   ( $cmd_pos == 0 ) {
      $patch = $byte;
      $state = "PATCH";
      @v1_patch = ();
      $checksum = 0;
    } else {
      die "FATAL: coding error in state ${state}\n";
    }
    ++$cmd_pos;

  } elsif( $state eq "PATCH" ) {
    $checksum = ($checksum + $byte) & 0x7f;

    push @v1_patch, $byte;

    if( scalar(@v1_patch) == 256 ) {
      if( --$expected_patches ) {
        ++$patches_found;
        convert_patch($bank, $patch, @v1_patch);
        @v1_patch = ();
      } else {
        $state = "CHECKSUM";
      }
    }

  } elsif( $state eq "CHECKSUM" ) {
    $checksum = ($checksum + $byte) & 0x7f;

    if( $checksum != 0 ) { # the sum of all checksums + the checksum itself must be zero
      printf "ERROR: checksum doesn't match - patch skipped!\n";
    } else {
      ++$patches_found;
      convert_patch($bank, $patch, @v1_patch);
    }
    $state = "F7";

  } elsif( $state eq "F7" ) {
    if( $byte != 0xf7 ) {
      die "ERROR: missing F7 - corrupted patch dump!\n";
    }

    $state = "WAIT_HEADER";

  } else {
    die "FATAL: unknown state: ${state}\n";
  }
}

if( !$patches_found ) {
  print "No patches found!\n";
  exit;
} else {
  print "${patches_found} patches found.\n";
}

#########################################################################################
# Write out MBSID V2 patches
#########################################################################################
open(OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";
binmode(OUT);

foreach $byte (@v2_syx) {
  printf OUT "%c", $byte;
}
close(OUT);

print "${out_file} written.\n";
exit;




#########################################################################################
#########################################################################################
## Subroutines
#########################################################################################
#########################################################################################

sub convert_patch
{
  my ($bank, $patch, @v1_patch) = @_;

  ####################################
  # print name
  ####################################
  my $name = "";
  my $i;
  for($i=0; $i<16; ++$i) {
    if( $v1_patch[$i] < 0x20 ) {
      $v1_patch[$i] = 0x20;
    }
    $name .= sprintf("%c", $v1_patch[$i]);
  }
  printf "Patch %c%03d: %s\n", ord('A')+$bank, $patch+1, $name;

  ####################################
  # copy default patch
  ####################################
  my @v2_patch = ();
  my $byte;
  foreach $byte (@v2_default_patch) {
    push @v2_patch, $byte;
  }

  ####################################
  # transfer v1 parameters to v2 patch
  ####################################

  # patch name
  for($i=0; $i<16; ++$i) {
    $v2_patch[$i] = $v1_patch[$i];
  }

  # volume, legato, WTO, suskey, oscillator sync
  $v2_patch[$SID_Ix_LEAD_VOLUME] = $v1_patch[0x10];
  $v2_patch[$SID_Ix_LEAD_FLAGS1] |= ($v1_patch[0x11] & 0x01) ? 0x01 : 0x00;
  $v2_patch[$SID_Ix_LEAD_FLAGS1] |= ($v1_patch[0x11] & 0x02) ? 0x02 : 0x00;
  $v2_patch[$SID_Ix_LEAD_FLAGS1] |= ($v1_patch[0x12] & 0x01) ? 0x04 : 0x00;
  $v2_patch[$SID_Ix_LEAD_OSC_PHASE] = $v1_patch[0x14] ? 0x01 : 0x00;

  # ENV curves
  if( $v1_patch[0x15] & 0x01 ) { $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_ATT_CURVE] = $v1_patch[0x1e]<<1; }
  if( $v1_patch[0x15] & 0x02 ) { $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_DEC_CURVE] = $v1_patch[0x1e]<<1; }
  if( $v1_patch[0x15] & 0x04 ) { $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_REL_CURVE] = $v1_patch[0x1e]<<1; }
  if( $v1_patch[0x15] & 0x10 ) { $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_ATT_CURVE] = $v1_patch[0x1f]<<1; }
  if( $v1_patch[0x15] & 0x20 ) { $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_DEC_CURVE] = $v1_patch[0x1f]<<1; }
  if( $v1_patch[0x15] & 0x40 ) { $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_REL_CURVE] = $v1_patch[0x1f]<<1; }

  # Filter
  $v2_patch[$SID_Ix_LEAD_S1F_BASE+$SID_Ix_LEAD_Fx_CHN_MODE] = ($v1_patch[0x18]&0x0f) | (($v1_patch[0x19]&0x0f)<<4);
  $v2_patch[$SID_Ix_LEAD_S1F_BASE+$SID_Ix_LEAD_Fx_CUTOFF_L] = ($v1_patch[0x1a]&0x07)<<5;
  $v2_patch[$SID_Ix_LEAD_S1F_BASE+$SID_Ix_LEAD_Fx_CUTOFF_H] = ($v1_patch[0x1a]&0x78)>>3;
  $v2_patch[$SID_Ix_LEAD_S1F_BASE+$SID_Ix_LEAD_Fx_RESONANCE] = ($v1_patch[0x1b]<<1);

  # duplicate Filter part
  for($i=0; $i<($SID_Ix_LEAD_S2F_BASE-$SID_Ix_LEAD_S1F_BASE); ++$i) {
    $v2_patch[$SID_Ix_LEAD_S2F_BASE + $i] = $v2_patch[$SID_Ix_LEAD_S1F_BASE + $i];
  }

  # Oscillators
  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_WAVEFORM] = $v1_patch[0x24];
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_WAVEFORM] = $v1_patch[0x34];
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_WAVEFORM] = $v1_patch[0x44];

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_AD] = (($v1_patch[0x28]&0x78)<<1) | (($v1_patch[0x29]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_AD] = (($v1_patch[0x38]&0x78)<<1) | (($v1_patch[0x39]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_AD] = (($v1_patch[0x48]&0x78)<<1) | (($v1_patch[0x49]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_SR] = (($v1_patch[0x2a]&0x78)<<1) | (($v1_patch[0x2b]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_SR] = (($v1_patch[0x3a]&0x78)<<1) | (($v1_patch[0x3b]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_SR] = (($v1_patch[0x4a]&0x78)<<1) | (($v1_patch[0x4b]&0x78)>>3);

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_PULSEWIDTH_L] = (($v1_patch[0x25]&0x03)<<5);
  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_PULSEWIDTH_H] = (($v1_patch[0x25]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_PULSEWIDTH_L] = (($v1_patch[0x35]&0x03)<<5);
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_PULSEWIDTH_H] = (($v1_patch[0x35]&0x78)>>3);
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_PULSEWIDTH_L] = (($v1_patch[0x45]&0x03)<<5);
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_PULSEWIDTH_H] = (($v1_patch[0x45]&0x78)>>3);

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_DELAY] = $v1_patch[0x27]<<1;
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_DELAY] = $v1_patch[0x37]<<1;
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_DELAY] = $v1_patch[0x47]<<1;

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_TRANSPOSE] = $v1_patch[0x20];
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_TRANSPOSE] = $v1_patch[0x30];
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_TRANSPOSE] = $v1_patch[0x40];

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_FINETUNE] = $v1_patch[0x21] + 0x40;
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_FINETUNE] = $v1_patch[0x31] + 0x40;
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_FINETUNE] = $v1_patch[0x41] + 0x40;

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_PITCHRANGE] = $v1_patch[0x22];
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_PITCHRANGE] = $v1_patch[0x32];
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_PITCHRANGE] = $v1_patch[0x42];

  $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_PORTAMENTO] = $v1_patch[0x23]<<1;
  $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_PORTAMENTO] = $v1_patch[0x33]<<1;
  $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_PORTAMENTO] = $v1_patch[0x43]<<1;

  if( $v1_patch[0x26] ) {
    $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_ARP_MODE] = 0x81; # enable, CAC
    $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_ARP_SPEED_DIV] = 0x0f;
    $v2_patch[$SID_Ix_S1V1_BASE+$SID_Ix_Vx_ARP_GL_RNG] = 0x1f;
  }

  if( $v1_patch[0x36] ) {
    $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_ARP_MODE] = 0x81; # enable, CAC
    $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_ARP_SPEED_DIV] = 0x0f;
    $v2_patch[$SID_Ix_S1V2_BASE+$SID_Ix_Vx_ARP_GL_RNG] = 0x1f;
  }

  if( $v1_patch[0x46] ) {
    $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_ARP_MODE] = 0x81; # enable, CAC
    $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_ARP_SPEED_DIV] = 0x0f;
    $v2_patch[$SID_Ix_S1V3_BASE+$SID_Ix_Vx_ARP_GL_RNG] = 0x1f;
  }

  # duplicate oscillator part
  for($i=0; $i<($SID_Ix_S2V1_BASE-$SID_Ix_S1V1_BASE); ++$i) {
    $v2_patch[$SID_Ix_S2V1_BASE + $i] = $v2_patch[$SID_Ix_S1V1_BASE + $i];
  }

  # LFOs
  $v2_patch[$SID_Ix_LFO1_BASE+$SID_Ix_LFOx_MODE] = $v1_patch[0x60] & 0xf1;
  $v2_patch[$SID_Ix_LFO2_BASE+$SID_Ix_LFOx_MODE] = $v1_patch[0x63] & 0xf1;
  $v2_patch[$SID_Ix_LFO3_BASE+$SID_Ix_LFOx_MODE] = $v1_patch[0x66] & 0xf1;
  $v2_patch[$SID_Ix_LFO4_BASE+$SID_Ix_LFOx_MODE] = $v1_patch[0x69] & 0xf1;
  $v2_patch[$SID_Ix_LFO5_BASE+$SID_Ix_LFOx_MODE] = $v1_patch[0x6c] & 0xf1;
  $v2_patch[$SID_Ix_LFO6_BASE+$SID_Ix_LFOx_MODE] = $v1_patch[0x6f] & 0xf1;

  $v2_patch[$SID_Ix_LFO1_BASE+$SID_Ix_LFOx_RATE] = $v1_patch[0x61]<<1;
  $v2_patch[$SID_Ix_LFO2_BASE+$SID_Ix_LFOx_RATE] = $v1_patch[0x64]<<1;
  $v2_patch[$SID_Ix_LFO3_BASE+$SID_Ix_LFOx_RATE] = $v1_patch[0x67]<<1;
  $v2_patch[$SID_Ix_LFO4_BASE+$SID_Ix_LFOx_RATE] = $v1_patch[0x6a]<<1;
  $v2_patch[$SID_Ix_LFO5_BASE+$SID_Ix_LFOx_RATE] = $v1_patch[0x6d]<<1;
  $v2_patch[$SID_Ix_LFO6_BASE+$SID_Ix_LFOx_RATE] = $v1_patch[0x70]<<1;

  $v2_patch[$SID_Ix_LFO1_BASE+$SID_Ix_LFOx_DEPTH] = $v1_patch[0x62]<<1;
  $v2_patch[$SID_Ix_LFO2_BASE+$SID_Ix_LFOx_DEPTH] = $v1_patch[0x65]<<1;
  $v2_patch[$SID_Ix_LFO3_BASE+$SID_Ix_LFOx_DEPTH] = $v1_patch[0x68]<<1;
  $v2_patch[$SID_Ix_LFO4_BASE+$SID_Ix_LFOx_DEPTH] = $v1_patch[0x6b]<<1;
  $v2_patch[$SID_Ix_LFO5_BASE+$SID_Ix_LFOx_DEPTH] = $v1_patch[0x6e]<<1;
  $v2_patch[$SID_Ix_LFO6_BASE+$SID_Ix_LFOx_DEPTH] = $v1_patch[0x71]<<1;

  # ENVs
  $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_DEPTH] = $v1_patch[0x72]<<1;
  $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_ATTACK1] = $v1_patch[0x73]<<1;
  $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_DECAY2] = $v1_patch[0x74]<<1;
  $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_SUSTAIN] = $v1_patch[0x75]<<1;
  $v2_patch[$SID_Ix_ENV1_BASE+$SID_Ix_ENVx_RELEASE1] = $v1_patch[0x76]<<1;

  $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_DEPTH] = $v1_patch[0x77]<<1;
  $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_ATTACK1] = $v1_patch[0x78]<<1;
  $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_DECAY2] = $v1_patch[0x79]<<1;
  $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_SUSTAIN] = $v1_patch[0x7a]<<1;
  $v2_patch[$SID_Ix_ENV2_BASE+$SID_Ix_ENVx_RELEASE1] = $v1_patch[0x7b]<<1;

  # MOD assignments
  $v2_patch[$SID_Ix_MOD1_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2e] & 0x01) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3e] & 0x01) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4e] & 0x01) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2e] & 0x10) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3e] & 0x10) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4e] & 0x10) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1d] & 0x01) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD1_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD1_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD2_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2e] & 0x02) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3e] & 0x02) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4e] & 0x02) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2e] & 0x20) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3e] & 0x20) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4e] & 0x20) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1d] & 0x02) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD2_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD2_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD3_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2c] & 0x01) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3c] & 0x01) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4c] & 0x01) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2d] & 0x01) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3d] & 0x01) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4d] & 0x01) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1c] & 0x01) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD3_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD3_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD4_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2c] & 0x02) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3c] & 0x02) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4c] & 0x02) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2d] & 0x02) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3d] & 0x02) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4d] & 0x02) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1c] & 0x02) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD4_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD4_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD5_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2c] & 0x04) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3c] & 0x04) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4c] & 0x04) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2d] & 0x04) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3d] & 0x04) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4d] & 0x04) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1c] & 0x04) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD5_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD5_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD6_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2c] & 0x08) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3c] & 0x08) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4c] & 0x08) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2d] & 0x08) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3d] & 0x08) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4d] & 0x08) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1c] & 0x08) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD6_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD6_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD7_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2c] & 0x10) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3c] & 0x10) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4c] & 0x10) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2d] & 0x10) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3d] & 0x10) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4d] & 0x10) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1c] & 0x10) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD7_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD7_BASE+$SID_Ix_MODx_TARG_L];

  $v2_patch[$SID_Ix_MOD8_BASE+$SID_Ix_MODx_TARG_L] = 
    (($v1_patch[0x2c] & 0x20) ? 0x01 : 0x00) | # V1 Pitch
    (($v1_patch[0x3c] & 0x20) ? 0x02 : 0x00) | # V2 Pitch
    (($v1_patch[0x4c] & 0x20) ? 0x04 : 0x00) | # V3 Pitch
    (($v1_patch[0x2d] & 0x20) ? 0x08 : 0x00) | # V1 PW
    (($v1_patch[0x3d] & 0x20) ? 0x10 : 0x00) | # V2 PW
    (($v1_patch[0x4d] & 0x20) ? 0x20 : 0x00) | # V3 PW
    (($v1_patch[0x1c] & 0x20) ? 0x40 : 0x00);  # Filter
  $v2_patch[$SID_Ix_MOD8_BASE+$SID_Ix_MODx_TARG_R] = $v2_patch[$SID_Ix_MOD8_BASE+$SID_Ix_MODx_TARG_L];


  # Wavetable
  $v2_patch[$SID_Ix_WT1_BASE+$SID_Ix_WTx_ASSGN] = conv_wt($v1_patch[0x5a]);
  $v2_patch[$SID_Ix_WT2_BASE+$SID_Ix_WTx_ASSGN] = conv_wt($v1_patch[0x5b]);
  $v2_patch[$SID_Ix_WT3_BASE+$SID_Ix_WTx_ASSGN] = conv_wt($v1_patch[0x5c]);

  my $loop = -1;
  my $stop = -1;
  for($i=0; $i<32; ++$i) {
    if( $v1_patch[0x80+$i*4] & 0x10 ) {
      $v2_patch[$SID_Ix_WAVETABLE+0*0x20+$i] = $v1_patch[0x80 + $i*4+1] | 0x80;
    } else {
      $v2_patch[$SID_Ix_WAVETABLE+0*0x20+$i] = ($v1_patch[0x80 + $i*4+1]-0x40) & 0x7f;
    }

    if( $v1_patch[0x80+$i*4] & 0x20 ) {
      $v2_patch[$SID_Ix_WAVETABLE+1*0x20+$i] = $v1_patch[0x80 + $i*4+2] | 0x80;
    } else {
      $v2_patch[$SID_Ix_WAVETABLE+1*0x20+$i] = ($v1_patch[0x80 + $i*4+2]-0x40) & 0x7f;
    }

    if( $v1_patch[0x80+$i*4] & 0x40 ) {
      $v2_patch[$SID_Ix_WAVETABLE+2*0x20+$i] = $v1_patch[0x80 + $i*4+3] | 0x80;
    } else {
      $v2_patch[$SID_Ix_WAVETABLE+2*0x20+$i] = ($v1_patch[0x80 + $i*4+3]-0x40) & 0x7f;
    }

    my $mode = $v1_patch[0x80+$i*4+0] & 0x03;
    if( $mode == 0 ) { # Inc
    } elsif( $mode == 1 ) { # Play
    } elsif( $mode == 2 ) { # Goto
      if( $stop == -1 ) {
        $stop = ($i-1) & 0x1f;
        $loop = ($v1_patch[0x80+$i*4+1]) & 0x1f;
      }
    } elsif( $mode == 3 ) { # Stop
      if( $loop == -1 ) {
        $stop = ($i-1) & 0x1f;
      }
    }
  }

  if( $loop == -1 && $stop == -1 && ($v1_patch[0x80+0x1f*4+0] & 0x03) != 3 ) { # assume loop if no stop at the end
    $loop = 0x00;
  }

  if( $stop != -1 ) {
    $v2_patch[$SID_Ix_WT1_BASE+$SID_Ix_WTx_END] = 0x00 + $stop;
    $v2_patch[$SID_Ix_WT2_BASE+$SID_Ix_WTx_END] = 0x20 + $stop;
    $v2_patch[$SID_Ix_WT3_BASE+$SID_Ix_WTx_END] = 0x40 + $stop;
    $v2_patch[$SID_Ix_WT4_BASE+$SID_Ix_WTx_END] = 0x60 + $stop;
  }

  if( $loop != -1 ) {
    $v2_patch[$SID_Ix_WT1_BASE+$SID_Ix_WTx_LOOP] = 0x00 + $loop;
    $v2_patch[$SID_Ix_WT2_BASE+$SID_Ix_WTx_LOOP] = 0x20 + $loop;
    $v2_patch[$SID_Ix_WT3_BASE+$SID_Ix_WTx_LOOP] = 0x40 + $loop;
    $v2_patch[$SID_Ix_WT4_BASE+$SID_Ix_WTx_LOOP] = 0x60 + $loop;
  }

  my $speed = int((127-$v1_patch[0x59]) / 3) - 3;
  if( $speed > 63 ) { $speed = 63; }
  if( $speed < 1 ) { $speed = 1; } # mostly 0 doesn't work properly
  $v2_patch[$SID_Ix_WT1_BASE+$SID_Ix_WTx_SPEED] = 0xc0 | $speed;
  $v2_patch[$SID_Ix_WT2_BASE+$SID_Ix_WTx_SPEED] = 0xc0 | $speed;
  $v2_patch[$SID_Ix_WT3_BASE+$SID_Ix_WTx_SPEED] = 0xc0 | $speed;
  $v2_patch[$SID_Ix_WT4_BASE+$SID_Ix_WTx_SPEED] = 0xc0 | $speed;

  ####################################
  # add new patch to syx file
  ####################################
  push @v2_syx, 0xf0;
  push @v2_syx, 0x00;
  push @v2_syx, 0x00;
  push @v2_syx, 0x7e;
  push @v2_syx, 0x4b;
  push @v2_syx, 0x00;

  push @v2_syx, 0x02;   # Patch Write
  push @v2_syx, 0x00;   # type 0x00: patch
  push @v2_syx, $bank;  # patch bank
  push @v2_syx, $patch; # patch number

  my $checksum = 0;
  for($i=0; $i<512; ++$i) {
    my $byte = $v2_patch[$i];
    my $bl = $byte & 0x0f;
    my $bh = ($byte >> 4) & 0x0f;

    push @v2_syx, $bl;
    $checksum = ($checksum + $bl) & 0x7f;
    push @v2_syx, $bh;
    $checksum = ($checksum + $bh) & 0x7f;
  }
  push @v2_syx, (0x80 - $checksum) & 0x7f; # checksum
  push @v2_syx, 0xf7;
}


#########################################################################################
sub conv_wt
{
  my ($par) = @_;
  my $ret = 0;

  if     ( $par >= 8 && $par <= 11 ) {
    $ret = ($par&0x3) + 252;
  } elsif(  $par >= 16 && $par <= 16 ) {
    $ret = ($par&0x3) + 36;
  } elsif(  $par >= 32 && $par <= 35 ) {
    $ret = ($par&0x3) + 32;
  } elsif( $par == 46 ) {
    $ret = 4;
  }

  return $ret;
}

