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
import org.midibox.midi.MidiKeyboardController;
import org.midibox.midi.VirtualMidiDevice;

public class VirtualKeyboard {

	private MidiDeviceManager midiDeviceManager;

	private MidiDeviceRouting midiDeviceRouting;

	private MidiKeyboardController midiKeyboardController;

	private VirtualMidiDevice midiKeyboardControllerDevice;

	private VirtualMidiDevice virtualKeyboardInPort;

	private VirtualMidiDevice virtualKeyboardThruPort;

	private VirtualMidiDevice virtualKeyboardOutPort;

	public VirtualKeyboard() {

		midiDeviceRouting = new MidiDeviceRouting();

		midiKeyboardControllerDevice = new VirtualMidiDevice(
				"Virtual MIDI Keyboard", -1, -1);

		midiKeyboardController = new MidiKeyboardController(
				midiKeyboardControllerDevice.getMidiOutReceiver(), 0);

		midiDeviceManager = new MidiDeviceManager();

		midiDeviceManager.rescanDevices();

		virtualKeyboardInPort = new VirtualMidiDevice("Virtual Keyboard In",
				-1, -1);
		virtualKeyboardInPort.setMidiInReceiver(virtualKeyboardInPort
				.getMidiOutReceiver());

		virtualKeyboardThruPort = new VirtualMidiDevice(
				"Virtual Keyboard Thru", -1, -1);
		virtualKeyboardThruPort.setMidiInReceiver(virtualKeyboardThruPort
				.getMidiOutReceiver());

		virtualKeyboardOutPort = new VirtualMidiDevice("Virtual Keyboard Out",
				-1, -1);
		virtualKeyboardOutPort.setMidiInReceiver(virtualKeyboardOutPort
				.getMidiOutReceiver());

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
				.getSelectedMidiReadDevices().values());

		midiDeviceRouting.addMidiWriteDevices(midiDeviceManager
				.getSelectedMidiWriteDevices().values());
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public MidiKeyboardController getMidiKeyboardController() {
		return midiKeyboardController;
	}
}
