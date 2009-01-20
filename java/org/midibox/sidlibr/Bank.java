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

package org.midibox.sidlibr;

import javax.sound.midi.Receiver;

public class Bank {
	private Patch[] bank;
	public static int patchSize = 2072; // Number of SysEx string characters
	public static int bankSize = 128;
	private Receiver receiver;

	public Bank(Receiver receiver) {
		this.receiver = receiver;
		initBank();
	}

	public void initBank() {
		bank = new Patch[bankSize];
		for (int c = 0; c < bankSize; c++) {
			bank[c] = new Patch(receiver);
		}
	}

	public void setPatchAt(int i, Patch p) {
		bank[i] = p;
	}

	public Patch getPatchAt(int i) {
		return bank[i];
	}

	public String parseBankSyx(String syx) {
		String status = "succesful";
		try {
			initBank();
			for (int i = 0; i < syx.length() / patchSize; i++) {
				bank[i] = new Patch(receiver);
				String stat = bank[i].parsePatch(syx.substring(i * patchSize,
						(i + 1) * patchSize));
				if (status == "checksum error") {
					status = stat;
					break;
				}
			}
		} catch (Exception e) {
			status = "parse error";
		}
		return status;
	}
}
