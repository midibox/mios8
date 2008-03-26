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

public class EnsBank  {
	private Ensemble[] bank;
	public static int ensSize = 1048;  // Number of SysEx string characters
	public static int bankSize = 128;
	private Receiver receiver;
	
	public EnsBank(Receiver receiver) {
		this.receiver = receiver;
		initBank();
	}
	
	public void initBank() {
		bank = new Ensemble[bankSize];
		for (int c=0;c<bankSize;c++) {
			bank[c] = new Ensemble(receiver);
		}
	}
	
	public void setEnsembleAt(int i, Ensemble p) {
		bank[i] = p;
	}
	
	public Ensemble getEnsembleAt(int i) {
		return bank[i];
	}
	
	public String parseBankSyx(String syx){		
		String status = "succesful";
		try {	
			initBank();
			for (int i=0;i<syx.length()/ensSize;i++) {
				bank[i] = new Ensemble(receiver);
				String stat = bank[i].parseEnsemble(syx.substring(i*ensSize,(i+1)*ensSize));
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
