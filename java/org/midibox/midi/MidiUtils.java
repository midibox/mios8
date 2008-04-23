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

	public static final char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	public static int get14bitValue(int nLowerPart, int nHigherPart) {
		return (nLowerPart & 0x7F) | ((nHigherPart & 0x7F) << 7);
	}

	public static int signedByteToUnsigned(byte b) {
		return b & 0xFF;
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
