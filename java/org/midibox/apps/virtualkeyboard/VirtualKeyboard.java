/*
 * @(#)VirtualKeyboard.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Adam King (adamjking@optusnet.com.au)
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

package org.midibox.apps.virtualkeyboard;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.midi.MidiDeviceRouting;
import org.midibox.midi.MidiKeyboardControllerDevice;
import org.midibox.midi.MidiRouterDevice;

public class VirtualKeyboard {

	private MidiDeviceManager midiDeviceManager;

	private MidiDeviceRouting midiDeviceRouting;

	private MidiKeyboardControllerDevice midiKeyboardControllerDevice;

	private MidiRouterDevice virtualKeyboardInPort;

	private MidiRouterDevice virtualKeyboardThruPort;

	private MidiRouterDevice virtualKeyboardOutPort;

	public VirtualKeyboard() {

		midiDeviceRouting = new MidiDeviceRouting();

		midiKeyboardControllerDevice = new MidiKeyboardControllerDevice(
				"Virtual MIDI Keyboard", 0);

		midiDeviceManager = new MidiDeviceManager();

		midiDeviceManager.rescanDevices();

		virtualKeyboardInPort = new MidiRouterDevice("Virtual Keyboard In");

		virtualKeyboardThruPort = new MidiRouterDevice("Virtual Keyboard Thru");

		virtualKeyboardOutPort = new MidiRouterDevice("Virtual Keyboard Out");

		midiDeviceRouting.connectDevices(virtualKeyboardInPort,
				midiKeyboardControllerDevice);

		midiDeviceRouting.connectDevices(virtualKeyboardInPort,
				virtualKeyboardThruPort);

		midiDeviceRouting.connectDevices(midiKeyboardControllerDevice,
				virtualKeyboardOutPort);

		midiDeviceRouting.addMidiReadDevice(virtualKeyboardOutPort);

		midiDeviceRouting.addMidiReadDevice(virtualKeyboardThruPort);

		midiDeviceRouting.addMidiWriteDevice(virtualKeyboardInPort);

		midiDeviceRouting.addMidiReadDevices(midiDeviceManager
				.getSelectedMidiReadDevices());

		midiDeviceRouting.addMidiWriteDevices(midiDeviceManager
				.getSelectedMidiWriteDevices());
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public MidiKeyboardControllerDevice getMidiKeyboardControllerDevice() {
		return midiKeyboardControllerDevice;
	}
}
