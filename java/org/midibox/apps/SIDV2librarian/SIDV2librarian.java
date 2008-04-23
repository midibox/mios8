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
		sysexControllerDevice = new SysExControllerDevice("MidiBox SID V2 Editor");
		midiDeviceRouting = new MidiDeviceRouting(sysexControllerDevice);	
		midiDeviceRoutingGUI = new MidiDeviceRoutingGUI(midiDeviceRouting);			
		sidLibController = new SIDLibController(sysexControllerDevice.getSysExController());
		
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
		int input = prefs.getInt("INPUT_DEVICE", 0);
		int output = prefs.getInt("OUTPUT_DEVICE", 0);
		midiDeviceRouting.setInputDevice(input);
		midiDeviceRouting.setOutputDevice(output);		
	}
		
	public void storeConnections() {
		int input = midiDeviceRouting.getInputDeviceIndex();
		int output = midiDeviceRouting.getOutputDeviceIndex();
		prefs.putInt("INPUT_DEVICE", input);
		prefs.putInt("OUTPUT_DEVICE", output);
		prefs.putInt("DEVICES_HASH", midiDeviceRouting.hashCode());
	}
}
