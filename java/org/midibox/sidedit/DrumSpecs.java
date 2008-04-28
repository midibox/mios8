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

public class DrumSpecs {
	// Control types:
	private static int rotary = 0;
	private static int button = 1;
	private static int simplebutton = 2;
	private static int slider = 3;	
	private static int waveselect = 4;	
	private static int lfoselect = 5;
	private static int combo = 6;
	private static int simplerotary = 7;
	
	public static Vector createDrumInstr(Patch patch) {
		Vector drumInstrControl = new Vector();
		
		for(int i=0;i<16;i++) {
			drumInstrControl.add(new SIDSysexParameterControl(rotary,SIDSysexInfo.voiceAssign,patch,96+(10*i),4,4,"Voice Assign","Voice assignment"));
			drumInstrControl.add(new SIDSysexParameterControl(rotary,SIDSysexInfo.drumModel,patch,97+(10*i),0,8,"Drum Model"));
			drumInstrControl.add(new SIDSysexParameterControl(slider,null,patch,98+(10*i),4,4,"Attack","DCA Attack Rate"));
			drumInstrControl.add(new SIDSysexParameterControl(slider,null,patch,98+(10*i),0,4,"Decay","DCA Decay Rate"));
			drumInstrControl.add(new SIDSysexParameterControl(slider,null,patch,99+(10*i),4,4,"Sustain","DCA Sustain Level"));
			drumInstrControl.add(new SIDSysexParameterControl(slider,null,patch,99+(10*i),0,4,"Release","DCA Release Rate"));			
			drumInstrControl.add(new SIDSysexParameterControl(rotary,null,patch,100+(10*i),0,-8,"Tune"));
			drumInstrControl.add(new SIDSysexParameterControl(rotary,null,patch,101+(10*i),0,-8,"Gatelength","Parameter #1: Gatelength"));
			drumInstrControl.add(new SIDSysexParameterControl(rotary,null,patch,102+(10*i),0,-8,"Speed","Parameter #2: Speed"));
			drumInstrControl.add(new SIDSysexParameterControl(rotary,null,patch,103+(10*i),0,-8,"Param #3","Parameter #3: depends on Drum Model"));			
			drumInstrControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.drumParams,patch,104+(10*i),0,8,"Velocity Assign","Velocity parameter assignment"));		
		}		
		return drumInstrControl;
	}
	
	public static Vector createSEQ(Patch patch) {
		Vector seqControl = new Vector();			
		// Sequencer
		seqControl.add(new SIDSysexParameterControl(rotary,null,patch,80,0,6,"Clock Divider","Clock divider (sets tempo)"));
		seqControl.add(new SIDSysexParameterControl(button,null,patch,80,6,1,"Manual/Seq","MIDI Note playing/Sequencer mode"));		
		seqControl.add(new SIDSysexParameterControl(button,null,patch,80,7,1,"Pattern Sync","Pattern changes synchronised to measure"));		
		seqControl.add(new SIDSysexParameterControl(combo,SIDSysexInfo.seq,patch,81,0,4,"Sequence","Sequencer number"));
		seqControl.add(new SIDSysexParameterControl(rotary,createSteps(),patch,83,0,4,"Steps","Number of steps"));		
		return seqControl;
	}
	
	public static Vector createSEQData(Patch patch) {
		Vector seqDataControl = new Vector();	
		// Sequence 1-8
		for(int seq=0;seq<8;seq++) {
			// Channels 1-8
			for(int ch=0;ch<8;ch++) {				
				int offset = 4*(ch+(8*seq));
				// Steps: 1-16
				for(int s=0;s<16;s++) {
					if (s<8) {
						seqDataControl.add(new SIDSysexParameterControl(simplebutton,null,patch,256+offset,s,1,"Gate"+Integer.toString(s)+Integer.toString(ch)));
					} else {
						seqDataControl.add(new SIDSysexParameterControl(simplebutton,null,patch,258+offset,s-8,1,"Gate"+Integer.toString(s)+Integer.toString(ch)));
					}					
				}
				for(int s=0;s<16;s++) {
					if (s<8) {
						seqDataControl.add(new SIDSysexParameterControl(simplebutton,null,patch,257+offset,s,1,"Accent"+Integer.toString(s)+Integer.toString(ch)));
					} else {
						seqDataControl.add(new SIDSysexParameterControl(simplebutton,null,patch,259+offset,s-8,1,"Accent"+Integer.toString(s)+Integer.toString(ch)));
					}					
				}
				
				
			}
		}
		return seqDataControl;
	}
	
	public static String[] createSteps() {
		String[] s = new String[16];
		for(int i=0;i<s.length;i++) {
			s[i] = Integer.toString(i+1);
		}
		return s;
	}
}

