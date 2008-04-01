/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
package org.midibox.sidedit;
import java.util.Vector;

public class SIDSysexInfo {
	
	// Patch/Ensemble Request commands
	public static String hardPatchRequestSysex = "F000007E4B<device>0100<bank><patch>F7";	// Request a dump of <patch> in <bank>
	public static String editPatchRequestSysex = "F000007E4B<device>01080000F7";	// Request the current patch edit buffer (direct read from RAM)
	public static String hardEnsembleRequestSysex = "F000007E4B<device>017000<ensemble>F7";	// Request a dump of <ensemble>
	public static String editEnsembleRequestSysex = "F000007E4B<device>017800<ensemble>F7";	// Request the current ensemble edit buffer (direct read from RAM) The locations of unavailable SID slaves will be read from EEPROM/BankStick, therefore the "default" <ensemble> number has to be specified as well.
	
	//  Patch/Ensemble Dump commands		   
	public static String hardPatchDumpSysex = "F000007E4B<device>0200<bank><patch><data><checksum>F7";	// Write a dump to <patch> in <bank>. Checksum is 2s complement over the 1024 bytes dump. The actual patch size is only 512 bytes - the 8bit values are divided into low- and high-nibble (low-nibble is sent first), therefore 1024 bytes have to be sent
	public static String editPatchDumpSysex = "F000007E4B<device>02080000<data><checksum>F7";	// Write a patch dump into edit buffer (direct write into RAM)
	public static String hardEsembleDumpSysex = "F000007E4B<device>027000<ensemble><data><checksum>F7";	// Write a dump to <ensemble>. Checksum is 2s complement over the 512 bytes dump. The actual ensemble size is only 256 bytes - the 8bit values are divided into low- and high-nibble (low-nibble is sent first), therefore 512 bytes have to be sent
	public static String editEsembleDumpSysex = "F000007E4B<device>02780000<data><checksum>F7";	// Write an ensemble dump into edit buffer (direct write into RAM)
	
	//  Bank Request commands
	public static String bankRequestSysex = "F000007E4B<device>0300<bank>F7";	// Request a dump of the whole patch <bank> (128 patches). Only 64 patches are sent when a 32k BankStick is accessed
	public static String ensembleBankRequestSysex = "F000007E4B<device>037000F7";	// Request a dump of all ensembles (128 ensembles)
	        
	// Parameter write
	public static String editPatchParameterSysex = "F000007E4B<device>0600<address><value>F7";	// Direct Write of parameter into patch buffer (<AH> = 0..3, <AL> = 0..7F). Patch address: (<AH> << 7) | <AL>
	public static String editEsembleParameterSysex = "F000007E4B<device>0670<address><value>F7";// Direct Write of parameter into ensemble buffer (<AH> = 0..1, <AL> = 0..7F). Ensemble address: (<AH> << 7) | <AL>

	// Ping
	public static String pingSysex = "F000007E4B<device>0FF7";	// Ping (just sends back the same SysEx string)
	
	// Feedback from core
	public static String acknowledgedSysex = "F000007E4B<device>0FF7";	// Acknowledged
	public static String error1Sysex = "F000007E4B<device>0E01F7";	// received less bytes then expected
	public static String error2Sysex = "F000007E4B<device>0E03F7";	// wrong checksum
	public static String error3Sysex = "F000007E4B<device>0E0AF7";	// bankstick or patch/drumset/ensemble not available
	public static String error4Sysex = "F000007E4B<device>0E0BF7";	// parameter not available
	public static String error5Sysex = "F000007E4B<device>0E10F7";	// RAM access not supported
	public static String error6Sysex = "F000007E4B<device>0E11F7";	// BankStick too small
	
	public static String[] leadParams = {"===None===","Volume","Osc Phase","Osc Detune","Filter Cutoff","Filter Res.","Filter Chan.","Filter Mode","Knob#1 Value","Knob#2 Value","Knob#3 Value","Knob#4 Value","Knob#5 Value","Knob#V Value","Knob#P Value","Knob#A Value","Ext1 Value","Ext2 Value","Ext3 Value","Ext4 Value",
		 "Ext5 Value","Ext6 Value","Ext7 Value","Ext8 Value","Ext1 Switch","Ext2 Switch","Ext3 Switch","Ext4 Switch","Ext5 Switch","Ext6 Switch","Ext7 Switch","Ext8 Switch","V123 Waveform","V1 Waveform","V2 Waveform","V3 Waveform","V123 Transpose","V1 Transpose","V2 Transpose","V3 Transpose",
		 "V123 Finetune","V1 Finetune","V2 Finetune","V3 Finetune","V123 Portamento","V1 Portamento","V2 Portamento","V3 Portamento","V123 Pulsewidth","V1 Pulsewidth","V2 Pulsewidth","V3 Pulsewidth","V123 VCA Delay","V1 VCA Delay","V2 VCA Delay","V3 VCA Delay","V123 VCA Attack","V1 VCA Attack","V2 VCA Attack","V3 VCA Attack",
		 "V123 VCA Decay","V1 VCA Decay","V2 VCA Decay","V3 VCA Decay","V123 VCA Sustain","V1 VCA Sustain","V2 VCA Sustain","V3 VCA Sustain","V123 VCA Release","V1 VCA Release","V2 VCA Release","V3 VCA Release","V123 Arp Speed","V1 Arp Speed","V2 Arp Speed","V3 Arp Speed","V123 Arp Gatelength","V1 Arp Gatelength","V2 Arp Gatelength","V3 Arp Gatelength",
		 "V123 Pitch Bender","V1 Pitch Bender","V2 Pitch Bender","V3 Pitch Bender","","","","","","","","","","","","","MOD1 Depth","MOD2 Depth","MOD3 Depth","MOD4 Depth",
		 "MOD5 Depth","MOD6 Depth","MOD7 Depth","MOD8 Depth","MOD1 Inv","MOD2 Inv","MOD3 Inv","MOD4 Inv","MOD5 Inv","MOD6 Inv","MOD7 Inv","MOD8 Inv","","","","","","","","",
		 "","","","","","","","","LFO1 Waveform","LFO2 Waveform","LFO3 Waveform","LFO4 Waveform","LFO5 Waveform","LFO6 Waveform","","","LFO1 Depth","LFO2 Depth","LFO3 Depth","LFO4 Depth",
		 "LFO5 Depth","LFO6 Depth","","","LFO1 Rate","LFO2 Rate","LFO3 Rate","LFO4 Rate","LFO5 Rate","LFO6 Rate","","","LFO1 Delay","LFO2 Delay","LFO3 Delay","LFO4 Delay","LFO5 Delay","LFO6 Delay","","",
		 "LFO1 Phase","LFO2 Phase","LFO3 Phase","LFO4 Phase","LFO5 Phase","LFO6 Phase","","","","","","","","","","","","","","",
		 "","","","","","","","","","","","","ENV1 Mode","ENV1 Depth","ENV1 Delay","ENV1 Attack 1","ENV1 Attack Lvl","ENV1 Attack 2","ENV1 Decay 1","ENV1 Decay Lvl",
		 "ENV1 Decay 2","ENV1 Sustain","ENV1 Release 1","ENV1 Release Lvl","ENV1 Release 2","ENV1 Curve Attack","ENV1 Curve Decay","ENV1 Curve Releas","ENV2 Mode","ENV2 Dept","ENV2 Delay","ENV2 Attack 1","ENV2 Attack Lvl","ENV2 Attack 2","ENV2 Decay 1","ENV2 Decay Lvl","ENV2 Decay 2","ENV2 Sustain","ENV2 Release 1","ENV2 Release Lvl",
		 "ENV2 Release 2","ENV2 Curve Attack","ENV2 Curve Decay","ENV2 Curve Release","WT1 Speed","WT2 Speed","WT3 Speed","WT4 Speed","WT1 Start","WT2 Start","WT3 Start","WT4 Start","WT1 End","WT2 End","WT3 End","WT4 End","WT1 Loop","WT2 Loop","WT3 Loop","WT4 Loop",
		 "WT1 Position","WT2 Position","WT3 Position","WT4 Position","","","","","","","","","V123 Play Note","V1 Play Note","V2 Play Note","V3 Play Note"};

	public static String[] basslineParams = {"===None===","Volume","","","Filter Cutoff","Filter Res.","Filter Chan.","Filter Mode","Knob#1 Value","Knob#2 Value","Knob#3 Value","Knob#4 Value","Knob#5 Value","Knob#V Value","Knob#P Value","Knob#A Value","Ext1 Value","Ext2 Value","Ext3 Value","Ext4 Value",
		 "Ext5 Value","Ext6 Value","Ext7 Value","Ext8 Value","Ext1 Switch","Ext2 Switch","Ext3 Switch","Ext4 Switch","Ext5 Switch","Ext6 Switch","Ext7 Switch","Ext8 Switch","Waveform LR","Waveform Cur","Waveform L","Waveform R","Transpose LR","Transpose Cur","Transpose L","Transpose R",
		 "Finetune LR","Finetune Cur","Finetune L","Finetune R","Portamento LR","Portamento Cur","Portamento L","Portamento R","Pulsewidth LR","Pulsewidth Cur","Pulsewidth L","Pulsewidth R","VCA Delay LR","VCA Delay Cur","VCA Delay L","VCA Delay R","VCA Attack LR","VCA Attack Cur","VCA Attack L","VCA Attack R",
		 "VCA Decay LR","VCA Decay Cur","VCA Decay L","VCA Decay R","VCA Sustain LR","VCA Sustain Cur","VCA Sustain L","VCA Sustain R","VCA Release LR","VCA Release Cur","VCA Release L","VCA Release R","Arp Speed LR","Arp Speed Cur","Arp Speed L","Arp Speed R","Arp Gatelength LR","Arp Gatelength Cur","Arp Gatelength L","Arp Gatelength R",
		 "Pitch Bender LR","Pitch Bender Cur","Pitch Bender L","Pitch Bender R","","","","","","","","","","","","","Cutoff LR","Cutoff Cur","Cutoff L","Cutoff R",
		 "Resonance LR","Resonance Cur","Resonance L","Resonance R","EnvMod LR","EnvMod Cur","EnvMod L","EnvMod R","Decay LR","Decay Cur","Decay L","Decay R","Accent LR","Accent Cur","Accent L","Accent R","","","","",
		 "","","","","","","","","LFO1 Waveform LR","LFO1 Waveform Cur","LFO1 Waveform L","LFO1 Waveform R","LFO2 Waveform LR","LFO2 Waveform Cur","LFO2 Waveform L","LFO2 Waveform R","LFO1 PitchD.LR","LFO1 PitchD.Cur","LFO1 PitchD.L","LFO1 PitchD.R",
		 "LFO2 PitchD.LR","LFO2 PitchD.Cur","LFO2 PitchD.L","LFO2 PitchD.R","LFO1 PWM D.LR","LFO1 PWM D.Cur","LFO1 PWM D.L","LFO1 PWM D.R","LFO2 PWM D.LR","LFO2 PWM D.Cur","LFO2 PWM D.L","LFO2 PWM D.R","LFO1 Filter D.LR","LFO1 Filter D.Cur","LFO1 Filter D.L","LFO1 Filter D.R","LFO2 Filter D.LR","LFO2 Filter D.Cur","LFO2 Filter D.L","LFO2 Filter D.R",
		 "LFO1 Rate LR","LFO1 Rate Cur","LFO1 Rate L","LFO1 Rate R","LFO2 Rate LR","LFO2 Rate Cur","LFO2 Rate L","LFO2 Rate R","LFO1 Delay LR","LFO1 Delay Cur","LFO1 Delay L","LFO1 Delay R","LFO2 Delay LR","LFO2 Delay Cur","LFO2 Delay L","LFO2 Delay R","LFO1 Phase LR","LFO1 Phase Cur","LFO1 Phase L","LFO1 Phase R",
		 "LFO2 Phase LR","LFO2 Phase Cur","LFO2 Phase L","LFO2 Phase R","","","","","","","","","ENV PitchD.LR","ENV PitchD.Cur","ENV PitchD.L","ENV PitchD.R","ENV PWM D.LR","ENV PWM D.Cur","ENV PWM D.L","ENV PWM D.R",
		 "ENV Filter D.LR","ENV Filter D.Cur","ENV Filter D.L","ENV Filter D.R","ENV Attack LR","ENV Attack Cur","ENV Attack L","ENV Attack R","ENV Decay LR","ENV Decay Cur","ENV Decay L","ENV Decay R","ENV Sustain LR","ENV Sustain Cur","ENV Sustain L","ENV Sustain R","ENV Release LR","ENV Release Cur","ENV Release L","ENV Release R",
		 "ENV Curve LR","ENV Curve Cur","ENV Curve L","ENV Curve R","ENV Decay A.LR","ENV Decay A.Cur","ENV Decay A.L","ENV Decay A.R","","","","","","","","","","","","",
		 "","","","","","","","","","","","","","","",""};
	
	public static String[] drumParams = {"===None===","Volume","","","Filter Cutoff","Filter Res.","","","Knob#1 Value","Knob#2 Value","Knob#3 Value","Knob#4 Value","Knob#5 Value","Knob#V Value","Knob#P Value","Knob#A Value","Ext1 Value","Ext2 Value","Ext3 Value","Ext4 Value",
		 "Ext5 Value","Ext6 Value","Ext7 Value","Ext8 Value","Ext1 Switch","Ext2 Switch","Ext3 Switch","Ext4 Switch","Ext5 Switch","Ext6 Switch","Ext7 Switch","Ext8 Switch","DrumModel Cur","Attack Cur","Decay Cur","Sustain Cur","Release Cur","Finetune Cur","Gatelength Cur","Speed Cur",
		 "Param.3 Cur","","","","","","","","DrumModel Dr1","DrumModel Dr2","DrumModel Dr3","DrumModel Dr4","DrumModel Dr5","DrumModel Dr6","DrumModel Dr7","DrumModel Dr8","DrumModel Dr9","DrumModel Dr10","DrumModel Dr11","DrumModel Dr12",
		 "DrumModel Dr13","DrumModel Dr14","DrumModel Dr15","DrumModel Dr16","Attack Dr1","Attack Dr2","Attack Dr3","Attack Dr4","Attack Dr5","Attack Dr6","Attack Dr7","Attack Dr8","Attack Dr9","Attack Dr10","Attack Dr11","Attack Dr12","Attack Dr13","Attack Dr14","Attack Dr15","Attack Dr16",
		 "Decay Dr1","Decay Dr2","Decay Dr3","Decay Dr4","Decay Dr5","Decay Dr6","Decay Dr7","Decay Dr8","Decay Dr9","Decay Dr10","Decay Dr11","Decay Dr12","Decay Dr13","Decay Dr14","Decay Dr15","Decay Dr16","Sustain Dr1","Sustain Dr2","Sustain Dr3","Sustain Dr4",
		 "Sustain Dr5","Sustain Dr6","Sustain Dr7","Sustain Dr8","Sustain Dr9","Sustain Dr10","Sustain Dr11","Sustain Dr12","Sustain Dr13","Sustain Dr14","Sustain Dr15","Sustain Dr16","Release Dr1","Release Dr2","Release Dr3","Release Dr4","Release Dr5","Release Dr6","Release Dr7","Release Dr8",
		 "Release Dr9","Release Dr10","Release Dr11","Release Dr12","Release Dr13","Release Dr14","Release Dr15","Release Dr16","Finetune Dr1","Finetune Dr2","Finetune Dr3","Finetune Dr4","Finetune Dr5","Finetune Dr6","Finetune Dr7","Finetune Dr8","Finetune Dr9","Finetune Dr10","Finetune Dr11","Finetune Dr12",
		 "Finetune Dr13","Finetune Dr14","Finetune Dr15","Finetune Dr16","Gatelength Dr1","Gatelength Dr2","Gatelength Dr3","Gatelength Dr4","Gatelength Dr5","Gatelength Dr6","Gatelength Dr7","Gatelength Dr8","Gatelength Dr9","Gatelength Dr10","Gatelength Dr11","Gatelength Dr12","Gatelength Dr13","Gatelength Dr14","Gatelength Dr15","Gatelength Dr16",
		 "Speed Dr1","Speed Dr2","Speed Dr3","Speed Dr4","Speed Dr5","Speed Dr6","Speed Dr7","Speed Dr8","Speed Dr9","Speed Dr10","Speed Dr11","Speed Dr12","Speed Dr13","Speed Dr14","Speed Dr15","Speed Dr16","Param.3 Dr1","Param.3 Dr2","Param.3 Dr3","Param.3 Dr4",
		 "Param.3 Dr5","Param.3 Dr6","Param.3 Dr7","Param.3 Dr8","Param.3 Dr9","Param.3 Dr10","Param.3 Dr11","Param.3 Dr12","Param.3 Dr13","Param.3 Dr14","Param.3 Dr15","Param.3 Dr16","","","","","","","","",
		 "","","","","","","","","","","","","","","","","","","","",
		 "","","","","","","","","","","","","","","","","","","","",
		 "","","","","","","","","","","","","","","",""};
	
	public static String[] multiParams = {"===None===","Volume","","","Filter Cutoff","Filter Res.","","","Knob#1 Value","Knob#2 Value","Knob#3 Value","Knob#4 Value","Knob#5 Value","Knob#V Value","Knob#P Value","Knob#A Value","Ext1 Value","Ext2 Value","Ext3 Value","Ext4 Value",
		 "Ext5 Value","Ext6 Value","Ext7 Value","Ext8 Value","Ext1 Switch","Ext2 Switch","Ext3 Switch","Ext4 Switch","Ext5 Switch","Ext6 Switch","Ext7 Switch","Ext8 Switch","Wavef.OSC All","Wavef.OSC Cur","Wavef.OSC Ins1","Wavef.OSC Ins2","Wavef.OSC Ins3","Wavef.OSC Ins4","Wavef.OSC Ins5","Wavef.OSC Ins6",
		 "Transp.OSC All","Transp.OSC Cur","Transp.OSC Ins1","Transp.OSC Ins2","Transp.OSC Ins3","Transp.OSC Ins4","Transp.OSC Ins5","Transp.OSC Ins6","Finet.OSC All","Finet.OSC Cur","Finet.OSC Ins1","Finet.OSC Ins2","Finet.OSC Ins3","Finet.OSC Ins4","Finet.OSC Ins5","Finet.OSC Ins6","Portam.OSC All","Portam.OSC Cur","Portam.OSC Ins1","Portam.OSC Ins2",
		 "Portam.OSC Ins3","Portam.OSC Ins4","Portam.OSC Ins5","Portam.OSC Ins6","Pulsew.OSC All","Pulsew.OSC Cur","Pulsew.OSC Ins1","Pulsew.OSC Ins2","Pulsew.OSC Ins3","Pulsew.OSC Ins4","Pulsew.OSC Ins5","Pulsew.OSC Ins6","Attack OSC All","Attack OSC Cur","Attack OSC Ins1","Attack OSC Ins2","Attack OSC Ins3","Attack OSC Ins4","Attack OSC Ins5","Attack OSC Ins6",
		 "Decay OSC All","Decay OSC Cur","Decay OSC Ins1","Decay OSC Ins2","Decay OSC Ins3","Decay OSC Ins4","Decay OSC Ins5","Decay OSC Ins6","Sustain OSC All","Sustain OSC Cur","Sustain OSC Ins1","Sustain OSC Ins2","Sustain OSC Ins3","Sustain OSC Ins4","Sustain OSC Ins5","Sustain OSC Ins6","Release OSC All","Release OSC Cur","Release OSC Ins1","Release OSC Ins2",
		 "Release OSC Ins3","Release OSC Ins4","Release OSC Ins5","Release OSC Ins6","ArpSpd.OSC All","ArpSpd.OSC Cur","ArpSpd.OSC Ins1","ArpSpd.OSC Ins2","ArpSpd.OSC Ins3","ArpSpd.OSC Ins4","ArpSpd.OSC Ins5","ArpSpd.OSC Ins6","ArpGL.OSC All","ArpGL.OSC Cur","ArpGL.OSC Ins1","ArpGL.OSC Ins2","ArpGL.OSC Ins3","ArpGL.OSC Ins4","ArpGL.OSC Ins5","ArpGL.OSC Ins6",
		 "P.Bender OSC All","P.Bender OSC Cur","P.Bender OSC Ins1","P.Bender OSC Ins2","P.Bender OSC Ins3","P.Bender OSC Ins4","P.Bender OSC Ins5","P.Bender OSC Ins6","PitchD.LFO1 All","PitchD.LFO1 Cur","PitchD.LFO1 Ins1","PitchD.LFO1 Ins2","PitchD.LFO1 Ins3","PitchD.LFO1 Ins4","PitchD.LFO1 Ins5","PitchD.LFO1 Ins6","PWM D.LFO1 All","PWM D.LFO1 Cur","PWM D.LFO1 Ins1","PWM D.LFO1 Ins2",
		 "PWM D.LFO1 Ins3","PWM D.LFO1 Ins4","PWM D.LFO1 Ins5","PWM D.LFO1 Ins6","FiltD.LFO1 All","FiltD.LFO1 Cur","FiltD.LFO1 Ins1","FiltD.LFO1 Ins2","FiltD.LFO1 Ins3","FiltD.LFO1 Ins4","FiltD.LFO1 Ins5","FiltD.LFO1 Ins6","Rate LFO1 All","Rate LFO1 Cur","Rate LFO1 Ins1","Rate LFO1 Ins2","Rate LFO1 Ins3","Rate LFO1 Ins4","Rate LFO1 Ins5","Rate LFO1 Ins6",		 
		 "PitchD.LFO2 All","PitchD.LFO2 Cur","PitchD.LFO2 Ins1","PitchD.LFO2 Ins2","PitchD.LFO2 Ins3","PitchD.LFO2 Ins4","PitchD.LFO2 Ins5","PitchD.LFO2 Ins6","PWM D.LFO2 All","PWM D.LFO2 Cur","PWM D.LFO2 Ins1","PWM D.LFO2 Ins2","PWM D.LFO2 Ins3","PWM D.LFO2 Ins4","PWM D.LFO2 Ins5","PWM D.LFO2 Ins6","FiltD.LFO2 All","FiltD.LFO2 Cur","FiltD.LFO2 Ins1","FiltD.LFO2 Ins2",
		 "FiltD.LFO2 Ins3","FiltD.LFO2 Ins4","FiltD.LFO2 Ins5","FiltD.LFO2 Ins6","Rate LFO2 All","Rate LFO2 Cur","Rate LFO2 Ins1","Rate LFO2 Ins2","Rate LFO2 Ins3","Rate LFO2 Ins4","Rate LFO2 Ins5","Rate LFO2 Ins6","PitchD.ENV2 All","PitchD.ENV2 Cur","PitchD.ENV2 Ins1","PitchD.ENV2 Ins2","PitchD.ENV2 Ins3","PitchD.ENV2 Ins4","PitchD.ENV2 Ins5","PitchD.ENV2 Ins6",
		 "PWM D. ENV All","PWM D. ENV Cur","PWM D. ENV Ins1","PWM D. ENV Ins2","PWM D. ENV Ins3","PWM D. ENV Ins4","PWM D. ENV Ins5","PWM D. ENV Ins6","FiltD. ENV All","FiltD. ENV Cur","FiltD. ENV Ins1","FiltD. ENV Ins2","FiltD. ENV Ins3","FiltD. ENV Ins4","FiltD. ENV Ins5","FiltD. ENV Ins6","Attack ENV All","Attack ENV Cur","Attack ENV Ins1","Attack ENV Ins2",
		 "Attack ENV Ins3","Attack ENV Ins4","Attack ENV Ins5","Attack ENV Ins6","Decay ENV All","Decay ENV Cur","Decay ENV Ins1","Decay ENV Ins2","Decay ENV Ins3","Decay ENV Ins4","Decay ENV Ins5","Decay ENV Ins6","Sustain ENV All","Sustain ENV Cur","Sustain ENV Ins1","Sustain ENV Ins2","Sustain ENV Ins3","Sustain ENV Ins4","Sustain ENV Ins5","Sustain ENV Ins6",
		 "Release ENV All","Release ENV Cur","Release ENV Ins1","Release ENV Ins2","Release ENV Ins3","Release ENV Ins4","Release ENV Ins5","Release ENV Ins6","Note OSC All","Note OSC Cur","Note OSC Ins1","Note OSC Ins2","Note OSC Ins3","Note OSC Ins4","Note OSC Ins5","Note OSC Ins6"};	
	
	// Aliases for values
	public static String[] srcNames = new String[]{"Off","Envelope 1","Envelope 2","LFO 1","LFO 2","LFO 3","LFO 4","LFO 5","LFO 6","ModPath 1","ModPath 2","ModPath 3","ModPath 4","ModPath 5","ModPath 6","ModPath 7","ModPath 8","ModWheel","Key","Knob #1","Knob #2","Knob #3","Knob #4","Knob #5","Velocity","Pitch bender","Aftertouch","WaveTable 1","WaveTable 2","WaveTable 3","WaveTable 4"};
	public static String[] operatorNames = new String[]{"Off","Src1","Src2","Src1 + Src2","Src1 - Src2","Src1 x Src2","XOR","OR","AND","MIN","MAX","Src1 < Src2","Src1 > Src2","Src1 = Src2","Sample & Hold"};
	public static String[] tgtNames = new String[]{"Off","Pitch OSC1","Pitch OSC2","Pitch OSC3","Pitch OSC4","Pitch OSC5","Pitch OSC6","PWM OSC1","PWM OSC2","PWM OSC3","PWM OSC4","PWM OSC5","PWM OSC6","Cutoff (L)","Cutoff (R)","Volume (L)","Volume (R)","LFO1 depth","LFO2 depth","LFO3 depth","LFO4 depth","LFO5 depth","LFO6 depth","LFO1 rate","LFO2 rate","LFO3 rate","LFO4 rate","LFO5 rate","LFO6 rate","Ext1","Ext2","Ext3","Ext4","Ext5","Ext6","Ext7","Ext8","WT1 pos","WT2 pos","WT3 pos","WT4 pos"};
	public static String[] trigColumnNames = new String[]{"Note On","Note Off","Env1 Sustain","Env2 Sustain","LFO1 Period","LFO2 Period","LFO3 Period","LFO4 Period","LFO5 Period","LFO6 Period","Global clock","Global clock / 6","Global clock / 24","MIDI Clock start"};
	public static String[] trigRowNames = new String[]{"Voice 1","Voice 2","Voice 3","Voice 4","Voice 5","Voice 6","Env1 Attack","Env2 Attack","Env1 Release","Env2 Release","LFO1 Reset","LFO2 Reset","LFO3 Reset","LFO4 Reset","LFO5 Reset","LFO6 Reset","WT 1 Reset","WT2 Reset","WT3 Reset","WT4 Reset","WT1 Step","WT2 Step","WT3 Step","WT4 Step"};
	public static String[] engines = new String[]{"Lead","Bassline","Drum","Multi"};
	public static String[] sidRestrict = new String[]{"No SID restrict.","SID 6581","SID 6582/8580","SwinSID"};
	public static String[] monoStereo = new String[]{"Mono","Stereo"};
	public static String[] caps = new String[]{"470pF","1nF","2.2nF","4.7nF","6.8nF","10nF","22nF","47nF","100nF"};
	public static String[] porta = new String[]{"Normal","Fixed time","Glissando"};
	public static String[] direction = new String[]{"Up","Down","Up & Down","Down & Up","Up & Down 2","Down & Up 2","Random"};
	public static String[] loop = new String[]{"Off","Attack 1","Attack 2","Decay 1","Decay 2","Sustain","Release 1","Release 2"};
	public static String[] seq = new String[]{"Sequence #1","Sequence #2","Sequence #3","Sequence #4","Sequence #5","Sequence #6","Sequence #7","Sequence #8","Disable pattern"};
	public static String[] notes = new String[]{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B","C+","C#+","D+","D#+"};
	public static String[] rates = new String[]{"64/1","32/1","16/1","8/1","4/1","2/1","1/1","1/2","1/4","1/8","1/16"};
	public static String[] octaves ={"0","+1","-1","+2"};
	public static String[] voiceAssign = {"LR","L-","-R","01","02","03","04","05","06"};
	public static String[] drumModel = {"BD1","BD2","BD3","SD1","SD2","SD3","HH1","HH2","TOM1","TOM2","CLAP","FX1","FX2","FX3","FX4","FX5","FX6","FX7","FX8","FX9"};	
}
