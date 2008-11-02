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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;
import org.midibox.midi.MidiUtils;
import org.midibox.sidlibr.Patch;

public class SIDSysexParameterControl extends SIDSysexParameter {

	public final static Object RECEIVE = new Object();

	public final static Object SEND = new Object();

	protected boolean receive;

	protected boolean send;
	
	protected int type;
	
	protected String[] valAlias;
	protected String[] spValAlias;
	
	public int[] snapvals = {0}; 
	public boolean useAlias = true;	// Only for rate knob
	public boolean snap = false;
	
	protected String tooltip;
	
	protected Object tooltipListener;
	
	public SIDSysexParameterControl(int type, String[] valAlias ,Patch patch, int addres, int start_bit, int reso, String name, String tooltip) {
		super(patch, addres, start_bit, reso, name);
		setReceive(true);
		setSend(true);
		this.tooltip = tooltip;
		this.type = type;
		this.valAlias = valAlias;
		this.spValAlias = sparseValAlias(valAlias);
	}
	
	public SIDSysexParameterControl(int type, String[] valAlias ,Patch patch, int addres, int start_bit, int reso, String name) {
		super(patch, addres, start_bit, reso, name);
		setReceive(true);
		setSend(true);
		this.tooltip = name;
		this.type = type;
		this.valAlias = valAlias;	
		this.spValAlias = sparseValAlias(valAlias);
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
	
	public String[] getSparseValAlias() {
		return spValAlias;
	}
	
	public int lookUpValue(int i) {
		String s = valAlias[i];
		int val = 0;
		for(int j=0;j<spValAlias.length;j++) {
			if (spValAlias[j].equals(s)) {
				val = j;
				break;
			}			
		}
		return val;
	}	
	
	public int lookUpAlias(String s) {
		int val = 0;
		for(int j=0;j<valAlias.length;j++) {
			if (valAlias[j].equals(s)) {
				val = j;
				break;
			}			
		}
		return val;
	}
	
	public String[] sparseValAlias(String[] in) {
		if (valAlias!=null) {
			int temp = 0;
			for(int i=0;i<in.length;i++) {
				if (in[i]!="") {
					temp = temp + 1;				
				}
			}		
		
			String [] out = new String[temp];
			temp = 0;
			for(int i=0;i<in.length;i++) {
				if (in[i]!="") {
					out[temp] = in[i];
					temp = temp + 1;
				}
			}
			return out; 
		} else {
			String [] out = {""};
			return out;
		}
	}

	public void send(MidiMessage message, long lTimeStamp) {
		String m = MidiUtils.getHexString(message.getMessage()).replace(" ", "");		
		if (m.indexOf(SIDSysexInfo.acknowledgedSysex.replace("<device>", "00"))== 0) {
			//System.out.println("MBSID: Acknowdledged!");
		}else if (m.equals(SIDSysexInfo.error1Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Received less bytes then expected");
		}else if (m.equals(SIDSysexInfo.error2Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Wrong checksum");
		}else if (m.equals(SIDSysexInfo.error3Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Bankstick or patch/drumset/ensemble not available");
		}else if (m.equals(SIDSysexInfo.error4Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: Parameter not available");
		}else if (m.equals(SIDSysexInfo.error5Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: RAM access not supported");
		}else if (m.equals(SIDSysexInfo.error6Sysex.replace("<device>", "00"))) {
			System.out.println("MBSID: BankStick too small");
		}else {
			System.out.println("MBSID: Unknown Sysex string: " + m);			
		}
		
		if (receive) {
			super.send(message, lTimeStamp);
		}
	}
		
}
