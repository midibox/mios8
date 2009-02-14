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

public class BasslineSpecs {
	// Control types:
	private static int rotary = 0;
	private static int button = 1;
	private static int simplebutton = 2;
	private static int slider = 3;
	private static int waveselect = 4;
	private static int lfoselect = 5;
	private static int combo = 6;
	private static int simplerotary = 7;
	private static int radio = 8;

	public static Vector createMSTR(Patch patch, int offset) {
		Vector basslineControl = new Vector();
		basslineControl.add(new SIDSysexParameterControl(button, null, 0, patch, 112 + offset, 0, 1, "Legato"));
		basslineControl.add(new SIDSysexParameterControl(button, null, 0, patch, 112 + offset, 2, 1, "Fingered Portamento"));
		basslineControl.add(new SIDSysexParameterControl(button, null, 0, patch, 112 + offset, 1, 1, "Manual/Seq", "MIDI Note playing/Sequencer mode"));
		basslineControl.add(new SIDSysexParameterControl(button, null, 0, patch, 112 + offset, 3, 1, "Osc Phase Sync", "Oscillator phase synchronisation"));
		// OSC2
		basslineControl.add(new SIDSysexParameterControl(waveselect, null, 0, patch, 160 + offset, 0, 8, "Waveform", "Select a waveform"));
		basslineControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 161 + offset, 0, 12, "Pulse Width", "Pulse Width"));
		basslineControl.add(new SIDSysexParameterControl(rotary, SIDSysexInfo.bassOctaves, 0, patch, 163 + offset, 0, 3, "Octave"));
		basslineControl.add(new SIDSysexParameterControl(rotary, createStatic(), 0, patch, 164 + offset, 0, 7, "Static note"));
		// OSC3
		basslineControl.add(new SIDSysexParameterControl(waveselect, null, 0, patch, 168 + offset, 0, 8, "Waveform", "Select a waveform"));
		basslineControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 169 + offset, 0, 12, "Pulse Width", "Pulse Width"));
		basslineControl.add(new SIDSysexParameterControl(rotary, SIDSysexInfo.bassOctaves, 0, patch, 171 + offset, 0, 3, "Octave"));
		basslineControl.add(new SIDSysexParameterControl(rotary, createStatic(), 0, patch, 172 + offset, 0, 7, "Static note"));

		return basslineControl;
	}

	public static Vector createLFO(Patch patch, int offset) {
		Vector lfoControl = new Vector();
		// LFO1
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 116 + offset, 0, 1, "On/Off", "Enable/disable LFO"));
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 116 + offset, 1, 1, "Key Sync", "Key synchronisation"));
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 116 + offset, 2, 1, "MIDI Sync", "Global clock synchronisation"));
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 116 + offset, 3, 1, "Oneshot", "Oneshot mode"));
		lfoControl.add(new SIDSysexParameterControl(lfoselect, null, 0, patch, 116 + offset, 4, 4, "Waveform"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 117 + offset, 0, 8, "Pitch Depth", "Pitch modulation depth"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 118 + offset, 0, 8, "Rate")); // Let op CSn flag!
		lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 119 + offset, 0, 8, "Delay"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,	120 + offset, 0, 8, "Phase"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, -128, patch,	121 + offset, 0, 8, "PWM depth", "Pulse width modulation depth"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 122 + offset, 0, 8, "Cutoff depth","Filter cutoff modulation depth"));
		// LFO2
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 123 + offset, 0, 1, "On/Off", "Enable/disable LFO"));
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch,	116 + offset, 1, 1, "Key Sync", "Key synchronisation"));
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 123 + offset, 2, 1, "MIDI Sync", "Global clock synchronisation"));
		lfoControl.add(new SIDSysexParameterControl(button, null, 0, patch, 123 + offset, 3, 1, "Oneshot", "One shot mode"));
		lfoControl.add(new SIDSysexParameterControl(lfoselect, null, 0, patch, 123 + offset, 4, 4, "Waveform"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 124 + offset, 0, 8, "Pitch Depth", "Pitch modulation depth"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 125 + offset, 0, 8, "Rate"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 126 + offset, 0, 8, "Delay"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 127 + offset, 0, 8, "Phase"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 128 + offset, 0, 8, "PWM Depth", "Pulse width modulation depth"));
		lfoControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 129 + offset, 0, 8, "Cutoff Depth", "Filter cutoff modulation depth"));
		return lfoControl;
	}

	public static Vector createENV(Patch patch, int offset) {
		Vector envControl = new Vector();
		// Envelope
		envControl.add(new SIDSysexParameterControl(button, null, 0, patch, 130 + offset, 4, 1, "Curve to Attack", "Curve assigned to Attack rate"));
		envControl.add(new SIDSysexParameterControl(button, null, 0, patch, 130 + offset, 5, 1, "Curve to Decay", "Curve assigned to Decay rate"));
		envControl.add(new SIDSysexParameterControl(button, null, 0, patch, 130 + offset, 6, 1, "Curve to Sustain", "Curve assigned to Sustain rate"));
		envControl.add(new SIDSysexParameterControl(button, null, 0, patch, 130 + offset, 7, 1, "MIDI Sync", "Global clock synchronisation"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 131 + offset, 0, 8, "Depth Pitch", "Pitch modulation depth"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 132 + offset, 0, 8, "Depth PWM", "Pulse width modulation depth"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 133 + offset, 0, 8, "Depth Cutoff", "Filter cutoff modulation depth"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch, 134 + offset, 0, 8, "Attack", "Attack rate"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch, 135 + offset, 0, 8, "Decay", "Decay rate"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch, 136 + offset, 0, 8, "Sustain", "Sustain level"));
		envControl.add(new SIDSysexParameterControl(slider, null, 0, patch, 137 + offset, 0, 8, "Release", "Release rate"));
		envControl.add(new SIDSysexParameterControl(rotary, null, -128, patch, 138 + offset, 0, 8, "Curve"));
		envControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 144 + offset, 0, 8, "Accent Decay","Decay rate used on accented notes"));
		return envControl;
	}

	public static Vector createSEQ(Patch patch, int offset) {
		Vector seqControl = new Vector();
		// Sequencer
		seqControl.add(new SIDSysexParameterControl(rotary, null, 0, patch, 139 + offset, 0, 6, "Clock Divider", "Clock divider (sets tempo)"));
		seqControl.add(new SIDSysexParameterControl(button, null, 0, patch, 139 + offset, 7, 1, "Pattern Sync", "Pattern changes synchronised to measure"));
		seqControl.add(new SIDSysexParameterControl(radio, new String[]{"#1","#2","#3","#4","#5","#6","#7","#8","OFF"}, 0, patch, 140 + offset, 0, 4, "Sequence", "Sequencer number"));
		seqControl.add(new SIDSysexParameterControl(rotary, createSteps(), 0, patch, 141 + offset, 0, 4, "Steps", "Number of steps"));
		seqControl.add(new SIDSysexParameterControl(combo, SIDSysexInfo.basslineParams, 0, patch, 142 + offset, 0, 8, "Assign"));
		return seqControl;
	}

	public static Vector createSEQData(Patch patch) {
		Vector seqDataControl = new Vector();
		// Sequencer data
		for (int i = 0; i < 16 * 8; i++) {
			seqDataControl.add(new SIDSysexParameterControl(simplerotary, null, 0, patch, 256 + i, 0, 4, "Note"));
			seqDataControl.add(new SIDSysexParameterControl(simplerotary, SIDSysexInfo.octaves, 0, patch, 256 + i, 4, 2, "Oct", "Octave"));
			seqDataControl.add(new SIDSysexParameterControl(simplerotary, null, 0, patch, 384 + i, 0, 7, "Par", "Parameter value"));
			seqDataControl.add(new SIDSysexParameterControl(simplebutton, null,	0, patch, 256 + i, 6, 1, "Glide"));
			seqDataControl.add(new SIDSysexParameterControl(simplebutton, null, 0, patch, 384 + i, 7, 1, "Acc", "Accent"));
			seqDataControl.add(new SIDSysexParameterControl(simplebutton, null, 0, patch, 256 + i, 7, 1, "Gate"));
		}
		return seqDataControl;
	}
	
	public static String[] createStatic() {		
		String[] staticStr = new String[128];
		String[] notes = { "c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-", "g#", "a-", "a#", "b-" };

		for (int i = 0; i < 128; i++) {
			int octave = (int) Math.floor((i) / 12) - 2;
			if (octave < 0) {
				staticStr[i] = notes[(i) % 12] + Math.abs(octave);
			} else {
				staticStr[i] = notes[(i) % 12].toUpperCase() + Math.abs(octave);
			}
		}		
		staticStr[0] = "OFF";
		return staticStr;
	}
	
	public static String[] createSteps() {
		String[] s = new String[16];
		for (int i = 0; i < s.length; i++) {
			s[i] = Integer.toString(i + 1);
		}
		return s;
	}
}
