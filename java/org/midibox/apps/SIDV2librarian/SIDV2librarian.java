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

package org.midibox.apps.SIDV2librarian;

import java.util.Observable;
import java.util.prefs.Preferences;
import java.util.Observer;
import javax.swing.JOptionPane;
import org.midibox.midi.MidiDeviceRouting;
import org.midibox.midi.gui.MidiDeviceRoutingGUI;
import org.midibox.sidlibr.SysExControllerDevice;
import org.midibox.sidlibr.SIDLibController;


public class SIDV2librarian{
	private Preferences prefs = Preferences.userNodeForPackage(this.getClass());
	private MidiDeviceRouting midiDeviceRouting;
	private SysExControllerDevice sysexControllerDevice;
	private SIDLibController sidLibController;
	private MidiDeviceRoutingGUI midiDeviceRoutingGUI;
	private boolean allowPrefs = false;

	public SIDV2librarian() {
		midiDeviceRouting = new MidiDeviceRouting();		
		midiDeviceRoutingGUI = new MidiDeviceRoutingGUI(midiDeviceRouting);	
		sysexControllerDevice = new SysExControllerDevice("MidiBox SID V2 Editor");
		sidLibController = new SIDLibController(sysexControllerDevice.getSysExController());

		midiDeviceRouting.addMidiWriteDevice(sysexControllerDevice);
		midiDeviceRouting.addMidiReadDevice(sysexControllerDevice);
		midiDeviceRouting.reorder();
		
		if (prefs.getInt("DEVICES_HASH", 0)==midiDeviceRouting.hashCode()) {
			recallConnections();			
		} else {
			JOptionPane.showMessageDialog(null,"Your MIDI configuration has changed, please check connections!","MIDI configuration changed!",JOptionPane.OK_OPTION);			
		}		
		allowPrefs = true;
	}
	
	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}
	
	public MidiDeviceRoutingGUI getMidiDeviceRoutingGUI() {
		return midiDeviceRoutingGUI;
	}

	public SysExControllerDevice getSysExControllerDevice() {
		return sysexControllerDevice;
	}
	
	public SIDLibController getSIDLibController() {
		return sidLibController;
	}
	
	private void recallConnections() {	
		String strMatrix = prefs.get("CONNECTION_MATRIX", "");
		int s1 = prefs.getInt("READ_DEVICES", 0);
		int s2 = prefs.getInt("WRITE_DEVICES", 0);
		int[][] matrix = new int[s1][s2];
		for(int r=0;r<s1;r++) {
			for(int w=0;w<s2;w++) {
				matrix[r][w] = Character.getNumericValue(strMatrix.charAt((r*s2)+w));				
			}
		}
		midiDeviceRouting.setConnectionMatrix(matrix);		
	}
		
	public void storeConnections() {
		int[][] matrix = midiDeviceRouting.getConnectionMatrix();
		if(matrix.length!=0) {
			int s1 = matrix.length;
			int s2 = matrix[0].length;
			String strMatrix = "";
			for(int r=0;r<s1;r++) {
				for(int w=0;w<s2;w++) {
					strMatrix = strMatrix + Integer.toString(matrix[r][w]);				
				}
			}
			prefs.putInt("READ_DEVICES", s1);
			prefs.putInt("WRITE_DEVICES", s2);
			prefs.put("CONNECTION_MATRIX", strMatrix);
			prefs.putInt("DEVICES_HASH", midiDeviceRouting.hashCode());
		}		
	}
}
