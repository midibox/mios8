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

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URL;
import java.net.URISyntaxException;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

import org.midibox.midi.MidiUtils;
import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.utils.gui.ImageLoader;

public class Patch implements Receiver {
	public static Object LEAD = new Object();
	public static Object BASSLINE = new Object();
	public static Object DRUM = new Object();
	public static Object MULTI = new Object();
	private static int delay = 10; // Delay in ms between Sysex Strings
	private int patchSize;
	private boolean isEnsemble;
	protected int[] patch;
	private int WOPT = 0;
	private int maxSysExLength = 0;
	protected Receiver receiver;
	private InitPatches initPatches;

	public Patch(Receiver receiver, int patchSize, InitPatches initPatches) {
		this.receiver = receiver;
		this.patchSize = patchSize;
		this.initPatches = initPatches;
		if (patchSize==256) {
			isEnsemble = true;
		} else {
			isEnsemble = false;
		}
		patch = new int[patchSize];
		initPatch();
	}

	private void initPatch() {		
		if (isEnsemble) {
			patch = initPatches.getInitEnsemble();
		} else {
			switch (getParameter(16, 0, 2)) {
			case 0:
				patch = initPatches.getInitLead();
				break;
			case 1:
				patch = initPatches.getInitBassline();
				break;
			case 2:
				patch = initPatches.getInitDrum();
				break;
			case 3:
				patch = initPatches.getInitMulti();
				break;
			}
		}
	}

	public void setPatchName(String s) {
		for (int i = 0; i < 16; i++) {
			int value = ((int) s.charAt(i)) & 0xFF;
			patch[i] = nibbleSwap(value);
		}
	}

	public String getPatchName() {
		String s = new String("");
		for (int i = 0; i < 16; i++) {
			s = s + (char) nibbleSwap(patch[i]);
		}
		return s;
	}

	public Object getEngine() {
		Object engine = new Object();
		switch (getParameter(16, 0, 2)) {
		case 0:
			engine = LEAD;
			break;
		case 1:
			engine = BASSLINE;
			break;
		case 2:
			engine = DRUM;
			break;
		case 3:
			engine = MULTI;
			break;
		default:
			System.out.println("Unknown engine!");
			break;
		}
		return engine;
	}

	public String getEngineStr() {
		String engine = "";
		switch (getParameter(16, 0, 2)) {
		case 0:
			engine = "LEAD";
			break;
		case 1:
			engine = "BASSLINE";
			break;
		case 2:
			engine = "DRUM";
			break;
		case 3:
			engine = "MULTI";
			break;
		default:
			System.out.println("Unknown engine!");
			break;
		}
		return engine;
	}

	public String getSysexString() {
		String s = "";
		int sum = 0;
		for (int i = 0; i < patch.length; i++) {
			String newStr = calcValue(nibbleSwap(patch[i]), 1);
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
	
	public byte[] getSysex(String pre, String post) {
		byte[] preByte = toByteArray(pre);
		byte[] postByte = toByteArray(post);
		int pr = preByte.length;
		int po = postByte.length;
		
		byte[] data = new byte[(2*patch.length)+1+pr+po];
		System.arraycopy(preByte, 0, data, 0, pr);   
		System.arraycopy(postByte, 0, data, data.length-po, po);
		
		int sum = 0;
		for (int i = 0; i < patch.length; i++) {
			data[(2*i)+pr] = (byte)((patch[i] & 0xF0) >> 4);			
			data[(2*i)+pr+1] = (byte)(patch[i] & 0x0F);
			sum = sum + (patch[i] & 0x0F) + ((patch[i] & 0xF0) >> 4);
		}
		data[2*patch.length+pr] = (byte)(-sum & 0x7F); // Checksum		
		return data;
	}
	
	public String parsePatch(String d) {
		String status;
		int dsi = 20; // Index where real patch data starts
		int checksum = 0;
		for (int i = 0; i < patchSize; i++) {
			String s1 = d.substring((4 * i) + dsi + 1, (4 * i) + dsi + 2);
			String s2 = d.substring((4 * i) + dsi + 3, (4 * i) + dsi + 4);
			checksum = checksum + Integer.parseInt(s1, 16);
			checksum = checksum + Integer.parseInt(s2, 16);
			int temp = Integer.parseInt(s1 + s2, 16);
			patch[i] = temp;
		}
		int chk = Integer.parseInt(d.substring((4 * patchSize) + dsi, (4 * patchSize) + dsi	+ 2), 16);
		if (chk != (-checksum & 0x7F)) {
			status = "checksum error";
		} else {
			status = "succesful";
		}		
		return status;
	}

	public void setPatch(int[] p) {
		patch = p;
	}

	public int[] getPatch() {
		return patch;
	}
	
	public void setEngine(Object object) {
		if (object == LEAD) {
			setParameter(16, 0, 0, 2, false);
		} else if (object == BASSLINE) {
			setParameter(16, 1, 0, 2, false);
		} else if (object == DRUM) {
			setParameter(16, 2, 0, 2, false);
		} else if (object == MULTI) {
			setParameter(16, 3, 0, 2, false);
		}
		initPatch();		
	}

	public int getParameter(int address, int start_bit, int resolution) {
		int value = 0;
		
		// resolution < 8 bit: gets value of bit length 'resolution' starting on
		// the start_bit
		if (resolution < 8) {
			int bitmask = 0;
			for (int j = 0; j < resolution; j++) { // Iterate over all bits
				bitmask = bitmask | (int) Math.pow(2, (start_bit + j));
			}
			value = (bitmask & nibbleSwap(patch[address])) >> start_bit;
		}

		// 8 bit values
		if (resolution == 8) {
			value = nibbleSwap(patch[address]);
		}

		// 12 bit values
		if (resolution == 12) {
			int temp1 = nibbleSwap(patch[address]);
			int temp2 = nibbleSwap(patch[address + 1] & 0xF0);
			value = (temp2 << 8) + temp1;
		}

		// 16 bit values
		if (resolution == 16) {
			int temp1 = nibbleSwap(patch[address]);
			int temp2 = nibbleSwap(patch[address + 1]);
			value = (temp2 << 8) + temp1;
		}

		return value;
	}
	
	public void setParameter(int address, int value, int start_bit,
			int resolution, boolean forward) {
		
		// resolution < 8 bit: sets value of bit length 'resolution' starting on
		// the start_bit
		if (resolution < 8) {
			int temp = nibbleSwap(patch[address]); // Retrieve data

			for (int j = 0; j < resolution; j++) { // Iterate over all bits
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
			patch[address] = nibbleSwap(temp); // Put back new data
			if (forward) {
				sysexSend(address, temp, 1);
			}
		}

		// 8 bit values
		if (resolution == 8) {
			patch[address] = nibbleSwap(value & 0xFF);
			if (forward) {
				sysexSend(address, (value & 0xFF), 1);
			}
		}

		// 12 bit values
		if (resolution == 12) {
			int temp = nibbleSwap(patch[address + 1]); // Retrieve data
			temp = (temp & 0xFF00);
			patch[address] = nibbleSwap(value & 0xFF);
			patch[address + 1] = nibbleSwap(temp + ((value & 0xF00) >> 8));
			if (forward) {
				sysexSend(address, ((temp << 4) + (value & 0xFFF)), 2);
			}
		}

		// 16 bit values
		if (resolution == 16) {
			patch[address] = nibbleSwap(value & 0xFF);
			patch[address + 1] = nibbleSwap((value & 0xFF00) >> 8);
			if (forward) {
				sysexSend(address, (value & 0xFFFF), 2);
			}
		}
	}

	private static int nibbleSwap(int inInt) {
		int nibble0 = (((byte) inInt) << 4) & 0xf0;
		int nibble1 = (((byte) inInt) >>> 4) & 0x0f;
		return (nibble0 | nibble1);
	}

	public void setStereoLink(boolean b) {
		if (b) {
			WOPT = WOPT | 0x01;
		} else {
			WOPT = WOPT & 0x02;
		}
	}

	public void setOscillatorLink(boolean b) {
		if (b) {
			WOPT = WOPT | 0x02;
		} else {
			WOPT = WOPT & 0x01;
		}
	}
	
	public boolean isEnsemble() {
		return isEnsemble;
	}

	public void sysexSend(int addr, int value, int bytes) {		
		String strMessage;
		if (isEnsemble) {
			strMessage = SIDSysexInfo.editEnsembleParameterSysex;			
		} else {
			strMessage = SIDSysexInfo.editPatchParameterSysex;
			strMessage = strMessage.replace("<wopt>", "0" + Integer.toHexString(WOPT));			
		}		
		strMessage = strMessage.replace("<device>", "00");
		strMessage = strMessage.replace("<address>", calcAddr(addr));
		strMessage = strMessage.replace("<value>", calcValue(value, bytes));
		
		sendString(strMessage);
	}
	
	public void sendPlay() {
		String strMessage = SIDSysexInfo.playSysex;
		strMessage = strMessage.replace("<device>", "00");
		sendString(strMessage);
	}
	
	public void sendPanic() {
		String strMessage = SIDSysexInfo.panicSysex;
		strMessage = strMessage.replace("<device>", "00");
		sendString(strMessage);
	}
	
	private byte[] toByteArray(String s) {
		int nLengthInBytes = s.length() / 2;
		byte[] abMessage = new byte[nLengthInBytes];
		for (int i = 0; i < nLengthInBytes; i++) {
			abMessage[i] = (byte) Integer.parseInt(s.substring(i * 2, i * 2 + 2), 16);
		}
		return abMessage;
	}
	
	private void sendString(String strMessage) {
		// When the length of the Sysex String is shorter than any previous strings, pad the end with F7 bytes to make it identical in size
		// This is yet another workaround for the freakin' Java Sysex bug!
		if (strMessage.length() > maxSysExLength) {
			maxSysExLength = strMessage.length();
		} else {
			while (strMessage.length() < maxSysExLength) {
				strMessage = strMessage + "F7";
			}				
		}		
		//System.out.println(strMessage);
		SysexMessage sysexMessage = new SysexMessage();
		try {
			byte[] abMessage = toByteArray(strMessage);
			sysexMessage.setMessage(abMessage, abMessage.length);
		} catch (Exception e) {
			System.out.println("exception!");
			System.out.println(e);
		}
		receiver.send(sysexMessage, -1);

		try {
			Thread.currentThread().sleep(delay);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
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

	public Patch clone() {
		Patch p = new Patch(receiver, patchSize, initPatches);
		p.setPatch(patch.clone());
		return p;
	}
		
}