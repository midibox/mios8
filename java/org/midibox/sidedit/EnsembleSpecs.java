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

public class EnsembleSpecs {
	// Control types:
	private static int rotary = 0;
	private static int button = 1;
	private static int simplebutton = 2;
	private static int slider = 3;
	private static int waveselect = 4;
	private static int lfoselect = 5;
	private static int combo = 6;

	public static Vector createENSSID(Patch patch, int offset) {
		Vector ensControl = new Vector();
		ensControl.add(new SIDSysexParameterControl(rotary, new String[] { "A",
				"B", "C", "D", "E", "F", "G", "H" }, 0, patch, 0 + offset, 0,
				3, "Bank"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 1, patch,
				1 + offset, 0, 7, "Patch"));
		ensControl.add(new SIDSysexParameterControl(combo, new String[] {
				"Master", "Slave", "Auto", "" }, 0, patch, 2 + offset, 0, 2,
				"BPM generator mode"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				2 + offset, 4, 1, "LogFilter", "Logarithmic filter cutoff"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				2 + offset, 6, 1, "Mono"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				2 + offset, 7, 1, "DOR",
				"Disable automatic Oscillator Reset during patch change"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				3 + offset, 0, 1, "F2A", "Filter to Analog"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				3 + offset, 1, 1, "V2A", "Volume to Analog"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				3 + offset, 2, 1, "P2A", "Pulsewidth to Analog"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				3 + offset, 3, 1, "K2A", "Key to Analog"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				3 + offset, 4, 1, "O2A", "Oscillator Frequency to Analog"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				5 + offset, 0, 8, "BPM rate"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				6 + offset, 0, 3, "SPM", "Super Poly Mode"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 0, 1, "AOUT1"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 1, 1, "AOUT2"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 2, 1, "AOUT3"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 3, 1, "AOUT4"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 4, 1, "AOUT5"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 5, 1, "AOUT6"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 6, 1, "AOUT7"));
		ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
				7 + offset, 7, 1, "AOUT8"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				8 + offset, 0, 16, "Filter Lmin"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				10 + offset, 0, 16, "Filter Lmax"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				12 + offset, 0, 16, "Filter Rmin"));
		ensControl.add(new SIDSysexParameterControl(rotary, null, 0, patch,
				14 + offset, 0, 16, "Filter Rmax"));
		for (int i = 0; i < 6; i++) {
			ensControl.add(new SIDSysexParameterControl(rotary, null, 1, patch,
					16 + offset + (i * 8), 0, 4, "MIDI chan"));
			ensControl.add(new SIDSysexParameterControl(button, null, 0, patch,
					16 + offset + (i * 8), 4, 1, "Disable"));
			ensControl.add(new SIDSysexParameterControl(rotary, createNotes(),
					0, patch, 17 + offset + (i * 8), 0, 7, "LowerSplit"));
			ensControl.add(new SIDSysexParameterControl(rotary, createNotes(),
					0, patch, 18 + offset + (i * 8), 0, 7, "UpperSplit"));
			ensControl.add(new SIDSysexParameterControl(rotary, null, -64,
					patch, 19 + offset + (i * 8), 0, 7, "Transpose"));
		}
		return ensControl;
	}

	public static String[] createNotes() {
		String[] staticStr = new String[128];
		String[] notes = { "c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-",
				"g#", "a-", "a#", "b-" };

		for (int i = 0; i < 128; i++) {
			int octave = (int) Math.floor((i) / 12) - 2;
			if (octave < 0) {
				staticStr[i] = notes[(i) % 12] + Math.abs(octave);
			} else {
				staticStr[i] = notes[(i) % 12].toUpperCase() + Math.abs(octave);
			}
		}
		return staticStr;
	}
}
