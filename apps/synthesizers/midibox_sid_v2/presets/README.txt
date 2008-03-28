$Id$

This package contains the preset patches for MIDIbox SID V2:

   o v2_vintage_bank.syx:
     Based on "midibox_sid_presets_8580.syx" of the MBSID V1 preset library,
     converted with the patch converter which can be found in the tools/
     directory. The results have been manually improved by TK to take new
     V2 features into account.


Currently there is no special SysEx upload tool available for MIDIbox
SID V2. Please use a SysEx tool like MIDI-Ox, and configure a delay of
750 mS between SysEx strings (MIDI-Ox: SysEx->Configure, set "Delay after
F7 to 750 mS)


Following preset patches are available:
--------------------------------------------------------------------------------------------
001 Internal Patch      033 Drum Kit 1          065 ARPSEQ One B        097 Monty Lead3
002 Techno PWM          034 Drum Kit 2          066 ARPSEQ One C        098 Monty Lead4
003 Techno Saw          035 Drum Kit 3          067 SEQ TranceBass      099 Bassline Demo1
004 Techno 5th          036 Drum Kit 4          068 SEQ Vintage A       100 Bassline Demo2
005 Cool Brass          037 Some Triggs         069 SEQ Vintage B       101 Vib Synth
006 Simple Saw          038 More Triggs         070 SEQ Vintage C       102 Whats That?
007 Simple Pulse        039 RingModMod          071 ARPSEQ Two A        103 Sample&Hold 1
008 Pulse w/o Body      040 RampUp              072 ARPSEQ Two B        104 WT pseudo S&H 2
009 Popcorn             041 Math Game           073 ARPSEQ Two C        105 Curve Filter D
010 WT Flute            042 WT Synth Drum 11    074 ARPSEQ Three        106 Curve total
011 Synth Plug          043 WT Synth Drum 12    075 ARPSEQ Four         107 El.Piano
012 WT Synth            044 Turntable           076 SEQ Mighty Bass     108 Zak Bass
013 WT HardcoreSynth    045 Driving a Car       077 Analog Dream1       109 Zak Bass 2
014 Sync Sound          046 Ufo Reverse         078 Analog Dream2       110 Kik A
015 Sync Pad            047 WT Falling          079 Analog Dream3       111 Cymbal A
016 Filt. Poly Pad      048 WT Helicopter       080 Analog Dream4       112 Cymbal B
017 Filt. Mono Pad      049 WT Neutron          081 Analog Dream5       113 Hat A
018 Arpeggio            050 WT Filtered Seq.    082 PWM Bass1           114 Snare A
019 Arpeggio 2          051 Alien Groove        083 PWM Bass2           115 Accomp A
020 WT Arp Fun          052 Nice Lead           084 PWM Bass3           116 Accomp B
021 Ringmodulation      053 NT Bass             085 PWM Bass4           117 Lead Stacco1
022 Filtered B. 6581    054 RingKabinett        086 Seq Bass1           118 Lead Stacco2
023 Filtered B. 8580    055 Random Fun          087 Seq Bass2           119 Lead Melodia
024 Filtered Bass 2     056 Don't cry baby!     088 Seq Bass3           120 Stacco4
025 C64 Bass            057 Tweak the LFOs      089 Seq Bass4           121 String Ponte 1
026 Autobahn            058 Random Sync         090 Seq Bass5           122 String Ponte 2
027 Bassdrum            059 A stormy Day        091 Seq Bass6           123 Crescendo
028 Bassdrum2           060 6 Octave Plug       092 Monty Bass1         124 Crazy Lead
029 Cymbal              061 Poly Saw            093 Monty Bass2         125 Accomp1
030 Klick               062 Step by Step        094 Monty Bass3         126 Casio Drums
031 Metal               063 Slow Intro          095 Monty Lead1         127 Classic Zelda
032 Deep Bass 9         064 ARPSEQ One A        096 Monty Lead2         128 Neo Zelda
--------------------------------------------------------------------------------------------

Patch 001-107 made by Thorsten Klose (2002-2004)
Patch 108-125 made by Matteo Bosi (2003-2004)
Patch 126-128 made by David Panseri (2004)

--------------------------------------------------------------------------------------------

ChangeLog:
~~~~~~~~~~

2008-03-27:
   o replaced Placeholders A035 and A036 by new Drum Kits
   o replaced Placeholders A037..A044 by new trigger and modulation matrix examples
   o Patch A089-A91 (not working "Miami1"..3) replaced by "SEQ Bass 4"..6
   o Patch A103 and A104 (not working "WT Pseudo S&H") replaced by "Sample&Hold 1" and 2
   o Patch A107 (not working "WT Flip LFO") replaced by "El.Piano?"

2007-09-29:
   o Patch A033 and A034 replaced by Drum Kits + Sequences
   o Patch A035..A044 (expired WT drum patches) replaced by placeholders


2007-07-22:
   o Patch A099 and A100 replaced by Bassline demo sequences
     Recommented Ensemble configuration (Ensemble->INS)
       - Ins1 and Ins2 assigned to same MIDI channel
       - Ins1 splitpoints SpL: c-2, SpU: B-3
       - Ins2 splitpoints SpL: C-4, SpU: G-8
     Note that the second bassline can only be played if
     a second MBHP_SID module is connected to the core.

     Optionally within Ensemble->Clk the MIDI Slave function 
     can be activated.

2007-06-30:
   o released converted bank A for MIDIbox SID V2
	
2006-08-20:
   o new Patches C038-C048 made by Sebastian (aka Rio)
   o new Patches C049-C055 made by TK (they demonstrate the
     possibilities of the V1.7303 sound engine)
   o A018 (Arpeggio): changed Arp speed from 110 to 70
   o A019 (Arpeggio2): changed Arp speed from 110 to 70/90

2006-02-27:
   o new Patches C006-C037 made by Sebastian (aka Rio)

2005-11-09:
   o A018 (Arpeggio): changed Arp speed from 99 to 110
   o A052 (Nice Lead): changed Portamento speed from 66 to 30
   o A067 (SEQ TranceBass): optimized Attack/Decay/Curve of ENV1,
                            activated HP filter in addition
   o A074 (ARPSEQ Three): activated oscillator phase synchronization
   o A076 (SEQ Mighty Bass): changed ENV1 depth from 14 to 33
   o A078 (Analog Dream 2): increased Release of VCA ENVs
   o A080 (Analog Dream 4): increased Release of VCA ENVs
   o A089-A091 (Miami *): set volume to 80
   o A093/A094 (Monty Bass *): increased CutOff
   o A105 (Curve Filter D): increased CutOff
   o added Patch C006-C011 made by Sebastian (aka Rio)
