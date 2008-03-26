/*
 * @(#)MidiUtils.java	beta7	2006/04/23
 *
 * Copyright (C) 2006    Adam King (adamjking@optusnet.com.au)
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

package org.midibox.midi;

import javax.sound.midi.ShortMessage;

public class MidiUtils {

	public static String[] ccNames = { "Bank Select", "Modulation Wheel",
			"Breath Controller", "Undefined", "Foot Controller",
			"Portamento Time", "Data Entry", "Volume", "Balance",
			"Undefined", "Pan", "Expression Controller", "Effect Control 1",
			"Effect Control 2", "Undefined", "Undefined", "GP Controller 1",
			"GP Controller 2", "GP Controller 3", "GP Controller 4",
			"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
			"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
			"Undefined", "Undefined", "LSB 0 (Bank Select)",
			"LSB 1 (Modulation Wheel)", "LSB 2 (Breath Controller)",
			"LSB 3 (Undefined)", "LSB 4 (Foot Controller)",
			"LSB 5 (Portamento Time)", "LSB 6 (Data Entry)",
			"LSB 7 (Channel Volume)", "LSB 8 (Balance)", "LSB 9 (Undefined)",
			"LSB 10 (Pan)", "LSB 11 (Expression Controller)",
			"LSB 12 (Effect  1)", "LSB 13 (Effect  2)", "LSB 14 (Undefined)",
			"LSB 15 (Undefined)", "LSB 16 (GP ler 1)", "LSB 17 (GP ler 2)",
			"LSB 18 (GP ler 3)", "LSB 19 (GP ler 4)", "LSB 20 (Undefined)",
			"LSB 21 (Undefined)", "LSB 22 (Undefined)", "LSB 23 (Undefined)",
			"LSB 24 (Undefined)", "LSB 25 (Undefined)", "LSB 26 (Undefined)",
			"LSB 27 (Undefined)", "LSB 28 (Undefined)", "LSB 29 (Undefined)",
			"LSB 30 (Undefined)", "LSB 31 (Undefined)",
			"Damper Pedal On/Off (Sustain)", "Portamento On/Off",
			"Sustenuto On/Off", "Soft Pedal On/Off", "Legato Footswitch",
			"Hold 2", "Sound Controller 1", "Sound Controller 2",
			"Sound Controller 3", "Sound Controller 4", "Sound Controller 5",
			"Sound Controller 6", "Sound Controller 7", "Sound Controller 8",
			"Sound Controller 9", "Sound Controller 10", "GP Controller 5",
			"GP Controller 6", "GP Controller 7", "GP Controller 8",
			"Portamento Control", "Undefined", "Undefined", "Undefined",
			"Undefined", "Undefined", "Undefined", "Effects 1 Depth",
			"Effects 2 Depth", "Effects 3 Depth", "Effects 4 Depth",
			"Effects 5 Depth", "Data Increment", "Data Decrement",
			"Non-Reg. Param. No. - LSB", "Non-Reg. Param. No. - MSB",
			"Reg. Param. No. - LSB", "Reg. Param. No. - MSB", "Undefined",
			"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
			"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
			"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
			"Undefined", "Undefined", "All Sound Off", "Reset All Controllers",
			"Local Control On/Off", "All Notes Off", "Omni Mode Off",
			"Omni Mode On", "Poly Mode On/Off", "Poly Mode On" };
	
	public static String[] cc14BitNames = { "Bank Select (14 bit)", "Modulation Wheel (14 bit)",
		"Breath Controller (14 bit)", "Undefined (14 bit)", "Foot Controller (14 bit)",
		"Portamento Time (14 bit)", "Data Entry MSB (14 bit)", "Volume (14 bit)", "Balance (14 bit)",
		"Undefined (14 bit)", "Pan (14 bit)", "Expression Controller (14 bit)", "Effect Control 1 (14 bit)",
		"Effect Control 2 (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "GP Controller 1 (14 bit)",
		"GP Controller 2 (14 bit)", "GP Controller 3 (14 bit)", "GP Controller 4 (14 bit)",
		"Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)",
		"Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)",
		"Undefined (14 bit)", "Undefined (14 bit)", "Bank Select (14 bit)", "Modulation Wheel (14 bit)",
		"Breath Controller (14 bit)", "Undefined (14 bit)", "Foot Controller (14 bit)",
		"Portamento Time (14 bit)", "Data Entry MSB (14 bit)", "Volume (14 bit)", "Balance (14 bit)",
		"Undefined (14 bit)", "Pan (14 bit)", "Expression Controller (14 bit)", "Effect Control 1 (14 bit)",
		"Effect Control 2 (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "GP Controller 1 (14 bit)",
		"GP Controller 2 (14 bit)", "GP Controller 3 (14 bit)", "GP Controller 4 (14 bit)",
		"Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)",
		"Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)", "Undefined (14 bit)",
		"Undefined (14 bit)", "Undefined (14 bit)", 
		"Damper Pedal On/Off (Sustain)", "Portamento On/Off",
		"Sustenuto On/Off", "Soft Pedal On/Off", "Legato Footswitch",
		"Hold 2", "Sound Controller 1", "Sound Controller 2",
		"Sound Controller 3", "Sound Controller 4", "Sound Controller 5",
		"Sound Controller 6", "Sound Controller 7", "Sound Controller 8",
		"Sound Controller 9", "Sound Controller 10", "GP Controller 5",
		"GP Controller 6", "GP Controller 7", "GP Controller 8",
		"Portamento Control", "Undefined", "Undefined", "Undefined",
		"Undefined", "Undefined", "Undefined", "Effects 1 Depth",
		"Effects 2 Depth", "Effects 3 Depth", "Effects 4 Depth",
		"Effects 5 Depth", "Data Increment", "Data Decrement",
		"Non-Reg. Param. No. (14 bit)", "Non-Reg. Param. No. (14 bit)",
		"Reg. Param. No. (14 bit)", "Reg. Param. No. (14 bit)", "Undefined",
		"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
		"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
		"Undefined", "Undefined", "Undefined", "Undefined", "Undefined",
		"Undefined", "Undefined", "All Sound Off", "Reset All Controllers",
		"Local Control On/Off", "All Notes Off", "Omni Mode Off",
		"Omni Mode On", "Poly Mode On/Off", "Poly Mode On" };
	
	public static final String[] s_astrKeyNames = { "C", "C#", "D", "D#", "E",
			"F", "F#", "G", "G#", "A", "A#", "B" };

	public static final String[] s_astrKeySignatures = { "Cb", "Gb", "Db",
			"Ab", "Eb", "Bb", "F", "C", "G", "D", "A", "E", "B", "F#", "C#" };

	public static final String[] SYSTEM_MESSAGE_TEXT = {
			"System Exclusive (should not be in ShortMessage!)",
			"MTC Quarter Frame: ", "Song Position: ", "Song Select: ",
			"Undefined", "Undefined", "Tune Request",
			"End of SysEx (should not be in ShortMessage!)", "Timing clock",
			"Undefined", "Start", "Continue", "Stop", "Undefined",
			"Active Sensing", "System Reset" };

	public static final String[] QUARTER_FRAME_MESSAGE_TEXT = {
			"frame count LS: ", "frame count MS: ", "seconds count LS: ",
			"seconds count MS: ", "minutes count LS: ", "minutes count MS: ",
			"hours count LS: ", "hours count MS: " };

	public static final String[] FRAME_TYPE_TEXT = { "24 frames/second",
			"25 frames/second", "30 frames/second (drop)",
			"30 frames/second (non-drop)", };

	public static final char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	public static String getKeyName(int nKeyNumber) {
		if (nKeyNumber > 127) {
			return "illegal value";
		} else {
			int nNote = nKeyNumber % 12;
			int nOctave = nKeyNumber / 12;
			return s_astrKeyNames[nNote] + (nOctave - 1);
		}
	}

	public static int get14bitValue(int nLowerPart, int nHigherPart) {
		return (nLowerPart & 0x7F) | ((nHigherPart & 0x7F) << 7);
	}

	public static int signedByteToUnsigned(byte b) {
		return b & 0xFF;
	}

	// convert from microseconds per quarter note to beats per minute and vice
	// versa
	public static float convertTempo(float value) {
		if (value <= 0) {
			value = 0.1f;
		}
		return 60000000.0f / value;
	}

	public static String getHexString(byte[] aByte) {
		StringBuffer sbuf = new StringBuffer(aByte.length * 3 + 2);
		for (int i = 0; i < aByte.length; i++) {
			if (i > 0) {
				sbuf.append(' ');
			}
			sbuf.append(getHexString((int) aByte[i]));
		}
		return new String(sbuf);
	}

	public static String getHexString(int num) {
		StringBuffer sbuf = new StringBuffer();
		sbuf.append(hexDigits[(num & 0xF0) >> 4]);
		sbuf.append(hexDigits[num & 0x0F]);

		return new String(sbuf);
	}

	public static String getDecimalString(byte[] aByte) {
		StringBuffer sbuf = new StringBuffer(aByte.length * 4 + 2);
		for (int i = 0; i < aByte.length; i++) {
			sbuf.append(' ');
			sbuf.append(getDecimalString((int) aByte[i]));
		}
		return new String(sbuf);
	}

	public static String getDecimalString(int num) {
		StringBuffer sbuf = new StringBuffer();

		sbuf.append("000");
		String value = "" + (num & 0xFF);
		sbuf.replace(sbuf.length() - value.length(), sbuf.length(), value);

		return new String(sbuf);
	}

	public static String intToHex(int i) {
		return "" + hexDigits[(i & 0xF0) >> 4] + hexDigits[i & 0x0F];
	}

	public static String getHexString(ShortMessage sm) {
		// bug in J2SDK 1.4.1
		// return getHexString(sm.getMessage());
		int status = sm.getStatus();
		String res = intToHex(sm.getStatus());
		// if one-byte message, return
		switch (status) {
		case 0xF6: // Tune Request
		case 0xF7: // EOX
		// System real-time messages
		case 0xF8: // Timing Clock
		case 0xF9: // Undefined
		case 0xFA: // Start
		case 0xFB: // Continue
		case 0xFC: // Stop
		case 0xFD: // Undefined
		case 0xFE: // Active Sensing
		case 0xFF:
			return res;
		}
		res += ' ' + intToHex(sm.getData1());
		// if 2-byte message, return
		switch (status) {
		case 0xF1: // MTC Quarter Frame
		case 0xF3: // Song Select
			return res;
		}
		// switch (sm.getCommand()) {
		// case 0xC0:
		// case 0xD0:
		// return res;
		// }
		// 3-byte messages left
		res += ' ' + intToHex(sm.getData2());
		return res;
	}
}
