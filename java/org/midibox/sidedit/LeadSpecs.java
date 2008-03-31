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

import org.midibox.sidlibr.Patch;

public class LeadSpecs {
	// Control types:
	private static int rotary = 0;
	private static int button = 1;
	private static int simplebutton = 2;
	private static int slider = 3;	
	private static int waveselect = 4;	
	private static int lfoselect = 5;
	private static int combo = 6;
	
	public static Vector createMSTR(Patch patch) {
		Vector leadControl = new Vector();		
		leadControl.add(new SIDSysexParameterControl(button,null,patch,80,0,1,"Legato on/off"));
		leadControl.add(new SIDSysexParameterControl(button,null,patch,80,1,1,"Wavetable only"));
		leadControl.add(new SIDSysexParameterControl(button,null,patch,80,2,1,"Fingered portamento"));
		leadControl.add(new SIDSysexParameterControl(rotary,null,patch,81,0,8,"Detune","Detune oscillators"));
		leadControl.add(new SIDSysexParameterControl(rotary,null,patch,83,0,8,"Phase Offset","Oscillators phase offset "));
		return leadControl;
	}
	
	public static Vector createLFO(Patch patch) {
		Vector lfoControl = new Vector();
		// LFO
		for (int c = 0; c <6; c++) {
			lfoControl.add(new SIDSysexParameterControl(button,null,patch,192+(5*c),0,1,"on/off","Enable/disable LFO"));
			lfoControl.add(new SIDSysexParameterControl(button,null,patch,192+(5*c),2,1,"MIDI Sync","Global clock synchronisation"));
			lfoControl.add(new SIDSysexParameterControl(button,null,patch,192+(5*c),3,1,"1 shot","One shot mode"));
			lfoControl.add(new SIDSysexParameterControl(lfoselect,null,patch,192+(5*c),4,4,"Waveform","Select a waveform"));
			lfoControl.add(new SIDSysexParameterControl(rotary,null,patch,193+(5*c),0,-8,"Depth"));
			lfoControl.add(new SIDSysexParameterControl(rotary,createRateStr(),patch,194+(5*c),0,8,"Rate"));
			lfoControl.add(new SIDSysexParameterControl(rotary,null,patch,195+(5*c),0,8,"Delay"));
			lfoControl.add(new SIDSysexParameterControl(rotary,null,patch,196+(5*c),0,8,"Phase"));
			lfoControl.add(new SIDSysexParameterControl(button,null,patch,321,(2+c),1,"KeySync","Reset LFO at key press"));
		}		
		return lfoControl;
	}
	
	public static Vector createENV(Patch patch, int offset) {
		Vector envControl = new Vector();	
		// Envelope
		envControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.loop,patch,224+offset,0,3,"Loop begin","Envelope loop begin point"));
		envControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.loop,patch,224+offset,4,3,"Loop end","Envelope loop end point"));
		envControl.add(new SIDSysexParameterControl(button,null,patch,224+offset,7,1,"MIDI Sync","Global clock synchronisation"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,225+offset,0,-8,"Depth"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,226+offset,0,8,"Delay"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,227+offset,0,8,"Attack1","Attack1 rate"));
		envControl.add(new SIDSysexParameterControl(slider,null,patch,228+offset,0,8,"Attack Lvl","Attack level"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,229+offset,0,8,"Attack2","Attack2 rate"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,230+offset,0,8,"Decay1","Decay1 rate"));
		envControl.add(new SIDSysexParameterControl(slider,null,patch,231+offset,0,8,"Decay Lvl","Decay level"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,232+offset,0,8,"Decay2","Decay2 rate"));
		envControl.add(new SIDSysexParameterControl(slider,null,patch,233+offset,0,8,"Sustain Lvl","Sustain level"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,234+offset,0,8,"Release1","Release1 rate"));
		envControl.add(new SIDSysexParameterControl(slider,null,patch,235+offset,0,8,"Release Lvl","Release level"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,236+offset,0,8,"Release2","Release2 rate"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,237+offset,0,-8,"Att.Curve","Attack curve"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,238+offset,0,-8,"Dec.Curve","Decay curve"));
		envControl.add(new SIDSysexParameterControl(rotary,null,patch,239+offset,0,-8,"Rel.Curve","Release curve"));
		return envControl;
	}
	
	public static Vector createMOD(Patch patch) {
		Vector modControl = new Vector();			
		// Modulation paths
		for (int c = 0; c <8; c++) {
			modControl.add(new SIDSysexParameterControl(combo,createSrcStr(),patch,256+(8*c),0,8,"Source 1"));
			modControl.add(new SIDSysexParameterControl(combo,createSrcStr(),patch,257+(8*c),0,8,"Source 2"));
			modControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.operatorNames,patch,258+(8*c),0,4,"Operator"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,258+(8*c),6,1,"Invert 1/L","Invert target 1/L"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,258+(8*c),7,1,"Invert 2/R","Invert target 2/R"));
			modControl.add(new SIDSysexParameterControl(rotary,null,patch,259+(8*c),0,-8,"Depth"));
			modControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.tgtNames,patch,262+(8*c),0,8,"Target 1"));
			modControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.tgtNames,patch,263+(8*c),0,8,"Target 2"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),0,1,"V1 pitch","Direct assignment to pitch of left OSC1"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),1,1,"V2 pitch","Direct assignment to pitch of left OSC2"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),2,1,"V3 pitch","Direct assignment to pitch of left OSC3"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),3,1,"V1 PWM","Direct assignment to pulse width of left OSC1"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),4,1,"V2 PWM","Direct assignment to pulse width of left OSC2"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),5,1,"V3 PWM","Direct assignment to pulse width of left OSC3"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),6,1,"Cutoff L","Direct assignment to filter cutoff of left SID"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,260+(8*c),7,1,"Volume L","Direct assignment to volume of left SID"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),0,1,"V4 pitch","Direct assignment to pitch of right OSC1"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),1,1,"V5 pitch","Direct assignment to pitch of right OSC2"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),2,1,"V6 pitch","Direct assignment to pitch of right OSC3"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),3,1,"V4 PW","Direct assignment to pulse width of right OSC1"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),4,1,"V5 PW","Direct assignment to pulse width of right OSC2"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),5,1,"V6 PW","Direct assignment to pulse width of right OSC3"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),6,1,"Cutoff R","Direct assignment to filter cutoff of right SID"));
			modControl.add(new SIDSysexParameterControl(button,null,patch,261+(8*c),7,1,"Volume R","Direct assignment to volume of right SID"));
		}
		return modControl;
	}
	
	public static Vector createTRIG(Patch patch) {
		Vector trigControl = new Vector();	
		// Trigger matrix
		for (int c = 0; c <14; c++) {
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),0,1,"OSC1 left ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),1,1,"OSC2 left ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),2,1,"OSC3 left ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),3,1,"OSC1 right ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),4,1,"OSC2 right ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),5,1,"OSC3 right ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),6,1,"ENV1 Attack"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,320+(3*c),7,1,"ENV2 Attack"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),0,1,"ENV1 Release"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),1,1,"ENV2 Release"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),2,1,"LFO1 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),3,1,"LFO2 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),4,1,"LFO3 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),5,1,"LFO4 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),6,1,"LFO5 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,321+(3*c),7,1,"LFO6 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),0,1,"WT1 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),1,1,"WT2 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),2,1,"WT3 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),3,1,"WT4 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),4,1,"WT1 Step"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),5,1,"WT2 Step"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),6,1,"WT3 Step"));
			trigControl.add(new SIDSysexParameterControl(simplebutton,null,patch,322+(3*c),7,1,"WT4 Step"));
		}
		return trigControl;
	}
	
	public static Vector createWT(Patch patch) {
		Vector wtControl = new Vector();		
		// Wavetable sequencer
		for (int c = 0; c <4; c++) {
			wtControl.add(new SIDSysexParameterControl(rotary,null,patch,364+(5*c),0,6,"Clock Divider","Clock divider (sets tempo)"));
			wtControl.add(new SIDSysexParameterControl(button,null,patch,364+(5*c),6,1,"To left ch","Parameter forwarded to left channel"));
			wtControl.add(new SIDSysexParameterControl(button,null,patch,364+(5*c),7,1,"To right ch","Parameter forwarded to right channel"));
			wtControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.leadParams,patch,365+(5*c),0,8,"Assign","Parameter assignment"));
			wtControl.add(new SIDSysexParameterControl(rotary,null,patch,366+(5*c),0,7,"Start pos","Start position in wavetable"));
			wtControl.add(new SIDSysexParameterControl(button,null,patch,366+(5*c),7,1,"Pos ctrl by MP","Position controlled by modulation path"));
			wtControl.add(new SIDSysexParameterControl(rotary,null,patch,367+(5*c),0,7,"End pos","End position in wavetable"));
			wtControl.add(new SIDSysexParameterControl(rotary,null,patch,368+(5*c),0,7,"Loop pos","Loop position in wavetable"));
			wtControl.add(new SIDSysexParameterControl(button,null,patch,368+(5*c),7,1,"Oneshot"));
		}		
		// Wavetable data
		for (int c = 0; c <128; c++) {
			wtControl.add(new SIDSysexParameterControl(slider,createWTStr(),patch,384+c,0,8,Integer.toString(c)));
		}				
		return wtControl;
	}
	
	private static String[] createRateStr() {
		String[] rateStr = new String[256];		
		for (int c = 0; c <rateStr.length; c++) {
			
			if (c < 245) {
				rateStr[c] = Integer.toString(c);
			} 
			else {
				rateStr[c] = SIDSysexInfo.rates[c-245];
			}
		}
		return rateStr;
	}
	
	private static String[] createSrcStr() {
		String[] srcStr = new String[256];
		String [] srcNames = SIDSysexInfo.srcNames;
		for (int c = 0; c <srcStr.length; c++) {
			if (c < srcNames.length) {
				srcStr[c] = srcNames[c];
			} 
			else if (c > 127) {
				srcStr[c] = Integer.toString(c-128);
			}
			else {
				srcStr[c] = "";
			}
		}
		return srcStr;
	}
	
	private static String[] createWTStr() {
		String[] wtStr = new String[256];
		String[] notes = {"c-","c#","d-","d#","e-","f-","f#","g-","g#","a-","a#","b-"};
		
		for (int i = 0; i < 256; i++) {
			if (i < 64) {				
				wtStr[i] = Integer.toString(i-64);				
			}
			else if (i < 128) {
				wtStr[i] = "+" + Integer.toString(i-64);
			}
			else if (i == 128) {
				wtStr[i] = "---";
			}
			else if (i == 129) {
				wtStr[i] = "+++";
			}
			else if (i < 252) {
				int octave = (int) Math.floor((i-128)/12)-2;
				if (octave < 0) {
					wtStr[i] = notes[(i-128)%12] + Math.abs(octave);
				}
				else {
					wtStr[i] = notes[(i-128)%12].toUpperCase() + Math.abs(octave);
				}
			}
			else if (i < 256) {
				wtStr[i] = "ky" + Integer.toString(i-251);
			}
		}
		return wtStr;
	}

}
