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
		leadControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				80, 0, 1, "Legato"));
		leadControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				80, 2, 1, "Fingered Portamento"));
		leadControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				80, 1, 1, "Wavetable only"));
		leadControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				81, 0, 8, "Detune", "Detune oscillators"));
		leadControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				83, 0, 8, "Phase Offset", "Oscillators phase offset "));
		return leadControl;
	}

	public static Vector createLFO(Patch patch) {
		Vector lfoControl = new Vector();
		// LFO
		for (int c = 0; c < 6; c++) {
			lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					192 + (5 * c), 0, 1, "On/Off", "Enable/disable LFO"));
			lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					192 + (5 * c), 2, 1, "MIDI Sync",
					"Global clock synchronisation"));
			lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					192 + (5 * c), 3, 1, "Oneshot", "Oneshot mode"));
			lfoControl
					.add(new SIDSysexParameterControl(lfoselect, null, 0,
							patch, 192 + (5 * c), 4, 4, "Waveform",
							"Select a waveform"));
			lfoControl.add(new SIDSysexParameterControl(rotary, null, -128,
					patch, 193 + (5 * c), 0, 8, "Depth"));
			lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
					194 + (5 * c), 0, 8, "Rate"));
			lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
					195 + (5 * c), 0, 8, "Delay"));
			lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
					196 + (5 * c), 0, 8, "Phase"));
			lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					321, (2 + c), 1, "Key Sync", "Reset LFO at key press"));
		}
		return lfoControl;
	}

	public static Vector createENV(Patch patch, int offset) {
		Vector envControl = new Vector();
		// Envelope
		envControl.add(new SIDSysexParameterControl(combo, SIDSysexInfo.loop,
				0, patch, 224 + offset, 0, 3, "Loop Begin",
				"Envelope loop begin point"));
		envControl.add(new SIDSysexParameterControl(combo, SIDSysexInfo.loop,
				0, patch, 224 + offset, 4, 3, "Loop End",
				"Envelope loop end point"));
		envControl
				.add(new SIDSysexParameterControl(button, null, 0, patch,
						224 + offset, 7, 1, "MIDI Sync",
						"Global clock synchronisation"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch,
				225 + offset, 0, 8, "Depth"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				226 + offset, 0, 8, "Delay"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				227 + offset, 0, 8, "Attack1", "Attack1 Rate"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch,
				228 + offset, 0, 8, "Attack Lvl", "Attack Level"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				229 + offset, 0, 8, "Attack2", "Attack2 Rate"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				230 + offset, 0, 8, "Decay1", "Decay1 Rate"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch,
				231 + offset, 0, 8, "Decay Lvl", "Decay Level"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				232 + offset, 0, 8, "Decay2", "Decay2 Rate"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch,
				233 + offset, 0, 8, "Sustain Lvl", "Sustain Level"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				234 + offset, 0, 8, "Release1", "Release1 Rate"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch,
				235 + offset, 0, 8, "Release Lvl", "Release Level"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				236 + offset, 0, 8, "Release2", "Release2 Rate"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch,
				237 + offset, 0, 8, "Att.Curve", "Attack Curve"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch,
				238 + offset, 0, 8, "Dec.Curve", "Decay Curve"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch,
				239 + offset, 0, 8, "Rel.Curve", "Release Curve"));
		return envControl;
	}

	public static Vector createMOD(Patch patch) {
		Vector modControl = new Vector();
		// Modulation paths
		for (int c = 0; c < 8; c++) {
			modControl.add(new SIDSysexParameterControl(combo, createSrcStr(),
					0, patch, 256 + (8 * c), 0, 8, "Source 1"));
			modControl.add(new SIDSysexParameterControl(combo, createSrcStr(),
					0, patch, 257 + (8 * c), 0, 8, "Source 2"));
			modControl.add(new SIDSysexParameterControl(combo,
					SIDSysexInfo.operatorNames, 0, patch, 258 + (8 * c), 0, 4,
					"Operator"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					258 + (8 * c), 6, 1, "Invert 1/L", "Invert target 1/L"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					258 + (8 * c), 7, 1, "Invert 2/R", "Invert target 2/R"));
			modControl.add(new SIDSysexParameterControl(rotary, null, -128,
					patch, 259 + (8 * c), 0, 8, "Depth"));
			modControl.add(new SIDSysexParameterControl(combo,
					SIDSysexInfo.tgtNames, 0, patch, 262 + (8 * c), 0, 8,
					"Target 1"));
			modControl.add(new SIDSysexParameterControl(combo,
					SIDSysexInfo.tgtNames, 0, patch, 263 + (8 * c), 0, 8,
					"Target 2"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 0, 1, "V1 Pitch",
					"Direct assignment to pitch of left OSC1"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 1, 1, "V2 Pitch",
					"Direct assignment to pitch of left OSC2"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 2, 1, "V3 Pitch",
					"Direct assignment to pitch of left OSC3"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 3, 1, "V1 PWM",
					"Direct assignment to pulse width of left OSC1"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 4, 1, "V2 PWM",
					"Direct assignment to pulse width of left OSC2"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 5, 1, "V3 PWM",
					"Direct assignment to pulse width of left OSC3"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 6, 1, "Cutoff L",
					"Direct assignment to filter cutoff of left SID"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					260 + (8 * c), 7, 1, "Volume L",
					"Direct assignment to volume of left SID"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 0, 1, "V4 Pitch",
					"Direct assignment to pitch of right OSC1"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 1, 1, "V5 Pitch",
					"Direct assignment to pitch of right OSC2"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 2, 1, "V6 Pitch",
					"Direct assignment to pitch of right OSC3"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 3, 1, "V4 PW",
					"Direct assignment to pulse width of right OSC1"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 4, 1, "V5 PW",
					"Direct assignment to pulse width of right OSC2"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 5, 1, "V6 PW",
					"Direct assignment to pulse width of right OSC3"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 6, 1, "Cutoff R",
					"Direct assignment to filter cutoff of right SID"));
			modControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					261 + (8 * c), 7, 1, "Volume R",
					"Direct assignment to volume of right SID"));
		}
		return modControl;
	}

	public static Vector createTRIG(Patch patch) {
		Vector trigControl = new Vector();
		// Trigger matrix
		for (int c = 0; c < 14; c++) {
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 0, 1, "OSC1 Left Ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 1, 1, "OSC2 Left Ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 2, 1, "OSC3 Left Ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 3, 1, "OSC1 Right Ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 4, 1, "OSC2 Right Ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 5, 1, "OSC3 Right Ch"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 6, 1, "ENV1 Attack"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 320 + (3 * c), 7, 1, "ENV2 Attack"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 0, 1, "ENV1 Release"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 1, 1, "ENV2 Release"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 2, 1, "LFO1 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 3, 1, "LFO2 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 4, 1, "LFO3 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 5, 1, "LFO4 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 6, 1, "LFO5 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 321 + (3 * c), 7, 1, "LFO6 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 0, 1, "WT1 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 1, 1, "WT2 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 2, 1, "WT3 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 3, 1, "WT4 Reset"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 4, 1, "WT1 Step"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 5, 1, "WT2 Step"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 6, 1, "WT3 Step"));
			trigControl.add(new SIDSysexParameterControl(simplebutton, null, 0,
					patch, 322 + (3 * c), 7, 1, "WT4 Step"));
		}
		return trigControl;
	}

	public static Vector createWT(Patch patch) {
		Vector wtControl = new Vector();
		// Wavetable sequencer
		for (int c = 0; c < 4; c++) {
			wtControl.add(new SIDSysexParameterControl(rotary,
					createDividers(), 0, patch, 364 + (5 * c), 0, 6,
					"Clock Divider", "Clock divider (sets tempo)"));
			wtControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					364 + (5 * c), 6, 1, "To Left Ch",
					"Parameter forwarded to left channel"));
			wtControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					364 + (5 * c), 7, 1, "To Right Ch",
					"Parameter forwarded to right channel"));
			wtControl.add(new SIDSysexParameterControl(combo,
					SIDSysexInfo.leadParams, 0, patch, 365 + (5 * c), 0, 8,
					"Assign", "Parameter assignment"));
			wtControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
					366 + (5 * c), 0, 7, "Start Pos",
					"Start position in wavetable"));
			wtControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					366 + (5 * c), 7, 1, "Pos Ctrl by MP",
					"Position controlled by modulation path"));
			wtControl
					.add(new SIDSysexParameterControl(rotary, null, 0, patch,
							367 + (5 * c), 0, 7, "End Pos",
							"End position in wavetable"));
			wtControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
					368 + (5 * c), 0, 7, "Loop Pos",
					"Loop position in wavetable"));
			wtControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					368 + (5 * c), 7, 1, "Oneshot"));
		}
		// Wavetable data
		for (int c = 0; c < 128; c++) {
			wtControl.add(new SIDSysexParameterControl(slider, null, 0, patch,
					384 + c, 0, 8, Integer.toString(c)));
		}
		return wtControl;
	}

	private static String[] createSrcStr() {
		String[] srcStr = new String[256];
		String[] srcNames = SIDSysexInfo.srcNames;
		for (int c = 0; c < srcStr.length; c++) {
			if (c < srcNames.length) {
				srcStr[c] = srcNames[c];
			} else if (c > 127) {
				srcStr[c] = Integer.toString(c - 128);
			} else {
				srcStr[c] = "";
			}
		}
		return srcStr;
	}

	public static String[] createDividers() {
		String[] s = new String[64];
		for (int i = 0; i < s.length; i++) {
			s[i] = Integer.toString(i + 1);
		}
		return s;
	}

}
