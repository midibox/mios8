/*
 * @(#)MIOSTerminal.java
 *
 * Copyright (C) 2008    Thorsten Klose (tk@midibox.org)
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
 *
 */

package org.midibox.mios;

import javax.sound.midi.SysexMessage;

import org.midibox.midi.MidiMonitor;

public class MIOSTerminal extends MidiMonitor {

	public String decodeMessage(SysexMessage message) {
		byte[] abData = message.getData();
		String strMessage = null;
		// System.out.println("sysex status: " + message.getStatus());
		if (message.getStatus() == SysexMessage.SYSTEM_EXCLUSIVE) {

			// Print MIOS debug message in text format
			if (abData[0] == 0x00
					&& // MIOS32 Header
					abData[1] == 0x00 && abData[2] == 0x7e && abData[3] == 0x32
					&&
					// abData[4] == 0x00 && // ignore Device ID
					abData[5] == 0x0d && // debug message
					abData[6] == 0x40) // string message
			{
				int i;
				strMessage = "";
				for (i = 7; i < abData.length - 1; ++i) {
					if (!(abData[i] == 0x0a && i == (abData.length - 2))) { // ommit
						// last
						// \n
						strMessage += (char) abData[i];
					}
				}
			}
		}
		return strMessage;
	}
}
