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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

import org.midibox.sidedit.SIDSysexInfo;

public class Ensemble implements Receiver {
	private boolean[] cores = { false, false, false, false };
	protected int[] ensemble = new int[256];

	protected Receiver receiver;

	public Ensemble(Receiver receiver) {
		this.receiver = receiver;
		initEnsemble();
	}

	private void initEnsemble() {
		for (int c = 0; c < 256; c++) {
			ensemble[c] = 0;
		}
		setEnsembleName("                ");
	}

	public void setEnsembleName(String s) {
		for (int i = 0; i < 16; i++) {
			int value = ((int) s.charAt(i)) & 0xFF;
			ensemble[i] = nibbleSwap(value);
			sysexSend(i, value, 1);
		}
	}

	public String getEnsembleName() {
		String s = new String("");
		for (int i = 0; i < 16; i++) {
			s = s + (char) nibbleSwap(ensemble[i]);
		}
		return s;
	}

	public String getSysexString() {
		String s = "";
		int sum = 0;
		for (int i = 0; i < ensemble.length; i++) {
			String newStr = calcValue(nibbleSwap(ensemble[i]), 1);
			sum = sum + Integer.parseInt(newStr.substring(1, 2), 16);
			sum = sum + Integer.parseInt(newStr.substring(3, 4), 16);
			s = s + newStr;
		}
		String checksum = Integer.toHexString(-sum & 0x7F);
		if (checksum.length() == 1) {
			checksum = "0" + checksum;
		}
		return (s + checksum);
	}

	public String parseEnsemble(String d) {
		String status;
		int dsi = 20; // Index where real patch data starts
		int checksum = 0;
		for (int i = 0; i < 512; i++) {
			String s1 = d.substring((4 * i) + dsi + 1, (4 * i) + dsi + 2);
			String s2 = d.substring((4 * i) + dsi + 3, (4 * i) + dsi + 4);
			checksum = checksum + Integer.parseInt(s1, 16);
			checksum = checksum + Integer.parseInt(s2, 16);
			int temp = Integer.parseInt(s1 + s2, 16);
			ensemble[i] = temp;
		}
		int chk = Integer.parseInt(d.substring((4 * 512) + dsi, (4 * 512) + dsi
				+ 2), 16);
		if (chk != (-checksum & 0x7F)) {
			status = "checksum error";
		} else {
			status = "succesful";
		}
		return status;
	}

	public void setEnsemble(int[] p) {
		ensemble = p;
	}

	public int[] getEnsemble() {
		return ensemble;
	}

	public int getParameter(int address, int start_bit, int resolution) {
		int value = 0;
		int absres = Math.abs(resolution);

		// resolution < 8 bit: gets value of bit length 'resolution' starting on
		// the start_bit
		if (absres < 8) {
			int bitmask = 0;
			for (int j = 0; j < absres; j++) { // Iterate over all bits
				bitmask = bitmask | (int) Math.pow(2, (start_bit + j));
			}
			value = (bitmask & nibbleSwap(ensemble[address])) >> start_bit;
		}

		// 8 bit values
		if (absres == 8) {
			value = nibbleSwap(ensemble[address]);
		}

		// 12 bit values
		if (absres == 12) {
			int temp1 = nibbleSwap(ensemble[address]);
			int temp2 = nibbleSwap(ensemble[address + 1] & 0xF0);
			value = (temp2 << 8) + temp1;
		}

		// 16 bit values
		if (absres == 16) {
			int temp1 = nibbleSwap(ensemble[address]);
			int temp2 = nibbleSwap(ensemble[address + 1]);
			value = (temp2 << 8) + temp1;
		}

		// Shift positive integers down to make them bipolar (if required)
		if (resolution < 0) {
			int midimin = -((int) Math.ceil((Math.pow(2, absres) - 1) / 2));
			value = value + midimin;
		}

		return value;
	}

	public void setParameter(int address, int value, int start_bit,
			int resolution) {
		int absres = Math.abs(resolution);

		// Shift all bipolar values up to make them fit into a positive integer
		if (resolution < 0) {
			int midimin = -((int) Math.ceil((Math.pow(2, absres) - 1) / 2));
			value = value - midimin;
		}

		// resolution < 8 bit: sets value of bit length 'resolution' starting on
		// the start_bit
		if (absres < 8) {
			int temp = nibbleSwap(ensemble[address]); // Retrieve data

			for (int j = 0; j < absres; j++) { // Iterate over all bits
				int this_bit = start_bit + j;
				int bitmask0 = (int) Math.pow(2, j); // Bitmask for checking if
				// current bit needs to
				// change
				int bitmask1 = (int) Math.pow(2, (this_bit)); // Bitmask for
				// turning bits
				// on
				int bitmask2 = (~bitmask1) & 0xFF; // Bitmask for turning bits
				// off
				if ((value & bitmask0) == 0) {
					temp = (temp & bitmask2) & 0xFF; // Turn bit off
				}
				if ((value & bitmask0) != 0) {
					temp = (temp | bitmask1) & 0xFF; // Turn bit on
				}
			}
			ensemble[address] = nibbleSwap(temp); // Put back new data
			sysexSend(address, temp, 1);
		}

		// 8 bit values
		if (absres == 8) {
			ensemble[address] = nibbleSwap(value & 0xFF);
			sysexSend(address, (value & 0xFF), 1);
		}

		// 12 bit values
		if (absres == 12) {
			int temp = nibbleSwap(ensemble[address + 1]); // Retrieve data
			temp = (temp & 0xFF00);
			ensemble[address] = nibbleSwap(value & 0xFF);
			ensemble[address + 1] = nibbleSwap(temp + ((value & 0xF00) >> 8));
			sysexSend(address, ((temp << 4) + (value & 0xFFF)), 2);
		}

		// 16 bit values
		if (absres == 16) {
			ensemble[address] = nibbleSwap(value & 0xFF);
			ensemble[address + 1] = nibbleSwap((value & 0xFF00) >> 8);
			sysexSend(address, (value & 0xFFFF), 2);
		}
	}

	private static int nibbleSwap(int inInt) {
		int nibble0 = (((byte) inInt) << 4) & 0xf0;
		int nibble1 = (((byte) inInt) >>> 4) & 0x0f;
		return (nibble0 | nibble1);
	}

	public void sysexSend(int addr, int value, int bytes) {
		for (int c = 0; c < cores.length; c++) {
			if (cores[c]) {
				SysexMessage sysexMessage = new SysexMessage();
				try {
					String strMessage = SIDSysexInfo.editEsembleParameterSysex;
					strMessage = strMessage.replace("<device>", "0"
							+ Integer.toString(c));
					strMessage = strMessage
							.replace("<address>", calcAddr(addr));
					strMessage = strMessage.replace("<value>", calcValue(value,
							bytes));

					int nLengthInBytes = strMessage.length() / 2;
					byte[] abMessage = new byte[nLengthInBytes];
					for (int i = 0; i < nLengthInBytes; i++) {
						abMessage[i] = (byte) Integer.parseInt(strMessage
								.substring(i * 2, i * 2 + 2), 16);
					}

					System.out.println(strMessage);
					sysexMessage.setMessage(abMessage, abMessage.length);
				} catch (Exception e) {
					System.out.println("exception!");
					System.out.println(e);
				}
				receiver.send(sysexMessage, -1);
			}
		}
	}

	public void setCores(boolean[] c) {
		cores = c;
	}

	public Boolean getCore(int i) {
		return cores[i];
	}

	protected String calcAddr(int addr) {
		String str0 = Integer.toHexString(addr & 0x00F);
		String str1 = Integer.toHexString((addr & 0x070) >> 4);
		String str2 = Integer.toHexString((addr & 0x180) >> 7);
		return ("0" + str2 + str1 + str0);
	}

	protected String calcValue(int val, int byt) {
		String s = "";
		if (byt == 1) {
			String s0 = "0";
			String s1 = Integer.toHexString(val & 0x0F);
			String s2 = Integer.toHexString((val & 0xF0) >> 4);
			s = (s0 + s1 + s0 + s2);
		} else if (byt == 2) {
			String s0 = "0";
			String s1 = Integer.toHexString(val & 0x000F);
			String s2 = Integer.toHexString((val & 0x00F0) >> 4);
			String s3 = Integer.toHexString((val & 0x0F00) >> 8);
			String s4 = Integer.toHexString((val & 0xF000) >> 12);
			s = (s0 + s1 + s0 + s2 + s0 + s3 + s0 + s4);
		}
		return s;
	}

	public void close() {
	}

	public Receiver getReceiver() {
		return receiver;
	}

	public void send(MidiMessage message, long lTimeStamp) {

	}

	public Ensemble clone() {
		Ensemble p = new Ensemble(receiver);
		p.setEnsemble(ensemble.clone());
		return p;
	}
}
