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

import java.util.Arrays;

import javax.sound.midi.MidiMessage;

import org.midibox.midi.MidiUtils;
import org.midibox.sidlibr.Patch;

public class SIDSysexParameterControl extends SIDSysexParameter {

	public final static Object RECEIVE = new Object();

	public final static Object SEND = new Object();

	protected boolean receive;

	protected boolean send;

	protected int type;

	private String[] valAlias;
	private int[] sparseMatrix;
	private int valueOffset;

	public int[] snapVals;
	public String[] snapAlias;
	public boolean snap = false;

	protected String tooltip;

	protected Object tooltipListener;

	public SIDSysexParameterControl(int type, String[] valAlias, int offset,
			Patch patch, int addres, int start_bit, int reso, String name,
			String tooltip) {
		super(patch, addres, start_bit, reso, name);
		setReceive(true);
		setSend(true);
		this.tooltip = tooltip;
		this.type = type;
		this.valueOffset = offset;
		if (valAlias != null) {
			this.valAlias = valAlias;
			sparseMatrix = generateSparseMatrix(valAlias);
		}
	}

	public SIDSysexParameterControl(int type, String[] valAlias, int offset,
			Patch patch, int addres, int start_bit, int reso, String name) {
		super(patch, addres, start_bit, reso, name);
		setReceive(true);
		setSend(true);
		this.tooltip = name;
		this.type = type;
		this.valueOffset = offset;
		if (valAlias != null) {
			this.valAlias = valAlias;
			sparseMatrix = generateSparseMatrix(valAlias);
		}
	}

	public boolean isReceive() {
		return receive;
	}

	public void setTooltipListener(Object object) {
		tooltipListener = object;
	}

	public Object getTooltipListener() {
		return tooltipListener;
	}

	public String getTooltip() {
		return tooltip;
	}

	public void setReceive(boolean respond) {
		this.receive = respond;
		setChanged();
		notifyObservers(RECEIVE);
		clearChanged();
	}

	public boolean isSend() {
		return send;
	}

	public void setSend(boolean send) {
		this.send = send;
		setChanged();
		notifyObservers(SEND);
		clearChanged();
	}

	public void setMidiValue(int value, boolean forward) {
		super.setMidiValue(value, forward);
	}

	public int getType() {
		return type;
	}

	public String[] getValAlias() {
		return valAlias;
	}

	public String getMidiValueWithAlias() {
		if (snap) {
			if (snapAlias == null) {
				return Integer.toString(snapVals[getMidiValSnapIndex()]
						+ valueOffset);
			} else {
				return snapAlias[getMidiValSnapIndex()];
			}
		} else {
			if (valAlias == null) {
				return Integer.toString(getMidiValue() + valueOffset);
			} else {
				return valAlias[getMidiValue()];
			}
		}
	}

	public void SetMidiValueWithAlias(String s) {
		int val = getMidiValue();
		if (snap) {
			if ((snapAlias == null)
					&& (Arrays.binarySearch(snapVals, Integer.valueOf(s)
							- valueOffset) != -1)) {
				val = Integer.valueOf(s) - valueOffset;
			} else {
				val = snapVals[searchStringArray(snapAlias, s, val)];
			}
		} else {
			if (valAlias == null) {
				val = Integer.valueOf(s) - valueOffset;
			} else {
				val = searchStringArray(valAlias, s, val);
			}
		}
		setMidiValue(val, true);
	}

	public int getMidiValSnapIndex() {
		int i = Arrays.binarySearch(snapVals, getMidiValue());
		if (i == -1) {
			i = 0;
		}
		return i;
	}

	private int searchStringArray(String[] sarray, String s, int def) {
		int val = def;
		for (int j = 0; j < sarray.length; j++) {
			if (sarray[j].equals(s)) {
				val = j;
				break;
			}
		}
		return val;
	}

	public String[] getSparseValAlias() {
		String[] s = new String[sparseMatrix.length];
		for (int i = 0; i < sparseMatrix.length; i++) {
			s[i] = valAlias[sparseMatrix[i]];
		}
		return s;
	}

	private String[] generateAlias() {
		String[] s = new String[midimax - midimin + 1];
		for (int i = midimin; i <= midimax; i++) {
			s[i - midimin] = Integer.toString(i + valueOffset);
		}
		return s;
	}

	public int getMidiValueWithSparse() {
		return Arrays.binarySearch(sparseMatrix, getMidiValue());
	}

	public void setMidiValueWithSparse(int i) {
		setMidiValue(sparseMatrix[i], true);
	}

	public int[] generateSparseMatrix(String[] in) {
		int temp = 0;
		for (int i = 0; i < in.length; i++) {
			if (in[i] != "") {
				temp = temp + 1;
			}
		}

		int[] sparseMat = new int[temp];
		temp = 0;
		for (int i = 0; i < in.length; i++) {
			if (in[i] != "") {
				sparseMat[temp++] = i;
			}
		}
		return sparseMat;
	}

	public void send(MidiMessage message, long lTimeStamp) {
		String m = MidiUtils.getHexString(message.getMessage())
				.replace(" ", "");
		if (m.indexOf(SIDSysexInfo.acknowledgedSysex.replace("<device>", "00")) == 0) {
			// System.out.println("MBSID: Acknowdledged!");
		} else if (m.equals(SIDSysexInfo.error1Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Received less bytes then expected");
		} else if (m.equals(SIDSysexInfo.error2Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Wrong checksum");
		} else if (m.equals(SIDSysexInfo.error3Sysex.replace("<device>", "00"))) {
			System.out
					.println("MBSID: Bankstick or patch/drumset/ensemble not available");
		} else if (m.equals(SIDSysexInfo.error4Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Parameter not available");
		} else if (m.equals(SIDSysexInfo.error5Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: RAM access not supported");
		} else if (m.equals(SIDSysexInfo.error6Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: BankStick too small");
		} else {
			System.out.println("MBSID: Unknown Sysex string: " + m);
		}

		if (receive) {
			super.send(message, lTimeStamp);
		}
	}

}
