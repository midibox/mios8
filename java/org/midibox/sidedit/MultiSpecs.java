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

public class MultiSpecs {
	// Control types:
	private static int rotary = 0;
	private static int button = 1;
	private static int simplebutton = 2;
	private static int slider = 3;
	private static int waveselect = 4;
	private static int lfoselect = 5;
	private static int combo = 6;
	private static int simplerotary = 7;

	public static Vector createMSTR(Patch patch) {
		Vector drumControl = new Vector();
		drumControl.add(new SIDSysexParameterControl(rotary, null, patch, 82,
				0, 7, "Volume"));
		return drumControl;
	}

	public static Vector createInstrument(Patch patch, int offset) {
		Vector multiControl = new Vector();
		// Master
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				112 + offset, 3, 1, "Poly", "Switches between Poly/Mono mode"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				112 + offset, 0, 1, "Legato"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				112 + offset, 2, 1, "Fingered Portamento"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				112 + offset, 1, 1, "Wavetable only"));
		multiControl.add(new SIDSysexParameterControl(rotary,
				SIDSysexInfo.voiceAssign, patch, 113 + offset, 0, 4,
				"Voice Assign", "Voice assignment"));
		multiControl.add(new SIDSysexParameterControl(combo,
				SIDSysexInfo.multiParams, patch, 114 + offset, 0, 8,
				"Velocity Assign", "Velocity parameter assignment"));
		multiControl.add(new SIDSysexParameterControl(combo,
				SIDSysexInfo.multiParams, patch, 115 + offset, 0, 8,
				"PitchB. Assign", "Pitch Bender assignment"));

		// LFO 1&2
		for (int i = 0; i < 2; i++) {
			multiControl.add(new SIDSysexParameterControl(button, null, patch,
					116 + (7 * i) + offset, 0, 1, "On/Off",
					"Enable/disable LFO"));
			multiControl.add(new SIDSysexParameterControl(button, null, patch,
					116 + (7 * i) + offset, 1, 1, "Key Sync",
					"Key synchronisation"));
			multiControl.add(new SIDSysexParameterControl(button, null, patch,
					116 + (7 * i) + offset, 2, 1, "MIDI Sync",
					"Global clock synchronisation"));
			multiControl.add(new SIDSysexParameterControl(button, null, patch,
					116 + (7 * i) + offset, 3, 1, "Oneshot", "Oneshot mode"));
			multiControl.add(new SIDSysexParameterControl(lfoselect, null,
					patch, 116 + (7 * i) + offset, 4, 4, "Waveform"));
			multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
					117 + (7 * i) + offset, 0, -8, "Pitch Depth",
					"Pitch modulation depth"));
			multiControl.add(new SIDSysexParameterControl(rotary,
					createRateStr(), patch, 118 + (7 * i) + offset, 0, 8,
					"Rate")); // Let op CSn flag!
			multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
					119 + (7 * i) + offset, 0, 8, "Delay"));
			multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
					120 + (7 * i) + offset, 0, 8, "Phase"));
			multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
					121 + (7 * i) + offset, 0, -8, "PWM Depth",
					"Pulse width modulation depth"));
			multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
					122 + (7 * i) + offset, 0, -8, "Cutoff Depth",
					"Filter cutoff modulation depth"));
		}

		// Envelope
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				130 + offset, 4, 1, "Curve to Attack",
				"Curve assigned to Attack rate"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				130 + offset, 5, 1, "Curve to Decay",
				"Curve assigned to Decay rate"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				130 + offset, 6, 1, "Curve to Sustain",
				"Curve assigned to Sustain rate"));
		multiControl
				.add(new SIDSysexParameterControl(button, null, patch,
						130 + offset, 7, 1, "MIDI Sync",
						"Global clock synchronisation"));
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				131 + offset, 0, -8, "Depth Pitch", "Pitch modulation depth"));
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				132 + offset, 0, -8, "Depth PWM",
				"Pulse width modulation depth"));
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				133 + offset, 0, -8, "Depth Cutoff",
				"Filter cutoff modulation depth"));
		multiControl.add(new SIDSysexParameterControl(slider, null, patch,
				134 + offset, 0, 8, "Attack", "Attack rate"));
		multiControl.add(new SIDSysexParameterControl(slider, null, patch,
				135 + offset, 0, 8, "Decay", "Decay rate"));
		multiControl.add(new SIDSysexParameterControl(slider, null, patch,
				136 + offset, 0, 8, "Sustain", "Sustain level"));
		multiControl.add(new SIDSysexParameterControl(slider, null, patch,
				137 + offset, 0, 8, "Release", "Release rate"));
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				138 + offset, 0, -8, "Curve"));

		// Wavetable
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				139 + offset, 0, 6, "Clock Divider",
				"Clock divider (sets tempo)"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				139 + offset, 6, 1, "reserved", "Reserved"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				139 + offset, 7, 1, "reserved", "Reserved"));
		multiControl.add(new SIDSysexParameterControl(combo,
				SIDSysexInfo.multiParams, patch, 140 + offset, 0, 8, "Assign",
				"Parameter assignment"));
		multiControl
				.add(new SIDSysexParameterControl(rotary, null, patch,
						141 + offset, 0, 7, "Start Pos",
						"Start position in wavetable"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				141 + offset, 7, 1, "reserved", "reserved flag"));
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				142 + offset, 0, 7, "End Pos", "End position in wavetable"));
		multiControl.add(new SIDSysexParameterControl(rotary, null, patch,
				143 + offset, 0, 7, "Loop Pos", "Loop position in wavetable"));
		multiControl.add(new SIDSysexParameterControl(button, null, patch,
				143 + offset, 7, 1, "Oneshot"));

		return multiControl;
	}

	public static Vector createWTdata(Patch patch) {
		Vector wtControl = new Vector();
		// Wavetable data
		for (int c = 0; c < 128; c++) {
			wtControl.add(new SIDSysexParameterControl(slider, createWTStr(),
					patch, 384 + c, 0, 8, Integer.toString(c)));
		}
		return wtControl;
	}

	private static String[] createRateStr() {
		String[] rateStr = new String[256];
		for (int c = 0; c < rateStr.length; c++) {

			if (c < 245) {
				rateStr[c] = Integer.toString(c);
			} else {
				rateStr[c] = SIDSysexInfo.rates[c - 245];
			}
		}
		return rateStr;
	}

	private static String[] createWTStr() {
		String[] wtStr = new String[256];
		String[] notes = { "c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-",
				"g#", "a-", "a#", "b-" };

		for (int i = 0; i < 256; i++) {
			if (i < 64) {
				wtStr[i] = Integer.toString(i - 64);
			} else if (i < 128) {
				wtStr[i] = "+" + Integer.toString(i - 64);
			} else if (i == 128) {
				wtStr[i] = "---";
			} else if (i == 129) {
				wtStr[i] = "+++";
			} else if (i < 252) {
				int octave = (int) Math.floor((i - 128) / 12) - 2;
				if (octave < 0) {
					wtStr[i] = notes[(i - 128) % 12] + Math.abs(octave);
				} else {
					wtStr[i] = notes[(i - 128) % 12].toUpperCase()
							+ Math.abs(octave);
				}
			} else if (i < 256) {
				wtStr[i] = "Ky" + Integer.toString(i - 251);
			}
		}
		return wtStr;
	}
}
