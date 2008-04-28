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

import java.awt.BorderLayout;
import java.util.Vector;
import org.midibox.sidedit.*;
import org.midibox.sidlibr.Patch;

import javax.sound.midi.Receiver;
import javax.swing.JToggleButton;

public class EngineSpecs {	
	// Control types:
	private static int rotary = 0;
	private static int button = 1;
	private static int simplebutton = 2;
	private static int slider = 3;	
	private static int waveselect = 4;	
	private static int lfoselect = 5;
	private static int combo = 6;
	
	public static Vector getEngine(Patch patch) {
		Vector engine = new Vector();
		if (patch.getEngine() == patch.LEAD) {
			// Lead engine
			engine.add(createGlobal(patch,SIDSysexInfo.leadParams));	// 0: Global parameters
			engine.add(createExt(patch));	     						// 1: External parameters
			engine.add(createFilter(patch,0)); 							// 2: Filter Left
			engine.add(createFilter(patch,6));  						// 3: Filter right				
			engine.add(LeadSpecs.createMSTR(patch)); 					// 4: Master settings			
			engine.add(createVoice(patch,0));	 						// 5: Voice 1 Left
			engine.add(createVoice(patch,16));	 						// 6: Voice 2 Left
			engine.add(createVoice(patch,32));	 						// 7: Voice 3 Left
			engine.add(createVoice(patch,48));  						// 8: Voice 4 Right
			engine.add(createVoice(patch,64));	 						// 9: Voice 5 Right
			engine.add(createVoice(patch,80));	 						//10: Voice 6 Right			
			engine.add(LeadSpecs.createLFO(patch));						//11: LFO's 			
			engine.add(LeadSpecs.createENV(patch,0));  					//12: Envelope 1
			engine.add(LeadSpecs.createENV(patch,2));  					//13: Envelope 2			
			engine.add(LeadSpecs.createMOD(patch));  					//14: Modulation paths
			engine.add(LeadSpecs.createTRIG(patch));  					//15: Trigger matrix			
			engine.add(LeadSpecs.createWT(patch));  					//16: Wavetable sequencer
		} 
		else if (patch.getEngine() == patch.BASSLINE) {
			// Bassline
			engine.add(createGlobal(patch,SIDSysexInfo.basslineParams));// 0: Global parameters
			engine.add(createExt(patch));	     						// 1: External parameters
			engine.add(createFilter(patch,0));  						// 2: Filter Left
			engine.add(createFilter(patch,6));  						// 3: Filter right	
			engine.add(createVoice(patch,0));							// 4: Left Oscillator
			engine.add(createVoice(patch,80));							// 5: Right Oscillator			
			engine.add(BasslineSpecs.createMSTR(patch,0));				// 6: Left master set
			engine.add(BasslineSpecs.createMSTR(patch,80));				// 7: Right master set			
			engine.add(BasslineSpecs.createLFO(patch,0));				// 8: Left LFO's
			engine.add(BasslineSpecs.createLFO(patch,80));				// 9: Right LFO's
			engine.add(BasslineSpecs.createENV(patch,0));				//10: Left Envelope
			engine.add(BasslineSpecs.createENV(patch,80));				//11 Right Envelope
			engine.add(BasslineSpecs.createSEQ(patch,0));				//12: Left Sequencer
			engine.add(BasslineSpecs.createSEQ(patch,80));				//13: Right Sequencer
			engine.add(BasslineSpecs.createSEQData(patch));				//14: Sequencer data
		}
		else if (patch.getEngine() == patch.DRUM) {
			// Drum
			engine.add(createGlobal(patch,SIDSysexInfo.drumParams));	// 0: Global parameters
			engine.add(createExt(patch));	     						// 1: External parameters
			engine.add(createFilter(patch,0)); 	 						// 2: Filter Left
			engine.add(createFilter(patch,6));  						// 3: Filter right
			engine.add(DrumSpecs.createDrumInstr(patch));				// 4: Drum Instruments			
			engine.add(DrumSpecs.createSEQ(patch));						// 5: Sequencer
			engine.add(DrumSpecs.createSEQData(patch));					// 6: Sequencer data
		}
		else if (patch.getEngine() == patch.MULTI) {
			// Multi
			engine.add(createGlobal(patch,SIDSysexInfo.multiParams)); 	// 0: Global parameters
			engine.add(createExt(patch));	     						// 1: External parameters
			engine.add(createFilter(patch,0));  						// 2: Filter Left
			engine.add(createFilter(patch,6));  						// 3: Filter right						
			engine.add(createVoice(patch,0));							// 4: Inst.1 Voice
			engine.add(createVoice(patch,48));							// 5: Inst.2 Voice
			engine.add(createVoice(patch,96));							// 6: Inst.3 Voice
			engine.add(createVoice(patch,144));							// 7: Inst.4 Voice
			engine.add(createVoice(patch,192));							// 8: Inst.5 Voice
			engine.add(createVoice(patch,240));							// 9: Inst.6 Voice			
			engine.add(MultiSpecs.createInstrument(patch,0));			//10: Inst.1 Rest
			engine.add(MultiSpecs.createInstrument(patch,48));			//11: Inst.2 Rest
			engine.add(MultiSpecs.createInstrument(patch,96));			//12: Inst.3 Rest
			engine.add(MultiSpecs.createInstrument(patch,144));			//13: Inst.4 Rest
			engine.add(MultiSpecs.createInstrument(patch,192));			//14: Inst.5 Rest
			engine.add(MultiSpecs.createInstrument(patch,240));			//15: Inst.6 Rest
			engine.add(MultiSpecs.createWTdata(patch));  				//16: Wavetable data			
		}		
		return engine;
	}
		
	private static Vector createGlobal(Patch patch, String[] engineParams) {
		Vector globalControl = new Vector();

		globalControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.sidRestrict,patch,17,0,2,"SID type","SID chip type to be used"));
		globalControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.monoStereo,patch,17,3,1,"Stereo","Stereo/Mono type to be used "));
		globalControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.caps,patch,17,4,4,"Filter caps","Type of filter capacitors to be used"));
		globalControl.add(new SIDSysexParameterControl(button,null,patch,18,0,1,"ABW","Workaround for the SID's ADSR-bug"));
		globalControl.add(new SIDSysexParameterControl(rotary,null,patch,82,0,7,"Volume"));
		
		for (int c = 0; c <8; c++) {
			globalControl.add(new SIDSysexParameterControl(combo,engineParams,patch,24+(5*c),0,8,"Assign 1","Assign this control to a parameter"));
			globalControl.add(new SIDSysexParameterControl(combo,engineParams,patch,25+(5*c),0,8,"Assign 2","Assign this control to a parameter"));
			globalControl.add(new SIDSysexParameterControl(rotary,null,patch,26+(5*c),0,8,"Init.","Initial value"));
			globalControl.add(new SIDSysexParameterControl(rotary,null,patch,27+(5*c),0,8,"Min.","Minimum value"));
			globalControl.add(new SIDSysexParameterControl(rotary,null,patch,28+(5*c),0,8,"Max.","Maximum value"));
		}
		
		return globalControl;
	}
	
	private static Vector createVoice(Patch patch, int offset) {
		Vector voiceControl = new Vector();
		// Single voice
		voiceControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.porta,patch,96+offset,0,2,"Portamento","Portamento type"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,96+offset,2,1,"GAE","Gate Always Enabled"));		
		voiceControl.add(new SIDSysexParameterControl(waveselect,null,patch,97+offset,0,8,"Waveform","Select a waveform"));		
		voiceControl.add(new SIDSysexParameterControl(slider,null,patch,98+offset,4,4,"Attack","SID's DCA Attack rate"));
		voiceControl.add(new SIDSysexParameterControl(slider,null,patch,98+offset,0,4,"Decay","SID's DCA Decay rate"));
		voiceControl.add(new SIDSysexParameterControl(slider,null,patch,99+offset,4,4,"Sustain","SID's DCA Sustain level"));
		voiceControl.add(new SIDSysexParameterControl(slider,null,patch,99+offset,0,4,"Release","SID's DCA Release rate"));		
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,100+offset,0,12,"Pulse Width","Pulse Width"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,102+offset,0,8,"Accent","Bassline Accent")); // SwinSID Phase...
		voiceControl.add(new SIDSysexParameterControl(slider,null,patch,103+offset,0,8,"Delay","DCA Envelope delay"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,104+offset,0,-7,"Transpose"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,105+offset,0,-8,"Finetune"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,106+offset,0,7,"Pitchrange","Range of the Pitch Bender"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,107+offset,0,8,"Porta. Rate","Portamento speed"));
		//Arpeggiator
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,108+offset,0,1,"Arp On/Off","Enables/disables the arpeggiator"));
		voiceControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.direction,patch,108+offset,1,3,"Direction","Direction of the arpeggio"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,108+offset,4,1,"Sorted","Sort notes"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,108+offset,5,1,"Hold","Hold the last arpeggio"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,108+offset,6,1,"Key Sync","Synchronize with key presses"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,108+offset,7,1,"Constant Cycle","Constant arpeggiator cycle"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,109+offset,0,6,"Speed Divider","Speed divider (sets the tempo)"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,109+offset,7,1,"Oneshot"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,110+offset,0,5,"Gatelength"));
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,110+offset,5,3,"Octave Range"));
		//SwinSID
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,111+offset,0,4,"Waveform OSC2","Waveform of second oscillator"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,111+offset,4,1,"Enable OSC2","Enable second oscillator"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,111+offset,5,1,"x2 pitch","scale pitch"));
		voiceControl.add(new SIDSysexParameterControl(button,null,patch,111+offset,6,1,"Rev. Waveform","Reverse Waveform"));	
		voiceControl.add(new SIDSysexParameterControl(rotary,null,patch,102+offset,0,8,"Phase"));
		return voiceControl;
	}
	
	private static Vector createFilter(Patch patch, int offset) {
		Vector filterControl = new Vector();
		filterControl.add(new SIDSysexParameterControl(rotary,null,patch,85+offset,0,12,"Cutoff","Cutoff frequency"));
		filterControl.add(new SIDSysexParameterControl(rotary,null,patch,87+offset,0,8,"Resonance"));
		filterControl.add(new SIDSysexParameterControl(rotary,null,patch,88+offset,0,8,"Keytrack","Key tracking"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,0,1,"OSC1->Filt","OSC1 routed through filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,1,1,"OSC2->Filt","OSC2 routed through filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,2,1,"OSC3->Filt","OSC3 routed through filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,3,1,"EXT->Filter","External input routed through filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,4,1,"Low  Pass","Low pass filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,5,1,"Band Pass","Band pass filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,6,1,"High Pass","High pass filter"));
		filterControl.add(new SIDSysexParameterControl(button,null,patch,84+offset,7,1,"OSC3 off","Turn off oscillator 3"));		
		filterControl.add(new SIDSysexParameterControl(button,null,patch,86+offset,7,1,"FIP","Filter Interpolation"));
		return filterControl;
	}
	
	private static Vector createExt(Patch patch) {
		Vector extControl = new Vector();
		// External switches 1-8
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,7,1,"Switch 1","External Switch 1"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,6,1,"Switch 2","External Switch 2"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,5,1,"Switch 3","External Switch 3"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,4,1,"Switch 4","External Switch 4"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,3,1,"Switch 5","External Switch 5"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,2,1,"Switch 6","External Switch 6"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,1,1,"Switch 7","External Switch 7"));
		extControl.add(new SIDSysexParameterControl(button,null,patch,20,0,1,"Switch 8","External Switch 8"));
		// External Parameter CV 1-8
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,64,0,16,"Ext.CV 1","External Parameter CV 1"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,66,0,16,"Ext.CV 2","External Parameter CV 2"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,68,0,16,"Ext.CV 3","External Parameter CV 3"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,70,0,16,"Ext.CV 4","External Parameter CV 4"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,72,0,16,"Ext.CV 5","External Parameter CV 5"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,74,0,16,"Ext.CV 6","External Parameter CV 6"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,76,0,16,"Ext.CV 7","External Parameter CV 7"));
		extControl.add(new SIDSysexParameterControl(rotary,null,patch,78,0,16,"Ext.CV 8","External Parameter CV 8"));
		return extControl;
	}
	
	
	
}